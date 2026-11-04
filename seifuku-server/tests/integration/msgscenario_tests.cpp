#include "msggate_env.h"
#include <functional>
#include <algorithm>

namespace
{
	CAccountStandIn*	g_pStandin	= NULL;
	CMsgServerProcess*	g_pServer	= NULL;
	int					g_nPort		= 0;
	unsigned long		g_idNextAccount	= 30001;
	int					g_nPlayers	= 0;

	void Drain(Player& p, int nMs = 300)
	{
		GameMsg m;
		while(p.client.Recv(m, nMs))
			;
	}

	void NewPlayer(Player& p, const char* pszPrefix, const char* pszSet = NULL,
					std::function<void(unsigned long idUser)> prepare = nullptr)
	{
		char szName[16];
		sprintf(szName, "%s%d", pszPrefix, ++g_nPlayers);
		unsigned long idAccount = g_idNextAccount++;
		std::string err = CreateRole(*g_pStandin, g_nPort, idAccount, szName);
		if(!err.empty())
			Fail(std::string(szName) + ": the creation: " + err);
		if(pszSet || prepare)
		{
			MYSQL* h = Connect(GATE_DB);
			if(pszSet)
				Exec(h, std::string("UPDATE cq_user SET ") + pszSet + " WHERE account_id=" + std::to_string(idAccount));
			std::vector<std::vector<std::string> > rows = Query(h, "SELECT id FROM cq_user WHERE account_id=" + std::to_string(idAccount));
			mysql_close(h);
			if(prepare && !rows.empty())
				prepare(strtoul(rows[0][0].c_str(), NULL, 10));
		}
		p.strName = szName;
		err = EnterWorld(*g_pStandin, g_nPort, idAccount, p);
		if(!err.empty())
			Fail(std::string(szName) + ": the login: " + err);
		Drain(p);
		Log("  %s (account %lu, role %lu) on map %lu at (%u,%u)", szName, idAccount, p.idUser, p.idMap, p.usX, p.usY);
	}

	template<class PRED>
	bool Expect(Player& p, GameMsg& msg, PRED pred, int nMs = 5000)
	{
		return RecvUntil(p.client, msg, nMs, pred);
	}

	bool ExpectTalk(Player& p, unsigned short usChannel, std::string& words, int nMs = 5000)
	{
		GameMsg msg;
		return Expect(p, msg, [&](const GameMsg& m) { return Is1004(m, usChannel, &words); }, nMs);
	}

	template<class PRED>
	bool Comes(Player& p, PRED pred, int nMs = 1500)
	{
		GameMsg msg;
		return RecvUntil(p.client, msg, nMs, pred);
	}

	const unsigned short	TXTATR_SYSTEM	= 2005;
	const unsigned long		ATTRIB_LIFE		= 0;
	const unsigned long		ATTRIB_MONEY	= 4;

	bool ExpectAttrib(Player& p, unsigned long dwType, unsigned long& dwValue, int nMs = 5000)
	{
		GameMsg msg;
		auto has = [dwType](const GameMsg& m)
		{
			if(m.usType != 1017)
				return false;
			unsigned long n = Get32(m.buf, 8);
			for(unsigned long i = 0; i < n && 0x0C + i * 8 + 8 <= m.buf.size(); i++)
				if(Get32(m.buf, 0x0C + i * 8) == dwType)
					return true;
			return false;
		};
		if(!Expect(p, msg, has, nMs))
			return false;
		unsigned long n = Get32(msg.buf, 8);
		for(unsigned long i = 0; i < n; i++)
			if(Get32(msg.buf, 0x0C + i * 8) == dwType)
				dwValue = Get32(msg.buf, 0x0C + i * 8 + 4);
		return true;
	}

	int CountRows(const char* pszTable, const std::string& where)
	{
		MYSQL* h = Connect(GATE_DB);
		std::vector<std::vector<std::string> > rows = Query(h, std::string("SELECT COUNT(*) FROM ") + pszTable + " WHERE " + where);
		mysql_close(h);
		return rows.empty() ? -1 : atoi(rows[0][0].c_str());
	}

	unsigned long GiveItem(unsigned long idUser, unsigned long idType, int nAmount, int nAmountLimit, int nPosition = 0,
							bool bGoodSum = true, int nGem1 = 0, int nGem2 = 0, int nMagic3 = 0)
	{
		MYSQL* h = Connect(GATE_DB);
		char sql[256];
		sprintf(sql, "INSERT cq_item SET type=%lu, owner_id=%lu, player_id=%lu, amount=%d, amount_limit=%d, position=%d, gem1=%d, gem2=%d, magic3=%d",
				idType, idUser, idUser, nAmount, nAmountLimit, nPosition, nGem1, nGem2, nMagic3);
		Exec(h, sql);
		unsigned long id = (unsigned long)mysql_insert_id(h);
		unsigned long dwSum = (nGem1 + nGem2 + 1) * 9527 + idType + nMagic3;
		dwSum = dwSum ^ (id + 0xB00CF) ^ idUser;
		if(!bGoodSum)
			dwSum ^= 1;
		sprintf(sql, "UPDATE cq_item SET chk_sum=%lu WHERE id=%lu", dwSum, id);
		Exec(h, sql);
		mysql_close(h);
		return id;
	}

	unsigned long BuyPotion(Player& p)
	{
		if(!p.client.Send(Msg1009(3, 1000000, 1, p.idUser)))
			Fail("the purchase could not be sent");
		GameMsg msg;
		if(!Expect(p, msg, [](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 8) == 1000000; }))
			Fail("no 1008 for the potion bought");
		Drain(p);
		return Get32(msg.buf, 4);
	}

	class CNpcLinkStandIn
	{
	public:
		CNpcLinkStandIn() : m_sock(INVALID_SOCKET), m_hThread(NULL), m_bStop(false) { ::InitializeCriticalSection(&m_cs); }
		~CNpcLinkStandIn() { Close(); ::DeleteCriticalSection(&m_cs); }
		void Login(int nGamePort)
		{
			m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			addr.sin_port = htons((u_short)nGamePort);
			if(connect(m_sock, (sockaddr*)&addr, sizeof(addr)) != 0)
				Fail("the NPC link stand-in cannot connect to the game port");
			m_hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
			Bytes body(32, 0);
			memcpy(&body[0], LINK_ACCOUNT, strlen(LINK_ACCOUNT));
			memcpy(&body[16], "skl8934##VJICW ", 15);
			Send(1002, 987654321, body);
			if(!Wait([](const LinkFrame& f) { return f.usType == 1004 && LinkTalkWords(f) == "ANSWER_OK"; }))
				Fail("MsgServer did not answer the NPC link's login with ANSWER_OK");
		}
		void CreateMonster(unsigned long id, unsigned short usType, unsigned long idMap, unsigned short x, unsigned short y)
		{
			Bytes body(0x20, 0);
			Put32(body, 0, id);
			Put32(body, 4, idMap);
			Put16(body, 0x14, x);
			Put16(body, 0x16, y);
			Put16(body, 0x18, 1);
			Put16(body, 0x1A, usType);
			Send(20001, id, body);
			if(!Wait([id](const LinkFrame& f) { return f.usType == 20001 && f.idNpc == id; }))
				Fail("MsgServer did not create the monster " + std::to_string(id));
		}
		void Close()
		{
			m_bStop = true;
			if(m_sock != INVALID_SOCKET) { shutdown(m_sock, SD_BOTH); closesocket(m_sock); m_sock = INVALID_SOCKET; }
			if(m_hThread) { ::WaitForSingleObject(m_hThread, 5000); ::CloseHandle(m_hThread); m_hThread = NULL; }
		}
	private:
		void Send(unsigned short usType, unsigned long idNpc, const Bytes& body)
		{
			Bytes b(8 + body.size(), 0);
			Put16(b, 0, (unsigned short)b.size());
			Put16(b, 2, usType);
			Put32(b, 4, idNpc);
			memcpy(&b[8], &body[0], body.size());
			if(send(m_sock, (const char*)&b[0], (int)b.size(), 0) != (int)b.size())
				Fail("the NPC link stand-in's send failed");
		}
		template<class PRED>
		bool Wait(PRED pred)
		{
			for(DWORD t0 = ::GetTickCount(); ::GetTickCount() - t0 < 10000; ::Sleep(50))
			{
				::EnterCriticalSection(&m_cs);
				std::vector<LinkFrame> frames;
				bool bFramed = SplitLinkFrames(m_stream, frames);
				bool bFound = false;
				for(size_t i = m_nTaken; bFramed && i < frames.size() && !bFound; i++)
					if(pred(frames[i]))
						{ bFound = true; m_nTaken = i + 1; }
				::LeaveCriticalSection(&m_cs);
				if(bFound)
					return true;
			}
			return false;
		}
		static DWORD WINAPI ThreadProc(LPVOID p) { ((CNpcLinkStandIn*)p)->Run(); return 0; }
		void Run()
		{
			char buf[8192];
			while(!m_bStop)
			{
				int n = recv(m_sock, buf, sizeof(buf), 0);
				if(n <= 0)
					return;
				::EnterCriticalSection(&m_cs);
				m_stream.insert(m_stream.end(), buf, buf + n);
				::LeaveCriticalSection(&m_cs);
			}
		}
		SOCKET				m_sock;
		HANDLE				m_hThread;
		volatile bool		m_bStop;
		CRITICAL_SECTION	m_cs;
		Bytes				m_stream;
		size_t				m_nTaken = 0;
	};

	template<class BODY>
	void Twice(BODY body)
	{
		for(int nRun = 1; nRun <= 2; nRun++)
		{
			Log("run %d:", nRun);
			body();
		}
	}
}

