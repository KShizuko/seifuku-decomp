#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "MsgGemEmbed.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MsgGemEmbedProbe : CMsgGemEmbed
{
    typedef CMsgGemEmbed::MSG_Info Info;
    static size_t InfoPtr() { return offsetof(MsgGemEmbedProbe, m_pInfo); }
};

static_assert(sizeof(CMsgGemEmbed) == 0x408, "");
static_assert(sizeof(MsgGemEmbedProbe::Info) == 0x14, "");
static_assert(offsetof(MsgGemEmbedProbe::Info, unMsgType) == 0x2, "");
static_assert(offsetof(MsgGemEmbedProbe::Info, idUser) == 0x4, "");
static_assert(offsetof(MsgGemEmbedProbe::Info, idItem) == 0x8, "");
static_assert(offsetof(MsgGemEmbedProbe::Info, idGem) == 0xC, "");
static_assert(offsetof(MsgGemEmbedProbe::Info, usPos) == 0x10, "");
static_assert(offsetof(MsgGemEmbedProbe::Info, usAction) == 0x12, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgGemEmbed::*)(OBJID, OBJID, OBJID, int, int)>(&CMsgGemEmbed::Create)), BOOL (CMsgGemEmbed::*)(OBJID, OBJID, OBJID, int, int)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMsgGemEmbed::*)(char*, DWORD)>(&CMsgGemEmbed::Create)), BOOL (CMsgGemEmbed::*)(char*, DWORD)>::value, "");
static_assert(GEM_EMBED == 0 && GEM_TAKEOUT == 1, "");

TEST_CLASS(ClientMsgGemEmbedHeader)
{
public:
    TEST_METHOD(InfoPointerFollowsTheBuffer)
    {
        Assert::IsTrue(MsgGemEmbedProbe::InfoPtr() == 0x404);
    }
};
