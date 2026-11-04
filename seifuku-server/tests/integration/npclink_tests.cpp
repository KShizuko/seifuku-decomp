#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include "mysql.h"
#include "realmaps.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	const char*	DB_HOST	= "127.0.0.1";
	const char*	DB_USER	= "root";
	const char*	DB_PASS	= "";
	const unsigned int DB_PORT = 3306;
	const char*	GATE_DB	= "cq_npcgate";

	const char*			LINK_ACCOUNT	= "NpcServer Tqdig";
	const char*			LINK_PASSWORD	= "skl8934##VJICW ";
	const unsigned long	LINK_VERSION	= 0x3ADE68B1;
	const unsigned long	LINK_VERSION_RELOGIN	= 0x3ADE68B2;

	const unsigned short	MSG_LOGIN		= 1002;
	const unsigned short	MSG_TALK		= 1004;
	const unsigned short	MSG_AINPCINFO	= 20001;

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
			Fail(sql + ": " + mysql_error(h));
		MYSQL_RES* res = mysql_store_result(h);
		if(res)
			mysql_free_result(res);
	}

	std::vector<std::vector<std::string> > Query(MYSQL* h, const std::string& sql)
	{
		std::vector<std::vector<std::string> > rows;
		if(mysql_query(h, sql.c_str()) != 0)
			Fail(sql + ": " + mysql_error(h));
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

	void BuildGateDatabase()
	{
		MYSQL* h = Connect(NULL);
		Exec(h, std::string("DROP DATABASE IF EXISTS ") + GATE_DB);
		Exec(h, std::string("CREATE DATABASE ") + GATE_DB + " DEFAULT CHARACTER SET latin1");
		const char* copied[] = { "cq_map", "cq_generator", "cq_monstertype", "cq_syndicate", "cq_dynamap", "cq_user", "cq_itemtype" };
		for(int i = 0; i < sizeof(copied)/sizeof(copied[0]); i++)
		{
			Exec(h, std::string("CREATE TABLE ") + GATE_DB + "." + copied[i] + " LIKE cq_merged." + copied[i]);
			Exec(h, std::string("INSERT INTO ") + GATE_DB + "." + copied[i] + " SELECT * FROM cq_merged." + copied[i]);
		}
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
		g_strRun = g_strRepo + "\\db\\npcgate";
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

	int		g_nMaps = 0, g_nMapDocs = 0;
	long long	g_nCells = 0;

	void BuildRunDirectory()
	{
		::CreateDirectoryA((g_strRepo + "\\db").c_str(), NULL);
		::CreateDirectoryA(g_strRun.c_str(), NULL);
		::CreateDirectoryA((g_strRun + "\\ini").c_str(), NULL);
		::CreateDirectoryA((g_strRun + "\\map").c_str(), NULL);
		::CreateDirectoryA((g_strRun + "\\map\\synth").c_str(), NULL);
		::CreateDirectoryA((g_strRun + "\\syslog").c_str(), NULL);
		::CreateDirectoryA((g_strRun + "\\gmlog").c_str(), NULL);

		WriteText(g_strRun + "\\shell.ini",
			"; SYNTHETIC (npclink_tests.cpp)\r\n[AccountServer]\r\nSERVERNAME=npcgate\r\n"
			"[Language]\r\nStringFile=ini\\StrRes.ini\r\n");
		WriteText(g_strRun + "\\ini\\StrRes.ini",
			"STR_GUARD_CALL_s=synthetic guard call %s\r\nSTR_PPKER_CALL_s=synthetic pker call %s\r\n");
		WriteText(g_strRun + "\\ini\\AdditiveSize.ini", "; SYNTHETIC\r\n[Header]\r\nAmount=0\r\n");
		WriteText(g_strRun + "\\ini\\TerrainNpc.ini", "; SYNTHETIC: no terrain item\r\n");

		if(realmaps::Enabled())
		{
			std::string err = realmaps::Use(g_strRepo, g_strRun, g_nMapDocs);
			if(!err.empty())
				Fail("the 4351 maps: " + err);
			return;
		}

		MYSQL* h = Connect(GATE_DB);
		std::vector<std::vector<std::string> > docs = Query(h,
			"SELECT m.mapdoc, COUNT(DISTINCT m.id), "
			"IFNULL(MAX(GREATEST(g.bound_x + g.bound_cx, g.born_x + 1)), 0), "
			"IFNULL(MAX(GREATEST(g.bound_y + g.bound_cy, g.born_y + 1)), 0) "
			"FROM cq_map m LEFT JOIN cq_generator g ON g.mapid = m.id GROUP BY m.mapdoc ORDER BY m.mapdoc");
		mysql_close(h);

		FILE* fp = fopen((g_strRun + "\\ini\\GameMap.dat").c_str(), "wb");
		if(!fp)
			Fail("cannot write GameMap.dat");
		PutI32(fp, (int)docs.size());
		g_nMaps = 0; g_nCells = 0;
		for(size_t i = 0; i < docs.size(); i++)
		{
			unsigned long idDoc = strtoul(docs[i][0].c_str(), NULL, 10);
			int nMaps = atoi(docs[i][1].c_str());
			int cx = max(atoi(docs[i][2].c_str()) + 2, 16);
			int cy = max(atoi(docs[i][3].c_str()) + 2, 16);
			char name[64];
			sprintf(name, "map\\synth\\%lu.dmap", idDoc);
			WriteFlatMap(g_strRun + "\\" + name, cx, cy);
			fwrite(&idDoc, 4, 1, fp);
			PutI32(fp, (int)strlen(name));
			fwrite(name, 1, strlen(name), fp);
			PutI32(fp, 256);
			g_nMaps += nMaps;
			g_nCells += (long long)cx * cy * nMaps;
		}
		fclose(fp);
		g_nMapDocs = (int)docs.size();
	}

	void WriteConfig(int nPort)
	{
		char buf[1024];
		sprintf(buf,
			"; SYNTHETIC (npclink_tests.cpp)\r\n"
			"[GameServer]\r\nGAMESERVER_IP=127.0.0.1\r\nGAMESERVER_PORT=%d\r\nSND_SIZE=65536\r\nRCV_SIZE=65536\r\n"
			"MaxNpcPerTimer=20\r\n"
			"[Database]\r\nDBServer=127.0.0.1\r\nLoginName=root\r\nPassword=\r\nDBName=%s\r\nDBPort=3306\r\n"
			"[InternetPort]\r\nPORT_SIZE=0\r\nCURRENT_PORTID=0\r\n"
			"[System]\r\nSERVERNAME=npcgate\r\n", nPort, GATE_DB);
		WriteText(g_strRun + "\\config.ini", buf);
	}

	struct LinkPacket
	{
		unsigned short		size, type;
		unsigned long		idNpc;
		std::vector<char>	body;
		std::vector<char>	raw;
	};

	class CStandIn
	{
	public:
		CStandIn() : m_sListen(INVALID_SOCKET), m_sLink(INVALID_SOCKET), m_nPort(0) {}
		~CStandIn() { Close(); }

		void Listen()
		{
			m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			addr.sin_port = 0;
			if(bind(m_sListen, (sockaddr*)&addr, sizeof(addr)) != 0 || listen(m_sListen, 1) != 0)
				Fail("the stand-in cannot listen on 127.0.0.1");
			int len = sizeof(addr);
			getsockname(m_sListen, (sockaddr*)&addr, &len);
			m_nPort = ntohs(addr.sin_port);
		}
		int Port() const { return m_nPort; }

		bool Accept(int nSecs)
		{
			fd_set set; FD_ZERO(&set); FD_SET(m_sListen, &set);
			timeval tv = { nSecs, 0 };
			if(select(0, &set, NULL, NULL, &tv) != 1)
				return false;
			m_sLink = accept(m_sListen, NULL, NULL);
			return m_sLink != INVALID_SOCKET;
		}

		bool Read(LinkPacket& pkt, int nMs)
		{
			DWORD tEnd = ::GetTickCount() + nMs;
			for(;;)
			{
				if(m_buf.size() >= 8)
				{
					unsigned short size = *(unsigned short*)&m_buf[0];
					if(size < 8)
						Fail("a link packet shorter than its header");
					if(m_buf.size() >= size)
					{
						pkt.size	= size;
						pkt.type	= *(unsigned short*)&m_buf[2];
						pkt.idNpc	= *(unsigned long*)&m_buf[4];
						pkt.raw.assign(m_buf.begin(), m_buf.begin() + size);
						pkt.body.assign(m_buf.begin() + 8, m_buf.begin() + size);
						m_buf.erase(m_buf.begin(), m_buf.begin() + size);
						return true;
					}
				}
				long nLeft = (long)(tEnd - ::GetTickCount());
				if(nLeft <= 0)
					return false;
				fd_set set; FD_ZERO(&set); FD_SET(m_sLink, &set);
				timeval tv = { nLeft / 1000, (nLeft % 1000) * 1000 };
				if(select(0, &set, NULL, NULL, &tv) != 1)
					return false;
				char buf[4096];
				int n = recv(m_sLink, buf, sizeof(buf), 0);
				if(n <= 0)
					return false;
				m_buf.insert(m_buf.end(), buf, buf + n);
			}
		}

		void Send(unsigned short type, unsigned long idNpc, const std::vector<char>& body)
		{
			std::vector<char> pkt(8 + body.size());
			*(unsigned short*)&pkt[0] = (unsigned short)pkt.size();
			*(unsigned short*)&pkt[2] = type;
			*(unsigned long*)&pkt[4] = idNpc;
			if(!body.empty())
				memcpy(&pkt[8], &body[0], body.size());
			if(send(m_sLink, &pkt[0], (int)pkt.size(), 0) != (int)pkt.size())
				Fail("the stand-in's send failed");
		}

		void Close()
		{
			if(m_sLink != INVALID_SOCKET) { closesocket(m_sLink); m_sLink = INVALID_SOCKET; }
			if(m_sListen != INVALID_SOCKET) { closesocket(m_sListen); m_sListen = INVALID_SOCKET; }
		}

	private:
		SOCKET	m_sListen, m_sLink;
		int		m_nPort;
		std::vector<char>	m_buf;
	};

	std::string CheckLogin(const LinkPacket& pkt)
	{
		if(pkt.type != MSG_LOGIN)
			return "the first packet is not 1002";
		if(pkt.idNpc != LINK_VERSION && pkt.idNpc != LINK_VERSION_RELOGIN)
			return "the version (the header's NPC id) is neither 3ADE68B1h nor 3ADE68B2h";
		if(pkt.body.size() < 0x20)
			return "the login body is shorter than the two names";
		std::vector<char> after(pkt.raw.begin() + 4, pkt.raw.end());
		after.push_back(0);
		if(strcmp(&after[4], LINK_ACCOUNT) != 0)
			return "the account at +4 differs";
		if(strcmp(&after[0x14], LINK_PASSWORD) != 0)
			return "the password at +14h differs";
		return "";
	}

	std::vector<char> BuildTalk(const char* pszWords)
	{
		std::vector<char> body(0x0C, 0);
		*(unsigned long*)&body[0]	= 0xFFFFFF;
		*(unsigned short*)&body[4]	= 0x835;
		*(unsigned short*)&body[6]	= 0;
		*(unsigned long*)&body[8]	= 0;
		const char* strs[] = { "SYSTEM", "ALLUSERS", NULL, pszWords };
		std::vector<char> packer(1, 0);
		for(int i = 0; i < 4; i++)
		{
			packer[0]++;
			size_t n = strs[i] ? strlen(strs[i]) : 0;
			packer.push_back((char)n);
			if(n)
				packer.insert(packer.end(), strs[i], strs[i] + n);
		}
		body.insert(body.end(), packer.begin(), packer.end());
		body.resize(packer.size() + 0x0F, 0);
		return body;
	}

	class CNpcServerProcess
	{
	public:
		CNpcServerProcess() { memset(&m_pi, 0, sizeof(m_pi)); }
		~CNpcServerProcess() { Kill(); }
		void Start()
		{
			std::string exe = g_strBin + "\\NpcServer.exe";
			STARTUPINFOA si = { sizeof(si) };
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOWMINNOACTIVE;
			if(!::CreateProcessA(exe.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, g_strRun.c_str(), &si, &m_pi))
				Fail("cannot start " + exe + " (tools\\build.ps1 first)");
		}
		bool WaitExit(int nMs) { return m_pi.hProcess && ::WaitForSingleObject(m_pi.hProcess, nMs) == WAIT_OBJECT_0; }
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

	struct GenRow { unsigned long idMap; int x, y, cx, cy, bornx, borny; unsigned int type; };
	std::map<unsigned long, GenRow> g_setGen;

	void LoadGenerators()
	{
		MYSQL* h = Connect(GATE_DB);
		std::vector<std::vector<std::string> > rows = Query(h,
			"SELECT id, mapid, bound_x, bound_y, bound_cx, bound_cy, born_x, born_y, npctype FROM cq_generator");
		mysql_close(h);
		g_setGen.clear();
		for(size_t i = 0; i < rows.size(); i++)
		{
			GenRow g;
			g.idMap = strtoul(rows[i][1].c_str(), NULL, 10);
			g.x = atoi(rows[i][2].c_str()); g.y = atoi(rows[i][3].c_str());
			g.cx = atoi(rows[i][4].c_str()); g.cy = atoi(rows[i][5].c_str());
			g.bornx = atoi(rows[i][6].c_str()); g.borny = atoi(rows[i][7].c_str());
			g.type = strtoul(rows[i][8].c_str(), NULL, 10);
			g_setGen[strtoul(rows[i][0].c_str(), NULL, 10)] = g;
		}
	}

	std::string CheckSpawn(const LinkPacket& pkt)
	{
		if(pkt.type != MSG_AINPCINFO) return "not 20001";
		if(pkt.idNpc != 0) return "the header's NPC id is not ID_NONE";
		if(pkt.body.size() != 0x20) return "the body is not 20h bytes";
		const char* b = &pkt.body[0];
		unsigned long id = *(unsigned long*)(b + 0), idMap = *(unsigned long*)(b + 4), idGen = *(unsigned long*)(b + 8);
		unsigned long idOwner = *(unsigned long*)(b + 0x0C);
		int nData = *(int*)(b + 0x10);
		unsigned short x = *(unsigned short*)(b + 0x14), y = *(unsigned short*)(b + 0x16);
		unsigned short action = *(unsigned short*)(b + 0x18), type = *(unsigned short*)(b + 0x1A);
		unsigned short ownerType = *(unsigned short*)(b + 0x1C);
		if(action != 1) return "the action is not MSGAINPCINFO_CREATENEW";
		if(id == 0) return "no NPC id";
		if(idOwner != 0 || nData != 0 || ownerType != 0) return "owner or data set";
		std::map<unsigned long, GenRow>::iterator it = g_setGen.find(idGen);
		if(it == g_setGen.end()) return "no such generator";
		const GenRow& g = it->second;
		if(idMap != g.idMap) return "the map is not the generator's";
		if(type != (unsigned short)g.type) return "the type is not the generator's";
		if(g.bornx > 0)
		{
			if(x != g.bornx || y != g.borny) return "not at the birth point";
		}
		else if(x < g.x || x >= g.x + g.cx || y < g.y || y >= g.y + g.cy)
			return "outside the generator's box";
		return "";
	}

	struct RunResult
	{
		std::vector<char>	login;
		std::vector<std::string>	spawns;
		int		nOther;
		DWORD	tFirstSpawn;
		std::map<unsigned short, int>	setOther;
	};

	RunResult RunGate(const char* pszAnswer, int nSecs, size_t nMax, bool* pbExited)
	{
		RunResult r; r.nOther = 0; r.tFirstSpawn = 0;
		CStandIn link;
		link.Listen();
		WriteConfig(link.Port());
		CNpcServerProcess proc;
		proc.Start();
		if(!link.Accept(180))
			Fail("NpcServer did not connect within 180 s (its syslog in db\\npcgate\\syslog)");
		LinkPacket pkt;
		if(!link.Read(pkt, 30000))
			Fail("no login within 30 s");
		std::string err = CheckLogin(pkt);
		if(!err.empty())
			{ std::string hex; char h[4]; for(size_t i = 0; i < pkt.raw.size() && i < 48; i++) { sprintf(h, "%02X ", (unsigned char)pkt.raw[i]); hex += h; } Fail("login: " + err + " [" + hex + "]"); }
		r.login = pkt.raw;
		Log("login: %u bytes, type %u, version %08lX, account/password as MsgServer checks them", pkt.size, pkt.type, pkt.idNpc);
		link.Send(MSG_TALK, 0, BuildTalk(pszAnswer));
		DWORD tAnswer = ::GetTickCount();

		DWORD tEnd = ::GetTickCount() + nSecs * 1000;
		while((long)(tEnd - ::GetTickCount()) > 0 && r.spawns.size() < nMax)
		{
			if(!link.Read(pkt, (int)(tEnd - ::GetTickCount())))
				break;
			if(pkt.type == MSG_AINPCINFO)
			{
				err = CheckSpawn(pkt);
				if(!err.empty())
					Fail("20001: " + err);
				char key[64];
				sprintf(key, "%lu/%u/%lu", *(unsigned long*)&pkt.body[8], *(unsigned short*)&pkt.body[0x1A], *(unsigned long*)&pkt.body[4]);
				if(r.spawns.empty())
					r.tFirstSpawn = ::GetTickCount() - tAnswer;
				r.spawns.push_back(key);
				std::vector<char> answer = pkt.body;
				*(unsigned long*)&answer[0x0C] = 0;
				answer[0x1C] = 0;
				*(unsigned short*)&answer[0x18] = 1;
				link.Send(MSG_AINPCINFO, *(unsigned long*)&pkt.body[0], answer);
			}
			else
			{
				r.nOther++;
				if(r.setOther[pkt.type]++ == 0)
				{
					std::string hex; char h[4];
					for(size_t i = 0; i < pkt.raw.size() && i < 48; i++) { sprintf(h, "%02X ", (unsigned char)pkt.raw[i]); hex += h; }
					Log("first packet of type %u: %s", pkt.type, hex.c_str());
				}
			}
		}
		*pbExited = proc.WaitExit(pszAnswer == std::string("ANSWER_OK") ? 0 : 60000);
		proc.Kill();
		link.Close();
		return r;
	}

	std::vector<std::string> WalkOrder(const std::vector<std::string>& spawns, const std::vector<std::string>& other)
	{
		std::vector<std::string> order;
		for(size_t i = 0; i < spawns.size(); i++)
		{
			std::string gen = spawns[i].substr(0, spawns[i].find('/'));
			bool bInOther = false;
			for(size_t k = 0; k < other.size() && !bInOther; k++)
				bInOther = other[k].substr(0, other[k].find('/')) == gen;
			if(bInOther && std::find(order.begin(), order.end(), gen) == order.end())
				order.push_back(gen);
		}
		return order;
	}
}

TEST_CLASS(NpcLinkGateTests)
{
public:
	TEST_CLASS_INITIALIZE(Setup)
	{
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 2), &wsa);
		FindPaths();
		BuildGateDatabase();
		BuildRunDirectory();
		LoadGenerators();
		Log("population: %d maps over %d map documents (%lld synthetic cells), %u generators; run directory %s",
			g_nMaps, g_nMapDocs, g_nCells, (unsigned)g_setGen.size(), g_strRun.c_str());
	}

	TEST_CLASS_CLEANUP(Teardown)
	{
		WSACleanup();
	}

	TEST_METHOD(G1_LoginAnsweredAndGeneratorsAsk)
	{
		const size_t N = 40;
		bool bExited = false;
		RunResult a = RunGate("ANSWER_OK", 60, N, &bExited);
		Assert::IsFalse(bExited, L"NpcServer exited after a good login");
		Log("run 1: %u requests, %d other packets, the first %lu ms after the answer", (unsigned)a.spawns.size(), a.nOther, a.tFirstSpawn);
		Assert::IsTrue(a.spawns.size() > 0, L"no 20001 request after the login (LOGIN_DELAY_SECS, then the generators)");
#ifdef	_DEBUG
		const DWORD LOGIN_DELAY_MS = 1000;
#else
		const DWORD LOGIN_DELAY_MS = 10000;
#endif
		Assert::IsTrue(a.tFirstSpawn >= LOGIN_DELAY_MS - 100, L"a request came before LOGIN_DELAY_SECS had passed");

		RunResult b = RunGate("ANSWER_OK", 60, N, &bExited);
		Log("run 2: %u requests, %d other packets", (unsigned)b.spawns.size(), b.nOther);
		Assert::IsTrue(a.login == b.login, L"the login differs between two runs");
		Assert::IsTrue(!b.spawns.empty(), L"no request in the second run");
		std::vector<std::string> wa = WalkOrder(a.spawns, b.spawns), wb = WalkOrder(b.spawns, a.spawns);
		Assert::IsTrue(wa.size() >= 2, L"fewer than two generators asked in both runs: the walk order is not measured");
		Assert::IsTrue(wa == wb, L"the generators are asked in another order in the second run");
		std::vector<std::string> wr(wa.rbegin(), wa.rend());
		Assert::IsFalse(wr == wb, L"positive control: a reversed walk order was not seen as different");
		Log("determinism: the login matches; %u generators asked in both runs' first %u requests, in the same order",
			(unsigned)wa.size(), (unsigned)N);
	}

	TEST_METHOD(G2_PositiveControl_RefusedLoginCloses)
	{
		bool bExited = false;
		RunResult r = RunGate("ERROR: login refuse!", 20, 1, &bExited);
		Assert::AreEqual((size_t)0, r.spawns.size(), L"NpcServer asked for NPCs after a refused login");
		Assert::IsTrue(bExited, L"NpcServer did not close within 60 s of a refused login");
	}

	TEST_METHOD(G3_PositiveControl_LoginCheckSeesPlantedDifferences)
	{
		LinkPacket pkt;
		pkt.size = 0x28; pkt.type = MSG_LOGIN; pkt.idNpc = LINK_VERSION;
		pkt.raw.assign(0x28, 0);
		*(unsigned short*)&pkt.raw[0] = 0x28; *(unsigned short*)&pkt.raw[2] = MSG_LOGIN; *(unsigned long*)&pkt.raw[4] = LINK_VERSION;
		strcpy(&pkt.raw[8], LINK_ACCOUNT); strcpy(&pkt.raw[0x18], LINK_PASSWORD);
		pkt.body.assign(pkt.raw.begin() + 8, pkt.raw.end());
		Assert::AreEqual(std::string(""), CheckLogin(pkt));
		LinkPacket bad = pkt; bad.raw[8] ^= 1; bad.body[0] ^= 1;
		Assert::AreNotEqual(std::string(""), CheckLogin(bad));
		bad = pkt; bad.raw[0x18] ^= 1; bad.body[0x10] ^= 1;
		Assert::AreNotEqual(std::string(""), CheckLogin(bad));
		bad = pkt; bad.idNpc = 0x3ADE68B3;
		Assert::AreNotEqual(std::string(""), CheckLogin(bad));
		std::vector<char> talk = BuildTalk("ANSWER_OK");
		Assert::AreEqual((size_t)(1 + 7 + 9 + 1 + 10 + 0x0F), talk.size());
		Assert::AreEqual((char)4, talk[0x0C]);
	}
};