TEST_CLASS(MsgScenarioTests)
{
public:
	TEST_CLASS_INITIALIZE(Setup)
	{
		g_pWinsock = new WinsockGuard;
		FindPaths();
		BuildGateDatabase();
		BuildRunDirectory();
		{
			MYSQL* h = Connect(GATE_DB);
			Exec(h, "INSERT cq_item_value_type SET type=5, data1=1088000, value=2000");
			mysql_close(h);
		}
		{
			MYSQL* h = Connect(GATE_DB);
			Exec(h, "INSERT cq_action SET id=990002, id_next=0, id_nextfail=0, type=1003, data=0, param='1036 187 263'");
			Exec(h, "INSERT cq_task SET id=990002, id_next=990002, id_nextfail=0, itemname1='', itemname2='', sex=999, min_pk=-100000, max_pk=100000, team=999");
			Exec(h, "INSERT cq_npc SET id=9991, name='SfkDoor2', type=2, lookface=391, idxserver=-1, mapid=1002, cellx=427, celly=383, task0=990002");
			Exec(h, "INSERT cq_action SET id=990003, id_next=990004, id_nextfail=0, type=510, data=0, param='1 level == 9'");
			Exec(h, "INSERT cq_action SET id=990004, id_next=0, id_nextfail=0, type=512, data=0, param='1 5'");
			Exec(h, "INSERT cq_task SET id=990003, id_next=990003, id_nextfail=0, itemname1='', itemname2='', sex=999, min_pk=-100000, max_pk=100000, team=999");
			Exec(h, "INSERT cq_npc SET id=9992, name='SfkDyer', type=2, lookface=391, idxserver=-1, mapid=1002, cellx=430, celly=383, task0=990003");
			Exec(h, "INSERT cq_action SET id=990005, id_next=0, id_nextfail=0, type=1045, data=0, param='1000'");
			Exec(h, "INSERT cq_task SET id=990005, id_next=990005, id_nextfail=0, itemname1='', itemname2='', sex=999, min_pk=-100000, max_pk=100000, team=999");
			Exec(h, "INSERT cq_npc SET id=9993, name='SfkReset', type=2, lookface=391, idxserver=-1, mapid=1002, cellx=431, celly=377, task0=990005");
			Exec(h, "INSERT cq_action SET id=990006, id_next=0, id_nextfail=990099, type=1001, data=0, param='level'");
			Exec(h, "INSERT cq_task SET id=990006, id_next=990006, id_nextfail=0, itemname1='', itemname2='', sex=999, min_pk=-100000, max_pk=100000, team=999");
			Exec(h, "INSERT cq_npc SET id=9994, name='SfkFault', type=2, lookface=391, idxserver=-1, mapid=1002, cellx=436, celly=377, task0=990006");
			Exec(h, "INSERT cq_task SET id=990007, id_next=5050104, id_nextfail=0, itemname1='', itemname2='', sex=999, min_pk=-100000, max_pk=100000, team=999");
			Exec(h, "INSERT cq_npc SET id=9995, name='SfkLotto', type=2, lookface=391, idxserver=-1, mapid=1002, cellx=429, celly=377, task0=990007");
			Exec(h, "CREATE TEMPORARY TABLE sfk_mst SELECT * FROM cq_monstertype WHERE id=2033");
			Exec(h, "UPDATE sfk_mst SET id=2998, extra_exp=0");
			Exec(h, "INSERT cq_monstertype SELECT * FROM sfk_mst");
			mysql_close(h);
		}
		g_pStandin = new CAccountStandIn;
		g_pServer = new CMsgServerProcess;
		RunResult r = StartGate(*g_pStandin, *g_pServer, g_nPort, SERVER_ID);
		if(!r.bLoginOK || !r.bGamePortOpen)
			Fail("MsgServer did not log into the stand-in or open its game port");
		Log("scenarios: MsgServer on the game port %d; cq_msggate: %d tables copied from cq_merged, %d item types",
			g_nPort, g_nCopied, g_nItemTypes);
	}

	TEST_CLASS_CLEANUP(Teardown)
	{
		if(g_pServer)
		{
			bool bExited = g_pServer->HasExited();
			g_pServer->Kill();
			delete g_pServer;
			g_pServer = NULL;
			if(bExited)
				Log("MsgServer had exited before the end of the scenarios (see %s)", LogPath().c_str());
		}
		delete g_pStandin;
		g_pStandin = NULL;
		delete g_pWinsock;
		g_pWinsock = NULL;
	}

	TEST_METHOD_CLEANUP(EachScenario)
	{
		Assert::IsFalse(g_pServer->HasExited(), L"MsgServer exited during the scenario");
	}

	TEST_METHOD(S1010_081_Emotion)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Emo");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 1, p.usX, p.usY, 0, 81)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 81; }),
				L"no 1010 81 back for emotion 1");
			Assert::AreEqual((int)p.idUser, (int)Get32(msg.buf, 8));
			Assert::AreEqual(1, (int)Get32(msg.buf, 0x0C));
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 9, p.usX, p.usY, 0, 81)));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 81; }),
				L"emotion 9, outside 2008's list, was broadcast");
		});
	}

	TEST_METHOD(S1010_096_SetPkMode)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Pk");
			const char* texts[] = { "STR_FREE_PK_MODE", "STR_SAFE_PK_MODE", "STR_TEAM_PK_MODE", "STR_ARRESTMENT_PK_MODE" };
			for(unsigned long nMode = 0; nMode < 4; nMode++)
			{
				Assert::IsTrue(p.client.Send(Msg1010(p.idUser, nMode, 0, 0, 0, 96)));
				GameMsg msg;
				Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 96; }),
					L"no 1010 96 echoed");
				Assert::AreEqual((int)nMode, (int)Get32(msg.buf, 0x0C));
				std::string words;
				Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no system text after the PK mode");
				Assert::AreEqual(std::string(texts[nMode]), words);
			}
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 4, 0, 0, 0, 96)));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 96; }),
				L"mode 4 (no syndicate mode in 2008) was answered");
		});
	}

	TEST_METHOD(S1010_099_MineOnNoMineField)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Mine");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, 0, 0, 0, 99)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no system text after mining");
			Assert::AreEqual(std::string("STR_NO_MINE"), words);
		});
	}

	TEST_METHOD(S1010_079_ChgDir)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Dir");
			NewPlayer(b, "Dir");
			Drain(a);
			Assert::IsTrue(a.client.Send(Msg1010(a.idUser, 0, a.usX, a.usY, 3, 79)));
			GameMsg msg;
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 79 && Get32(m.buf, 8) == a.idUser; }),
				L"the second player did not see the turn");
			Assert::AreEqual(3, (int)Get16(msg.buf, 0x14));
			Assert::IsFalse(Comes(a, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 79; }),
				L"the turn came back to its sender");
		});
	}

	TEST_METHOD(S1010_090_230_231_Ignored)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Ign");
			const unsigned short actions[] = { 90, 230, 231 };
			for(int i = 0; i < 3; i++)
			{
				unsigned short a = actions[i];
				Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, a)));
				Assert::IsFalse(Comes(p, [a](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == a; }),
					L"an ignored action was answered");
			}
			Assert::IsFalse(p.client.IsClosed(), L"the connection was closed");
		});
	}

	TEST_METHOD(S1010_102_QueryPlayer)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Qry");
			NewPlayer(b, "Qry");
			Drain(a);
			Assert::IsTrue(b.client.Send(Msg1010(b.idUser, a.idUser, b.usX, b.usY, 0, 102)));
			Assert::IsFalse(Comes(b, [&](const GameMsg& m) { return m.usType == 1014 && Get32(m.buf, 4) == a.idUser; }),
				L"a query on the newbie map was answered");

			const char* pszMap = "recordmap_id=1002, recordx=430, recordy=380";
			Player c, d;
			NewPlayer(c, "Qry", pszMap);
			NewPlayer(d, "Qry", pszMap);
			Drain(c);
			Drain(d);
			Assert::IsTrue(d.client.Send(Msg1010(d.idUser, c.idUser, d.usX, d.usY, 0, 102)));
			GameMsg msg;
			Assert::IsTrue(Expect(d, msg, [&](const GameMsg& m) { return m.usType == 1014 && Get32(m.buf, 4) == c.idUser; }),
				L"the query on map 1002 did not show the target");
		});
	}

	TEST_METHOD(S1010_117_QueryEquipment)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Eqp");
			NewPlayer(b, "Eqp");
			Drain(a);
			Assert::IsTrue(b.client.Send(Msg1010(b.idUser, a.idUser, b.usX, b.usY, 0, 117)));
			std::string words;
			Assert::IsTrue(ExpectTalk(a, TXTATR_SYSTEM, words), L"the target was not told");
			Assert::AreEqual(std::string("STR_QUERYEQUIPMENT_HINT"), words);
		});
	}

	TEST_METHOD(S1010_133_Jump)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Jmp");
			unsigned long dwTarget = (unsigned long)(p.usX + 3) | ((unsigned long)p.usY << 16);
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, dwTarget, p.usX, p.usY, 0, 133)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 133; }),
				L"no 1010 133 back for a jump of 3 cells");
			Assert::AreEqual((int)dwTarget, (int)Get32(msg.buf, 0x0C));
			unsigned short usX = (unsigned short)(p.usX + 3);
			dwTarget = (unsigned long)(usX + 19) | ((unsigned long)p.usY << 16);
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, dwTarget, usX, p.usY, 0, 133)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text after a jump of 19 cells");
			Assert::AreEqual(std::string("STR_INVALID_MSG"), words);
			while(p.client.Recv(msg, 3000))
				;
			Assert::IsTrue(p.client.IsClosed(), L"the connection was not closed after a jump of 19 cells");
		});
	}

	TEST_METHOD(S1010_141_QueryLeaveWord)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Lw");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, 141)));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1004 && Read1004Str(m, 1) == p.strName; }), L"a player with no words was answered");
		});
	}

	TEST_METHOD(S1010_142_ChangeFace)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Face");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 7, p.usX, p.usY, 0, 142)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a face without the money");
			Assert::AreEqual(std::string("STR_NOT_SO_MUCH_MONEY"), words);

			Player q;
			NewPlayer(q, "Face", "money=1000");
			Assert::IsTrue(q.client.Send(Msg1010(q.idUser, 7, q.usX, q.usY, 0, 142)));
			GameMsg msg;
			Assert::IsTrue(Expect(q, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 142; }),
				L"no 1010 142 back for face 7");
			Assert::AreEqual(7, (int)Get32(msg.buf, 0x0C));
			Assert::IsTrue(q.client.Send(Msg1010(q.idUser, 401, q.usX, q.usY, 0, 142)));
			Assert::IsFalse(Comes(q, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 142; }),
				L"face 401 was taken on an English server");
		});
	}

	TEST_METHOD(S1010_085_ChgMapWithoutPassway)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Port");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, 85)));
			GameMsg msg;
			while(p.client.Recv(msg, 3000))
				;
			Assert::IsTrue(p.client.IsClosed(), L"a portal without a passway did not close the connection");
		});
	}

	TEST_METHOD(S1010_093_XpClear)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Xp");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, 93)));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return (m.usType == 1010 && Get32(m.buf, 8) == p.idUser) || (m.usType == 1004 && Read1004Str(m, 1) == p.strName); }),
				L"the ordinary XP clear was answered");
			Assert::IsTrue(p.client.Send(Msg1010(809834, 0, p.usX, p.usY, 5, 93)));
			SYSTEMTIME t;
			::GetLocalTime(&t);
			char szLog[64];
			sprintf(szLog, "\\gmlog\\cheater %d-%d-%d.log", t.wYear, t.wMonth, t.wDay);
			std::string line = std::string("client cheater found: name=") + p.strName + ", type=5";
			bool bLogged = false;
			std::string text;
			for(int i = 0; i < 25 && !bLogged; i++, ::Sleep(200))
			{
				text = ReadFile(g_strRun + szLog);
				bLogged = text.find(line) != std::string::npos;
			}
			if(!bLogged)
				Log("  %s: %u bytes, the line sought \"%s\"; its end: %s", (g_strRun + szLog).c_str(), (unsigned)text.size(),
					line.c_str(), text.substr(text.size() > 300 ? text.size() - 300 : 0).c_str());
			Assert::IsTrue(bLogged, L"the cheat report is not in gmlog\\cheater");
		});
	}

	TEST_METHOD(S1010_100_VerifyCode)
	{
		Twice([&]()
		{
			Player g, a, b;
			NewPlayer(g, "Vg", "name=CONCAT('[GM]',name)");
			g.strName = "[GM]" + g.strName;
			NewPlayer(a, "Va");
			NewPlayer(b, "Vb");
			Drain(a);
			Drain(b);
			SYSTEMTIME t;
			::GetLocalTime(&t);
			char szLog[64];
			sprintf(szLog, "\\gmlog\\cheater %d-%d-%d.log", t.wYear, t.wMonth, t.wDay);
			size_t nFrom = ReadFile(g_strRun + szLog).size();
			Assert::IsTrue(g.client.Send(Msg1004(2000, g.strName.c_str(), "ALLUSERS", "/client")));
			unsigned long dwDataA = 0, dwDataB = 0;
			GameMsg msg;
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 100 && Get32(m.buf, 8) == a.idUser; }),
				L"no verify code for the first player");
			dwDataA = Get32(msg.buf, 0x0C);
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 100 && Get32(m.buf, 8) == b.idUser; }),
				L"no verify code for the second player");
			dwDataB = Get32(msg.buf, 0x0C);

			auto answer = [](unsigned long idUser, unsigned long dwData) -> unsigned long
			{
				unsigned long table[256];
				srand(idUser);
				for(int i = 0; i < 256; i++)
					table[i] = rand() % 0xFFFF;
				return (::GetTickCount() & 0xFFFF0000) + table[dwData & 0xFF];
			};
			auto wait = [](unsigned long idUser) -> int { switch(idUser % 10) { case 2: return 2000; case 5: return 5000; case 8: return 8000; } return 0; };
			::Sleep((wait(a.idUser) > wait(b.idUser) ? wait(a.idUser) : wait(b.idUser)) + 300);
			Assert::IsTrue(a.client.Send(Msg1010(a.idUser, answer(a.idUser, dwDataA), a.usX, a.usY, 0, 100)));
			Assert::IsTrue(b.client.Send(Msg1010(b.idUser, answer(b.idUser, dwDataB) + 1, b.usX, b.usY, 0, 100)));

			std::string lineB = "User:[" + b.strName + "][" + std::to_string(b.idUser) + "], cheat type:[_TYPE_INVALID_CLIENT], subtype:[1]";
			std::string lineA = "User:[" + a.strName + "][" + std::to_string(a.idUser) + "]";
			bool bLogged = false;
			std::string text;
			for(int i = 0; i < 25 && !bLogged; i++, ::Sleep(200))
			{
				text = ReadFile(g_strRun + szLog);
				text = text.size() > nFrom ? text.substr(nFrom) : "";
				bLogged = text.find(lineB) != std::string::npos;
			}
			Assert::IsTrue(bLogged, L"the wrong answer is not in gmlog\\cheater (the control)");
			if(text.find(lineA) != std::string::npos)
				Log("  %s", text.substr(text.find(lineA), 120).c_str());
			Assert::IsTrue(text.find(lineA) == std::string::npos, L"the client's answer was marked a cheat");
		});
	}

	TEST_METHOD(S_Login_ExpCheater)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Exc", "exp=999999999");
			SYSTEMTIME t;
			::GetLocalTime(&t);
			char szLog[64];
			sprintf(szLog, "\\gmlog\\expcheater1 %d-%d-%d.log", t.wYear, t.wMonth, t.wDay);
			std::string line = std::to_string(p.idAccount) + "\t" + std::to_string(p.idUser) + "\t" + p.strName + "\t1\t999999999\t";
			bool bLogged = false;
			for(int i = 0; i < 25 && !bLogged; i++, ::Sleep(200))
				bLogged = ReadFile(g_strRun + szLog).find(line) != std::string::npos;
			Assert::IsTrue(bLogged, L"the cut experience is not in gmlog\\expcheater1");
		});
	}

	TEST_METHOD(S1012_TickAnswered)
	{
		Player p;
		NewPlayer(p, "Tck");
		GameMsg msg;
		int nTicks = 0;
		for(DWORD tEnd = ::GetTickCount() + 30000; ::GetTickCount() < tEnd && !p.client.IsClosed(); )
			if(p.client.Recv(msg, 500) && msg.usType == 1012)
				nTicks++;
		Log("  %d ticks in 30 s", nTicks);
		Assert::IsFalse(p.client.IsClosed(), L"the player was disconnected (unanswered ticks)");
#ifdef _DEBUG
		Assert::AreEqual(0, nTicks, L"a Debug server ticked (the tick block is #ifndef _DEBUG)");
#else
		Assert::IsTrue(nTicks >= 2, L"fewer than two ticks in 30 s");
#endif
		Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 3, 79)));
		Drain(p, 1000);
		Assert::IsFalse(p.client.IsClosed(), L"the connection closed after 30 s");
	}

	TEST_METHOD(S1010_094_137_RebornGhostWhenAlive)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Reb");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, 94)));
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, 137)));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return (m.usType == 1010 && (Get16(m.buf, 0x16) == 94 || Get16(m.buf, 0x16) == 137)) || (m.usType == 1014 && Get32(m.buf, 4) == p.idUser); }),
				L"a living player's rebirth or ghost was answered");
		});
	}

	TEST_METHOD(S1010_095_DelRole)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Del");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 1, p.usX, p.usY, 0, 95)));
			Drain(p, 1000);
			Assert::IsFalse(p.client.IsClosed(), L"a wrong key closed the connection");
			MYSQL* h = Connect(GATE_DB);
			std::string sql = "SELECT COUNT(*) FROM cq_user WHERE id=" + std::to_string(p.idUser);
			Assert::AreEqual(std::string("1"), Query(h, sql)[0][0], L"a wrong key deleted the role");
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, 0, p.usX, p.usY, 0, 95)));
			GameMsg msg;
			while(p.client.Recv(msg, 3000))
				;
			Assert::IsTrue(p.client.IsClosed(), L"the deletion did not close the connection");
			Assert::AreEqual(std::string("0"), Query(h, sql)[0][0], L"the role is still in cq_user");
			Assert::AreEqual(std::string("1"), Query(h, "SELECT COUNT(*) FROM cq_deluser WHERE id=" + std::to_string(p.idUser))[0][0],
				L"the role is not in cq_deluser");
			mysql_close(h);
		});
	}

	TEST_METHOD(S1010_StatelessActionsAreSilent)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Sil");
			NewPlayer(b, "Sil");
			Drain(a);
			const unsigned short actions[] = { 100, 118, 120, 122, 123, 140, 127, 129, 139 };
			for(int i = 0; i < sizeof(actions)/sizeof(actions[0]); i++)
			{
				unsigned short act = actions[i];
				unsigned long dwData = (act == 123 || act == 140) ? b.idUser : 0;
				Assert::IsTrue(a.client.Send(Msg1010(a.idUser, dwData, a.usX, a.usY, 0, act)));
				Assert::IsFalse(Comes(a, [&](const GameMsg& m) { return (m.usType == 1010 && Get16(m.buf, 0x16) == act) || m.usType == 2033 || (m.usType == 1004 && Read1004Str(m, 1) == a.strName); }, 1000),
					L"an action without its state was answered");
			}
			Assert::IsFalse(a.client.IsClosed(), L"the connection was closed");
		});
	}

	TEST_METHOD(S1010_138_UnsolicitedSynchro)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Syn");
			unsigned long dwPos = (unsigned long)p.usX | ((unsigned long)p.usY << 16);
			Assert::IsTrue(p.client.Send(Msg1010(p.idUser, dwPos, p.usX, p.usY, 0, 138)));
			GameMsg msg;
			while(p.client.Recv(msg, 3000))
				;
			Assert::IsTrue(p.client.IsClosed(), L"an unsolicited synchro did not close the connection");
		});
	}

	TEST_METHOD(S1004_2000_Talk)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Tlk");
			NewPlayer(b, "Tlk");
			Drain(a);
			Assert::IsTrue(a.client.Send(Msg1004(2000, a.strName.c_str(), "ALLUSERS", "hello there")));
			GameMsg msg;
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return Is1004(m, 2000); }), L"the second player did not hear the talk");
			Assert::AreEqual(a.strName, Read1004Str(msg, 0));
			Assert::AreEqual(std::string("hello there"), Read1004Str(msg, 3));
			Assert::IsFalse(Comes(a, [](const GameMsg& m) { return Is1004(m, 2000); }), L"the talk came back to its speaker");
		});
	}

	TEST_METHOD(S1004_2001_Whisper)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Wsp");
			NewPlayer(b, "Wsp");
			Drain(a);
			Assert::IsTrue(a.client.Send(Msg1004(2001, a.strName.c_str(), b.strName.c_str(), "a secret")));
			GameMsg msg;
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return Is1004(m, 2001); }), L"the hearer did not get the whisper");
			Assert::AreEqual(std::string("a secret"), Read1004Str(msg, 3));
			Assert::AreEqual(a.strName, Read1004Str(msg, 0));
			Assert::IsTrue(a.client.Send(Msg1004(2001, a.strName.c_str(), "NobodyAtAll", "anyone?")));
			std::string words;
			Assert::IsTrue(ExpectTalk(a, 2000, words), L"no answer for a whisper to nobody");
			Assert::AreEqual(std::string("STR_TARGET_OFFLINE"), words);
		});
	}

	TEST_METHOD(S1004_GroupChannelsWithoutAGroup)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Grp");
			NewPlayer(b, "Grp");
			Drain(a);
			const unsigned short channels[] = { 2003, 2004, 2111 };
			for(int i = 0; i < 3; i++)
			{
				unsigned short ch = channels[i];
				Assert::IsTrue(a.client.Send(Msg1004(ch, a.strName.c_str(), "ALLUSERS", "to my group")));
				Assert::IsFalse(Comes(b, [ch](const GameMsg& m) { return Is1004(m, ch); }, 1000), L"a group channel without a group reached another player");
				Assert::IsFalse(Comes(a, [ch](const GameMsg& m) { return Is1004(m, ch); }, 500), L"a group channel without a group came back");
			}
		});
	}

	TEST_METHOD(S1009_001_002_BuyAndSell)
	{
		Twice([&]()
		{
			Player away;
			NewPlayer(away, "Buy", "money=1000");
			Assert::IsTrue(away.client.Send(Msg1009(3, 1000000, 1, away.idUser)));
			Assert::IsFalse(Comes(away, [](const GameMsg& m) { return m.usType == 1008; }), L"a purchase away from the shop was served");

			Player p;
			NewPlayer(p, "Buy", "money=1000, recordmap_id=1002, recordx=467, recordy=328");
			Assert::IsTrue(p.client.Send(Msg1009(3, 1000000, 1, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 8) == 1000000; }),
				L"no 1008 for the potion bought");
			unsigned long idItem = Get32(msg.buf, 4);
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after the purchase");
			Assert::AreEqual(995, (int)dwMoney);
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idItem) + " AND player_id=" + std::to_string(p.idUser)));

			Assert::IsTrue(p.client.Send(Msg1009(3, idItem, 2, p.idUser)));
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after the sale");
			Assert::IsTrue(dwMoney > 995, L"the sale gained nothing");
			Assert::AreEqual(0, CountRows("cq_item", "id=" + std::to_string(idItem)), L"the item sold is still in cq_item");
		});
	}

	TEST_METHOD(S1009_002_SellBestEquipment)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0;
			Player p;
			NewPlayer(p, "Sbe", "money=1000, recordmap_id=1002, recordx=467, recordy=328", [&](unsigned long idUser)
			{
				idBlade = GiveItem(idUser, 410009, 1599, 1599);
			});
			Assert::IsTrue(p.client.Send(Msg1009(3, idBlade, 2, p.idUser)));
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"the best blade was not sold");
			Assert::IsTrue(dwMoney > 1000, L"the sale gained nothing");
			Assert::AreEqual(0, CountRows("cq_item", "id=" + std::to_string(idBlade)), L"the blade sold is still in cq_item");
		});
	}

	TEST_METHOD(S1009_004_UsePotion)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Use", "money=1000, recordmap_id=1002, recordx=467, recordy=328");
			unsigned long idItem = BuyPotion(p);
			Assert::IsTrue(p.client.Send(Msg1009(idItem, 0, 4, p.idUser)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a potion at full life");
			Assert::AreEqual(std::string("STR_UNABLE_USE_ITEM"), words);

			Player q;
			NewPlayer(q, "Use", "money=1000, life=10, recordmap_id=1002, recordx=467, recordy=328");
			idItem = BuyPotion(q);
			Assert::IsTrue(q.client.Send(Msg1009(idItem, 0, 4, q.idUser)));
			unsigned long dwLife = 0;
			Assert::IsTrue(ExpectAttrib(q, ATTRIB_LIFE, dwLife), L"no life update after the potion");
			Assert::AreEqual(80, (int)dwLife);
			Assert::AreEqual(0, CountRows("cq_item", "id=" + std::to_string(idItem)), L"the potion eaten is still in cq_item");
		});
	}
	TEST_METHOD(S1009_003_Drop)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Drp", "money=1000, recordmap_id=1002, recordx=467, recordy=328");
			unsigned long idItem = BuyPotion(p);
			Assert::IsTrue(p.client.Send(Msg1009(idItem, (unsigned long)p.usY | ((unsigned long)p.usX << 16), 3, p.idUser)));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return m.usType == 1101; }), L"an item was dropped beside an NPC");
			static int s_nCell = 0;
			unsigned short usX = (unsigned short)(p.usX + 3 + s_nCell), usY = (unsigned short)(p.usY + 3);
			s_nCell++;
			Assert::IsTrue(p.client.Send(Msg1009(idItem, (unsigned long)usY | ((unsigned long)usX << 16), 3, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1101 && Get32(m.buf, 8) == 1000000; }),
				L"no 1101 for the potion dropped three cells away");
			Assert::AreEqual((int)usX, (int)Get16(msg.buf, 0x0C));
			Assert::AreEqual((int)usY, (int)Get16(msg.buf, 0x0E));
		});
	}

	TEST_METHOD(S1101_003_Pick)
	{
		Twice([&]()
		{
			static int s_nCell = 0;
			char szSet[128];
			sprintf(szSet, "money=1000, recordmap_id=1002, recordx=%d, recordy=335", 470 + s_nCell++);
			Player p;
			NewPlayer(p, "Pck", szSet);
			unsigned long idItem = BuyPotion(p);
			Assert::IsTrue(p.client.Send(Msg1009(idItem, (unsigned long)p.usY | ((unsigned long)p.usX << 16), 3, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1101 && Get32(m.buf, 8) == 1000000; }),
				L"no 1101 for the potion dropped on the player's cell");
			unsigned long idMapItem = Get32(msg.buf, 4);
			Drain(p);

			Bytes pick = Header(0x14, 1101);
			Put32(pick, 4, idMapItem);
			Put16(pick, 0x0C, p.usX);
			Put16(pick, 0x0E, p.usY);
			Put16(pick, 0x12, 3);
			Assert::IsTrue(p.client.Send(pick));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return m.usType == 1008; }), L"a 1101 with the action at +12h picked the item");
			Put16(pick, 0x12, 0);
			pick[0x10] = 3;
			Assert::IsTrue(p.client.Send(pick));
			bool bPicked = false, bDeleted = false;
			while(!(bPicked && bDeleted) && Expect(p, msg, [&](const GameMsg& m) {
					return (m.usType == 1008 && Get32(m.buf, 8) == 1000000) || (m.usType == 1101 && Get32(m.buf, 4) == idMapItem && m.buf[0x10] == 2); }))
				(msg.usType == 1008 ? bPicked : bDeleted) = true;
			Assert::IsTrue(bPicked, L"the potion was not picked up");
			Assert::IsTrue(bDeleted, L"the map item was not deleted");
		});
	}

	TEST_METHOD(S1009_012_DropMoney)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Mny", "money=1000");
			unsigned long dwPos = (unsigned long)p.usY | ((unsigned long)p.usX << 16);
			Assert::IsTrue(p.client.Send(Msg1009(99, dwPos, 12, p.idUser)));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return m.usType == 1101; }), L"99 silver was dropped");
			Assert::IsTrue(p.client.Send(Msg1009(150, dwPos, 12, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1101; }), L"no 1101 for 150 silver");
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after the drop");
			Assert::AreEqual(850, (int)dwMoney);
		});
	}

	TEST_METHOD(S1009_027_Ping)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Png");
			Bytes ping = Msg1009(0x1234, 0x5678, 27, p.idUser);
			Assert::IsTrue(p.client.Send(ping));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 27; }), L"no ping back");
			Assert::AreEqual(0x1234, (int)Get32(msg.buf, 4));
			Assert::AreEqual(0x5678, (int)Get32(msg.buf, 8));
		});
	}

	TEST_METHOD(S1009_ForgedItemIsNotLoaded)
	{
		Twice([&]()
		{
			unsigned long idGood = 0, idForged = 0;
			Player p;
			NewPlayer(p, "Frg", "level=10", [&](unsigned long idUser)
			{
				idGood = GiveItem(idUser, 410005, 1599, 1599);
				idForged = GiveItem(idUser, 410005, 1599, 1599, 0, false);
			});
			Assert::IsTrue(p.client.Send(Msg1009(idForged, 4, 4, p.idUser)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for the forged item");
			Assert::AreEqual(std::string("STR_UNABLE_USE_ITEM"), words);
			SYSTEMTIME t;
			::GetLocalTime(&t);
			char szLog[64];
			sprintf(szLog, "\\gmlog\\invalid_item %d-%d-%d.log", t.wYear, t.wMonth, t.wDay);
			std::string line = "invalid item:[" + std::to_string(idForged) + "] found in user:[" + p.strName + "]";
			Assert::IsTrue(ReadFile(g_strRun + szLog).find(line) != std::string::npos, L"the forged item is not in gmlog\\invalid_item");
			Assert::IsTrue(p.client.Send(Msg1009(idGood, 4, 4, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 5 && Get32(m.buf, 4) == idGood; }),
				L"the good blade was not equipped");
		});
	}

	TEST_METHOD(S1009_004_006_EquipAndUnequip)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0;
			Player p;
			NewPlayer(p, "Eq", "level=10", [&](unsigned long idUser) { idBlade = GiveItem(idUser, 410005, 1599, 1599); });
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, 4, 4, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 5 && Get32(m.buf, 4) == idBlade; }),
				L"no 1009 EQUIP for the blade");
			Assert::AreEqual(4, (int)Get32(msg.buf, 8));
			Assert::IsTrue(p.client.Send(Msg1009(0, 4, 6, p.idUser)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 6 && Get32(m.buf, 4) == idBlade; }),
				L"no 1009 UNEQUIP for the blade");
			Assert::IsTrue(p.client.Send(Msg1009(0, 4, 6, p.idUser)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for an empty hand taken off");
			Assert::AreEqual(std::string("STR_YOUR_BAG_FULL"), words);
		});
	}

	TEST_METHOD(S1009_009_010_011_Warehouse)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Wh", "money=1000, recordmap_id=1002, recordx=410, recordy=352");
			auto query = [&](unsigned long& dwSaved)
			{
				Assert::IsTrue(p.client.Send(Msg1009(8, 0, 9, p.idUser)));
				GameMsg msg;
				Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 9; }), L"no answer to the query");
				dwSaved = Get32(msg.buf, 8);
			};
			unsigned long dwSaved = 99, dwMoney = 0;
			query(dwSaved);
			Assert::AreEqual(0, (int)dwSaved);
			Assert::IsTrue(p.client.Send(Msg1009(8, 300, 10, p.idUser)));
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after saving");
			Assert::AreEqual(700, (int)dwMoney);
			Assert::IsTrue(p.client.Send(Msg1009(8, 100, 11, p.idUser)));
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after drawing");
			Assert::AreEqual(800, (int)dwMoney);
			query(dwSaved);
			Assert::AreEqual(200, (int)dwSaved);
			Assert::IsTrue(p.client.Send(Msg1009(8, 500, 11, p.idUser)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for drawing more than saved");
			Assert::AreEqual(std::string("STR_NOT_SO_MUCH_MONEY"), words);
		});
	}

	TEST_METHOD(S1009_014_Repair)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0;
			Player away;
			NewPlayer(away, "Rep", "money=10000, recordmap_id=1002, recordx=700, recordy=500",
				[&](unsigned long idUser) { idBlade = GiveItem(idUser, 410005, 1000, 1599); });
			Assert::IsTrue(away.client.Send(Msg1009(idBlade, 0, 14, away.idUser)));
			std::string words;
			Assert::IsTrue(ExpectTalk(away, TXTATR_SYSTEM, words), L"no text for a repair far from any shop");
			Assert::AreEqual(std::string("STR_REPAIR_FAILED"), words);

			Player p;
			NewPlayer(p, "Rep", "money=10000, recordmap_id=1002, recordx=453, recordy=331",
				[&](unsigned long idUser) { idBlade = GiveItem(idUser, 410005, 1000, 1599); });
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, 0, 14, p.idUser)));
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after the repair");
			Assert::IsTrue(dwMoney < 10000, L"the repair cost nothing");
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idBlade; }),
				L"no 1008 for the blade repaired");
			Assert::AreEqual(1599, (int)Get16(msg.buf, 0x0C));
		});
	}

	TEST_METHOD(S1009_007_008_SplitAndCombine)
	{
		Twice([&]()
		{
			unsigned long idA = 0, idB = 0;
			Player p;
			NewPlayer(p, "Arw", NULL, [&](unsigned long idUser)
			{
				idA = GiveItem(idUser, 1050001, 60, 100);
				idB = GiveItem(idUser, 1050001, 30, 100);
			});
			Assert::IsTrue(p.client.Send(Msg1009(idA, 100, 7, p.idUser)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a split of the whole stack");
			Assert::AreEqual(std::string("STR_YOUR_BAG_FULL"), words);
			Assert::IsTrue(p.client.Send(Msg1009(idA, 20, 7, p.idUser)));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return m.usType == 1009 || m.usType == 1008; }), L"the split was answered");
			Assert::AreEqual(3, CountRows("cq_item", "type=1050001 AND player_id=" + std::to_string(p.idUser)), L"the split stack is not in cq_item");
			Assert::IsTrue(p.client.Send(Msg1009(idA, idB, 8, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 3 && Get32(m.buf, 4) == idB; }),
				L"the other stack was not erased");
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 25 && Get32(m.buf, 4) == idA; }),
				L"no amount for the combined stack");
			Assert::AreEqual(70, (int)Get32(msg.buf, 8));
		});
	}

	TEST_METHOD(S1009_019_020_ImproveAndUpLevel)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0, idBall = 0, idMeteor = 0, idPotion = 0;
			auto give = [&](unsigned long idUser)
			{
				idBlade = GiveItem(idUser, 410005, 1599, 1599);
				idBall = GiveItem(idUser, 1088000, 1, 1);
				idMeteor = GiveItem(idUser, 1088001, 1, 1);
				idPotion = GiveItem(idUser, 1000000, 1, 1);
			};
			Player away;
			NewPlayer(away, "Art", "level=10", give);
			Assert::IsTrue(away.client.Send(Msg1009(idBlade, idBall, 19, away.idUser)));
			Assert::IsFalse(Comes(away, [&](const GameMsg& m) { return m.usType == 1009 || m.usType == 1008 || (m.usType == 1004 && Read1004Str(m, 1) == away.strName); }),
				L"an improvement far from an artisan was answered");

			Player p;
			NewPlayer(p, "Art", "level=10, recordmap_id=1002, recordx=422, recordy=353", give);
			std::string words;
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, idPotion, 19, p.idUser)));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a potion as the dragon ball");
			Assert::AreEqual(std::string("STR_DRAGONBALL"), words);
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, idPotion, 20, p.idUser)));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a potion as the meteor");
			Assert::AreEqual(std::string("STR_METEOR"), words);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, idBall, 19, p.idUser)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 3 && Get32(m.buf, 4) == idBall; }),
				L"the dragon ball was not spent");
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, idMeteor, 20, p.idUser)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 3 && Get32(m.buf, 4) == idMeteor; }),
				L"the meteor was not spent");
		});
	}

	TEST_METHOD(S1009_020_UpLevelColourDigit)
	{
		Twice([&]()
		{
			unsigned long idShield = 0, idTop = 0, idHelmet = 0, idMeteor1 = 0, idMeteor2 = 0, idMeteor3 = 0;
			Player p;
			NewPlayer(p, "Col", "level=10, recordmap_id=1002, recordx=422, recordy=353", [&](unsigned long idUser)
			{
				idShield = GiveItem(idUser, 900393, 3498, 3498);
				idTop = GiveItem(idUser, 112313, 4098, 4098);
				idHelmet = GiveItem(idUser, 111303, 3998, 3998);
				idMeteor1 = GiveItem(idUser, 1088001, 1, 1);
				idMeteor2 = GiveItem(idUser, 1088001, 1, 1);
				idMeteor3 = GiveItem(idUser, 1088001, 1, 1);
			});
			std::string words;
			Assert::IsTrue(p.client.Send(Msg1009(idShield, idMeteor1, 20, p.idUser)));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a level-9 shield");
			Assert::AreEqual(std::string("STR_UPGRADE_NOMORE"), words);
			Assert::IsTrue(p.client.Send(Msg1009(idTop, idMeteor2, 20, p.idUser)));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a ConquestHelmet");
			Assert::AreEqual(std::string("STR_UPGRADE_NOMORE"), words);
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 3; }), L"a refused upgrade spent a meteor");
			Assert::AreEqual(3, CountRows("cq_item", ("type=1088001 AND player_id=" + std::to_string(p.idUser)).c_str()));

			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg1009(idHelmet, idMeteor3, 20, p.idUser)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 3 && Get32(m.buf, 4) == idMeteor3; }),
				L"the level-0 helmet's meteor was not spent");
		});
	}

	TEST_METHOD(S0510_0512_DyeByLevel)
	{
		Twice([&]()
		{
			unsigned long idHelmet = 0, idLow = 0;
			Player p, q;
			NewPlayer(p, "Dye", "recordmap_id=1002, recordx=430, recordy=380", [&](unsigned long idUser)
			{
				idHelmet = GiveItem(idUser, 111393, 2698, 2698, 1);
			});
			NewPlayer(q, "Dye", "recordmap_id=1002, recordx=430, recordy=380", [&](unsigned long idUser)
			{
				idLow = GiveItem(idUser, 111343, 3698, 3698, 1);
			});
			Drain(p);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg2031(9992, 0)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1108 && Get32(m.buf, 4) == idHelmet; }), L"the level-9 helmet was not dyed");
			Assert::AreEqual(111593UL, Get32(msg.buf, 0x10));
			Sleep(500);
			Assert::AreEqual(1, CountRows("cq_item", ("type=111593 AND id=" + std::to_string(idHelmet)).c_str()));

			Assert::IsTrue(q.client.Send(Msg2031(9992, 0)));
			Assert::IsFalse(Comes(q, [&](const GameMsg& m) { return m.usType == 1108 && Get32(m.buf, 4) == idLow; }), L"the level-4 helmet was dyed");
			Assert::AreEqual(1, CountRows("cq_item", ("type=111343 AND id=" + std::to_string(idLow)).c_str()));
		});
	}

	TEST_METHOD(S1508_Lottery)
	{
		Twice([&]()
		{
			SYSTEMTIME t;
			::GetLocalTime(&t);
			char szLog[64], szTest[64];
			sprintf(szLog, "\\gmlog\\lottery %d-%d-%d.log", t.wYear, t.wMonth, t.wDay);
			sprintf(szTest, "\\gmlog\\lotterytest %d-%d-%d.log", t.wYear, t.wMonth, t.wDay);
			auto lines = [](const std::string& text) { return (int)std::count(text.begin(), text.end(), '\n'); };

			Player p;
			NewPlayer(p, "Lot", "recordmap_id=1002, recordx=430, recordy=380");
			std::string owner = "owner_id=" + std::to_string(p.idUser);
			int nItems = CountRows("cq_item", owner);
			std::vector<unsigned long> prizes;
			{
				MYSQL* h = Connect(GATE_DB);
				for(auto& r : Query(h, "SELECT DISTINCT prize_item FROM cq_lottery WHERE type=1"))
					prizes.push_back(strtoul(r[0].c_str(), NULL, 10));
				mysql_close(h);
			}
			Assert::IsFalse(prizes.empty(), L"cq_lottery has no prize of type 1");
			Drain(p);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg2031(9995, 0)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1008 && std::find(prizes.begin(), prizes.end(), Get32(m.buf, 8)) != prizes.end(); }),
				L"no 1008 of a lottery prize");
			unsigned long idPrize = Get32(msg.buf, 4);
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"the winner was not told");
			Assert::AreEqual(std::string("STR_LOTTERY_GENERAL"), words);
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 242 && Get32(m.buf, 8) == p.idUser; }),
				L"no 1010 action 242 to the room");
			Sleep(500);
			Assert::AreEqual(nItems + 1, CountRows("cq_item", owner), L"not one item awarded");
			Assert::AreEqual(1, CountRows("cq_item", owner + " AND id=" + std::to_string(idPrize)), L"the prize's row is not the winner's");
			std::string line = "lottery user [id:" + std::to_string(p.idUser) + "] [name:" + p.strName + "] | award [rank:";
			bool bLogged = false;
			for(int i = 0; i < 25 && !bLogged; i++, ::Sleep(200))
				bLogged = ReadFile(g_strRun + szLog).find(line) != std::string::npos;
			Assert::IsTrue(bLogged, L"the draw is not in gmlog\\lottery");

			Player c;
			NewPlayer(c, "Lc", "name=CONCAT('[GM]',name), recordmap_id=1002, recordx=430, recordy=380");
			c.strName = "[GM]" + c.strName;
			int nTests = lines(ReadFile(g_strRun + szTest));
			Assert::IsTrue(c.client.Send(Msg1004(2000, c.strName.c_str(), "ALLUSERS", "/lotterytest 3 1 1")));
			Sleep(1500);
			Assert::AreEqual(nTests, lines(ReadFile(g_strRun + szTest)), L"a GM who is not a PM drew test lotteries");

			Player g;
			NewPlayer(g, "Lt", "name=CONCAT('[PM]',name), recordmap_id=1002, recordx=430, recordy=380");
			g.strName = "[PM]" + g.strName;
			std::string gowner = "owner_id=" + std::to_string(g.idUser);
			int nGmItems = CountRows("cq_item", gowner);
			Assert::IsTrue(g.client.Send(Msg1004(2000, g.strName.c_str(), "ALLUSERS", "/lotterytest 3 1 1")));
			int nNow = nTests;
			for(int i = 0; i < 25 && nNow < nTests + 3; i++, ::Sleep(200))
				nNow = lines(ReadFile(g_strRun + szTest));
			Sleep(500);
			Assert::AreEqual(nTests + 3, lines(ReadFile(g_strRun + szTest)), L"not three test draws in gmlog\\lotterytest");
			Assert::AreEqual(nGmItems, CountRows("cq_item", gowner), L"a test draw awarded an item");
		});
	}

	TEST_METHOD(S1004_AwardItemOptions)
	{
		Twice([&]()
		{
			Player g;
			NewPlayer(g, "Ai", "name=CONCAT('[PM]',name), recordmap_id=1002, recordx=430, recordy=380");
			g.strName = "[PM]" + g.strName;
			std::string owner = "owner_id=" + std::to_string(g.idUser);
			int nItems = CountRows("cq_item", owner);
			Drain(g);
			GameMsg msg;

			Assert::IsTrue(g.client.Send(Msg1004(2000, g.strName.c_str(), "ALLUSERS", "/awarditem 410339 9 7 255 13 13")));
			Assert::IsTrue(Expect(g, msg, [](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 8) == 410339; }), L"no 1008 of the blade");
			unsigned long idBlade = Get32(msg.buf, 4);
			Assert::AreEqual(1, (int)msg.buf[0x10], L"not ITEMINFO_ADDITEM");
			Assert::AreEqual(13, (int)msg.buf[0x13]);
			Assert::AreEqual(13, (int)msg.buf[0x14]);
			Assert::AreEqual(9, (int)msg.buf[0x17]);
			Assert::AreEqual(7, (int)msg.buf[0x18]);
			Assert::AreEqual(255, (int)msg.buf[0x19]);
			Assert::IsFalse(Comes(g, [](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 8) == 410339; }), L"the blade was sent twice");

			Assert::IsTrue(g.client.Send(Msg1004(2000, g.strName.c_str(), "ALLUSERS", "/awarditem 1088000")));
			Assert::IsTrue(Expect(g, msg, [](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 8) == 1088000; }), L"no 1008 of the DragonBall");

			const char* refused[] = { "/awarditem 1002000 0 7", "/awarditem 410339 10", "/awarditem 410339 9 0 0 14", "/awarditem 410339 9 256" };
			for(int i = 0; i < 4; i++)
			{
				Assert::IsTrue(g.client.Send(Msg1004(2000, g.strName.c_str(), "ALLUSERS", refused[i])));
				Assert::IsFalse(Comes(g, [](const GameMsg& m) { return m.usType == 1008; }), L"a refused option awarded an item");
			}
			Sleep(500);
			Assert::AreEqual(nItems + 2, CountRows("cq_item", owner), L"not two items awarded");

			g.client.Close();
			std::string blade = owner + " AND id=" + std::to_string(idBlade) + " AND type=410339 AND magic3=9 AND reduce_dmg=7 AND add_life=255 AND gem1=13 AND gem2=13";
			int nSaved = 0;
			for(int i = 0; i < 25 && !nSaved; i++, ::Sleep(200))
				nSaved = CountRows("cq_item", blade);
			Assert::AreEqual(1, nSaved, L"the blade's row was not saved with its options");
		});
	}

	TEST_METHOD(S1045_ResetMagic)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Rst", "recordmap_id=1002, recordx=430, recordy=380", [&](unsigned long idUser)
			{
				MYSQL* h = Connect(GATE_DB);
				Exec(h, "INSERT cq_magic SET ownerid=" + std::to_string(idUser) + ", type=1000, level=2, exp=500, unlearn=0, old_level=0");
				mysql_close(h);
			});
			Drain(p);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg2031(9993, 0)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return (m.usType == 1103 && Get16(m.buf, 8) == 1000) || (m.usType == 1010 && Get16(m.buf, 0x16) == 125); }),
				L"nothing told the client of the reset");
			Assert::AreEqual(1103, (int)msg.usType, L"the reset came as 2008's 1010 action 125, which the 4351 client ignores");
			Assert::AreEqual(0, (int)Get16(msg.buf, 0x0A), L"the level is not 0");
			Assert::AreEqual(0UL, Get32(msg.buf, 4), L"the experience is not 0");
			Sleep(500);
			Assert::AreEqual(1, CountRows("cq_magic", ("type=1000 AND level=0 AND exp=0 AND ownerid=" + std::to_string(p.idUser)).c_str()));
		});
	}

	TEST_METHOD(S1009_028_Enhance)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0, idGem = 0, idPotion = 0;
			Player p;
			NewPlayer(p, "Enh", NULL, [&](unsigned long idUser)
			{
				idBlade = GiveItem(idUser, 410005, 1599, 1599);
				idGem = GiveItem(idUser, 700001, 1, 1);
				idPotion = GiveItem(idUser, 1000000, 1, 1);
			});
			std::string words;
			Assert::IsTrue(p.client.Send(Msg1009(123456789, idGem, 28, p.idUser)));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for an unknown item");
			Assert::AreEqual(std::string("target item not found!"), words);
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, idPotion, 28, p.idUser)));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for a potion as the gem");
			Assert::AreEqual(std::string("assistance item is not gem!"), words);
			Assert::IsTrue(p.client.Send(Msg1009(idBlade, idGem, 28, p.idUser)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 3 && Get32(m.buf, 4) == idGem; }),
				L"the gem was not spent");
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 28 && Get32(m.buf, 4) == idBlade; }),
				L"no 1009 28 for the blade");
			unsigned long nLife = Get32(msg.buf, 8);
			Assert::IsTrue(nLife >= 1 && nLife <= 59, L"the life drawn is outside 1..59");
		});
	}

	TEST_METHOD(S1015_016_QueryMate)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Mat");
			NewPlayer(b, "Mat");
			Assert::IsTrue(b.client.Send(Msg1015(a.idUser, 16)));
			GameMsg msg;
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return m.usType == 1015 && m.buf.size() > 8 && m.buf[8] == 16; }),
				L"no 1015 16 back");
			Assert::AreEqual((int)a.idUser, (int)Get32(msg.buf, 4));
			Assert::IsTrue(msg.buf.size() >= 15 && msg.buf[9] >= 1 && msg.buf[10] == 4 && memcmp(&msg.buf[11], "None", 4) == 0,
				L"the mate is not the none name");
		});
	}

	TEST_METHOD(S1015_018_LeaveDice)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Dice");
			Assert::IsTrue(p.client.Send(Msg1015(0, 18)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1015 && m.buf.size() > 8 && m.buf[8] == 18; }),
				L"no 1015 18 back");
			Assert::IsTrue(msg.buf.size() >= 11 && msg.buf[9] >= 1, L"no name in the answer");
			Assert::AreEqual(p.strName, std::string((const char*)&msg.buf[11], msg.buf[10]));
		});
	}

	TEST_METHOD(S1015_StatelessKindsAreSilent)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Nm");
			const unsigned char kinds[] = { 4, 11, 12, 13, 14, 17 };
			for(int i = 0; i < sizeof(kinds); i++)
			{
				Assert::IsTrue(p.client.Send(Msg1015(0, kinds[i], "Nobody")));
				Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1015 || (m.usType == 1004 && Read1004Str(m, 1) == p.strName); }, 1000),
					L"a name kind without its state was answered");
			}
		});
	}

	TEST_METHOD(S1019_010_014_Friendship)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Frd");
			NewPlayer(b, "Frd");
			Drain(a);
			std::string words;
			Assert::IsTrue(a.client.Send(Msg1019(b.idUser, 10, 0, b.strName.c_str())));
			Assert::IsTrue(ExpectTalk(a, TXTATR_SYSTEM, words), L"no text for the application");
			Assert::AreEqual(std::string("STR_MAKE_FRIEND_SENT"), words);
			Assert::IsTrue(ExpectTalk(b, TXTATR_SYSTEM, words), L"the target was not told of the application");
			Assert::AreEqual(std::string("STR_TO_MAKE_FRIEND"), words);
			Assert::IsTrue(b.client.Send(Msg1019(a.idUser, 10, 0, a.strName.c_str())));
			GameMsg msg;
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1019 && m.buf[8] == 15 && Get32(m.buf, 4) == a.idUser; }),
				L"the second applicant got no friend info");
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1019 && m.buf[8] == 15 && Get32(m.buf, 4) == b.idUser; }),
				L"the first applicant got no friend info");
			Assert::AreEqual(b.strName, std::string((const char*)&msg.buf[0x0C]));
			Assert::AreEqual(2, CountRows("cq_friend", "(userid=" + std::to_string(a.idUser) + " AND friend=" + std::to_string(b.idUser)
				+ ") OR (userid=" + std::to_string(b.idUser) + " AND friend=" + std::to_string(a.idUser) + ")"), L"the friendship is not in cq_friend");

			Assert::IsTrue(a.client.Send(Msg1004(2009, a.strName.c_str(), "ALLUSERS", "to my friends")));
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return Is1004(m, 2009); }), L"the friend did not hear the friend channel");
			Assert::AreEqual(std::string("to my friends"), Read1004Str(msg, 3));

			Assert::IsTrue(a.client.Send(Msg1019(b.idUser, 14, 0, b.strName.c_str())));
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1019 && m.buf[8] == 14; }), L"the break was not echoed");
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1019 && m.buf[8] == 14 && Get32(m.buf, 4) == a.idUser; }),
				L"the other side was not told of the break");
			Assert::AreEqual(0, CountRows("cq_friend", "userid=" + std::to_string(a.idUser) + " OR userid=" + std::to_string(b.idUser)),
				L"the friendship is still in cq_friend");
		});
	}

	TEST_METHOD(S1019_018_EnemyDelWithoutEnemies)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Eny");
			Assert::IsTrue(p.client.Send(Msg1019(1234567, 18, 0, "Nobody")));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1019 || (m.usType == 1004 && Read1004Str(m, 1) == p.strName); }), L"an enemy deletion was answered");
		});
	}

	TEST_METHOD(S1023_000_001_005_002_TeamByApplication)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Tm");
			NewPlayer(b, "Tm");
			Drain(a);
			GameMsg msg;
			Assert::IsTrue(a.client.Send(Msg1023(0, 0)));
			Assert::IsTrue(Expect(a, msg, [](const GameMsg& m) { return m.usType == 1023 && Get16(m.buf, 4) == 0; }), L"the creation was not echoed");
			Assert::IsTrue(b.client.Send(Msg1023(1, a.idUser)));
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1023 && Get16(m.buf, 4) == 1 && Get32(m.buf, 8) == b.idUser; }),
				L"the leader got no application");
			std::string words;
			Assert::IsTrue(ExpectTalk(b, TXTATR_SYSTEM, words), L"the applicant got no text");
			Assert::AreEqual(std::string("STR_JOIN_REQUEST_SENT"), words);
			Assert::IsTrue(a.client.Send(Msg1023(5, b.idUser)));
			Assert::IsTrue(ExpectTalk(b, 2003, words), L"the new member was not told the team's rules");
			Assert::AreEqual(std::string("STR_TEAM_MONEY_s"), words);
			Assert::IsTrue(ExpectTalk(b, 2003, words), L"the new member was not told the item rule");
			Assert::AreEqual(std::string("STR_TEAM_ITEM_s"), words);

			Assert::IsTrue(a.client.Send(Msg1004(2003, a.strName.c_str(), "ALLUSERS", "to my team")));
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return Is1004(m, 2003) && Read1004Str(m, 3) == "to my team"; }),
				L"the member did not hear the team channel");
			Assert::IsTrue(b.client.Send(Msg1010(b.idUser, a.idUser, b.usX, b.usY, 0, 106)));
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 106; }),
				L"no answer to the query for a team member");

			Assert::IsTrue(b.client.Send(Msg1023(2, 0)));
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1023 && Get16(m.buf, 4) == 2 && Get32(m.buf, 8) == b.idUser; }),
				L"the leader was not told of the leave");
		});
	}

	TEST_METHOD(S1023_004_003_008_009_007_006_TeamByInvitation)
	{
		Twice([&]()
		{
			Player a, b, c;
			NewPlayer(a, "Inv");
			NewPlayer(b, "Inv");
			NewPlayer(c, "Inv");
			Drain(a);
			Drain(b);
			GameMsg msg;
			std::string words;
			Assert::IsTrue(a.client.Send(Msg1023(0, 0)));
			Assert::IsTrue(a.client.Send(Msg1023(4, b.idUser)));
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1023 && Get16(m.buf, 4) == 4 && Get32(m.buf, 8) == a.idUser; }),
				L"the invited player got no invitation");
			Assert::IsTrue(b.client.Send(Msg1023(3, a.idUser)));
			Assert::IsTrue(ExpectTalk(b, 2003, words), L"the invited player did not join");
			Assert::AreEqual(std::string("STR_TEAM_MONEY_s"), words);

			Assert::IsTrue(a.client.Send(Msg1023(8, 0)));
			::Sleep(500);
			Assert::IsTrue(c.client.Send(Msg1023(1, a.idUser)));
			Assert::IsTrue(ExpectTalk(c, TXTATR_SYSTEM, words), L"no text for an application to a closed team");
			Assert::AreEqual(std::string("STR_FORBIDDEN_JOIN"), words);
			Assert::IsTrue(a.client.Send(Msg1023(9, 0)));
			::Sleep(500);
			Assert::IsTrue(c.client.Send(Msg1023(1, a.idUser)));
			Assert::IsTrue(ExpectTalk(c, TXTATR_SYSTEM, words), L"no text for an application to the reopened team");
			Assert::AreEqual(std::string("STR_JOIN_REQUEST_SENT"), words);

			Assert::IsTrue(a.client.Send(Msg1023(7, b.idUser)));
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1023 && Get16(m.buf, 4) == 7 && Get32(m.buf, 8) == b.idUser; }),
				L"the kicked member was not told");
			Assert::IsTrue(a.client.Send(Msg1023(6, 0)));
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1023 && Get16(m.buf, 4) == 6; }), L"the dismissal was not sent");
		});
	}

	TEST_METHOD(S1023_010_013_RulesAreSilent)
	{
		Twice([&]()
		{
			Player a;
			NewPlayer(a, "Rul");
			Assert::IsTrue(a.client.Send(Msg1023(0, 0)));
			Drain(a);
			for(unsigned short act = 10; act <= 13; act++)
			{
				Assert::IsTrue(a.client.Send(Msg1023(act, 0)));
				Assert::IsFalse(Comes(a, [&](const GameMsg& m) { return m.usType == 1023 || (m.usType == 1004 && Read1004Str(m, 1) == a.strName); }, 1000),
					L"a team rule was answered");
			}
		});
	}

	TEST_METHOD(S1022_002_PkKillGhostReborn)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Pk");
			NewPlayer(b, "Pk");
			Drain(a);
			std::string words;
			Assert::IsTrue(a.client.Send(Msg1022(2, a.idUser, b.idUser, a.usX, a.usY, 0)));
			Assert::IsTrue(ExpectTalk(a, TXTATR_SYSTEM, words), L"no text for an attack on the newbie map");
			Assert::AreEqual(std::string("STR_NO_PK_VILLAGE"), words);
