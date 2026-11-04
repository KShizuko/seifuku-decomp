#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/NetMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct MapGroupMsgProbe : public CNetMsg
	{
		static size_t	OffType()		{ return offsetof(MapGroupMsgProbe, m_unMsgType); }
		static size_t	OffSize()		{ return offsetof(MapGroupMsgProbe, m_unMsgSize); }
		static size_t	OffBuf()		{ return offsetof(MapGroupMsgProbe, m_bufMsg); }
		static size_t	OffProcess()	{ return offsetof(MapGroupMsgProbe, m_idProcess); }
		static size_t	OffSocket()		{ return offsetof(MapGroupMsgProbe, m_idSocket); }
		static size_t	OffNpc()		{ return offsetof(MapGroupMsgProbe, m_idNpc); }
		static size_t	OffTrans()		{ return offsetof(MapGroupMsgProbe, m_nTransData); }
	};
}

TEST_CLASS(MapGroupNetMsgTests)
{
public:
	TEST_METHOD(Layout)
	{
		Assert::AreEqual((size_t)0x04, MapGroupMsgProbe::OffType());
		Assert::AreEqual((size_t)0x08, MapGroupMsgProbe::OffSize());
		Assert::AreEqual((size_t)0x0C, MapGroupMsgProbe::OffBuf());
		Assert::AreEqual((size_t)0x40C, MapGroupMsgProbe::OffProcess());
		Assert::AreEqual((size_t)0x410, MapGroupMsgProbe::OffSocket());
		Assert::AreEqual((size_t)0x414, MapGroupMsgProbe::OffNpc());
		Assert::AreEqual((size_t)0x418, MapGroupMsgProbe::OffTrans());
		Assert::AreEqual((size_t)0x41C, sizeof(CNetMsg));
		Assert::AreEqual(0x400, CNetMsg::GetMaxSize());
	}

	TEST_METHOD(FuseType)
	{
		Assert::AreEqual(1028, (int)_MSG_FUSE);
	}
};
