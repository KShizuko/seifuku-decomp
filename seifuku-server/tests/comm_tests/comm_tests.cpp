#include <afxwin.h>
#include "CppUnitTest.h"
#include "MessagePort.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(MessagePortTests)
{
public:
	TEST_METHOD_INITIALIZE(Setup)		{ Assert::IsTrue(CMessagePort::InitPortSet(3)); }
	TEST_METHOD_CLEANUP(Teardown)		{ CMessagePort::ClearPortSet(); }

	TEST_METHOD(SendThenRecvCarriesPortPacketAndBytes)
	{
		IMessagePort* p0 = CMessagePort::GetInterface(0);
		IMessagePort* p2 = CMessagePort::GetInterface(2);
		Assert::IsTrue(p0->Open());
		Assert::IsTrue(p2->Open());
		int nData = 1234;
		Assert::IsTrue(p0->Send(2, 77, VARTYPE_INT, &nData));
		Assert::AreEqual(1, CMessagePort::GetQueueSize(2));
		Assert::IsTrue(p2->WaitMsg(0));
		int nRecv = 0;
		CMessageStatus cStatus;
		Assert::IsTrue(p2->Recv(PORT_ANY, PACKET_ANY, VARTYPE_INT, &nRecv, &cStatus));
		Assert::AreEqual(1234, nRecv);
		Assert::AreEqual(0, cStatus.m_nPortFrom);
		Assert::AreEqual(77, cStatus.m_nPacket);
		Assert::AreEqual((int)STATUS_FLAG_OK, cStatus.m_nError);
		Assert::IsFalse(p2->Recv(PORT_ANY, PACKET_ANY, VARTYPE_INT, &nRecv, &cStatus));
	}

	TEST_METHOD(RecvFiltersByPacket)
	{
		IMessagePort* p0 = CMessagePort::GetInterface(0);
		IMessagePort* p1 = CMessagePort::GetInterface(1);
		p0->Open(); p1->Open();
		char buf[8] = "abcdefg";
		Assert::IsTrue(p0->Send(1, 5, STRUCT_TYPE(buf), buf));
		short sData = 9;
		Assert::IsTrue(p0->Send(1, 6, VARTYPE_SHORT, &sData));
		short sRecv = 0;
		CMessageStatus cStatus;
		Assert::IsTrue(p1->Recv(PORT_ANY, 6, VARTYPE_SHORT, &sRecv, &cStatus));
		Assert::AreEqual((short)9, sRecv);
		char out[8];
		Assert::IsTrue(p1->Recv(PORT_ANY, PACKET_ANY, STRUCT_TYPE(out), out, &cStatus));
		Assert::AreEqual(0, memcmp(out, buf, sizeof(buf)));
		Assert::AreEqual(0, CMessagePort::GetQueueSize(1));
	}

	TEST_METHOD(AClosedPortReportsClose)
	{
		IMessagePort* p1 = CMessagePort::GetInterface(1);
		int n;
		CMessageStatus cStatus;
		Assert::IsFalse(p1->Recv(PORT_ANY, PACKET_ANY, VARTYPE_INT, &n, &cStatus));
		Assert::AreEqual((int)STATUS_FLAG_CLOSE, cStatus.m_nError);
		Assert::IsFalse(p1->Send(0, 1, VARTYPE_INT, &n));
		Assert::IsTrue(CMessagePort::GetInterface(9) == NULL);
	}
};
