#include "msggate_env.h"

namespace
{
	const char*		ACCOUNT_DB		= "account_gate";
	const char*		ACCOUNT_DB_USER	= "sfkaccount";
	const char*		ACCOUNT_DB_PASS	= "sfkaccount";
	const char*		ACCOUNT_PASSWORD	= "gatepw";
	unsigned long	g_idNextGateAccount	= 40001;

	std::string		g_strAccRun;
	int				g_nLoginPort = 0, g_nPointPort = 0, g_nGatePort = 0;
	WinsockGuard*	g_pGateWinsock = NULL;

	class CAccountServerProcess
	{
	public:
		CAccountServerProcess() { memset(&m_pi, 0, sizeof(m_pi)); }
		~CAccountServerProcess() { Kill(); }
		void Start()
		{
			std::string exe = g_strBin + "\\AccountServer.exe";
			STARTUPINFOA si = { sizeof(si) };
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOWMINNOACTIVE;
			if(!::CreateProcessA(exe.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, g_strAccRun.c_str(), &si, &m_pi))
				Fail("cannot start " + exe + " (tools\\build.ps1 first)");
		}
		bool HasExited() { return m_pi.hProcess && ::WaitForSingleObject(m_pi.hProcess, 0) == WAIT_OBJECT_0; }
		void Kill()
		{
			if(m_pi.hProcess)
			{
				::TerminateProcess(m_pi.hProcess, 1);
				::WaitForSingleObject(m_pi.hProcess, 10000);
				::CloseHandle(m_pi.hProcess);
				::CloseHandle(m_pi.hThread);
				memset(&m_pi, 0, sizeof(m_pi));
			}
		}
	private:
		PROCESS_INFORMATION m_pi;
	};

	CAccountServerProcess*	g_pAccountServer = NULL;
	CMsgServerProcess*		g_pGateServer = NULL;

	unsigned long Rotl(unsigned long v, unsigned long n) { n %= 32; return n ? (v << n) | (v >> (32 - n)) : v; }

	void ClientRc5(unsigned char* buf16)
	{
		static const unsigned char KEY[16] = { 0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E, 0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE };
		unsigned long S[26], L[4];
		memcpy(L, KEY, 16);
		S[0] = 0xB7E15163;
		for(int i = 1; i < 26; i++) S[i] = S[i - 1] + 0x9E3779B9;
		unsigned long A = 0, B = 0;
		for(int k = 0, i = 0, j = 0; k < 3 * 26; k++, i = (i + 1) % 26, j = (j + 1) % 4)
		{
			A = S[i] = Rotl(S[i] + A + B, 3);
			B = L[j] = Rotl(L[j] + A + B, A + B);
		}
		for(int k = 0; k < 2; k++)
		{
			unsigned long a, b;
			memcpy(&a, buf16 + 8 * k, 4); memcpy(&b, buf16 + 8 * k + 4, 4);
			a += S[0]; b += S[1];
			for(int i = 1; i <= 12; i++) { a = Rotl(a ^ b, b) + S[2 * i]; b = Rotl(b ^ a, a) + S[2 * i + 1]; }
			memcpy(buf16 + 8 * k, &a, 4); memcpy(buf16 + 8 * k + 4, &b, 4);
		}
	}

	Bytes Msg1051(const char* pszAccount, const char* pszPassword, const char* pszServer)
	{
		Bytes b = Header(0x34, 1051);
		PutStr(b, 4, pszAccount, 16);
		unsigned char psw[16] = { 0 };
		memcpy(psw, pszPassword, strlen(pszPassword));
		ClientRc5(psw);
		memcpy(&b[0x14], psw, 16);
		PutStr(b, 0x24, pszServer, 16);
		return b;
	}

