#define _WINSOCKAPI_
#include <winsock2.h>
#include <windows.h>
#include "CppUnitTest.h"
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "ClientSocket.h"
#include "MsgAccount_A.h"
#include "MsgLogin_A.h"
#include "MsgConnect_A.h"
#include "MsgFee_A.h"
#include "MsgQueryFee_A.h"
#include "account_standin.h"

#pragma comment(lib, "ws2_32.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	const char*	SERVER_ACCOUNT	= "sfkworld";
	const char*	SERVER_PASSWORD	= "sfkpassword";
	const char*	SERVER_NAME		= "sfkserver";
	const int	GAME_PORT		= 5816;
	const unsigned long	SERVER_ID	= 7;

	void Log(const char* fmt, ...)
	{
		char buf[512];
		va_list args;
		va_start(args, fmt);
		_vsnprintf(buf, sizeof(buf) - 1, fmt, args);
		va_end(args);
		buf[sizeof(buf) - 1] = 0;
		Logger::WriteMessage(buf);
		Logger::WriteMessage("\n");
	}

	void Fail(const char* psz)
	{
		std::string s(psz);
		std::wstring w(s.begin(), s.end());
		Assert::Fail(w.c_str());
	}

	struct WinsockGuard
	{
		WinsockGuard()	{ WSADATA d; WSAStartup(MAKEWORD(2, 2), &d); }
		~WinsockGuard()	{ WSACleanup(); }
	};

	const char* ReadMsg(CClientSocket& sock, int& nMsgSize, int nMilliSecs)
	{
		DWORD tEnd = GetTickCount() + nMilliSecs;
		while ((long)(tEnd - GetTickCount()) > 0)
		{
			int nNetSize = 0;
			const char* pMsg = sock.GetPacket(&nNetSize, true);
			if (pMsg && nNetSize >= 2)
			{
				nMsgSize = *((unsigned short*)pMsg);
				if (nNetSize >= nMsgSize)
					return pMsg;
			}
			if (!sock.IsOpen())
				return NULL;
			Sleep(5);
		}
		return NULL;
	}

	bool Connect(CClientSocket& sock, CAccountStandIn& standin)
	{
		sock.Open("127.0.0.1", standin.GetPort(), 0);
		if (!standin.Accept(3000))
			return false;
		for (int i = 0; i < 300; i++)
		{
			if (sock.Open(0))
				return true;
			Sleep(10);
		}
		return false;
	}

	struct Result
	{
		bool						bLoginOK;
		int							nMessages;
		std::vector<unsigned char>	wireIn, wireOut;
	};

	Result Exchange(const char* szPassword)
	{
		Result res = { false, 0 };
		CAccountStandIn standin;
		if (!standin.Listen(0))
			Fail("the stand-in cannot listen on 127.0.0.1");
		CClientSocket sock;
		if (!Connect(sock, standin))
			Fail("MsgServer's account socket did not connect to the stand-in");

		CMsgAccount_A::MSG_Info info;
		memset(&info, 0, sizeof(info));
		info.unMsgSize	= sizeof(info);
		info.unMsgType	= _MSG_ACCOUNT_A;
		strcpy(info.szAccount, SERVER_ACCOUNT);
		strcpy(info.szPassword, szPassword);
		strcpy(info.szServer, SERVER_NAME);
		info.nServerPort = GAME_PORT;
		if (!sock.SendPacket((const char*)&info, info.unMsgSize, true))
			Fail("SendPacket 1099");

		CAccountStandIn::Message msg;
		if (!standin.Recv(msg, 3000))
			Fail("the stand-in received no 1099");
		res.nMessages++;
		Log("  in  %u (%u bytes)", msg.usType, (unsigned)msg.buf.size());
		if (msg.usType != CAccountStandIn::MSG_ACCOUNT || msg.buf.size() != CAccountStandIn::SIZE_ACCOUNT)
			Fail("the 1099's type or size");
		if (strcmp((const char*)&msg.buf[4], SERVER_ACCOUNT) != 0 || strcmp((const char*)&msg.buf[0x24], SERVER_NAME) != 0
				|| *(int*)&msg.buf[0x34] != GAME_PORT)
			Fail("the 1099's account, server or port as the stand-in read them");
		bool bPass = strcmp((const char*)&msg.buf[0x14], SERVER_PASSWORD) == 0;
		standin.SendLoginAnswer(bPass ? SERVER_ID : 0, 0x12345678, 4351, SERVER_NAME);
		res.nMessages++;

		int nMsgSize = 0;
		const char* pMsg = ReadMsg(sock, nMsgSize, 3000);
		if (!pMsg)
			Fail("MsgServer received no 1002");
		const CMsgLogin_A::MSG_Info* pLogin = (const CMsgLogin_A::MSG_Info*)pMsg;
		if (pLogin->unMsgType != _MSG_LOGIN_A || pLogin->unMsgSize != sizeof(CMsgLogin_A::MSG_Info))
			Fail("the 1002 as MsgServer reads it (CHECK 893: its size)");
		res.bLoginOK = pLogin->idAccount != ID_NONE;
		Log("  out 1002: server id %u, check %08X, version %u, name %s", pLogin->idAccount, pLogin->dwCheckData,
				pLogin->usVersion, pLogin->szUserName);
		if (pLogin->dwCheckData != 0x12345678 || pLogin->usVersion != 4351 || strcmp(pLogin->szUserName, SERVER_NAME) != 0)
			Fail("the 1002's fields as MsgServer reads them");
		CMsgLogin_A::MSG_Info echo = *pLogin;
		sock.ClearPacket(nMsgSize);
		if (!res.bLoginOK)
		{
			standin.Close();
			sock.Close();
			res.wireIn = standin.WireIn();
			res.wireOut = standin.WireOut();
			return res;
		}

		sock.SendPacket((const char*)&echo, sizeof(echo), true);
		if (!standin.Recv(msg, 3000) || msg.usType != CAccountStandIn::MSG_LOGIN || msg.buf.size() != CAccountStandIn::SIZE_LOGIN
				|| memcmp(&msg.buf[0], &echo, sizeof(echo)) != 0)
			Fail("the stand-in did not read SendLogin's 1002 back");
		res.nMessages++;
		Log("  in  1002 (the login echoed)");

		standin.SendConnect(1000001, 0xCAFEBABE, "123 19800101");
		standin.SendFee(1000001, CMsgFee_A::HEARTBEAT);
		standin.SendQueryFee(1000001, 42, 3600, 1);
		res.nMessages += 3;

		pMsg = ReadMsg(sock, nMsgSize, 3000);
		const CMsgConnect_A::MSG_Info* pConnect = (const CMsgConnect_A::MSG_Info*)pMsg;
		if (!pMsg || pConnect->unMsgType != _MSG_CONNECT || pConnect->unMsgSize != sizeof(CMsgConnect_A::MSG_Info)
				|| pConnect->idAccount != 1000001 || pConnect->dwData != 0xCAFEBABE || strcmp(pConnect->szInfo, "123 19800101") != 0)
			Fail("the 1052 as MsgServer reads it (CHECK 904)");
		Log("  out 1052: account %u, data %08X, info \"%s\"", pConnect->idAccount, pConnect->dwData, pConnect->szInfo);
		sock.ClearPacket(nMsgSize);

		pMsg = ReadMsg(sock, nMsgSize, 3000);
		const CMsgFee_A::MSG_Info* pFee = (const CMsgFee_A::MSG_Info*)pMsg;
		if (!pMsg || pFee->unMsgType != _MSG_FEE_A || pFee->unMsgSize != sizeof(CMsgFee_A::MSG_Info)
				|| pFee->idAccount != 1000001 || pFee->ucType != CMsgFee_A::HEARTBEAT)
			Fail("the 1200 as MsgServer reads it (CHECK 919)");
		Log("  out 1200: account %u, type %u", pFee->idAccount, pFee->ucType);
		sock.ClearPacket(nMsgSize);

		pMsg = ReadMsg(sock, nMsgSize, 3000);
		const CMsgQueryFee_A::MSG_Info* pQFee = (const CMsgQueryFee_A::MSG_Info*)pMsg;
		if (!pMsg || pQFee->unMsgType != _MSG_QUERYFEE_A || pQFee->unMsgSize != sizeof(CMsgQueryFee_A::MSG_Info)
				|| pQFee->idAccount != 1000001 || pQFee->dwData != 42 || pQFee->nTime != 3600 || pQFee->ucType != 1)
			Fail("the 1201 as MsgServer reads it (CHECK 928)");
		Log("  out 1201: account %u, data %u, time %d, type %u", pQFee->idAccount, pQFee->dwData, pQFee->nTime, pQFee->ucType);
		sock.ClearPacket(nMsgSize);

		CMsgQueryFee_A::MSG_Info q;
		memset(&q, 0, sizeof(q));
		q.unMsgSize = sizeof(q); q.unMsgType = _MSG_QUERYFEE_A; q.idAccount = ID_NONE; q.dwData = 3; q.nTime = 0; q.ucType = 103;
		CMsgFee_A::MSG_Info f;
		memset(&f, 0, sizeof(f));
		f.unMsgSize = sizeof(f); f.unMsgType = _MSG_FEE_A; f.idAccount = ID_NONE; f.ucType = CMsgFee_A::SERVER_FULL;
		sock.SendPacket((const char*)&q, sizeof(q));
		sock.SendPacket((const char*)&f, sizeof(f), true);
		if (!standin.Recv(msg, 3000) || msg.usType != CAccountStandIn::MSG_QUERYFEE || msg.buf.size() != CAccountStandIn::SIZE_QUERYFEE
				|| *(unsigned long*)&msg.buf[8] != 3 || msg.buf[0x10] != 103)
			Fail("the stand-in's reading of 1201");
		if (!standin.Recv(msg, 3000) || msg.usType != CAccountStandIn::MSG_FEE || msg.buf.size() != CAccountStandIn::SIZE_FEE
				|| msg.buf[8] != CMsgFee_A::SERVER_FULL)
			Fail("the stand-in's reading of 1200");
		res.nMessages += 2;
		Log("  in  1201 (users 3, type 103), 1200 (SERVER_FULL)");

		standin.Close();
		sock.Close();
		res.wireIn = standin.WireIn();
		res.wireOut = standin.WireOut();
		return res;
	}
}

