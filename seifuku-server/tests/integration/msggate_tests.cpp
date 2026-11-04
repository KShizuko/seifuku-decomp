#include "msggate_env.h"

TEST_CLASS(MsgGateTests)
{
public:
	TEST_CLASS_INITIALIZE(Setup)
	{
		g_pWinsock = new WinsockGuard;
		FindPaths();
		BuildGateDatabase();
		BuildRunDirectory();
		Log("cq_msggate: %d tables copied from cq_merged, %d item types (the 4351 client's); %d synthetic map documents",
				g_nCopied, g_nItemTypes, g_nMapDocs);
	}

	TEST_CLASS_CLEANUP(Teardown)
	{
		delete g_pWinsock;
		g_pWinsock = NULL;
	}

	TEST_METHOD(M1_StartsAndLogsIntoTheAccountServer)
	{
		Log("run 1:");
		RunResult a = RunGate(SERVER_ID);
		Assert::IsTrue(a.bLoginOK, L"the log has no \"Account server login OK\"");
		Assert::IsFalse(a.bLoginFailed);
		Assert::IsTrue(a.bGamePortOpen, L"the game port does not accept");
		Log("run 2:");
		RunResult b = RunGate(SERVER_ID);
		Assert::IsTrue(b.bLoginOK && b.bGamePortOpen);
		for(size_t i = 0; i < a.echo.size() && i < b.echo.size(); i++)
			if(a.echo[i] != b.echo[i])
				Log("  the echoed 1002 differs at +%02Xh: %02X / %02X", (unsigned)i, a.echo[i], b.echo[i]);
		Assert::IsTrue(a.login == b.login, L"the 1099 differs between two runs");
		Assert::IsTrue(a.echo == b.echo, L"the echoed 1002 differs between two runs");
		Log("determinism: the 1099 (%u bytes) and the echoed 1002 (%u bytes) match", (unsigned)a.login.size(), (unsigned)a.echo.size());
	}

	TEST_METHOD(M2_PositiveControl_RefusedLoginIsReported)
	{
		RunResult r = RunGate(0);
		Assert::IsTrue(r.bLoginFailed, L"the log has no \"Account server login failed!\" after a refusal");
		Assert::IsFalse(r.bLoginOK);
		Assert::IsTrue(r.echo.empty());
	}

	TEST_METHOD(M4_ScriptedClientCreatesARoleLogsInAndWalks)
	{
		Log("run 1:");
		Transcript a = RunScriptedClient();
		Log("run 2:");
		Transcript b = RunScriptedClient();
		for(size_t i = 0; i < a.lines.size() && i < b.lines.size(); i++)
			if(a.lines[i] != b.lines[i])
				Log("  differs: \"%s\" / \"%s\"", a.lines[i].c_str(), b.lines[i].c_str());
		Assert::IsTrue(a.lines == b.lines, L"the scripted client's transcript differs between two runs");
		Log("determinism: the %u transcript lines match", (unsigned)a.lines.size());
	}

	TEST_METHOD(M5_PositiveControl_WrongKeyIsNotAnswered)
	{
		CAccountStandIn standin;
		CMsgServerProcess server;
		int nGamePort = 0;
		RunResult r = StartGate(standin, server, nGamePort, SERVER_ID);
		Assert::IsTrue(r.bLoginOK && r.bGamePortOpen);
		CScriptedClient client;
		ClientLogin(standin, client, nGamePort, PLAYER_ACCOUNT_ID + 1, 0x0BADF00D, 0x0BADF00D ^ 1);
		GameMsg msg;
		std::string words;
		bool bAnswered = RecvUntil(client, msg, 3000, [&](const GameMsg& m) { return Is1004(m, 2101, &words); });
		Log("  a wrong key: %s", bAnswered ? ("answered \"" + words + "\"").c_str() : "no answer within 3 s");
		Assert::IsFalse(bAnswered, L"a client with the wrong key was answered");
		server.Kill();
	}

	TEST_METHOD(M6_NpcServerLinksToMsgServer)
	{
		NpcLinkResult r[2];
		for(int n = 0; n < 2; n++)
		{
			Log("run %d:", n + 1);
			r[n] = RunNpcLink(40);
			size_t nAnswered = 0;
			std::string err = CheckNpcLink(r[n], &nAnswered);
			Log("  NpcServer's login: version %08lX%s; MsgServer's log %s \"NPC server login OK.\"; %u of %u requests answered",
				r[n].up.empty() ? 0 : r[n].up[0].idNpc,
				!r[n].up.empty() && r[n].up[0].idNpc == LINK_VERSION_RELOGIN ? "" : "",
				r[n].bLoginOK ? "has" : "lacks", (unsigned)nAnswered, (unsigned)(r[n].up.empty() ? 0 : r[n].up.size() - 1));
			if(!r[n].bLoginOK)
				Fail("MsgServer's log has no \"NPC server login OK.\"");
			if(!err.empty())
				Fail("the NPC link: " + err);
		}
		std::vector<Bytes> a = NpcLinkTranscript(r[0]), b = NpcLinkTranscript(r[1]);
		Assert::IsTrue(a.size() == 22 && b.size() == 22, L"fewer than 20 requests in a run");
		Assert::IsTrue(a[0] == b[0] && a[1] == b[1], L"the login or the answer differs between two runs");
		std::vector<unsigned long> wa, wb;
		for(int k = 0; k < 2; k++)
		{
			const std::vector<Bytes>& x = k ? b : a;
			const std::vector<Bytes>& y = k ? a : b;
			std::vector<unsigned long>& w = k ? wb : wa;
			for(size_t i = 2; i < x.size(); i++)
			{
				unsigned long gen = Get32(x[i], 8);
				bool bInOther = false;
				for(size_t j = 2; j < y.size() && !bInOther; j++)
					bInOther = Get32(y[j], 8) == gen;
				if(bInOther && std::find(w.begin(), w.end(), gen) == w.end())
					w.push_back(gen);
			}
		}
		Assert::IsTrue(wa.size() >= 2, L"fewer than two generators asked in both runs: the walk order is not measured");
		Assert::IsTrue(wa == wb, L"the generators are asked in another order in the second run");
		Log("determinism: the login and the answer match; %u generators asked in both runs' first 20 requests, in the same order",
			(unsigned)wa.size());
	}

	TEST_METHOD(M7_PositiveControl_LinkCheckSeesPlantedDifferences)
	{
		NpcLinkResult r = {};
		r.bLoginOK = r.bFramed = true;
		LinkFrame login = { 0x28, 1002, LINK_VERSION_RELOGIN };
		login.body.assign(0x20, 0);
		PutStr(login.body, 0, LINK_ACCOUNT, 16);
		LinkFrame req = { 0x28, 20001, 0 };
		req.body.assign(0x20, 0);
		Put32(req.body, 0, 400001);
		Put32(req.body, 4, 1052);
		Put16(req.body, 0x18, 1);
		Put16(req.body, 0x1A, 6997);
		LinkFrame talk = { 0, 1004, 0 };
		talk.body.assign(0x0C, 0);
		Put16(talk.body, 4, 2101);
		const char* strs[] = { "SYSTEM", "ALLUSERS", "", "ANSWER_OK" };
		talk.body.push_back(4);
		for(int i = 0; i < 4; i++) { talk.body.push_back((unsigned char)strlen(strs[i])); talk.body.insert(talk.body.end(), strs[i], strs[i] + strlen(strs[i])); }
		LinkFrame ans = req;
		ans.idNpc = 400001;
		r.up.push_back(login); r.up.push_back(req);
		r.down.push_back(talk); r.down.push_back(ans);
		size_t n = 0;
		Assert::AreEqual(std::string(""), CheckNpcLink(r, &n));
		Assert::AreEqual((size_t)1, n);

		NpcLinkResult bad = r; bad.down[0].body[0x0C + 1 + 1 + 6 + 1 + 8 + 1 + 1] ^= 1;
		Assert::AreNotEqual(std::string(""), CheckNpcLink(bad, &n));
		bad = r; bad.down[1].idNpc = 400002;
		Assert::AreNotEqual(std::string(""), CheckNpcLink(bad, &n));
		bad = r; bad.down[1].body[0x14] ^= 1;
		Assert::AreNotEqual(std::string(""), CheckNpcLink(bad, &n));
		bad = r; bad.down.pop_back();
		Assert::AreNotEqual(std::string(""), CheckNpcLink(bad, &n));
		bad = r; bad.up.erase(bad.up.begin());
		Assert::AreNotEqual(std::string(""), CheckNpcLink(bad, &n));
		bad = r; bad.bFramed = false;
		Assert::AreNotEqual(std::string(""), CheckNpcLink(bad, &n));
	}

	TEST_METHOD(M3_PositiveControl_LoginCheckSeesPlantedDifferences)
	{
		CAccountStandIn::Message msg;
		msg.usType = CAccountStandIn::MSG_ACCOUNT;
		msg.buf.assign(CAccountStandIn::SIZE_ACCOUNT, 0);
		*(unsigned short*)&msg.buf[0] = CAccountStandIn::SIZE_ACCOUNT;
		*(unsigned short*)&msg.buf[2] = CAccountStandIn::MSG_ACCOUNT;
		strcpy((char*)&msg.buf[4], SERVER_ACCOUNT);
		strcpy((char*)&msg.buf[0x14], SERVER_PASSWORD);
		strcpy((char*)&msg.buf[0x24], SERVER_NAME);
		*(int*)&msg.buf[0x34] = 5816;
		Assert::AreEqual(std::string(""), CheckAccountLogin(msg, 5816));
		const int fields[] = { 4, 0x14, 0x24 };
		for(int i = 0; i < 3; i++)
		{
			CAccountStandIn::Message bad = msg;
			bad.buf[fields[i]] ^= 1;
			Assert::AreNotEqual(std::string(""), CheckAccountLogin(bad, 5816));
		}
		Assert::AreNotEqual(std::string(""), CheckAccountLogin(msg, 5817));
		CAccountStandIn::Message shortMsg = msg;
		shortMsg.buf.pop_back();
		Assert::AreNotEqual(std::string(""), CheckAccountLogin(shortMsg, 5816));
	}
};