	void BuildAccountDatabase()
	{
		MYSQL* h = Connect(NULL);
		Exec(h, std::string("DROP DATABASE IF EXISTS ") + ACCOUNT_DB);
		Exec(h, std::string("CREATE DATABASE ") + ACCOUNT_DB);
		Exec(h, std::string("USE ") + ACCOUNT_DB);
		std::string sql = ReadFile(g_strRepo + "\\src\\AccountServer\\account.sql"), stmt;
		size_t i = 0;
		while(i < sql.size())
		{
			size_t e = sql.find('\n', i);
			std::string line = sql.substr(i, e == std::string::npos ? std::string::npos : e - i);
			i = e == std::string::npos ? sql.size() : e + 1;
			size_t c = line.find('#');
			if(c != std::string::npos)
				line = line.substr(0, c);
			stmt += line + "\n";
			if(line.find(';') != std::string::npos)
			{
				if(stmt.find_first_not_of(" \t\r\n;") != std::string::npos)
					Exec(h, stmt);
				stmt.clear();
			}
		}
		for(const char* pszHost : { "localhost", "127.0.0.1" })
		{
			Exec(h, std::string("CREATE USER IF NOT EXISTS '") + ACCOUNT_DB_USER + "'@'" + pszHost + "' IDENTIFIED BY '" + ACCOUNT_DB_PASS + "'");
			Exec(h, std::string("GRANT ALL ON ") + ACCOUNT_DB + ".* TO '" + ACCOUNT_DB_USER + "'@'" + pszHost + "'");
		}
		mysql_close(h);
	}

	void AddAccount(unsigned long id, const std::string& strName)
	{
		MYSQL* h = Connect(ACCOUNT_DB);
		char sql[512];
		sprintf(sql, "INSERT account SET id=%lu, name='%s', password='%s', sex=0, age=0, phone='', email='', address='', idnumber=''",
				id, strName.c_str(), ACCOUNT_PASSWORD);
		Exec(h, sql);
		sprintf(sql, "INSERT point001 SET account_id=%lu, type=1, point=1000, pointtime=0, online=0, licence=0, netbar_ip='', ip_mask=''", id);
		Exec(h, sql);
		mysql_close(h);
	}

	int FreePortInRange(int nFrom)
	{
		for(int n = nFrom; n < 32767; n++)
		{
			SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_port = htons((u_short)n);
			bool bFree = bind(s, (sockaddr*)&addr, sizeof(addr)) == 0;
			closesocket(s);
			if(bFree)
				return n;
		}
		Fail("no free port below 32767");
		return 0;
	}

	void BuildAccountRunDirectory()
	{
		g_strAccRun = g_strRepo + "\\db\\accountgate";
		::CreateDirectoryA(g_strAccRun.c_str(), NULL);
		g_nLoginPort = FreePortInRange(29958);
		g_nPointPort = FreePortInRange(g_nLoginPort + 1);
		char buf[4096];
		sprintf(buf,
			"; SYNTHETIC (accountgate_tests.cpp): the reference's config.ini with the gate's ports and database\r\n"
			"[System]\r\nSERVERKEY=a61fce5e-443ffc04\r\nLOGINLOOPDELAY=50\r\nPOINTLOOPDELAY=10\r\nREBUILDLISTENDELAYSEC=5\r\n"
			"LOGINLISTENPORT=%d\r\nPOINTLISTENPORT=%d\r\nPOINTSOCKETSNDBUF=65000\r\nMAXBANIPS=50\r\nBANERRORS=10\r\n"
			"BANERRORSECS=30\r\nBANSECS=300\r\nONLINETABLESIZE=10000\r\nHEARTBEATINTERVALSECS=300\r\nHEARTBEATKICKSECS=1500\r\n"
			"ONLINEHEARTBEATSECS=600\r\nPOINTFEEINTERVALSECS=180\r\nENABLELOGINDELAY=0\r\nOUTPUTONLINESECS=60\r\n"
			"REJOINTIMEWAITSECS=0\r\nCOUNTFILESECS=60\r\nACCOUNT_AHEAD_DAYS=7\r\nSERVER_BUSY_DELAY_SECS=10\r\n"
			"SERVER_FULL_DELAY_SECS=30\r\nKICKOUT_USER_WHEN_NO_POINT=1\r\nENABLE_LOGIN_NO_POINT=1\r\nMONTH_CARD_TO_POINT_CARD=1\r\n"
			"MAX_GM_ID=0\r\nSTAT_SERIAL=0\r\nENABLE_91U_CREATE_ACCOUNT=0\r\n"
			"[Database]\r\nDBHOSTNAME=127.0.0.1\r\nDATABASENAME=%s\r\n"
			"[Special]\r\nPOINTTABLE=point001\r\n",
			g_nLoginPort, g_nPointPort, ACCOUNT_DB);
		WriteText(g_strAccRun + "\\config.ini", buf);
		sprintf(buf, "; SYNTHETIC: MsgServer's login (shell.ini's [AccountServer], msggate_env.h), a free server\r\n"
			"[Account1]\r\nSERVERNAME=%s\r\nLOGINNAME=%s\r\nPASSWORD=%s\r\nFREE_DEBUG=1\r\n91U_ACCOUNT=0\r\n",
			SERVER_NAME, SERVER_ACCOUNT, SERVER_PASSWORD);
		WriteText(g_strAccRun + "\\Account.ini", buf);
		WriteText(g_strAccRun + "\\isp.ini", "; SYNTHETIC: no provider\r\n[MAIN]\r\nMaxPart=0\r\n");
		sprintf(buf, "1000 1 127.0.0.1 %s %s %s\n", ACCOUNT_DB_USER, ACCOUNT_DB_PASS, ACCOUNT_DB);
		WriteText(g_strAccRun + "\\gameserver.cfg", buf);
	}

