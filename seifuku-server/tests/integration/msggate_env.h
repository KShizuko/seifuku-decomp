#ifndef MSGGATE_ENV_H
#define MSGGATE_ENV_H

#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include "CppUnitTest.h"
#include "mysql.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "account_standin.h"
#include "realmaps.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "psapi.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	const char*	DB_HOST	= "127.0.0.1";
	const char*	DB_USER	= "root";
	const char*	DB_PASS	= "";
	const unsigned int DB_PORT = 3306;
	const char*	GATE_DB	= "cq_msggate";
	const char*	MERGED_DB	= "cq_merged";

	const char*	SERVER_ACCOUNT	= "msggate";
	const char*	SERVER_PASSWORD	= "msggatepw";
	const char*	SERVER_NAME		= "msggate";
	const unsigned long	SERVER_ID	= 7;

	void Log(const char* fmt, ...)
	{
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		_vsnprintf(buf, sizeof(buf) - 1, fmt, args);
		va_end(args);
		buf[sizeof(buf) - 1] = 0;
		Logger::WriteMessage(buf);
		Logger::WriteMessage("\n");
	}

	void Fail(const std::string& str)
	{
		std::wstring w(str.begin(), str.end());
		Assert::Fail(w.c_str());
	}

	MYSQL* Connect(const char* pszDb)
	{
		MYSQL* h = mysql_init(NULL);
		mysql_options(h, MYSQL_SET_CHARSET_NAME, "latin1");
		if(!mysql_real_connect(h, DB_HOST, DB_USER, DB_PASS, pszDb, DB_PORT, NULL, 0))
		{
			std::string s = std::string("MariaDB not reachable (tools\\db.ps1 start): ") + mysql_error(h);
			mysql_close(h);
			Fail(s);
		}
		return h;
	}

	void Exec(MYSQL* h, const std::string& sql)
	{
		if(mysql_query(h, sql.c_str()) != 0)
			Fail(sql.substr(0, 200) + ": " + mysql_error(h));
		MYSQL_RES* res = mysql_store_result(h);
		if(res)
			mysql_free_result(res);
	}

	std::vector<std::vector<std::string> > Query(MYSQL* h, const std::string& sql)
	{
		std::vector<std::vector<std::string> > rows;
		if(mysql_query(h, sql.c_str()) != 0)
			Fail(sql.substr(0, 200) + ": " + mysql_error(h));
		MYSQL_RES* res = mysql_store_result(h);
		if(!res)
			return rows;
		unsigned int n = mysql_num_fields(res);
		while(MYSQL_ROW r = mysql_fetch_row(res))
		{
			std::vector<std::string> row;
			for(unsigned int i = 0; i < n; i++)
				row.push_back(r[i] ? r[i] : "");
			rows.push_back(row);
		}
		mysql_free_result(res);
		return rows;
	}

	std::string ReadFile(const std::string& path)
	{
		FILE* fp = fopen(path.c_str(), "rb");
		if(!fp)
			return "";
		std::string s;
		char buf[65536];
		size_t n;
		while((n = fread(buf, 1, sizeof(buf), fp)) > 0)
			s.append(buf, n);
		fclose(fp);
		return s;
	}

	int		g_nCopied = 0, g_nItemTypes = 0;

	void BuildGateDatabase()
	{
		MYSQL* h = Connect(NULL);
		Exec(h, std::string("DROP DATABASE IF EXISTS ") + GATE_DB);
		Exec(h, std::string("CREATE DATABASE ") + GATE_DB + " DEFAULT CHARACTER SET latin1");
		Exec(h, "SET SESSION sql_mode='NO_AUTO_VALUE_ON_ZERO'");
		std::vector<std::vector<std::string> > tables = Query(h, std::string("SHOW TABLES FROM ") + MERGED_DB);
		if(tables.size() != 90)
			Fail(std::string(MERGED_DB) + " does not hold the 90 tables (tools\\db.ps1 -Action merged)");
		Exec(h, std::string("USE ") + GATE_DB);
		g_nCopied = 0;
		for(size_t i = 0; i < tables.size(); i++)
		{
			const std::string& t = tables[i][0];
			std::vector<std::vector<std::string> > create = Query(h, std::string("SHOW CREATE TABLE ") + MERGED_DB + ".`" + t + "`");
			if(create.size() != 1 || create[0].size() != 2)
				Fail("no CREATE statement for " + t);
			Exec(h, create[0][1]);
			Exec(h, std::string("INSERT INTO `") + t + "` SELECT * FROM " + MERGED_DB + ".`" + t + "`");
			g_nCopied++;
		}
		g_nItemTypes = atoi(Query(h, "SELECT COUNT(*) FROM cq_itemtype")[0][0].c_str());
		mysql_close(h);
	}

	std::string g_strRepo, g_strBin, g_strRun;

	void FindPaths()
	{
		HMODULE hSelf = NULL;
		::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
							(LPCSTR)&FindPaths, &hSelf);
		char sz[MAX_PATH];
		::GetModuleFileNameA(hSelf, sz, MAX_PATH);
		*strrchr(sz, '\\') = 0;
		g_strBin = sz;
		*strrchr(sz, '\\') = 0;
		*strrchr(sz, '\\') = 0;
		g_strRepo = sz;
		g_strRun = g_strRepo + "\\db\\msggate";
	}

	int EndStaleServers()
	{
		int nEnded = 0;
		const std::string names[] = { g_strBin + "\\MsgServer.exe", g_strBin + "\\NpcServer.exe" };
		DWORD ids[4096], cb = 0;
		if(!::EnumProcesses(ids, sizeof(ids), &cb))
			return 0;
		for(DWORD i = 0; i < cb / sizeof(DWORD); i++)
		{
			HANDLE h = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, FALSE, ids[i]);
			if(!h)
				continue;
			char sz[MAX_PATH];

			if(::GetModuleFileNameExA(h, NULL, sz, MAX_PATH) && (_stricmp(sz, names[0].c_str()) == 0 || _stricmp(sz, names[1].c_str()) == 0))
			{
				::TerminateProcess(h, 1);
				::WaitForSingleObject(h, 10000);
				nEnded++;
			}
			::CloseHandle(h);
		}
		return nEnded;
	}

	void WriteText(const std::string& path, const std::string& text)
	{
		FILE* fp = fopen(path.c_str(), "wb");
		if(!fp)
			Fail("cannot write " + path);
		fwrite(text.data(), 1, text.size(), fp);
		fclose(fp);
	}

	void PutI32(FILE* fp, int v) { fwrite(&v, 4, 1, fp); }

	void WriteFlatMap(const std::string& path, int cx, int cy)
	{
		FILE* fp = fopen(path.c_str(), "wb");
		if(!fp)
			Fail("cannot write " + path);
		char head[8] = "SYNTH";
		fwrite(head, 1, 8, fp);
		char puzzle[260] = "none";
		fwrite(puzzle, 1, 260, fp);
		PutI32(fp, cx);
		PutI32(fp, cy);
		std::vector<char> row(cx * 6 + 4, 0);
		for(int y = 0; y < cy; y++)
			fwrite(&row[0], 1, row.size(), fp);
		PutI32(fp, 0);
		PutI32(fp, 0);
		fclose(fp);
	}

	int		g_nMapDocs = 0;

	void BuildRunDirectory()
	{
		const char* dirs[] = { "\\db", "\\db\\msggate", "\\db\\msggate\\ini", "\\db\\msggate\\map", "\\db\\msggate\\map\\synth",
								"\\db\\msggate\\syslog", "\\db\\msggate\\gmlog" };
		for(int i = 0; i < sizeof(dirs)/sizeof(dirs[0]); i++)
			::CreateDirectoryA((g_strRepo + dirs[i]).c_str(), NULL);

		std::string strings = "Language=English\r\n";
		std::string res = ReadFile(g_strRepo + "\\src\\MsgServer\\share\\Cn_DDRes.h");
		for(size_t a = res.find("GetStr(\""); a != std::string::npos; a = res.find("GetStr(\"", a + 1))
		{
			size_t b = res.find('"', a + 8);
			std::string key = b != std::string::npos ? res.substr(a + 8, b - (a + 8)) : "";
			if(key.empty() || key == "Language")
				continue;
			if(key == "NOMATE_NAME" || key == "TITILE_NONE" || key == "SYNNAME_NONE" || key == "ACTIONSTR_NONE")
				strings += key + "=None\r\n";
			else
				strings += key + "=" + key + "\r\n";
		}
		WriteText(g_strRun + "\\ini\\StrRes.ini", strings +
			"STR_GUARD_CALL_s=synthetic guard call %s\r\nSTR_PPKER_CALL_s=synthetic pker call %s\r\n");
		WriteText(g_strRun + "\\ini\\AdditiveSize.ini", "; SYNTHETIC\r\n[Header]\r\nAmount=0\r\n");
		WriteText(g_strRun + "\\ini\\TerrainNpc.ini", "; SYNTHETIC: no terrain item\r\n");
		WriteText(g_strRun + "\\adapter.ini", "[Module]\r\nAmount=1\r\nModule0=Lottery.dll\r\n");

		if(realmaps::Enabled())
		{
			std::string err = realmaps::Use(g_strRepo, g_strRun, g_nMapDocs);
			if(!err.empty())
				Fail("the 4351 maps: " + err);
			return;
		}

		MYSQL* h = Connect(GATE_DB);
		std::vector<std::vector<std::string> > docs = Query(h,
			"SELECT m.mapdoc, GREATEST(64, IFNULL(MAX(p.x), 0) + 16), GREATEST(64, IFNULL(MAX(p.y), 0) + 16) FROM ("
			" SELECT id, mapdoc FROM cq_map UNION ALL SELECT id, mapdoc FROM cq_dynamap WHERE mapdoc<>0) m LEFT JOIN ("
			" SELECT id AS mapid, portal0_x AS x, portal0_y AS y FROM cq_map UNION ALL"
			" SELECT id, link_x, link_y FROM cq_map UNION ALL"
			" SELECT mapid, cellx, celly FROM cq_npc UNION ALL SELECT mapid, cellx, celly FROM cq_dynanpc UNION ALL"
			" SELECT mapid, bound_x + bound_cx, bound_y + bound_cy FROM cq_generator UNION ALL"
			" SELECT mapid, born_x + 1, born_y + 1 FROM cq_generator UNION ALL"
			" SELECT mapid, portal_x, portal_y FROM cq_portal UNION ALL SELECT map_id, pos_x, pos_y FROM cq_trap UNION ALL"
			" SELECT mapid, bound_x + bound_cx, bound_y + bound_cy FROM cq_region UNION ALL"
			" SELECT CAST(SUBSTRING_INDEX(param, ' ', 1) AS UNSIGNED), CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(param, ' ', 2), ' ', -1) AS UNSIGNED),"
			" CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(param, ' ', 3), ' ', -1) AS UNSIGNED) FROM cq_action WHERE type IN (301, 1003, 1004) AND param REGEXP '^[0-9]+ [0-9]+ [0-9]+'"
			") p ON p.mapid = m.id GROUP BY m.mapdoc ORDER BY m.mapdoc");
		mysql_close(h);

		FILE* fp = fopen((g_strRun + "\\ini\\GameMap.dat").c_str(), "wb");
		if(!fp)
			Fail("cannot write GameMap.dat");
		PutI32(fp, (int)docs.size());
		for(size_t i = 0; i < docs.size(); i++)
		{
			unsigned long idDoc = strtoul(docs[i][0].c_str(), NULL, 10);
			int cx = atoi(docs[i][1].c_str());
			int cy = atoi(docs[i][2].c_str());
			char name[64];
			sprintf(name, "map\\synth\\%lu.dmap", idDoc);
			WriteFlatMap(g_strRun + "\\" + name, cx, cy);
			fwrite(&idDoc, 4, 1, fp);
			PutI32(fp, (int)strlen(name));
			fwrite(name, 1, strlen(name), fp);
			PutI32(fp, 256);
		}
		fclose(fp);
		g_nMapDocs = (int)docs.size();
	}

	void WriteShellIni(int nAccountPort, int nGamePort)
	{
		char buf[2048];
		sprintf(buf,
			"; SYNTHETIC (msggate_tests.cpp)\r\n"
			"[AccountServer]\r\nACCOUNT_IP=127.0.0.1\r\nACCOUNT_PORT=%d\r\nLOGINNAME=%s\r\nPASSWORD=%s\r\nSERVERNAME=%s\r\n"
			"[Database]\r\nDB_IP=127.0.0.1\r\nDB_USER=root\r\nDB_PW=\r\nDB_DB=%s\r\nDB_PORT=3306\r\n"
			"[InternetPort]\r\nCURRENT_PORTID=0\r\n"
			"[Language]\r\nStringFile=ini\\StrRes.ini\r\n"
			"[System]\r\nMAPGROUP_SIZE=1\r\nSERVE_PORT=%d\r\n",
			nAccountPort, SERVER_ACCOUNT, SERVER_PASSWORD, SERVER_NAME, GATE_DB, nGamePort);
		WriteText(g_strRun + "\\shell.ini", buf);
	}

	int FreePort()
	{
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		bind(s, (sockaddr*)&addr, sizeof(addr));
		int len = sizeof(addr);
		getsockname(s, (sockaddr*)&addr, &len);
		closesocket(s);
		return ntohs(addr.sin_port);
	}

	bool PortAccepts(int nPort)
	{
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		addr.sin_port = htons((u_short)nPort);
		bool bOk = connect(s, (sockaddr*)&addr, sizeof(addr)) == 0;
		closesocket(s);
		return bOk;
	}

	std::string LogPath()
	{
		SYSTEMTIME t;
		::GetLocalTime(&t);
		char buf[64];
		sprintf(buf, "\\syslog\\CQ_SERVER %04d-%02d-%02d.LOG", t.wYear, t.wMonth, t.wDay);
		return g_strRun + buf;
	}

	struct FindWarning { DWORD pid; HWND hWnd; };
	BOOL CALLBACK FindWarningProc(HWND hWnd, LPARAM l)
	{
		FindWarning* p = (FindWarning*)l;
		DWORD pid = 0;
		::GetWindowThreadProcessId(hWnd, &pid);
		char sz[64] = "";
		::GetWindowTextA(hWnd, sz, sizeof(sz));
		if(pid == p->pid && strcmp(sz, "Warning") == 0)
		{
			p->hWnd = hWnd;
			return FALSE;
		}
		return TRUE;
	}

	class CMsgServerProcess
	{
	public:
		CMsgServerProcess() { memset(&m_pi, 0, sizeof(m_pi)); }
		~CMsgServerProcess() { Kill(); }
		void Start(const char* pszExe = "MsgServer.exe")
		{
			std::string exe = g_strBin + "\\" + pszExe;
			STARTUPINFOA si = { sizeof(si) };
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOWMINNOACTIVE;
			if(!::CreateProcessA(exe.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, g_strRun.c_str(), &si, &m_pi))
				Fail("cannot start " + exe + " (tools\\build.ps1 first)");
		}
		void PressDebugWarning()
		{
			FindWarning f = { m_pi.dwProcessId, NULL };
			::EnumWindows(FindWarningProc, (LPARAM)&f);
			if(f.hWnd)
				::PostMessageA(f.hWnd, WM_COMMAND, IDCANCEL, 0);
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

	std::string CheckAccountLogin(const CAccountStandIn::Message& msg, int nGamePort)
	{
		if(msg.usType != CAccountStandIn::MSG_ACCOUNT) return "not 1099";
		if(msg.buf.size() != CAccountStandIn::SIZE_ACCOUNT) return "not 38h bytes";
		const char* b = (const char*)&msg.buf[0];
		if(strncmp(b + 4, SERVER_ACCOUNT, 16) != 0) return "the account (+4) is not shell.ini's LOGINNAME";
		if(strncmp(b + 0x14, SERVER_PASSWORD, 16) != 0) return "the password (+14h) is not shell.ini's PASSWORD";
		if(strncmp(b + 0x24, SERVER_NAME, 16) != 0) return "the server (+24h) is not shell.ini's SERVERNAME";
		if(*(int*)(b + 0x34) != nGamePort) return "the port (+34h) is not shell.ini's SERVE_PORT";
		return "";
	}

	struct RunResult
	{
		std::vector<unsigned char>	login;
		std::vector<unsigned char>	echo;
		bool	bGamePortOpen;
		bool	bLoginOK, bLoginFailed;
		DWORD	tAccept;
	};

	RunResult StartGate(CAccountStandIn& standin, CMsgServerProcess& server, int& nGamePort, unsigned long idServer)
	{
		RunResult r = {};
		if(!standin.Listen(0))
			Fail("the stand-in cannot listen on 127.0.0.1");
		nGamePort = FreePort();
		WriteShellIni(standin.GetPort(), nGamePort);

		std::string log = LogPath();
		size_t nLog0 = ReadFile(log).size();

		DWORD t0 = ::GetTickCount();
		server.Start();
		const DWORD START_LIMIT_MS = 180000;
		bool bAccepted = false;
		while(!bAccepted && ::GetTickCount() - t0 < START_LIMIT_MS)
		{
			server.PressDebugWarning();
			if(server.HasExited())
				Fail("MsgServer exited before it connected to the account server (see " + log + ")");
			bAccepted = standin.Accept(1000);
		}
		if(!bAccepted)
			Fail("MsgServer did not connect to the account stand-in within 180 s");
		CAccountStandIn::Message msg;
		bool bLogin = false;
		while(!bLogin && ::GetTickCount() - t0 < START_LIMIT_MS)
		{
			if(server.HasExited())
				Fail("MsgServer exited before it sent its 1099 (see " + log + ")");
			bLogin = standin.Recv(msg, 1000);
		}
		if(!bLogin)
			Fail("no 1099 within 180 s of the start");
		r.tAccept = ::GetTickCount() - t0;
		std::string err = CheckAccountLogin(msg, nGamePort);
		if(!err.empty())
			Fail("the 1099: " + err);
		r.login = msg.buf;
		*(int*)&r.login[0x34] = 0;
		Log("  in  1099 (%u bytes) %lu ms after the start: account \"%s\", server \"%s\", port %d", (unsigned)msg.buf.size(),
				r.tAccept, (const char*)&msg.buf[4], (const char*)&msg.buf[0x24], *(int*)&msg.buf[0x34]);

		standin.SendLoginAnswer(idServer, 0x12345678, 4351, SERVER_NAME);
		Log("  out 1002: server id %lu", idServer);
		if(idServer)
		{
			if(!standin.Recv(msg, 10000) || msg.usType != CAccountStandIn::MSG_LOGIN || msg.buf.size() != CAccountStandIn::SIZE_LOGIN)
				Fail("MsgServer did not echo the 1002");
			r.echo = msg.buf;
			for(size_t i = 0x0E, bEnd = 0; i < 0x20; i++)
			{
				if(bEnd)
					r.echo[i] = 0;
				else if(r.echo[i] == 0)
					bEnd = 1;
			}
			Log("  in  1002 (the login echoed)");
		}

		DWORD t1 = ::GetTickCount();
		while(::GetTickCount() - t1 < 10000)
		{
			std::string s = ReadFile(log);
			if(s.size() > nLog0)
				s = s.substr(nLog0);
			else
				s = "";
			r.bLoginOK		= s.find("SHELL: Account server login OK") != std::string::npos;
			r.bLoginFailed	= s.find("SHELL: Account server login failed!") != std::string::npos;
			if(r.bLoginOK || r.bLoginFailed)
				break;
			::Sleep(200);
		}
		r.bGamePortOpen = PortAccepts(nGamePort);
		Log("  the game port %d %s; the log says%s%s", nGamePort, r.bGamePortOpen ? "accepts" : "does not accept",
				r.bLoginOK ? " \"Account server login OK\"" : "", r.bLoginFailed ? " \"Account server login failed!\"" : "");
		return r;
	}

	RunResult RunGate(unsigned long idServer)
	{
		CAccountStandIn standin;
		CMsgServerProcess server;
		int nGamePort = 0;
		RunResult r = StartGate(standin, server, nGamePort, idServer);
		server.Kill();
		return r;
	}

	struct ClientCipher
	{
		int c1, c2;
		unsigned char t1[256], t2[256];
		ClientCipher() : c1(0), c2(0)
		{
			t1[0] = 0x9D; t2[0] = 0x62;
			for(int i = 0; i < 255; i++)
			{
				t1[i + 1] = (unsigned char)((((t1[i] * 0xFA) & 0xFF) * t1[i] + t1[i] * 0x0F + 0x13) & 0xFF);
				t2[i + 1] = (unsigned char)(((t2[i] * 0xA4 + 0x79) * t2[i] + 0x6D) & 0xFF);
			}
		}
		void Transform(unsigned char* buf, int len)
		{
			for(int i = 0; i < len; i++)
			{
				unsigned char b = buf[i];
				b ^= t1[c1];
				b ^= t2[c2];
				if(++c1 >= 256) { c1 = 0; if(++c2 >= 256) c2 = 0; }
				b = (unsigned char)(((b & 0x0F) << 4) + ((b & 0xF0) >> 4));
				b ^= 0xAB;
				buf[i] = b;
			}
		}
		void Rekey(DWORD key)
		{
			for(int i = 0; i < 64; i++)
			{
				((DWORD*)t1)[i] ^= key;
				((DWORD*)t2)[i] ^= key * key;
			}
		}
	};

	typedef std::vector<unsigned char>	Bytes;

	struct GameMsg
	{
		unsigned short	usType;
		Bytes			buf;
	};

	unsigned short	Get16(const Bytes& b, size_t i)	{ return i + 2 <= b.size() ? (unsigned short)(b[i] | b[i + 1] << 8) : 0; }
	unsigned long	Get32(const Bytes& b, size_t i)	{ return i + 4 <= b.size() ? (unsigned long)(b[i] | b[i + 1] << 8 | b[i + 2] << 16 | b[i + 3] << 24) : 0; }
	void Put16(Bytes& b, size_t i, unsigned short v)	{ b[i] = (unsigned char)v; b[i + 1] = (unsigned char)(v >> 8); }
	void Put32(Bytes& b, size_t i, unsigned long v)	{ for(int k = 0; k < 4; k++) b[i + k] = (unsigned char)(v >> (8 * k)); }
	void PutStr(Bytes& b, size_t i, const char* s, size_t n)	{ for(size_t k = 0; k < n && s[k]; k++) b[i + k] = (unsigned char)s[k]; }

	Bytes Header(unsigned short usSize, unsigned short usType)
	{
		Bytes b(usSize, 0);
		Put16(b, 0, usSize);
		Put16(b, 2, usType);
		return b;
	}

	Bytes Msg1052(unsigned long idAccount, unsigned long dwKey)
	{
		Bytes b = Header(0x1C, 1052);
		Put32(b, 4, idAccount);
		Put32(b, 8, dwKey);
		Put16(b, 0x0C, 121);
		return b;
	}

	Bytes Msg1001(const char* pszAccount, const char* pszName, const char* pszPassword, unsigned short usLook,
					unsigned short usProfession, unsigned long idAccount)
	{
		Bytes b = Header(0x3C, 1001);
		PutStr(b, 4, pszAccount, 15);
		PutStr(b, 0x14, pszName, 15);
		PutStr(b, 0x24, pszPassword, 15);
		Put16(b, 0x34, usLook);
		Put16(b, 0x36, usProfession);
		Put32(b, 0x38, idAccount);
		return b;
	}

	Bytes Msg1010(unsigned long id, unsigned long dwData, unsigned short x, unsigned short y, unsigned short usDir,
					unsigned short usAction)
	{
		Bytes b = Header(0x18, 1010);
		Put32(b, 4, ::GetTickCount());
		Put32(b, 8, id);
		Put32(b, 0x0C, dwData);
		Put16(b, 0x10, x);
		Put16(b, 0x12, y);
		Put16(b, 0x14, usDir);
		Put16(b, 0x16, usAction);
		return b;
	}

	Bytes Msg1009(unsigned long id, unsigned long dwData, unsigned short usAction, unsigned long idHero)
	{
		Bytes b = Header(0x14, 1009);
		Put32(b, 4, id);
		Put32(b, 8, dwData);
		Put16(b, 0x0C, usAction);
		Put32(b, 0x10, ::GetTickCount() ^ idHero);
		return b;
	}

	Bytes Msg1015(unsigned long dwData, unsigned char ucKind, const char* pszName = NULL)
	{
		Bytes packer(1, pszName ? 1 : 0);
		if(pszName)
		{
			packer.push_back((unsigned char)strlen(pszName));
			packer.insert(packer.end(), pszName, pszName + strlen(pszName));
		}
		Bytes b = Header((unsigned short)(9 + packer.size()), 1015);
		Put32(b, 4, dwData);
		b[8] = ucKind;
		memcpy(&b[9], &packer[0], packer.size());
		return b;
	}

	Bytes Msg1019(unsigned long id, unsigned char ucAction, unsigned char ucOnline, const char* pszName)
	{
		Bytes b = Header(0x1C, 1019);
		Put32(b, 4, id);
		b[8] = ucAction;
		b[9] = ucOnline;
		PutStr(b, 0x0C, pszName, 15);
		return b;
	}

	Bytes Msg1022(unsigned short usType, unsigned long idSender, unsigned long idTarget, unsigned short x, unsigned short y,
					unsigned long dwData)
	{
		Bytes b = Header(0x1C, 1022);
		Put32(b, 4, ::GetTickCount());
		Put32(b, 8, idSender);
		Put32(b, 0x0C, idTarget);
		Put16(b, 0x10, x);
		Put16(b, 0x12, y);
		Put16(b, 0x14, usType);
		Put32(b, 0x18, dwData);
		return b;
	}

	Bytes Msg1022Magic(unsigned long idUser, unsigned short usMagic, unsigned short usLevel, unsigned long idTarget,
						unsigned short x, unsigned short y, bool bBadCheck = false)
	{
		auto rotShort = [](unsigned long n, int nBits) { n &= 0xFFFF; return (unsigned short)(((n >> nBits) | (n << (16 - nBits))) & 0xFFFF); };
		auto rotLong = [](unsigned long n, int nBits) { return (n >> nBits) | (n << (32 - nBits)); };
		unsigned long dwTime = ::GetTickCount();
		Bytes b = Msg1022(21, idUser, 0, 0, 0, 0);
		Put32(b, 4, dwTime);
		Put32(b, 0x0C, rotLong(((idTarget - 0x8B90B51A) ^ idUser ^ 0x5F2D2463), 32 - 13));
		Put16(b, 0x10, (unsigned short)(rotShort(x - 0xDD12, 1) ^ idUser ^ 0x2ED6));
		Put16(b, 0x12, (unsigned short)(rotShort(y - 0x76DE, 5) ^ idUser ^ 0xB99B));
		Put16(b, 0x18, (unsigned short)(rotShort(usMagic - 0x14BE, 3) ^ idUser ^ 0x915D));
		Put16(b, 0x1A, (unsigned short)((((dwTime + (bBadCheck ? 1 : 0)) % 0x100) << 8 | usLevel) ^ 0x3721));
		return b;
	}

	Bytes Msg1024(unsigned char ucForce, unsigned char ucDex, unsigned char ucHealth, unsigned char ucSoul)
	{
		Bytes b = Header(8, 1024);
		b[4] = ucForce;
		b[5] = ucDex;
		b[6] = ucHealth;
		b[7] = ucSoul;
		return b;
	}

	Bytes Msg2031(unsigned long idNpc, unsigned short usAction, unsigned long dwData = 0, unsigned short usType = 0)
	{
		Bytes b = Header(0x10, 2031);
		Put32(b, 4, idNpc);
		Put32(b, 8, dwData);
		Put16(b, 0x0C, usAction);
		Put16(b, 0x0E, usType);
		return b;
	}

	Bytes Msg2032(unsigned long idTask, unsigned char ucIndex, unsigned char ucAction, const char* pszText = NULL)
	{
		size_t nText = pszText ? strlen(pszText) : 0;
		Bytes b = Header((unsigned short)(pszText ? 0x0E + nText : 0x10), 2032);
		Put32(b, 4, idTask);
		b[0x0A] = ucIndex;
		b[0x0B] = ucAction;
		if(pszText)
		{
			b[0x0C] = 1;
			b[0x0D] = (unsigned char)nText;
			memcpy(&b[0x0E], pszText, nText);
		}
		return b;
	}

	Bytes Msg1027(unsigned long idUser, unsigned long idItem, unsigned long idGem, unsigned short usPos, unsigned short usAction)
	{
		Bytes b = Header(0x14, 1027);
		Put32(b, 4, idUser);
		Put32(b, 8, idItem);
		Put32(b, 0x0C, idGem);
		Put16(b, 0x10, usPos);
		Put16(b, 0x12, usAction);
		return b;
	}

	Bytes Msg1028(unsigned long idNpc, unsigned short usType, const std::vector<unsigned long>& items)
	{
		Bytes b = Header((unsigned short)(0x0C + 4 * items.size()), 1028);
		Put32(b, 4, idNpc);
		Put16(b, 8, usType);
		Put16(b, 0x0A, (unsigned short)items.size());
		for(size_t i = 0; i < items.size(); i++)
			Put32(b, 0x0C + 4 * i, items[i]);
		return b;
	}

	Bytes Msg2036(unsigned char ucAction, const unsigned long (&dw)[5])
	{
		Bytes b = Header(0x1C, 2036);
		b[4] = ucAction;
		b[5] = 5;
		for(int i = 0; i < 5; i++)
			Put32(b, 8 + 4 * i, dw[i]);
		return b;
	}

	Bytes Msg1102(unsigned long idNpc, unsigned char ucAction, unsigned char ucType, unsigned long idItem)
	{
		Bytes b = Header(0x24, 1102);
		Put32(b, 4, idNpc);
		b[8] = ucAction;
		b[9] = ucType;
		Put32(b, 0x0C, idItem);
		return b;
	}

	Bytes Msg1113(unsigned char ucAction, unsigned long idNpc, unsigned char ucType, unsigned long dwMoney)
	{
		Bytes b = Header(0x14, 1113);
		b[4] = ucAction;
		b[5] = 1;
		Put32(b, 8, idNpc);
		b[0x0C] = ucType;
		Put32(b, 0x10, dwMoney);
		return b;
	}

	Bytes Msg2030(unsigned short usX, unsigned short usY, unsigned short usLook, unsigned short usType, unsigned short usSort)
	{
		Bytes b = Header(0x14, 2030);
		Put16(b, 8, usX);
		Put16(b, 0x0A, usY);
		Put16(b, 0x0C, usLook);
		Put16(b, 0x0E, usType);
		Put16(b, 0x10, usSort);
		return b;
	}

	Bytes Msg1056(unsigned short usAction, unsigned long id)
	{
		Bytes b = Header(0x0C, 1056);
		Put32(b, 4, id);
		Put16(b, 8, usAction);
		return b;
	}

	Bytes Msg1111(unsigned short usIndex, unsigned short usChannel, unsigned char ucAction, const char* pszName = NULL)
	{
		Bytes packer(1, pszName ? 1 : 0);
		if(pszName)
		{
			packer.push_back((unsigned char)strlen(pszName));
			packer.insert(packer.end(), pszName, pszName + strlen(pszName));
		}
		Bytes b = Header((unsigned short)(9 + packer.size()), 1111);
		Put16(b, 4, usIndex);
		Put16(b, 6, usChannel);
		b[8] = ucAction;
		memcpy(&b[9], &packer[0], packer.size());
		return b;
	}

	Bytes Msg1107(unsigned short usAction, unsigned long dwData)
	{
		Bytes b = Header(0x0C, 1107);
		Put16(b, 4, usAction);
		Put32(b, 8, dwData);
		return b;
	}

	Bytes Msg1112(const char* pszName)
	{
		Bytes b = Header(0x1C, 1112);
		PutStr(b, 9, pszName, 15);
		return b;
	}

	Bytes Msg1023(unsigned short usAction, unsigned long idPlayer)
	{
		Bytes b = Header(0x0C, 1023);
		Put16(b, 4, usAction);
		Put32(b, 8, idPlayer);
		return b;
	}

	Bytes Msg1005(unsigned long id, unsigned char ucDir, unsigned char ucMode)
	{
		Bytes b = Header(0x0C, 1005);
		Put32(b, 4, id);
		b[8] = ucDir;
		b[9] = ucMode;
		return b;
	}

	Bytes Msg1004(unsigned short usChannel, const char* pszSpeaker, const char* pszHearer, const char* pszWords,
					unsigned long dwColor = 0xFFFFFF)
	{
		const char* strs[] = { pszSpeaker, pszHearer, "", pszWords };
		Bytes packer(1, 4);
		for(int i = 0; i < 4; i++)
		{
			size_t n = strlen(strs[i]);
			packer.push_back((unsigned char)n);
			packer.insert(packer.end(), strs[i], strs[i] + n);
		}
		Bytes b = Header((unsigned short)(packer.size() + 0x13), 1004);
		Put32(b, 4, dwColor);
		Put16(b, 8, usChannel);
		Put16(b, 0x0A, 0);
		Put32(b, 0x0C, ::GetTickCount());
		memcpy(&b[0x10], &packer[0], packer.size());
		return b;
	}

	std::string Read1004Str(const GameMsg& msg, int nIndex)
	{
		if(msg.usType != 1004 || msg.buf.size() < 0x11)
			return "";
		size_t i = 0x10;
		int nCount = msg.buf[i++];
		for(int n = 0; n < nCount && i < msg.buf.size(); n++)
		{
			size_t nLen = msg.buf[i++];
			if(i + nLen > msg.buf.size())
				return "";
			if(n == nIndex)
				return std::string((const char*)&msg.buf[i], nLen);
			i += nLen;
		}
		return "";
	}

	bool Read1004(const GameMsg& msg, unsigned short& usChannel, std::string& strWords)
	{
		if(msg.usType != 1004 || msg.buf.size() < 0x11)
			return false;
		usChannel = Get16(msg.buf, 8);
		size_t i = 0x10;
		int nCount = msg.buf[i++];
		for(int n = 0; n < nCount; n++)
		{
			if(i >= msg.buf.size())
				return false;
			size_t nLen = msg.buf[i++];
			if(i + nLen > msg.buf.size())
				return false;
			if(n == 3)
			{
				strWords.assign((const char*)&msg.buf[i], nLen);
				return true;
			}
			i += nLen;
		}
		return false;
	}

	class CScriptedClient
	{
	public:
		CScriptedClient() : m_sock(INVALID_SOCKET) {}
		~CScriptedClient() { Close(); }
		bool Connect(int nPort)
		{
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in from = {};
			from.sin_family = AF_INET;
			from.sin_addr.s_addr = inet_addr("127.0.0.2");
			if(bind(m_sock, (sockaddr*)&from, sizeof(from)) != 0)
				return false;
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			addr.sin_port = htons((u_short)nPort);
			return connect(m_sock, (sockaddr*)&addr, sizeof(addr)) == 0;
		}
		bool Send(const Bytes& plain)
		{
			Bytes b = plain;
			m_send.Transform(&b[0], (int)b.size());
			return send(m_sock, (const char*)&b[0], (int)b.size(), 0) == (int)b.size();
		}
		void Rekey(DWORD key) { m_send.Rekey(key); }
		bool Recv(GameMsg& msg, int nMilliSecs)
		{
			DWORD tEnd = ::GetTickCount() + nMilliSecs;
			while(true)
			{
				if(m_plain.size() >= 4)
				{
					unsigned short usSize = Get16(m_plain, 0);
					if(usSize < 4)
						return false;
					if(m_plain.size() >= usSize)
					{
						msg.buf.assign(m_plain.begin(), m_plain.begin() + usSize);
						msg.usType = Get16(m_plain, 2);
						m_plain.erase(m_plain.begin(), m_plain.begin() + usSize);
						if(msg.usType == 1012 && msg.buf.size() >= 0x20)
						{
							Bytes reply(msg.buf.begin(), msg.buf.end());
							Put32(reply, 8, ::GetTickCount() ^ Get32(reply, 4));
							Put32(reply, 0x1C, 0);
							Send(reply);
						}
						return true;
					}
				}
				long nLeft = (long)(tEnd - ::GetTickCount());
				if(nLeft <= 0 || m_sock == INVALID_SOCKET)
					return false;
				fd_set set;
				FD_ZERO(&set);
				FD_SET(m_sock, &set);
				timeval tv = { nLeft / 1000, (nLeft % 1000) * 1000 };
				if(select(0, &set, NULL, NULL, &tv) <= 0)
					return false;
				char buf[4096];
				int n = recv(m_sock, buf, sizeof(buf), 0);
				if(n <= 0)
				{
					m_bClosed = true;
					return false;
				}
				m_recv.Transform((unsigned char*)buf, n);
				m_plain.insert(m_plain.end(), buf, buf + n);
			}
		}
		bool IsClosed() const { return m_bClosed; }
		void Close()
		{
			if(m_sock != INVALID_SOCKET)
				closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
	private:
		SOCKET			m_sock;
		ClientCipher	m_send, m_recv;
		Bytes			m_plain;
		bool			m_bClosed = false;
	};

	struct Transcript
	{
		std::vector<std::string>	lines;
		void Add(const char* fmt, ...)
		{
			char buf[512];
			va_list args;
			va_start(args, fmt);
			_vsnprintf(buf, sizeof(buf) - 1, fmt, args);
			va_end(args);
			buf[sizeof(buf) - 1] = 0;
			lines.push_back(buf);
			Log("    %s", buf);
		}
	};

	template<class PRED>
	bool RecvUntil(CScriptedClient& client, GameMsg& msg, int nMilliSecs, PRED pred, int* pnSkipped = NULL)
	{
		DWORD tEnd = ::GetTickCount() + nMilliSecs;
		while(true)
		{
			long nLeft = (long)(tEnd - ::GetTickCount());
			if(nLeft <= 0 || !client.Recv(msg, nLeft))
				return false;
			if(pred(msg))
				return true;
			Log("      (in %u, %u bytes)", msg.usType, (unsigned)msg.buf.size());
			if(pnSkipped)
				(*pnSkipped)++;
		}
	}

	bool Is1004(const GameMsg& m, unsigned short usChannel, std::string* pWords = NULL)
	{
		unsigned short ch = 0;
		std::string words;
		if(!Read1004(m, ch, words) || ch != usChannel)
			return false;
		if(pWords)
			*pWords = words;
		return true;
	}

	const unsigned long	PLAYER_ACCOUNT_ID	= 20001;
	const char*	PLAYER_ACCOUNT		= "gateplayer";
	const char*	PLAYER_PASSWORD		= "gatepw";
	const char*	PLAYER_NAME			= "GateWalker";
	const unsigned short	PLAYER_LOOK			= 1003;
	const unsigned short	PLAYER_PROFESSION	= 10;
	const unsigned long	USER_ID_START		= 4983955;

	void ClientLogin(CAccountStandIn& standin, CScriptedClient& client, int nGamePort, unsigned long idAccount,
					unsigned long dwKey, unsigned long dwClientKey)
	{
		if(!standin.SendConnect(idAccount, dwKey, ""))
			Fail("the stand-in could not send its 1052");
		::Sleep(500);
		if(!client.Connect(nGamePort))
			Fail("the scripted client cannot connect to the game port");
		if(!client.Send(Msg1052(idAccount, dwClientKey)))
			Fail("the scripted client could not send its 1052");
		client.Rekey(dwClientKey ^ ((idAccount + dwClientKey) ^ 0x4321));
	}

	struct Player
	{
		CScriptedClient	client;
		unsigned long	idAccount, idUser, idMap;
		unsigned short	usX, usY;
		std::string		strName;
		Bytes			info1006;
	};

	unsigned long	g_dwNextKey = 0x13572468;

	std::string CreateRole(CAccountStandIn& standin, int nGamePort, unsigned long idAccount, const char* pszName,
							unsigned short usLook = PLAYER_LOOK, unsigned short usProfession = PLAYER_PROFESSION)
	{
		CScriptedClient client;
		unsigned long dwKey = g_dwNextKey++;
		ClientLogin(standin, client, nGamePort, idAccount, dwKey, dwKey);
		GameMsg msg;
		std::string words;
		if(!RecvUntil(client, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2101, &words); }))
			return "no 1004 on 2101 after the 1052";
		if(words != "NEW_ROLE")
			return "the new account was answered " + words;
		if(!client.Send(Msg1001("scenario", pszName, "scenariopw", usLook, usProfession, idAccount)))
			return "the 1001 could not be sent";
		if(!RecvUntil(client, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2100, &words); }))
			return "no 1004 on 2100 after the 1001";
		return words == "ANSWER_OK" ? "" : "the 1001 was answered " + words;
	}

	std::string EnterWorld(CAccountStandIn& standin, int nGamePort, unsigned long idAccount, Player& p)
	{
		p.idAccount = idAccount;
		unsigned long dwKey = g_dwNextKey++;
		ClientLogin(standin, p.client, nGamePort, idAccount, dwKey, dwKey);
		GameMsg msg;
		std::string words;
		if(!RecvUntil(p.client, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2101, &words); }))
			return "no 1004 on 2101 after the 1052";
		if(words != "ANSWER_OK")
			return "the login was answered " + words;
		if(!RecvUntil(p.client, msg, 10000, [](const GameMsg& m) { return m.usType == 1006; }))
			return "no 1006 after ANSWER_OK";
		p.info1006 = msg.buf;
		p.idUser = Get32(msg.buf, 4);
		const unsigned short steps[] = { 74, 75, 76, 77, 78, 97 };
		for(int i = 0; i < sizeof(steps)/sizeof(steps[0]); i++)
		{
			unsigned short a = steps[i];
			if(!p.client.Send(Msg1010(p.idUser, 0, 0, 0, 0, a)))
				return "a login step could not be sent";
			if(!RecvUntil(p.client, msg, 10000, [a](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == a; }))
				return "no 1010 reply to a login step";
			if(a == 74)
			{
				p.idMap = Get32(msg.buf, 8);
				p.usX = Get16(msg.buf, 0x10);
				p.usY = Get16(msg.buf, 0x12);
			}
		}
		if(!p.client.Send(Msg1010(p.idUser, 0, 0, 0, 0, 130)))
			return "the login's end (130) could not be sent";
		return "";
	}

	Transcript RunScriptedClient()
	{
		Transcript t;
		MYSQL* h = Connect(GATE_DB);
		char sql[256];
		sprintf(sql, "DELETE FROM cq_user WHERE account_id=%lu", PLAYER_ACCOUNT_ID);
		Exec(h, sql);
		sprintf(sql, "ALTER TABLE cq_user AUTO_INCREMENT=%lu", USER_ID_START);
		Exec(h, sql);

		CAccountStandIn standin;
		CMsgServerProcess server;
		int nGamePort = 0;
		RunResult r = StartGate(standin, server, nGamePort, SERVER_ID);
		if(!r.bLoginOK || !r.bGamePortOpen)
			Fail("MsgServer did not log into the stand-in or open its game port");

		GameMsg msg;
		std::string words;

		Log("  session 1: a new account");
		{
			CScriptedClient client;
			ClientLogin(standin, client, nGamePort, PLAYER_ACCOUNT_ID, 0x11223344, 0x11223344);
			if(!RecvUntil(client, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2101, &words); }))
				Fail("no 1004 on channel 2101 after the client's 1052");
			t.Add("1052 -> 1004 ch 2101 \"%s\"", words.c_str());
			if(words != "NEW_ROLE")
				Fail("the answer to a new account is not NEW_ROLE: " + words);

			if(!client.Send(Msg1001(PLAYER_ACCOUNT, PLAYER_NAME, PLAYER_PASSWORD, PLAYER_LOOK, PLAYER_PROFESSION, PLAYER_ACCOUNT_ID)))
				Fail("the scripted client could not send its 1001");
			if(!RecvUntil(client, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2100, &words); }))
				Fail("no 1004 on channel 2100 after the client's 1001");
			t.Add("1001 -> 1004 ch 2100 \"%s\"", words.c_str());
			if(words != "ANSWER_OK")
				Fail("the answer to the character creation is not ANSWER_OK: " + words);
		}
		sprintf(sql, "SELECT id, name, recordmap_id, recordx, recordy, profession FROM cq_user WHERE account_id=%lu", PLAYER_ACCOUNT_ID);
		std::vector<std::vector<std::string> > rows = Query(h, sql);
		mysql_close(h);
		if(rows.size() != 1)
			Fail("cq_user has no single row for the account after the creation");
		unsigned long idUser = strtoul(rows[0][0].c_str(), NULL, 10);
		unsigned long idMap = strtoul(rows[0][2].c_str(), NULL, 10);
		unsigned short usX = (unsigned short)atoi(rows[0][3].c_str());
		unsigned short usY = (unsigned short)atoi(rows[0][4].c_str());
		t.Add("cq_user: id %lu, name %s, map %lu (%u,%u), profession %s", idUser, rows[0][1].c_str(), idMap, usX, usY, rows[0][5].c_str());

		Log("  session 2: the role logs in");
		CScriptedClient client;
		ClientLogin(standin, client, nGamePort, PLAYER_ACCOUNT_ID, 0x55667788, 0x55667788);
		if(!RecvUntil(client, msg, 10000, [&](const GameMsg& m) { return Is1004(m, 2101, &words); }))
			Fail("no 1004 on channel 2101 after the second 1052");
		t.Add("1052 -> 1004 ch 2101 \"%s\"", words.c_str());
		if(words != "ANSWER_OK")
			Fail("the answer to a known account is not ANSWER_OK: " + words);
		if(!RecvUntil(client, msg, 10000, [](const GameMsg& m) { return m.usType == 1006; }))
			Fail("no 1006 after ANSWER_OK");
		unsigned long idHero = Get32(msg.buf, 4);
		t.Add("1006: %u bytes, id %lu", (unsigned)msg.buf.size(), idHero);
		if(idHero != idUser)
			Fail("the 1006's id (+4) is not the role's");

		const unsigned short steps[] = { 74, 75, 76, 77, 78, 97 };
		for(int i = 0; i < sizeof(steps)/sizeof(steps[0]); i++)
		{
			unsigned short a = steps[i];
			if(!client.Send(Msg1010(idHero, 0, 0, 0, 0, a)))
				Fail("the scripted client could not send a 1010");
			int nSkipped = 0;
			if(!RecvUntil(client, msg, 10000, [a](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == a; }, &nSkipped))
			{
				char sz[64];
				sprintf(sz, "no 1010 reply to action %u", a);
				Fail(sz);
			}
			if(a == 74)
			{
				t.Add("1010 74 -> 1010 74: map %lu, doc %lu, (%u,%u)", Get32(msg.buf, 8), Get32(msg.buf, 0x0C),
						Get16(msg.buf, 0x10), Get16(msg.buf, 0x12));
				if(Get32(msg.buf, 8) != idMap || Get16(msg.buf, 0x10) != usX || Get16(msg.buf, 0x12) != usY)
					Fail("the reply to 74 does not carry the role's map and position");
			}
			else
				t.Add("1010 %u -> 1010 %u: id %lu", a, a, Get32(msg.buf, 8));
		}
		if(!client.Send(Msg1010(idHero, 0, 0, 0, 0, 130)))
			Fail("the scripted client could not send 1010 action 130");
		t.Add("1010 130 sent");

		const unsigned char dirs[] = { 0, 0, 2, 4, 6 };
		for(int i = 0; i < sizeof(dirs)/sizeof(dirs[0]); i++)
		{
			if(!client.Send(Msg1005(idHero, dirs[i], 0)))
				Fail("the scripted client could not send a 1005");
			if(!RecvUntil(client, msg, 10000, [](const GameMsg& m) { return m.usType == 1005; }))
				Fail("no 1005 back for a step");
			t.Add("1005 dir %u -> 1005 %u bytes: id %lu, dir %u, mode %u", dirs[i], (unsigned)msg.buf.size(),
					Get32(msg.buf, 4), msg.buf[8], msg.buf[9]);
			if(Get32(msg.buf, 4) != idHero || msg.buf[8] != dirs[i])
				Fail("the walk's echo is not the step sent");
		}

		::Sleep(1000);
		while(client.Recv(msg, 200))
			Log("      (in %u, %u bytes)", msg.usType, (unsigned)msg.buf.size());
		t.Add("after the walk: the connection %s, the server %s", client.IsClosed() ? "closed" : "open",
				server.HasExited() ? "exited" : "running");
		if(client.IsClosed() || server.HasExited())
			Fail("the server closed the connection or exited after the walk (see " + LogPath() + ")");
		client.Close();
		server.Kill();
		return t;
	}

	const char*	LINK_ACCOUNT	= "NpcServer Tqdig";

	class CLinkRelay
	{
	public:
		CLinkRelay() : m_sListen(INVALID_SOCKET), m_sNpc(INVALID_SOCKET), m_sMsg(INVALID_SOCKET), m_nPort(0), m_hThread(NULL), m_bStop(false), m_bHoldUp(false)
		{ ::InitializeCriticalSection(&m_cs); }
		~CLinkRelay() { Stop(); ::DeleteCriticalSection(&m_cs); }
		void Listen()
		{
			m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			if(bind(m_sListen, (sockaddr*)&addr, sizeof(addr)) != 0 || listen(m_sListen, 1) != 0)
				Fail("the relay cannot listen");
			int len = sizeof(addr);
			getsockname(m_sListen, (sockaddr*)&addr, &len);
			m_nPort = ntohs(addr.sin_port);
		}
		int Port() const { return m_nPort; }
		bool Accept(int nSecs, int nGamePort)
		{
			fd_set set; FD_ZERO(&set); FD_SET(m_sListen, &set);
			timeval tv = { nSecs, 0 };
			if(select(0, &set, NULL, NULL, &tv) != 1)
				return false;
			m_sNpc = accept(m_sListen, NULL, NULL);
			m_sMsg = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			addr.sin_port = htons((u_short)nGamePort);
			if(connect(m_sMsg, (sockaddr*)&addr, sizeof(addr)) != 0)
				return false;
			m_hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
			return m_hThread != NULL;
		}
		void Snapshot(Bytes& up, Bytes& down, bool& bClosed)
		{
			::EnterCriticalSection(&m_cs);
			up = m_up; down = m_down; bClosed = m_bClosed;
			::LeaveCriticalSection(&m_cs);
		}
		void HoldUp() { m_bHoldUp = true; }
		void Stop()
		{
			m_bStop = true;
			if(m_hThread) { ::WaitForSingleObject(m_hThread, 5000); ::CloseHandle(m_hThread); m_hThread = NULL; }
			SOCKET* s[] = { &m_sListen, &m_sNpc, &m_sMsg };
			for(int i = 0; i < 3; i++)
				if(*s[i] != INVALID_SOCKET) { closesocket(*s[i]); *s[i] = INVALID_SOCKET; }
		}
	private:
		static DWORD WINAPI ThreadProc(LPVOID p) { ((CLinkRelay*)p)->Run(); return 0; }
		void Run()
		{
			char buf[8192];
			while(!m_bStop)
			{
				fd_set set; FD_ZERO(&set); if(!m_bHoldUp) FD_SET(m_sNpc, &set); FD_SET(m_sMsg, &set);
				timeval tv = { 0, 100000 };
				if(select(0, &set, NULL, NULL, &tv) <= 0)
					continue;
				SOCKET pairs[2][2] = { { m_sNpc, m_sMsg }, { m_sMsg, m_sNpc } };
				for(int i = 0; i < 2; i++)
				{
					if(!FD_ISSET(pairs[i][0], &set))
						continue;
					int n = recv(pairs[i][0], buf, sizeof(buf), 0);
					if(n <= 0)
					{
						::EnterCriticalSection(&m_cs); m_bClosed = true; ::LeaveCriticalSection(&m_cs);
						return;
					}
					::EnterCriticalSection(&m_cs);
					Bytes& v = i == 0 ? m_up : m_down;
					v.insert(v.end(), buf, buf + n);
					::LeaveCriticalSection(&m_cs);
					for(int sent = 0; sent < n; )
					{
						int k = send(pairs[i][1], buf + sent, n - sent, 0);
						if(k <= 0)
							return;
						sent += k;
					}
				}
			}
		}
		SOCKET				m_sListen, m_sNpc, m_sMsg;
		int					m_nPort;
		HANDLE				m_hThread;
		volatile bool		m_bStop, m_bHoldUp;
		CRITICAL_SECTION	m_cs;
		Bytes				m_up, m_down;
		bool				m_bClosed = false;
	};

	struct LinkFrame
	{
		unsigned short	usSize, usType;
		unsigned long	idNpc;
		Bytes			body;
	};

	bool SplitLinkFrames(const Bytes& stream, std::vector<LinkFrame>& frames)
	{
		frames.clear();
		size_t i = 0;
		while(i + 8 <= stream.size())
		{
			LinkFrame f;
			f.usSize = Get16(stream, i);
			f.usType = Get16(stream, i + 2);
			f.idNpc = Get32(stream, i + 4);
			if(f.usSize < 8)
				return false;
			if(i + f.usSize > stream.size())
				break;
			f.body.assign(stream.begin() + i + 8, stream.begin() + i + f.usSize);
			frames.push_back(f);
			i += f.usSize;
		}
		return true;
	}

	std::string LinkTalkWords(const LinkFrame& f)
	{
		size_t i = 0x0C;
		if(f.usType != 1004 || f.body.size() <= i)
			return "";
		int nCount = f.body[i++];
		for(int n = 0; n < nCount && i < f.body.size(); n++)
		{
			size_t nLen = f.body[i++];
			if(i + nLen > f.body.size())
				return "";
			if(n == 3)
				return std::string((const char*)&f.body[i], nLen);
			i += nLen;
		}
		return "";
	}

	void WriteNpcConfig(int nPort)
	{
		char buf[1024];
		sprintf(buf,
			"; SYNTHETIC (msggate_tests.cpp, M6; npclink_tests.cpp's keys)\r\n"
			"[GameServer]\r\nGAMESERVER_IP=127.0.0.1\r\nGAMESERVER_PORT=%d\r\nSND_SIZE=65536\r\nRCV_SIZE=65536\r\n"
			"MaxNpcPerTimer=20\r\n"
			"[Database]\r\nDBServer=127.0.0.1\r\nLoginName=root\r\nPassword=\r\nDBName=%s\r\nDBPort=3306\r\n"
			"[InternetPort]\r\nPORT_SIZE=0\r\nCURRENT_PORTID=0\r\n"
			"[System]\r\nSERVERNAME=%s\r\n", nPort, GATE_DB, SERVER_NAME);
		WriteText(g_strRun + "\\config.ini", buf);
	}

	struct NpcLinkResult
	{
		std::vector<LinkFrame>	up, down;
		bool	bLoginOK;
		bool	bFramed;
	};

	NpcLinkResult RunNpcLink(int nSecs)
	{
		NpcLinkResult r = {};
		CAccountStandIn standin;
		CMsgServerProcess server;
		int nGamePort = 0;
		std::string log = LogPath();
		size_t nLog0 = ReadFile(log).size();
		RunResult g = StartGate(standin, server, nGamePort, SERVER_ID);
		if(!g.bLoginOK || !g.bGamePortOpen)
			Fail("MsgServer did not log into the stand-in or open its game port");

		CLinkRelay relay;
		relay.Listen();
		WriteNpcConfig(relay.Port());
		CMsgServerProcess npc;
		npc.Start("NpcServer.exe");
		if(!relay.Accept(180, nGamePort))
			Fail("NpcServer did not connect to the relay within 180 s (its syslog in db\\msggate\\syslog)");
		Log("  NpcServer connected through the relay to the game port %d", nGamePort);
		DWORD tEnd = ::GetTickCount() + nSecs * 1000;
		Bytes up, down;
		bool bClosed = false;
		while((long)(tEnd - ::GetTickCount()) > 0 && !bClosed && !npc.HasExited() && !server.HasExited())
		{
			CAccountStandIn::Message m;
			while(standin.Recv(m, 0))
				;
			::Sleep(500);
			relay.Snapshot(up, down, bClosed);
		}
		relay.HoldUp();
		::Sleep(3000);
		relay.Snapshot(up, down, bClosed);
		std::string s = ReadFile(log);
		s = s.size() > nLog0 ? s.substr(nLog0) : "";
		r.bLoginOK = s.find("SHELL: NPC server login OK.") != std::string::npos;
		r.bFramed = SplitLinkFrames(up, r.up) && SplitLinkFrames(down, r.down);
		Log("  %u bytes up in %u frames, %u bytes down in %u frames; the relay %s, NpcServer %s, MsgServer %s",
			(unsigned)up.size(), (unsigned)r.up.size(), (unsigned)down.size(), (unsigned)r.down.size(), bClosed ? "closed" : "open",
			npc.HasExited() ? "exited" : "running", server.HasExited() ? "exited" : "running");
		npc.Kill();
		relay.Stop();
		server.Kill();
		return r;
	}

	const unsigned long	LINK_VERSION_RELOGIN	= 0x3ADE68B2;

	std::string CheckNpcLink(const NpcLinkResult& r, size_t* pnAnswered)
	{
		*pnAnswered = 0;
		if(!r.bFramed) return "a stream does not split into NPC frames";
		if(r.up.empty() || r.up[0].usType != 1002 || r.up[0].body.size() < 0x20) return "NpcServer's first frame is not its 1002";
		if(strncmp((const char*)&r.up[0].body[0], LINK_ACCOUNT, 16) != 0) return "the 1002's account is not NPCSERVER_ACCOUNT";
		if(r.down.empty()) return "MsgServer sent nothing";
		const LinkFrame& a = r.down[0];
		if(a.usType != 1004 || a.idNpc != 0) return "MsgServer's first frame is not a 1004 to NPC id 0";
		if(Get16(a.body, 4) != 2101) return "the 1004's attribute (+4) is not 2101";
		if(LinkTalkWords(a) != "ANSWER_OK") return "the 1004's words are not ANSWER_OK";
		std::vector<const LinkFrame*> reqs;
		for(size_t i = 1; i < r.up.size(); i++)
		{
			if(r.up[i].usType != 20001) return "NpcServer sent a frame other than 20001 after its login";
			if(r.up[i].body.size() != 0x20) return "a 20001 request is not 20h bytes";
			reqs.push_back(&r.up[i]);
		}
		if(reqs.empty()) return "no 20001 request reached MsgServer";
		if(r.down.size() - 1 != reqs.size()) return "MsgServer's 20001 answers are not one per request";
		for(size_t i = 0; i < reqs.size(); i++)
		{
			const LinkFrame& q = *reqs[i];
			const LinkFrame& d = r.down[i + 1];
			Bytes want = q.body;
			Put32(want, 0x0C, 0);
			want[0x1C] = 0;
			Put16(want, 0x18, 1);
			char sz[96];
			sprintf(sz, "answer %u (to the monster %lu): ", (unsigned)i, Get32(q.body, 0));
			if(d.usType != 20001) return std::string(sz) + "not a 20001";
			if(d.idNpc != Get32(q.body, 0)) return std::string(sz) + "not sent to the monster's id";
			if(d.body != want) return std::string(sz) + "not the request's body";
			(*pnAnswered)++;
		}
		return "";
	}

	std::vector<Bytes> NpcLinkTranscript(const NpcLinkResult& r)
	{
		std::vector<Bytes> t;
		if(!r.up.empty()) t.push_back(r.up[0].body);
		if(!r.down.empty()) { Bytes b = r.down[0].body; if(b.size() >= 0x0C) Put32(b, 8, 0); t.push_back(b); }
		for(size_t i = 1; i < r.up.size() && i <= 20; i++)
			t.push_back(r.up[i].body);
		return t;
	}

	struct WinsockGuard
	{
		WinsockGuard()	{ WSADATA d; WSAStartup(MAKEWORD(2, 2), &d); }
		~WinsockGuard()	{ WSACleanup(); }
	};
	WinsockGuard*	g_pWinsock = NULL;
}

#endif