#ifdef _DEBUG
			return;
#endif
			const char* pszMap = "recordmap_id=1000, recordx=499, recordy=650";
			Player c, d;
			NewPlayer(c, "Pk", pszMap);
			NewPlayer(d, "Pk", (std::string(pszMap) + ", life=5").c_str());
			Assert::IsTrue(c.client.Send(Msg1010(c.idUser, 0, 0, 0, 0, 96)));
			Assert::IsTrue(d.client.Send(Msg1005(d.idUser, 0, 0)));
			Drain(c);
			Drain(d);
			Assert::IsTrue(c.client.Send(Msg1022(2, c.idUser, d.idUser, c.usX, c.usY, 0)));
			GameMsg msg;
			Assert::IsTrue(Expect(d, msg, [&](const GameMsg& m) { return m.usType == 1022 && Get16(m.buf, 0x14) == 2 && Get32(m.buf, 8) == c.idUser && Get32(m.buf, 0x0C) == d.idUser; }, 15000),
				L"the victim saw no hit");
			Assert::IsTrue(Expect(d, msg, [&](const GameMsg& m) { return m.usType == 1022 && Get16(m.buf, 0x14) == 14 && Get32(m.buf, 0x0C) == d.idUser; }, 30000),
				L"the victim saw no kill");
			DWORD tKill = ::GetTickCount();
			Assert::IsTrue(Expect(d, msg, [&](const GameMsg& m) { return m.usType == 1019 && m.buf[8] == 19 && Get32(m.buf, 4) == c.idUser; }),
				L"the killer was not added to the victim's enemies");

			Assert::IsTrue(d.client.Send(Msg1010(d.idUser, 0, d.usX, d.usY, 0, 137)));
			unsigned long dwValue = 0;
			Assert::IsTrue(ExpectAttrib(d, 26, dwValue), L"the ghost's effect was not sent");

			while(::GetTickCount() - tKill < 21000)
				d.client.Recv(msg, 500);
			Assert::IsTrue(d.client.Send(Msg1010(d.idUser, 0, d.usX, d.usY, 0, 94)));
			Assert::IsTrue(ExpectAttrib(d, ATTRIB_LIFE, dwValue), L"no life after the rebirth");
			Assert::IsTrue(dwValue > 5, L"the reborn life is not the maximum");
		});
	}

	TEST_METHOD(S1024_Allot)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Alt", "auto_allot=0, additional_point=5");
			Assert::IsTrue(p.client.Send(Msg1024(1, 1, 1, 0)));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1004 && Read1004Str(m, 1) == p.strName; }), L"a valid allotment was answered");
			Assert::IsTrue(p.client.Send(Msg1024(3, 0, 0, 0)));
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text for more points than are left");
			Assert::AreEqual(std::string("STR_CHEAT"), words);

			Player q;
			NewPlayer(q, "Alt", "auto_allot=1, additional_point=5");
			Assert::IsTrue(q.client.Send(Msg1024(9, 0, 0, 0)));
			Assert::IsFalse(Comes(q, [&](const GameMsg& m) { return m.usType == 1004 && Read1004Str(m, 1) == q.strName; }),
				L"an allotment with automatic allotment on was answered");
		});
	}

	std::string Read2032Text(const GameMsg& m)
	{
		if(m.buf.size() < 0x0E || m.buf[0x0C] == 0)
			return "";
		size_t n = m.buf[0x0D];
		return 0x0E + n <= m.buf.size() ? std::string((const char*)&m.buf[0x0E], n) : "";
	}

	TEST_METHOD(S2031_2032_GuideDialog)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Gd");
			Assert::IsTrue(p.client.Send(Msg2031(1060, 0)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 2032 && m.buf[0x0B] == 1; }), L"no text from the Guide");
			Assert::AreEqual(std::string("Hi,~are~you~new~to~Conquer~Online?~What~can~I~do~for~you?"), Read2032Text(msg));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 2032 && m.buf[0x0B] == 2; }), L"no link from the Guide");
			Assert::AreEqual(std::string("Where~am~I?"), Read2032Text(msg));
			unsigned char ucIndex = msg.buf[0x0A];
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 2032 && m.buf[0x0B] == 4; }), L"no picture from the Guide");
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 2032 && m.buf[0x0B] == 100; }), L"no dialog created");
			Assert::IsTrue(p.client.Send(Msg2032(0, ucIndex, 101)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 2032 && m.buf[0x0B] == 1; }), L"no answer to the link");
			Assert::IsTrue(Read2032Text(msg).find("This~is~the~way~to~Birth~Village") == 0, L"the answer is not 8001155's text");
		});
	}

	TEST_METHOD(S1056_Trade)
	{
		Twice([&]()
		{
			unsigned long idPotion = 0, idBall = 0;
			Player a, b;
			NewPlayer(a, "Trd", "money=5000", [&](unsigned long idUser) { idPotion = GiveItem(idUser, 1000000, 1, 1); idBall = GiveItem(idUser, 1088000, 1, 1); });
			NewPlayer(b, "Trd", "money=1000");
			Drain(a);
			GameMsg msg;
			auto is1056 = [](unsigned short act) { return [act](const GameMsg& m) { return m.usType == 1056 && Get16(m.buf, 8) == act; }; };
			std::string words;
			Assert::IsTrue(a.client.Send(Msg1056(1, b.idUser)));
			Assert::IsTrue(Expect(b, msg, is1056(1)), L"the target got no application");
			Assert::AreEqual((int)a.idUser, (int)Get32(msg.buf, 4));
			Assert::IsTrue(ExpectTalk(a, TXTATR_SYSTEM, words), L"the applicant got no text");
			Assert::AreEqual(std::string("STR_TRADING_REQEST_SENT"), words);
			Assert::IsTrue(b.client.Send(Msg1056(1, a.idUser)));
			Assert::IsTrue(Expect(a, msg, is1056(3)), L"the trade did not open for the first side");
			Assert::AreEqual((int)b.idUser, (int)Get32(msg.buf, 4));
			Assert::IsTrue(Expect(b, msg, is1056(3)), L"the trade did not open for the second side");

			Assert::IsTrue(a.client.Send(Msg1056(7, 50)));
			Assert::IsTrue(Expect(b, msg, is1056(8)), L"the other side was not told the money");
			Assert::AreEqual(50, (int)Get32(msg.buf, 4));
			Assert::IsTrue(a.client.Send(Msg1056(6, idPotion)));
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idPotion; }), L"the other side was not shown the item");
			Assert::IsTrue(a.client.Send(Msg1056(10, 0)));
			Assert::IsTrue(Expect(b, msg, is1056(10)), L"the other side was not told of the OK");
			Assert::IsTrue(b.client.Send(Msg1056(10, 0)));
			Assert::IsTrue(Expect(a, msg, is1056(4)), L"the trade did not succeed");
			Assert::IsTrue(Expect(b, msg, is1056(4)), L"the trade did not succeed for the other side");
			Drain(a);
			Drain(b);
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idPotion) + " AND player_id=" + std::to_string(b.idUser)),
				L"the potion is not the other side's");

			auto open = [&]()
			{
				Assert::IsTrue(a.client.Send(Msg1056(1, b.idUser)));
				Assert::IsTrue(Expect(b, msg, is1056(1)), L"a later trade's application did not arrive");
				Assert::IsTrue(b.client.Send(Msg1056(1, a.idUser)));
				Assert::IsTrue(Expect(a, msg, is1056(3)), L"a later trade did not open");
			};
			open();
			Assert::IsTrue(a.client.Send(Msg1056(7, 2000)));
			Assert::IsTrue(Expect(b, msg, is1056(8)), L"the other side was not told the 2000");
			Assert::IsTrue(a.client.Send(Msg1056(10, 0)));
			Assert::IsTrue(Expect(b, msg, is1056(10)), L"the other side was not told of the first OK");
			Assert::IsTrue(b.client.Send(Msg1056(10, 0)));
			Assert::IsTrue(Expect(b, msg, is1056(4)), L"a trade of money alone did not complete");
			Drain(a);
			Drain(b);

			open();
			Assert::IsTrue(a.client.Send(Msg1056(6, idBall)));
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idBall; }), L"the dragon ball was not shown");
			Assert::IsTrue(a.client.Send(Msg1056(10, 0)));
			Assert::IsTrue(Expect(b, msg, is1056(10)), L"the other side was not told of the OK");
			Assert::IsTrue(b.client.Send(Msg1056(10, 0)));
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return m.usType == 1056 && (Get16(m.buf, 8) == 15 || Get16(m.buf, 8) == 4); }),
				L"no answer to an unfair trade");
			Assert::AreEqual(15, (int)Get16(msg.buf, 8), L"an unfair trade completed");
			Assert::IsTrue(Expect(a, msg, is1056(15)), L"the giver was not told the trade is unfair");
			Assert::IsFalse(Comes(a, is1056(4)), L"an unfair trade completed for its giver");
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idBall) + " AND player_id=" + std::to_string(a.idUser)),
				L"the dragon ball changed hands");
			Assert::IsTrue(a.client.Send(Msg1056(2, 0)));
		});
	}

	TEST_METHOD(S1111_MessageBoard)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Brd", "level=20");
			std::string words = "selling potions " + p.strName;
			Assert::IsTrue(p.client.Send(Msg1004(2201, p.strName.c_str(), "ALLUSERS", words.c_str())));
			::Sleep(500);
			Assert::IsTrue(p.client.Send(Msg1111(0, 2201, 2)));
			GameMsg msg;
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1111 && m.buf[8] == 3; }), L"no list of the trade board");
			Assert::IsTrue(std::string(msg.buf.begin(), msg.buf.end()).find(p.strName) != std::string::npos, L"the post is not in the list");
			Assert::IsTrue(p.client.Send(Msg1111(0, 2201, 4, p.strName.c_str())));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return Is1004(m, 2201); }), L"the post's words were not sent");
			Assert::AreEqual(words, Read1004Str(msg, 3));
			Assert::IsTrue(p.client.Send(Msg1111(0, 2201, 1, p.strName.c_str())));
			::Sleep(500);
			Assert::IsTrue(p.client.Send(Msg1111(0, 2201, 4, p.strName.c_str())));
			Assert::IsFalse(Comes(p, [](const GameMsg& m) { return Is1004(m, 2201); }), L"a deleted post was read back");
		});
	}

	TEST_METHOD(S1107_1112_SyndicateMember)
	{
		Twice([&]()
		{
			auto member = [](unsigned long idUser)
			{
				MYSQL* h = Connect(GATE_DB);
				Exec(h, "INSERT cq_synattr SET id=" + std::to_string(idUser) + ", syn_id=8, rank=50, proffer=7");
				mysql_close(h);
			};
			Player a, b;
			NewPlayer(a, "Syn", "money=1000", member);
			NewPlayer(b, "Syn", NULL, member);
			Drain(a);
			GameMsg msg;
			Assert::IsTrue(a.client.Send(Msg1107(6, 8)));
			Assert::IsTrue(Expect(a, msg, [](const GameMsg& m) { return m.usType == 1015 && m.buf[8] == 3; }), L"no name for the syndicate");
			Assert::AreEqual(8, (int)Get32(msg.buf, 4));
			Assert::AreEqual(std::string("DigitalDevil"), std::string((const char*)&msg.buf[11], msg.buf[10]));
			Assert::IsTrue(a.client.Send(Msg1107(6, 77)));
			Assert::IsTrue(Expect(a, msg, [](const GameMsg& m) { return m.usType == 1015 && m.buf[8] == 22; }), L"no enemy name");
			Assert::AreEqual(77, (int)Get32(msg.buf, 4));
			Assert::AreEqual(std::string("MECH"), std::string((const char*)&msg.buf[11], msg.buf[10]));

			Assert::IsTrue(a.client.Send(Msg1107(12, 0)));
			Assert::IsTrue(Expect(a, msg, [](const GameMsg& m) { return m.usType == 1106; }), L"no syndicate attributes");

			Assert::IsTrue(a.client.Send(Msg1107(11, 100)));
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(a, ATTRIB_MONEY, dwMoney), L"no money update after the donation");
			Assert::AreEqual(900, (int)dwMoney);

			Assert::IsTrue(a.client.Send(Msg1004(2004, a.strName.c_str(), "ALLUSERS", "to my syndicate")));
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return Is1004(m, 2004) && Read1004Str(m, 3) == "to my syndicate"; }),
				L"the other member did not hear the syndicate channel");

			Assert::IsTrue(a.client.Send(Msg1112(b.strName.c_str())));
			Assert::IsTrue(Expect(a, msg, [](const GameMsg& m) { return m.usType == 1112; }), L"no member information");
			Assert::AreEqual(b.strName, std::string((const char*)&msg.buf[9]));

			std::string words;
			Assert::IsTrue(a.client.Send(Msg1107(4, b.idUser)));
			Assert::IsTrue(ExpectTalk(a, TXTATR_SYSTEM, words), L"no text for a member's kick");
			Assert::AreEqual(std::string("STR_NOT_AUTHORIZED"), words);
			Assert::IsTrue(a.client.Send(Msg1107(3, 0)));
			Assert::IsFalse(Comes(a, [&](const GameMsg& m) { return (m.usType == 1004 && Read1004Str(m, 1) == a.strName) || m.usType == 1107; }),
				L"a leave below 20000 proffer was answered");
		});
	}

	TEST_METHOD(S1027_GemEmbed)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0, idGem = 0;
			auto items = [&](unsigned long idUser)
			{
				idBlade = GiveItem(idUser, 410003, 1599, 1599, 0, true, 255, 0);
				idGem = GiveItem(idUser, 700001, 1, 1);
			};
			Player p;
			NewPlayer(p, "Gem", "recordmap_id=1011, recordx=245, recordy=240", items);
			Drain(p);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg1027(p.idUser, idBlade, idGem, 1, 0)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idBlade; }), L"no item update after the embedding");
			Assert::AreEqual(1, (int)msg.buf[0x13]);
			Assert::AreEqual(0, CountRows("cq_item", "id=" + std::to_string(idGem)));
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idBlade) + " AND gem1=1"));

			Assert::IsTrue(p.client.Send(Msg1027(p.idUser, idBlade, 0, 1, 1)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idBlade; }), L"no item update after the taking out");
			Assert::AreEqual(255, (int)msg.buf[0x13]);
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idBlade) + " AND gem1=255"));

			Player q;
			NewPlayer(q, "Gem", "recordmap_id=1002, recordx=430, recordy=380", items);
			Drain(q);
			Assert::IsTrue(q.client.Send(Msg1027(q.idUser, idBlade, idGem, 1, 0)));
			Assert::IsFalse(Comes(q, [&](const GameMsg& m) { return m.usType == 1008; }), L"an embedding far from the artisan was answered");
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idGem)));
		});
	}

	TEST_METHOD(S1028_Fuse)
	{
		Twice([&]()
		{
			unsigned long idWorn = 0, idWhole = 0;
			Player p;
			NewPlayer(p, "Fus", "recordmap_id=1002, recordx=467, recordy=328", [&](unsigned long idUser)
			{
				idWorn = GiveItem(idUser, 410003, 100, 1599);
				idWhole = GiveItem(idUser, 410003, 1599, 1599);
			});
			Drain(p);
			Assert::IsTrue(p.client.Send(Msg1028(35, 0, { idWorn })));
			Assert::IsTrue(p.client.Send(Msg1028(3, 0, { idWhole })));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1009; }), L"a refused fusion was answered");
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idWorn)));
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idWhole)));

			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg1028(3, 0, { idWorn })));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get32(m.buf, 4) == idWorn; }), L"the fused item was not taken away");
			Assert::AreEqual(0, CountRows("cq_item", "id=" + std::to_string(idWorn)));
		});
	}

	TEST_METHOD(S2036_Compose)
	{
		Twice([&]()
		{
			unsigned long idMain = 0, idAcc1 = 0, idAcc2 = 0, idPlain = 0;
			Player p;
			NewPlayer(p, "Cmp", NULL, [&](unsigned long idUser)
			{
				idMain = GiveItem(idUser, 410003, 1599, 1599);
				idAcc1 = GiveItem(idUser, 410003, 1599, 1599, 0, true, 0, 0, 1);
				idAcc2 = GiveItem(idUser, 410003, 1599, 1599, 0, true, 0, 0, 1);
				idPlain = GiveItem(idUser, 410003, 1599, 1599);
			});
			Drain(p);
			Assert::IsTrue(p.client.Send(Msg2036(0, { idMain, idAcc1, idPlain, 0, 0 })));
			Assert::IsFalse(Comes(p, [&](const GameMsg& m) { return m.usType == 1008 || m.usType == 1009; }), L"a refused composition was answered");
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idAcc1)));

			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg2036(0, { idMain, idAcc1, idAcc2, 0, 0 })));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idMain; }), L"no item update after the composition");
			Assert::AreEqual(1, (int)msg.buf[0x17]);
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idMain) + " AND magic3=1"));
			Assert::AreEqual(0, CountRows("cq_item", "id IN (" + std::to_string(idAcc1) + "," + std::to_string(idAcc2) + ")"));
		});
	}

	TEST_METHOD(S1102_Warehouse)
	{
		Twice([&]()
		{
			unsigned long idBlade = 0;
			Player p;
			NewPlayer(p, "Pkg", "recordmap_id=1002, recordx=410, recordy=352", [&](unsigned long idUser)
			{
				idBlade = GiveItem(idUser, 410003, 1599, 1599);
			});
			Drain(p);
			GameMsg msg;
			auto list = [&]() -> int
			{
				Assert::IsTrue(p.client.Send(Msg1102(8, 0, 10, 0)));
				Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1102 && m.buf[8] == 0; }), L"no list");
				Assert::AreEqual(8, (int)Get32(msg.buf, 4));
				return Get16(msg.buf, 0x0C);
			};
			Assert::AreEqual(0, list());

			Assert::IsTrue(p.client.Send(Msg1102(8, 1, 10, idBlade)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1102 && m.buf[8] == 1; }), L"no answer to the check-in");
			Assert::AreEqual(idBlade, Get32(msg.buf, 0x10));
			Assert::AreEqual(410003UL, Get32(msg.buf, 0x14));
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idBlade) + " AND owner_id=8"));
			Assert::AreEqual(1, list());
			Assert::AreEqual(idBlade, Get32(msg.buf, 0x10));

			Assert::IsTrue(p.client.Send(Msg1102(8, 2, 10, idBlade)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1102 && m.buf[8] == 2; }), L"no answer to the check-out");
			Assert::AreEqual(idBlade, Get32(msg.buf, 0x0C));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idBlade; }), L"the item did not come back");
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idBlade) + " AND owner_id=" + std::to_string(p.idUser) + " AND position=0"));
			Assert::AreEqual(0, list());
		});
	}

	TEST_METHOD(S1113_Dice)
	{
		Twice([&]()
		{
			Player p;
			NewPlayer(p, "Dic", "money=1100, recordmap_id=1002, recordx=433, recordy=437");
			Drain(p);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg2031(10050, 0)));
			int nMarket = -1;
			while(nMarket < 0 && Expect(p, msg, [](const GameMsg& m) { return m.usType == 2032 && m.buf[0x0B] == 2; }))
				if(Read2032Text(msg) == "Market")
					nMarket = msg.buf[0x0A];
			Assert::IsTrue(nMarket >= 0, L"the Conductress offered no link to the market");
			Drain(p);
			Assert::IsTrue(p.client.Send(Msg2032(0, (unsigned char)nMarket, 101)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 86; }), L"the Conductress did not lead to the market");
			Assert::AreEqual(211, (int)Get16(msg.buf, 0x10));
			Drain(p);

			const unsigned short cells[][2] = { { 211, 196 }, { 193, 178 }, { 175, 160 }, { 157, 142 }, { 141, 141 } };
			for(int i = 1; i < 5; i++)
			{
				unsigned long dwTarget = (unsigned long)cells[i][0] | ((unsigned long)cells[i][1] << 16);
				Assert::IsTrue(p.client.Send(Msg1010(p.idUser, dwTarget, cells[i-1][0], cells[i-1][1], 0, 133)));
				Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 133; }), L"a jump in the market was not answered");
				Assert::AreEqual((int)dwTarget, (int)Get32(msg.buf, 0x0C), L"a jump in the market did not land");
			}
			Drain(p);

			Assert::IsTrue(p.client.Send(Msg1015(9999, 17, p.strName.c_str())));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1015 && m.buf[8] == 17; }), L"no answer to the joining");
			Assert::AreEqual(9999, (int)Get32(msg.buf, 4));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1113 && m.buf[4] == 4; }), L"no countdown");

			Assert::IsTrue(p.client.Send(Msg1113(0, 9999, 3, 100)));
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after the stake");
			Assert::AreEqual(900, (int)dwMoney);
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1113 && m.buf[4] == 1; }), L"the stake was not confirmed");
			Assert::AreEqual(3, (int)msg.buf[0x0C]);
			Assert::AreEqual(100, (int)Get32(msg.buf, 0x10));

			Assert::IsTrue(p.client.Send(Msg1113(2, 9999, 22, 0)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1113 && m.buf[4] == 3; }), L"the cancel was not confirmed");
			Assert::IsTrue(ExpectAttrib(p, ATTRIB_MONEY, dwMoney), L"no money update after the cancel");
			Assert::AreEqual(1000, (int)dwMoney);

			Assert::IsTrue(p.client.Send(Msg1015(9999, 18, p.strName.c_str())));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1015 && m.buf[8] == 18; }), L"no answer to the leaving");
		});
	}

	TEST_METHOD(S2030_LayStatue)
	{
		Twice([&]()
		{
			static int s_nRun = 0;
			const unsigned short usX = s_nRun == 0 ? 140 : 130, usY = s_nRun == 0 ? 134 : 138;
			s_nRun++;
			unsigned long idStatue = 0;
			Player p;
			char szSet[96];
			sprintf(szSet, "recordmap_id=1038, recordx=%u, recordy=%u", usX + 2, usY + 2);
			NewPlayer(p, "Lay", szSet, [&](unsigned long idUser)
			{
				MYSQL* h = Connect(GATE_DB);
				Exec(h, "INSERT cq_synattr SET id=" + std::to_string(idUser) + ", syn_id=8, rank=100, proffer=7");
				mysql_close(h);
				idStatue = GiveItem(idUser, 720020, 199, 199);
			});
			Drain(p);
			GameMsg msg;
			Assert::IsTrue(p.client.Send(Msg1009(idStatue, 0, 4, p.idUser)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 2031; }), L"no request to lay the NPC");
			Assert::IsTrue(p.client.Send(Msg2030(usX, usY, 2, 0, 0)));
			Assert::IsTrue(Expect(p, msg, [&](const GameMsg& m) { return m.usType == 1014 && Get32(m.buf, 4) != p.idUser; }), L"the statue was not shown");
			unsigned long idNpc = Get32(msg.buf, 4);
			std::string words;
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text after the laying");
			Assert::AreEqual(std::string("STR_USE_ITEM"), words);
			Assert::AreEqual(0, CountRows("cq_item", "id=" + std::to_string(idStatue)));
			std::string where = "mapid=1038 AND cellx=" + std::to_string(usX) + " AND celly=" + std::to_string(usY);
			Assert::AreEqual(1, CountRows("cq_dynanpc", where + " AND type=9 AND ownerid=8"), L"the statue is not in cq_dynanpc");

			Assert::IsTrue(p.client.Send(Msg2031(idNpc, 3)));
			Assert::IsTrue(Expect(p, msg, [](const GameMsg& m) { return m.usType == 1008; }), L"the statue's item did not come back");
			Assert::AreEqual(720020UL, Get32(msg.buf, 8));
			Assert::IsTrue(ExpectTalk(p, TXTATR_SYSTEM, words), L"no text after the recycling");
			Assert::AreEqual(std::string("STR_GOT_ITEM"), words);
			Sleep(5000);
			Assert::AreEqual(0, CountRows("cq_dynanpc", where));
		});
	}

	TEST_METHOD(S2032_102_ClientTask)
	{
		Twice([&]()
		{
			auto rank = [](int nRank)
			{
				return [nRank](unsigned long idUser)
				{
					MYSQL* h = Connect(GATE_DB);
					Exec(h, "INSERT cq_synattr SET id=" + std::to_string(idUser) + ", syn_id=8, rank=" + std::to_string(nRank) + ", proffer=7");
					mysql_close(h);
				};
			};
			Player a, b;
			NewPlayer(a, "Kik", NULL, rank(100));
			NewPlayer(b, "Kik", NULL, rank(50));
			Drain(a);
			Drain(b);
			Assert::IsTrue(b.client.Send(Msg2032(31100, 0, 102, a.strName.c_str())));
			Assert::IsFalse(Comes(a, [&](const GameMsg& m) { return Is1004(m, TXTATR_SYSTEM) && Read1004Str(m, 1) == a.strName; }),
				L"a member's kick reached the leader");
			Assert::AreEqual(1, CountRows("cq_synattr", "id=" + std::to_string(a.idUser) + " AND syn_id=8"));

			Assert::IsTrue(a.client.Send(Msg2032(31100, 0, 102, b.strName.c_str())));
			std::string words;
			Assert::IsTrue(ExpectTalk(b, TXTATR_SYSTEM, words), L"the member was not told");
			Assert::AreEqual(std::string("STR_KICKOUT_ME_s"), words);
			Sleep(500);
			Assert::AreEqual(0, CountRows("cq_synattr", "id=" + std::to_string(b.idUser) + " AND syn_id=8"));
		});
	}

	TEST_METHOD(S1010_111_114_1009_021_024_Booth)
	{
		Twice([&]()
		{
			unsigned long idSold = 0, idKept = 0;
			Player s, b;
			NewPlayer(s, "Bth", "recordmap_id=1002, recordx=430, recordy=380", [&](unsigned long idUser)
			{
				idSold = GiveItem(idUser, 410003, 1599, 1599);
				idKept = GiveItem(idUser, 410003, 1599, 1599);
			});
			NewPlayer(b, "Bth", "money=1000, recordmap_id=1002, recordx=430, recordy=380");
			GameMsg msg;
			for(Player* p : { &s, &b })
			{
				Drain(*p);
				Assert::IsTrue(p->client.Send(Msg2031(9991, 0)));
				Assert::IsTrue(Expect(*p, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 86; }), L"the door did not lead to the market");
				Assert::AreEqual(187, (int)Get16(msg.buf, 0x10));
				Drain(*p);
			}

			Assert::IsTrue(s.client.Send(Msg1010(s.idUser, 0, 188, 263, 0, 111)));
			Assert::IsTrue(Expect(s, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 111; }), L"no booth");
			unsigned long idBooth = Get32(msg.buf, 0x0C);
			Assert::AreNotEqual(0UL, idBooth);
			for(unsigned long id : { idSold, idKept })
			{
				Assert::IsTrue(s.client.Send(Msg1009(id, 500, 22, s.idUser)));
				Assert::IsTrue(Expect(s, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 22 && Get32(m.buf, 4) == id; }), L"a good was not taken");
			}

			Sleep(500);
			Assert::IsTrue(b.client.Send(Msg1009(0, idBooth, 21, b.idUser)));
			int nGoods = 0;
			while(Expect(b, msg, [](const GameMsg& m) { return m.usType == 1108; }, 1500))
			{
				Assert::AreEqual(idBooth, Get32(msg.buf, 8));
				Assert::AreEqual(500UL, Get32(msg.buf, 0x0C));
				nGoods++;
			}
			Assert::AreEqual(2, nGoods);

			Assert::IsTrue(s.client.Send(Msg1004(2104, s.strName.c_str(), "ALL", "cheap blades")));
			Sleep(500);
			Assert::IsTrue(b.client.Send(Msg1010(b.idUser, s.idUser, b.usX, b.usY, 0, 115)));
			Assert::IsTrue(Expect(b, msg, [](const GameMsg& m) { return Is1004(m, 2104) && Read1004Str(m, 3) == "cheap blades"; }), L"no cry from the booth");

			Assert::IsTrue(b.client.Send(Msg1009(idSold, idBooth, 24, b.idUser)));
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1008 && Get32(m.buf, 4) == idSold; }), L"the bought item did not come");
			unsigned long dwMoney = 0;
			Assert::IsTrue(ExpectAttrib(b, ATTRIB_MONEY, dwMoney), L"no money update for the buyer");
			Assert::AreEqual(500, (int)dwMoney);
			std::string words;
			Assert::IsTrue(ExpectTalk(s, TXTATR_SYSTEM, words), L"the seller was not told");
			Assert::AreEqual(std::string("STR_BOOTH_BUY_sds"), words);
			Assert::AreEqual(1, CountRows("cq_item", "id=" + std::to_string(idSold) + " AND owner_id=" + std::to_string(b.idUser)));

			Assert::IsTrue(s.client.Send(Msg1009(idKept, idBooth, 23, s.idUser)));
			Assert::IsTrue(Expect(s, msg, [&](const GameMsg& m) { return m.usType == 1009 && Get16(m.buf, 0x0C) == 23 && Get32(m.buf, 4) == idKept; }), L"the good was not taken back");

			Assert::IsTrue(s.client.Send(Msg1010(s.idUser, 0, 188, 263, 0, 112)));
			Sleep(500);
			Assert::IsTrue(s.client.Send(Msg1010(s.idUser, 0, 188, 263, 0, 113)));
			Assert::IsTrue(Expect(s, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 113; }), L"the booth was not resumed");
			Assert::AreEqual(idBooth, Get32(msg.buf, 0x0C));
			Assert::IsTrue(s.client.Send(Msg1010(s.idUser, 0, 188, 263, 0, 114)));
			Sleep(500);
			Drain(b);
			Assert::IsTrue(b.client.Send(Msg1009(0, idBooth, 21, b.idUser)));
			Assert::IsFalse(Comes(b, [](const GameMsg& m) { return m.usType == 1108; }), L"a destroyed booth answered");
		});
	}

	TEST_METHOD(S1022_008_009_CourtMarry)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Mar", "recordmap_id=1002, recordx=430, recordy=380");
			NewPlayer(b, "Mar", "lookface=2001, recordmap_id=1002, recordx=430, recordy=380");
			Drain(a);
			Drain(b);
			GameMsg msg;
			Assert::IsTrue(a.client.Send(Msg1022(8, a.idUser, b.idUser, a.usX, a.usY, 0)));
			Assert::IsTrue(Expect(b, msg, [&](const GameMsg& m) { return m.usType == 1022 && Get16(m.buf, 0x14) == 8 && Get32(m.buf, 8) == a.idUser; }), L"the court did not reach the woman");

			Assert::IsTrue(b.client.Send(Msg1022(9, b.idUser, a.idUser, b.usX, b.usY, 0)));
			std::string words;
			Assert::IsTrue(ExpectTalk(a, 2011, words), L"no marriage announced");
			Assert::AreEqual(std::string("STR_MARRY"), words);
			Sleep(500);
			Assert::AreEqual(1, CountRows("cq_user", "id=" + std::to_string(a.idUser) + " AND mate='" + b.strName + "'"));
			Assert::AreEqual(1, CountRows("cq_user", "id=" + std::to_string(b.idUser) + " AND mate='" + a.strName + "'"));

			Player c;
			NewPlayer(c, "Mar", "lookface=2001, recordmap_id=1002, recordx=430, recordy=380");
			Drain(c);
			Assert::IsTrue(a.client.Send(Msg1022(8, a.idUser, c.idUser, a.usX, a.usY, 0)));
			Assert::IsFalse(Comes(c, [&](const GameMsg& m) { return m.usType == 1022 && Get16(m.buf, 0x14) == 8; }), L"a married man's court was passed on");
		});
	}

	TEST_METHOD(S1022_020_025_Others)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Int", "recordmap_id=1002, recordx=430, recordy=380");
			NewPlayer(b, "Int", "recordmap_id=1002, recordx=430, recordy=380");
			Drain(a);
			GameMsg msg;
			for(unsigned short usKind : { 10, 1, 4, 5, 15, 16 })
				Assert::IsTrue(a.client.Send(Msg1022(usKind, a.idUser, b.idUser, a.usX, a.usY, 0)));
			Assert::IsFalse(Comes(a, [&](const GameMsg& m) { return m.usType == 1022 || (Is1004(m, TXTATR_SYSTEM) && Read1004Str(m, 1) == a.strName); }),
				L"an ignored interaction was answered");
			Assert::IsFalse(a.client.IsClosed(), L"an ignored interaction closed the connection");
			Assert::IsTrue(a.client.Send(Msg1022(22, a.idUser, b.idUser, a.usX, a.usY, 0)));
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1022 && Get16(m.buf, 0x14) == 22 && Get32(m.buf, 8) == a.idUser; }), L"the abort was not echoed");
			Assert::IsTrue(a.client.Send(Msg1022(20, a.idUser, b.idUser, a.usX, a.usY, a.usX | (unsigned long)a.usY << 16)));
			Assert::IsTrue(Expect(a, msg, [&](const GameMsg& m) { return m.usType == 1022 && Get16(m.buf, 0x14) == 20 && Get32(m.buf, 8) == a.idUser; }), L"the rush was not echoed");

			Player c, d;
			NewPlayer(c, "Int");
			NewPlayer(d, "Int");
			Drain(c);
			Assert::IsTrue(c.client.Send(Msg1022(25, c.idUser, d.idUser, c.usX, c.usY, 0)));
			std::string words;
			Assert::IsTrue(ExpectTalk(c, TXTATR_SYSTEM, words), L"no text for a shot on the newbie map");
			Assert::AreEqual(std::string("STR_NO_PK_VILLAGE"), words);
		});
	}

	TEST_METHOD(S1022_021_MagicAttack)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Mag", "recordmap_id=1002, recordx=430, recordy=380");
			NewPlayer(b, "Mag", "recordmap_id=1002, recordx=430, recordy=380");
			Drain(a);
			GameMsg msg;
			Assert::IsTrue(a.client.Send(Msg1022Magic(a.idUser, 1010, 0, b.idUser, b.usX, b.usY)));
			Assert::IsTrue(Expect(a, msg, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 103; }), L"the unknown magic was not aborted");
			Assert::IsFalse(a.client.IsClosed());

			Sleep(100);
			Assert::IsTrue(a.client.Send(Msg1022Magic(a.idUser, 1010, 0, b.idUser, b.usX, b.usY, true)));
			Assert::IsFalse(Comes(a, [](const GameMsg& m) { return m.usType == 1010 && Get16(m.buf, 0x16) == 103; }, 3000), L"a spoiled check byte was answered");
			Assert::IsTrue(a.client.IsClosed(), L"a spoiled check byte did not close the connection");
		});
	}

	TEST_METHOD(S1022_002_MonsterExpRate)
	{
		static unsigned long s_idMonster = 400100;
		Twice([&]()
		{
			CNpcLinkStandIn link;
			link.Login(g_nPort);
			Player a;
			NewPlayer(a, "Bxp", "level=110, exp=0, recordmap_id=1015, recordx=300, recordy=300",
						[](unsigned long idUser) { GiveItem(idUser, 410215, 7198, 7198, 4); });
			unsigned long idReal = ++s_idMonster, idControl = ++s_idMonster;
			link.CreateMonster(idReal, 2033, 1015, 301, 300);
			link.CreateMonster(idControl, 2998, 1015, 300, 301);
			Drain(a);

			MYSQL* h = Connect(GATE_DB);
			std::vector<std::vector<std::string> > rows = Query(h,
				"SELECT exp_factor FROM cq_disdain WHERE delta_lev=0 AND type IN (0, 2033) ORDER BY type DESC LIMIT 1");
			mysql_close(h);
			Assert::IsFalse(rows.empty(), L"cq_disdain has no row for the level difference 0");
			int nFactor = atoi(rows[0][0].c_str());

			unsigned long dwExp = 0;
			auto hit = [&](unsigned long idTarget, unsigned long& dwDamage, unsigned long& dwGained)
			{
				Assert::IsTrue(a.client.Send(Msg1022(2, a.idUser, idTarget, a.usX, a.usY, 0)), L"the attack could not be sent");
				dwDamage = 0;
				GameMsg msg;
				bool bDone = RecvUntil(a.client, msg, 20000, [&](const GameMsg& m)
				{
					if(m.usType == 1022 && Get16(m.buf, 0x14) == 2 && Get32(m.buf, 8) == a.idUser && Get32(m.buf, 0x0C) == idTarget
							&& dwDamage == 0)
						dwDamage = Get32(m.buf, 0x18);
					if(m.usType != 1017)
						return false;
					for(unsigned long i = 0, n = Get32(m.buf, 8); i < n && 0x0C + i * 8 + 8 <= m.buf.size(); i++)
						if(Get32(m.buf, 0x0C + i * 8) == 5)
						{
							unsigned long dwNew = Get32(m.buf, 0x0C + i * 8 + 4);
							if(dwDamage == 0)
								dwExp = dwNew;
							else
							{
								dwGained = dwNew - dwExp;
								dwExp = dwNew;
								return true;
							}
						}
					return false;
				});
				Assert::IsTrue(bDone && dwDamage > 0, L"no hit with damage and its experience within 20 s");
			};
			auto expected = [&](unsigned long dwDamage, int nRate) { return (unsigned long)((nFactor + 100) * ((int)dwDamage + (int)dwDamage * nRate / 100) / 100); };

			unsigned long dwDamage = 0, dwGained = 0;
			hit(idReal, dwDamage, dwGained);
			Log("  NightDevil 2033 (extra_exp 100): damage %lu, experience %lu; with the rate 110 %lu, without %lu (disdain factor %d)",
				dwDamage, dwGained, expected(dwDamage, 110), expected(dwDamage, 0), nFactor);
			Assert::AreEqual(expected(dwDamage, 110), dwGained, L"4351's monster: not the experience with the battle-power rate");
			Assert::IsTrue(dwDamage >= 10, L"a hit below 10 cannot tell the rate 110 from 100");
			Assert::AreNotEqual(expected(dwDamage, 100), dwGained, L"the experience of the rate 100: the battle power is not 110");

			hit(idControl, dwDamage, dwGained);
			Log("  the control 2998 (extra_exp 0): damage %lu, experience %lu; without the rate %lu", dwDamage, dwGained, expected(dwDamage, 0));
			Assert::AreEqual(expected(dwDamage, 0), dwGained, L"the control (extra_exp 0): not the experience without the rate");
			link.Close();
		});
	}

	TEST_METHOD(S1056_013_Emoney)
	{
		Twice([&]()
		{
			Player a, b;
			NewPlayer(a, "Emo", NULL, [](unsigned long idUser)
			{
				MYSQL* h = Connect(GATE_DB);
				Exec(h, "UPDATE cq_user SET emoney=100, chk_sum=((3118018 + account_id) ^ 100) WHERE id=" + std::to_string(idUser));
				mysql_close(h);
			});
			NewPlayer(b, "Emo");
			Drain(a);
			GameMsg msg;
			auto is1056 = [](unsigned short act) { return [act](const GameMsg& m) { return m.usType == 1056 && Get16(m.buf, 8) == act; }; };
			Assert::IsTrue(a.client.Send(Msg1056(1, b.idUser)));
			Assert::IsTrue(Expect(b, msg, is1056(1)), L"the target got no application");
			Assert::IsTrue(b.client.Send(Msg1056(1, a.idUser)));
			Assert::IsTrue(Expect(a, msg, is1056(3)), L"the trade did not open");

			Assert::IsTrue(a.client.Send(Msg1056(13, 500)));
			Assert::IsTrue(Expect(a, msg, is1056(14)), L"no total for e-money one lacks");
			Assert::AreEqual(0, (int)Get32(msg.buf, 4));
			Assert::IsTrue(a.client.Send(Msg1056(13, 50)));
			Assert::IsTrue(Expect(a, msg, is1056(14)), L"no total for the e-money");
			Assert::AreEqual(50, (int)Get32(msg.buf, 4));
			Assert::IsTrue(Expect(b, msg, is1056(12)), L"the other side was not told the e-money");
			Assert::AreEqual(50, (int)Get32(msg.buf, 4));
			Assert::IsTrue(a.client.Send(Msg1056(2, 0)));
		});
	}

	TEST_METHOD(W1_ScriptWalk)
	{
		char szEnv[8] = "";
		if(!::GetEnvironmentVariableA("SFK_SCRIPTWALK", szEnv, sizeof(szEnv)) || strcmp(szEnv, "1") != 0)
		{
			Log("  W1_ScriptWalk: skipped (set SFK_SCRIPTWALK=1 to run it)");
			return;
		}
		SYSTEMTIME t;
		::GetLocalTime(&t);
		char szSys[64];
		sprintf(szSys, "\\syslog\\sys %d-%d-%d 0.log", t.wYear, t.wMonth, t.wDay);
		const std::string logs[] = { LogPath(), g_strRun + szSys };

		MYSQL* h = Connect(GATE_DB);
		std::vector<std::vector<std::string> > npcs = Query(h,
			"SELECT id, name, mapid, cellx, celly, 'npc' FROM cq_npc WHERE task0<>0 OR task1<>0 OR task2<>0 OR task3<>0 UNION ALL "
			"SELECT d.id, d.name, d.mapid, d.cellx, d.celly, 'dynanpc' FROM cq_dynanpc d JOIN ("
			" SELECT task0, MIN(id) AS id FROM cq_dynanpc WHERE task0<>0 AND mapid IN (SELECT id FROM cq_map) GROUP BY task0 UNION "
			" SELECT task0, MIN(id) FROM cq_dynanpc WHERE task0<>0 AND task0 NOT IN (SELECT task0 FROM cq_dynanpc WHERE mapid IN (SELECT id FROM cq_map)) GROUP BY task0"
			") r ON r.id = d.id ORDER BY 6 DESC, 1");
		mysql_close(h);
		char szIds[4096] = "";
		if(::GetEnvironmentVariableA("SFK_SCRIPTWALK_IDS", szIds, sizeof(szIds)) && szIds[0])
		{
			std::vector<std::vector<std::string> > chosen;
			for(char* p = strtok(szIds, ","); p; p = strtok(NULL, ","))
				for(size_t i = 0; i < npcs.size(); i++)
					if(npcs[i][0] == p)
					{
						chosen.push_back(npcs[i]);
						break;
					}
			npcs.swap(chosen);
		}
		Log("  W1_ScriptWalk: %u NPCs to walk", (unsigned)npcs.size());

		Player g;
		int nLogins = 0;
		std::string report;
		auto login = [&]()
		{
			for(int nTry = 1; ; nTry++)
			{
				g = Player();
				try
				{
					NewPlayer(g, "Wk", "name=CONCAT('[GM]',name), level=120, money=10000000");
					break;
				}
				catch(...)
				{
					report += "  (a walker's login failed, try " + std::to_string(nTry) + ")\r\n";
					if(nTry >= 3)
						throw;
					::Sleep(5000);
				}
			}
			g.strName = "[GM]" + g.strName;
			nLogins++;
		};
		login();

		int nWithLog = 0, nUnreached = 0, nSteps = 0;
		bool bControl = false;
		std::string strPrev = "none";
		auto collect = [&](std::vector<unsigned char>& links, std::string& text)
		{
			GameMsg m;
			DWORD tEnd = ::GetTickCount() + 4000;
			while(::GetTickCount() < tEnd && g.client.Recv(m, 400))
			{
				if(m.usType == 2032 && m.buf.size() > 0x0B)
				{
					if(m.buf[0x0B] == 2)
						links.push_back(m.buf[0x0A]);
					else if(m.buf[0x0B] == 1 && text.empty())
						text = Read2032Text(m).substr(0, 60);
				}
			}
		};
		for(size_t i = 0; i < npcs.size(); i++)
		{
			const std::vector<std::string>& n = npcs[i];
			if(g.client.IsClosed())
				login();
			size_t nFrom[2] = { ReadFile(logs[0]).size(), ReadFile(logs[1]).size() };
			int x = atoi(n[3].c_str()), y = atoi(n[4].c_str());
			auto fly = [&]() -> bool
			{
				const int off[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
				for(int k = 0; k < 4; k++)
				{
					std::string where = n[2] + " " + std::to_string(x + off[k][0]) + " " + std::to_string(y + off[k][1]);
					g.client.Send(Msg1004(2000, g.strName.c_str(), "ALLUSERS", ("/chgmap " + where).c_str()));
					GameMsg m;
					bool bMoved = RecvUntil(g.client, m, 1500, [&](const GameMsg& mm) { return mm.usType == 1010 && Get32(mm.buf, 8) == g.idUser; });
					Drain(g, 300);
					if(bMoved)
						return true;
				}
				return false;
			};
			if(!fly())
			{
				report += "  (the walker did not move: a new one, after npc " + strPrev + ")\r\n";
				char szHold[16] = "";
				if(::GetEnvironmentVariableA("SFK_SCRIPTWALK_HOLD", szHold, sizeof(szHold)) && atoi(szHold) > 0)
				{
					WriteText(g_strRun + "\\scriptwalk.txt", "; W1_ScriptWalk: held\r\n" + report);
					::Sleep(atoi(szHold) * 1000);
				}
				login();
				if(!fly())
					report += "  (the new walker did not move either)\r\n";
			}

			std::vector<unsigned char> links;
			std::string text;
			g.client.Send(Msg2031(strtoul(n[0].c_str(), NULL, 10), 0));
			collect(links, text);
			nSteps++;
			std::string line = n[5] + " " + n[0] + " [" + n[1] + "] map " + n[2] + " (" + n[3] + "," + n[4] + "): " +
				std::to_string(links.size()) + " links" + (text.empty() ? ", no text" : "") + "\r\n";
			if(text.empty() && links.empty())
				nUnreached++;
			for(size_t k = 0; k < links.size() && k < 12 && !g.client.IsClosed(); k++)
			{
				std::vector<unsigned char> more;
				std::string text2;
				g.client.Send(Msg2031(strtoul(n[0].c_str(), NULL, 10), 0));
				collect(more, text2);
				g.client.Send(Msg2032(0, links[k], 101));
				more.clear();
				text2.clear();
				collect(more, text2);
				nSteps++;
			}
			if(g.client.IsClosed())
				line += "    (the connection closed)\r\n";
			::Sleep(200);
			std::string logged;
			for(int f = 0; f < 2; f++)
			{
				std::string all = ReadFile(logs[f]);
				if(all.size() > nFrom[f])
					logged += all.substr(nFrom[f]);
			}
			if(!logged.empty())
			{
				nWithLog++;
				size_t p = 0;
				while(p < logged.size())
				{
					size_t e = logged.find('\n', p);
					std::string l = logged.substr(p, e == std::string::npos ? std::string::npos : e - p);
					while(!l.empty() && (l.back() == '\r' || l.back() == '\n'))
						l.pop_back();
					if(!l.empty())
						line += "    " + l + "\r\n";
					if(e == std::string::npos)
						break;
					p = e + 1;
				}
				if(n[0] == "9994" && logged.find("990006") != std::string::npos && logged.find("990099") != std::string::npos)
					bControl = true;
			}
			report += line;
			strPrev = n[0];
			WriteText(g_strRun + "\\scriptwalk.txt", "; W1_ScriptWalk: in progress\r\n" + report);
		}
		char szHead[256];
		sprintf(szHead, "; W1_ScriptWalk (msgscenario_tests.cpp): %u NPCs, %d steps, %d with log lines, %d without a dialog, %d logins\r\n",
			(unsigned)npcs.size(), nSteps, nWithLog, nUnreached, nLogins);
		WriteText(g_strRun + "\\scriptwalk.txt", std::string(szHead) + report);
		Log("  %s", szHead);
		Assert::IsTrue(bControl, L"the planted fault (cq_npc 9994) was not reported");
	}
};