TEST_CLASS(AccountStandInTests)
{
public:
	TEST_METHOD(StandInCipherIsMsgServersInverse)
	{
		unsigned char plain[600], wire[600];
		for (int i = 0; i < 600; i++) plain[i] = (unsigned char)(i * 7 + 3);
		memcpy(wire, plain, sizeof(wire));
		CEncryptClient<0x12, 0x97, 0x19, 0x7E, 0x48, 0x51, 0x1B, 0xD3> msgserver;
		msgserver.Encrypt(wire, sizeof(wire));
		CAccountStandIn::Cipher standin;
		standin.Inverse(wire, sizeof(wire));
		Assert::IsTrue(memcmp(wire, plain, sizeof(wire)) == 0);
		CAccountStandIn::Cipher standin2;
		standin2.Inverse(wire, sizeof(wire));
		CEncryptClient<0x12, 0x97, 0x19, 0x7E, 0x48, 0x51, 0x1B, 0xD3> msgserver2;
		msgserver2.Encrypt(wire, sizeof(wire));
		Assert::IsTrue(memcmp(wire, plain, sizeof(wire)) == 0);
	}

	TEST_METHOD(ExchangeWithMsgServersSocket)
	{
		WinsockGuard ws;
		Log("run 1:");
		Result a = Exchange(SERVER_PASSWORD);
		Log("run 2:");
		Result b = Exchange(SERVER_PASSWORD);
		Log("population: %d messages per run (4 to MsgServer, 4 from it), %u + %u wire bytes",
				a.nMessages, (unsigned)a.wireIn.size(), (unsigned)a.wireOut.size());
		Assert::IsTrue(a.bLoginOK && b.bLoginOK);
		Assert::AreEqual(8, a.nMessages);
		Assert::IsTrue(a.wireIn == b.wireIn, L"the bytes MsgServer sent differ between runs");
		Assert::IsTrue(a.wireOut == b.wireOut, L"the bytes the stand-in sent differ between runs");
	}

	TEST_METHOD(WrongPasswordIsRefused)
	{
		WinsockGuard ws;
		Result r = Exchange("not the password");
		Assert::IsFalse(r.bLoginOK);
		Assert::AreEqual(2, r.nMessages);
	}

	TEST_METHOD(WrongSizeFailsMsgServersCheck)
	{
		WinsockGuard ws;
		CAccountStandIn standin;
		Assert::IsTrue(standin.Listen(0));
		CClientSocket sock;
		Assert::IsTrue(Connect(sock, standin));
		unsigned char bad[CAccountStandIn::SIZE_CONNECT + 1];
		memset(bad, 0, sizeof(bad));
		*(unsigned short*)&bad[0] = sizeof(bad);
		*(unsigned short*)&bad[2] = CAccountStandIn::MSG_CONNECT;
		Assert::IsTrue(standin.SendRaw(bad, sizeof(bad)));
		int nMsgSize = 0;
		const char* pMsg = ReadMsg(sock, nMsgSize, 3000);
		Assert::IsNotNull(pMsg);
		const CMsgConnect_A::MSG_Info* p = (const CMsgConnect_A::MSG_Info*)pMsg;
		Assert::AreEqual((int)_MSG_CONNECT, (int)p->unMsgType);
		Assert::IsFalse(p->unMsgSize == sizeof(CMsgConnect_A::MSG_Info));
		sock.Close();
	}
};