	struct Reply1052 { unsigned long idAccount, dwKey; std::string strText; };

	bool AccountLogin(CScriptedClient& client, const char* pszAccount, const char* pszPassword, const char* pszServer, Reply1052& r)
	{
		if(!client.Connect(g_nLoginPort))
			Fail("the scripted client cannot connect to the account server's login port");
		if(!client.Send(Msg1051(pszAccount, pszPassword, pszServer)))
			Fail("the 1051 could not be sent");
		GameMsg msg;
		if(!RecvUntil(client, msg, 10000, [](const GameMsg& m) { return m.usType == 1052; }))
			return false;
		if(msg.buf.size() != 0x1C)
			Fail("the 1052 is not 1Ch");
		r.idAccount = Get32(msg.buf, 4);
		r.dwKey = Get32(msg.buf, 8);
		r.strText.assign((const char*)&msg.buf[0x0C], strnlen((const char*)&msg.buf[0x0C], 16));
		client.Close();
		return true;
	}

	void GameLogin(CScriptedClient& client, const Reply1052& r)
	{
		if(!client.Connect(g_nGatePort))
			Fail("the scripted client cannot connect to the game port");
		if(!client.Send(Msg1052(r.idAccount, r.dwKey)))
			Fail("the scripted client could not send its 1052");
		client.Rekey(r.dwKey ^ ((r.idAccount + r.dwKey) ^ 0x4321));
	}
}

TEST_CLASS(AccountGateTests)
{
public:
	TEST_CLASS_INITIALIZE(Setup)
	{
		g_pGateWinsock = new WinsockGuard;
		FindPaths();
		int nStale = EndStaleServers();
		if(nStale)
			Log("ended %d MsgServer/NpcServer of this build left by another test class", nStale);
		BuildGateDatabase();
		BuildRunDirectory();
		BuildAccountDatabase();
		BuildAccountRunDirectory();

		g_pAccountServer = new CAccountServerProcess;
		g_pAccountServer->Start();
		DWORD t0 = ::GetTickCount();
		while(!(PortAccepts(g_nLoginPort) && PortAccepts(g_nPointPort)))
		{
			if(g_pAccountServer->HasExited())
				Fail("AccountServer exited at its start (see " + g_strAccRun + "\\!log!)");
			if(::GetTickCount() - t0 > 30000)
				Fail("AccountServer did not open its two ports within 30 s");
			::Sleep(200);
		}
		Log("our AccountServer listens: login port %d, point port %d", g_nLoginPort, g_nPointPort);

		g_nGatePort = FreePort();
		WriteShellIni(g_nPointPort, g_nGatePort);
		std::string log = LogPath();
		size_t nLog0 = ReadFile(log).size();
		g_pGateServer = new CMsgServerProcess;
		g_pGateServer->Start();
		bool bOK = false;
		while(!bOK && ::GetTickCount() - t0 < 240000)
		{
			g_pGateServer->PressDebugWarning();
			if(g_pGateServer->HasExited())
				Fail("MsgServer exited (see " + log + ")");
			std::string s = ReadFile(log);
			s = s.size() > nLog0 ? s.substr(nLog0) : "";
			if(s.find("SHELL: Account server login failed!") != std::string::npos)
				Fail("MsgServer's login into our AccountServer failed (its log)");
			bOK = s.find("SHELL: Account server login OK") != std::string::npos && PortAccepts(g_nGatePort);
			::Sleep(500);
		}
		if(!bOK)
			Fail("MsgServer did not log into our AccountServer and open its game port within 240 s");
		Log("MsgServer logged into our AccountServer; its game port %d is open", g_nGatePort);
	}

	TEST_CLASS_CLEANUP(Teardown)
	{
		delete g_pGateServer;
		delete g_pAccountServer;
		delete g_pGateWinsock;
	}

	TEST_METHOD(A1_ClientLogsInThroughOurAccountServer)
	{
		for(int nRun = 1; nRun <= 2; nRun++)
		{
			unsigned long id = g_idNextGateAccount++;
			std::string strAccount = "gate" + std::to_string(id);
			AddAccount(id, strAccount);

			Reply1052 r = {};
			CScriptedClient c1;
			Assert::IsTrue(AccountLogin(c1, strAccount.c_str(), ACCOUNT_PASSWORD, SERVER_NAME, r), L"no 1052 from the account server");
			Log("run %d: 1052 id %lu, key %08lX, text \"%s\"", nRun, r.idAccount, r.dwKey, r.strText.c_str());
			unsigned long dwFirstKey = r.dwKey;
			Assert::AreEqual(id, r.idAccount);
			Assert::AreEqual(std::string("127.0.0.1"), r.strText, L"the text is not the game server's address");

			CScriptedClient c2;
			GameLogin(c2, r);
			GameMsg msg;
			std::string words;
			Assert::IsTrue(RecvUntil(c2, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2101, &words); }), L"no 1004 on 2101 from the game server");
			Assert::AreEqual(std::string("NEW_ROLE"), words);
			std::string strRole = "Gate" + std::to_string(id % 10000);
			Assert::IsTrue(c2.Send(Msg1001("scenario", strRole.c_str(), "scenariopw", PLAYER_LOOK, PLAYER_PROFESSION, id)));
			Assert::IsTrue(RecvUntil(c2, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2100, &words); }), L"no 1004 on 2100 after the 1001");
			Assert::AreEqual(std::string("ANSWER_OK"), words);
			c2.Close();

			CScriptedClient c3, c4;
			Assert::IsTrue(AccountLogin(c3, strAccount.c_str(), ACCOUNT_PASSWORD, SERVER_NAME, r), L"no 1052 for the second login");
			Assert::AreNotEqual(dwFirstKey, r.dwKey, L"the second login brought the first login's key again");
			Assert::AreEqual(id, r.idAccount);
			GameLogin(c4, r);
			bool bFeeLine = false;
			auto SeeFeeLine = [&](const GameMsg& m) { unsigned short ch = 0; std::string w; if(Read1004(m, ch, w) && w.compare(0, 11, "STR_FEETYPE") == 0) bFeeLine = true; };
			Assert::IsTrue(RecvUntil(c4, msg, 10000, [&](const GameMsg& m) { SeeFeeLine(m); return Is1004(m, 2101, &words); }), L"no 1004 on 2101 for the second login");
			Assert::AreEqual(std::string("ANSWER_OK"), words);
			Assert::IsTrue(RecvUntil(c4, msg, 10000, [&](const GameMsg& m) { SeeFeeLine(m); return m.usType == 1006; }), L"no 1006: the role did not enter");
			Assert::AreNotEqual(std::string::npos, std::string(msg.buf.begin(), msg.buf.end()).find(strRole), L"the 1006 is not the role created");
			Assert::IsTrue(c4.Send(Msg1010(Get32(msg.buf, 4), 0, 0, 0, 0, 74)), L"the scripted client could not send action 74");
			Assert::IsTrue(RecvUntil(c4, msg, 10000, [&](const GameMsg& m) { SeeFeeLine(m); return m.usType == 1010 && Get16(m.buf, 0x16) == 74; }), L"no 1010 reply to action 74");
			RecvUntil(c4, msg, 3000, [&](const GameMsg& m) { SeeFeeLine(m); return bFeeLine; });
			Log("run %d: billing line %s", nRun, bFeeLine ? "seen" : "not seen");
			Assert::IsFalse(bFeeLine, L"the billing line reached the player: the account server did not answer 100");
		}
	}

	TEST_METHOD(A2_PositiveControl_RefusalsReachTheClient)
	{
		unsigned long id = g_idNextGateAccount++;
		std::string strAccount = "gate" + std::to_string(id);
		AddAccount(id, strAccount);

		Reply1052 r = {};
		CScriptedClient c1;
		Assert::IsTrue(AccountLogin(c1, strAccount.c_str(), "wrongpw", SERVER_NAME, r), L"no 1052 for a wrong password");
		Assert::AreEqual(0UL, r.idAccount);
		Assert::AreEqual(std::string("\xD5\xCA\xBA\xC5\xC3\xFB\xBB\xF2\xBF\xDA\xC1\xEE\xB4\xED"), r.strText, L"not the client's wrong-password text");

		CScriptedClient c2;
		Assert::IsTrue(AccountLogin(c2, strAccount.c_str(), ACCOUNT_PASSWORD, "noserver", r), L"no 1052 for an unknown server");
		Assert::AreEqual(0UL, r.idAccount);
		Assert::AreEqual(std::string("\xB7\xFE\xCE\xF1\xC6\xF7\xCE\xB4\xC6\xF4\xB6\xAF"), r.strText, L"not the client's server-not-started text");
	}

	DWORD RunAddAccount(const std::string& strName, const std::string& strPassword)
	{
		std::string cmd = "powershell.exe -NoProfile -ExecutionPolicy Bypass -File \"" + g_strRepo + "\\tools\\add-account.ps1\" "
				+ strName + " " + strPassword + " -Database " + ACCOUNT_DB;
		STARTUPINFOA si = { sizeof(si) };
		PROCESS_INFORMATION pi = {};
		std::vector<char> line(cmd.begin(), cmd.end());
		line.push_back(0);
		if(!::CreateProcessA(NULL, &line[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, g_strRepo.c_str(), &si, &pi))
			Fail("cannot start powershell.exe");
		DWORD dwExit = 0xFFFFFFFF;
		if(::WaitForSingleObject(pi.hProcess, 60000) == WAIT_OBJECT_0)
			::GetExitCodeProcess(pi.hProcess, &dwExit);
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
		return dwExit;
	}

	TEST_METHOD(A3_AddAccountScriptLogsIn)
	{
		for(int nRun = 1; nRun <= 2; nRun++)
		{
			std::string strAccount = "script" + std::to_string(nRun), strPassword = "scr!pt#" + std::to_string(nRun);
			Assert::AreEqual(0UL, RunAddAccount(strAccount, strPassword), L"add-account.ps1 failed");
			MYSQL* h = Connect(ACCOUNT_DB);
			std::vector<std::vector<std::string> > rows = Query(h, "SELECT a.id, p.netbar_ip IS NULL, p.ip_mask IS NULL FROM account a JOIN point001 p ON p.account_id = a.id WHERE a.name = '" + strAccount + "'");
			mysql_close(h);
			Assert::AreEqual((size_t)1, rows.size(), L"not one account row with its point001 row");
			Assert::AreEqual(std::string("00"), rows[0][1] + rows[0][2], L"netbar_ip or ip_mask is NULL");
			unsigned long id = strtoul(rows[0][0].c_str(), NULL, 10);

			Reply1052 r = {};
			CScriptedClient c1;
			Assert::IsTrue(AccountLogin(c1, strAccount.c_str(), strPassword.c_str(), SERVER_NAME, r), L"no 1052 from the account server");
			Log("run %d: script account id %lu, 1052 id %lu, text \"%s\"", nRun, id, r.idAccount, r.strText.c_str());
			Assert::AreEqual(id, r.idAccount, L"the script's account did not log in");
			Assert::AreEqual(std::string("127.0.0.1"), r.strText);

			Assert::AreEqual(1UL, RunAddAccount(strAccount, "other"), L"a second add of the same name was not refused");
		}
	}

	static std::string AccountErrorLog(time_t t)
	{
		tm* pTm = localtime(&t);
		char buf[64];
		sprintf(buf, "\\!log!\\AccountServer %04d-%02d-%02d.LOG", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
		return g_strAccRun + buf;
	}

	static int CountLines(const std::string& s, const char* pszText)
	{
		int n = 0;
		for(size_t i = s.find(pszText); i != std::string::npos; i = s.find(pszText, i + 1))
			n++;
		return n;
	}

	TEST_METHOD(A4_PlayerCountTakenSilently)
	{
		time_t t0 = time(NULL);
		std::string strLog0 = AccountErrorLog(t0);
		size_t nSize0 = ReadFile(strLog0).size();
		::Sleep(45000);
		time_t t1 = time(NULL);
		std::string strNew = ReadFile(strLog0);
		strNew = strNew.size() > nSize0 ? strNew.substr(nSize0) : "";
		if(AccountErrorLog(t1) != strLog0)
			strNew += ReadFile(AccountErrorLog(t1));
		int nBad = CountLines(strNew, "Bad _MSG_QUERYFEE");
		Log("45 s of the account server's error log: %d bytes, %d \"Bad _MSG_QUERYFEE\" lines", (int)strNew.size(), nBad);
		Assert::IsFalse(g_pAccountServer->HasExited(), L"AccountServer exited");
		Assert::AreEqual(0, nBad, L"the player count was logged as a bad query");
	}
};
