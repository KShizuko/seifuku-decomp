#include "stdafx.h"
#include "MyShell.h"
#include "DlgMain.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_base_of<CDialog, CDlgMain>::value, "");
static_assert(std::is_polymorphic<CDlgMain>::value, "");
static_assert(IDC_MAIN_FORCE == 1025 && IDC_MAIN_FACTION == 1024 && IDC_MAIN_GOODBOX == 1021 && IDC_MAIN_EXP == 1019 && IDC_MAIN_XP == 1018 && IDC_MAIN_MP == 1502 && IDC_MAIN_HP == 1501 && IDC_MAIN_TALK == 1014 && IDC_MAIN_MAP == 1015 && IDC_MAIN_RUN == 1012, "");
static_assert(IDC_MAIN_SCREENMOVE == 1023 && IDC_MAIN_STAND == 1016 && IDC_MAIN_PK == 1013 && IDC_MAIN_QUERY == 1503 && IDC_MAIN_LEVWORD == 1006 && IDC_MAIN_GOOD == 1007 && IDC_MAIN_SETUP == 1008 && IDC_MAIN_ACT == 1009 && IDC_MAIN_CHAT == 1010 && IDC_MAIN_GROUP == 1011 && IDC_MAIN_FRIEND == 1026 && IDC_MAIN_MAGIC == 1022, "");
static_assert(DLG_MAIN == 3 && DLG_QUERY == 4 && DLG_FRIEND == 5 && DLG_QUERY_FACTION == 8 && DLG_QUERY_SKILL == 10 && DLG_NPCBUY == 12 && DLG_TRADE == 14 && DLG_DEPOT == 17 && DLG_IMPROVE == 22 && DLG_NPCFACE == 23 && DLG_EMBED == 24 && DLG_MPCBOOTH == 28 && DLG_NPCBOOTH == 29 && DLG_EFFIGY == 33 && DLG_LAY == 34 && DLG_EXP == 41 && DLG_FUSE == 43, "");
static_assert(ON_MSGDLG_HIDE == 9 && ON_MSGDLG_STATIC == 11 && ON_MAIN_TRADE == 27 && ON_MAIN_SCREENMOVE == 30 && ON_MAIN_NPCEQUIP == 84 && ON_MAIN_EXPDOWN == 90, "");
static_assert(CPlayer::_PLAYER_LIFE == 1 && CPlayer::_PLAYER_CURRENTLIFE == 15, "");
static_assert(MY_PROGRESS_V == 0 && MY_PROGRESS_HLEFT == 10, "");
static_assert(ICON_TYPE_ITEM == 0 && ICON_TYPE_ACTION == 1 && ICON_TYPE_SKILL == 2 && ICON_TYPE_XP == 3 && GRID_ICONQUICK_NUM == 10, "");
static_assert(USERSTATUS_FLY == 0x08000000 && PKMODE_FREE == 0 && PKMODE_MONSTERONLY == 1, "");
static_assert(IDD_DIALOG_LEVWORD == 240 && IDD_DIALOG_MAGIC == 188 && IDD_DIALOG_XP == 189 && IDD_DIALOG_KONGFU == 214 && IDD_DIALOG_TALK == 145 && IDD_DIALOG_QUERY == 131 && IDD_DIALOG_FRIEND == 160 && IDD_DIALOG_INSHELP == 272 && IDD_DIALOG_MPCGOODS == 153 && IDD_DIALOG_TRADE == 137 && IDD_DIALOG_SETUP == 138 && IDD_DIALOG_HOTKEY == 231 && IDD_DIALOG_ACT == 140 && IDD_DIALOG_GROUP == 141 && IDD_DIALOG_BAG == 277 && IDD_DIALOG_NPCBUY == 152 && IDD_DIALOG_DEPOT == 161 && IDD_DIALOG_PK == 191 && IDD_DIALOG_IMPROVE == 211, "");
static_assert(IDD_DIALOG_NPCFACE == 212 && IDD_DIALOG_QUERY_FACTION == 133 && IDD_DIALOG_EMBED == 229 && IDD_DIALOG_EXP == 275 && IDD_DIALOG_FUSE == 278 && IDD_DIALOG_EFFIGY == 241 && IDD_DIALOG_MSGSEND == 232 && IDD_DIALOG_MSGRECEIVE == 238 && IDD_DIALOG_MPCBOOTH == 233 && IDD_DIALOG_NPCBOOTH == 234 && IDD_DIALOG_HAWK == 235 && IDD_DIALOG_CHAT == 139 && IDD_DIALOG_BLACK == 239 && IDD_DIALOG_LAY == 242 && IDD_DIALOG_OTHER == 255 && IDD_DIALOG_NPCEQUIP == 258 && IDD_DIALOG_DICE == 262 && IDD_DIALOG_DICELIST == 267 && IDD_DIALOG_ENEMY == 269, "");

static size_t RoundUp(size_t n, size_t a) { return (n + a - 1) / a * a; }

struct DlgMainProbe
{
    static size_t DlgPnt() { return offsetof(CDlgMain, m_DlgPnt); }
    static size_t Brush() { return offsetof(CDlgMain, m_Hbr); }
    static size_t Tip() { return offsetof(CDlgMain, m_rcTip); }
    static size_t TipSize() { return sizeof(CDlgMain::m_rcTip); }
    static size_t Rgn() { return offsetof(CDlgMain, m_pRgnTip); }
    static size_t RgnSize() { return sizeof(CDlgMain::m_pRgnTip); }
    static size_t CurTip() { return offsetof(CDlgMain, m_btCurTip); }
    static size_t ActPnt() { return offsetof(CDlgMain, m_ActPnt); }
    static size_t ItemAct() { return offsetof(CDlgMain, m_bItemAct); }
    static size_t ActAmount() { return offsetof(CDlgMain, m_nActAmount); }
    static size_t LastActTime() { return offsetof(CDlgMain, m_dwLastActTime); }
    static size_t XpAct() { return offsetof(CDlgMain, m_bXpAct); }
    static size_t LastXpTime() { return offsetof(CDlgMain, m_dwLastXpTime); }
    static size_t OwnerIni() { return offsetof(CDlgMain, m_strOwnerIni); }
    static size_t OwnerIniSize() { return sizeof(CDlgMain::m_strOwnerIni); }
    static size_t Unknown1() { return offsetof(CDlgMain, m_nUnknown1); }
};

static_assert(CDlgMain::IDD == 130, "");

#define SFK_NEXT_MEMBER(m, T) o = RoundUp(o, __alignof(T)); Assert::IsTrue(offsetof(CDlgMain, m) == o); o += sizeof(T);

TEST_CLASS(ClientDlgMainHeader)
{
public:
    TEST_METHOD(TheMembersSitWhereTheCtorStoresThem)
    {
        const size_t b = sizeof(CDialog);
        Assert::IsTrue(offsetof(CDlgMain, m_bNoMana) == b + 0x00);
        Assert::IsTrue(offsetof(CDlgMain, m_bLevRead) == b + 0x04);
        Assert::IsTrue(offsetof(CDlgMain, m_bFactionRead) == b + 0x08);
        Assert::IsTrue(offsetof(CDlgMain, m_bPkFlash) == b + 0x0C);
        Assert::IsTrue(DlgMainProbe::DlgPnt() == b + 0x10);
        Assert::IsTrue(DlgMainProbe::Brush() == b + 0x18);
        Assert::IsTrue(offsetof(CDlgMain, m_bCreate) == b + 0x1C);
        Assert::IsTrue(DlgMainProbe::Tip() == b + 0x20);
        Assert::IsTrue(DlgMainProbe::TipSize() == 6 * sizeof(RECT));
        Assert::IsTrue(DlgMainProbe::Rgn() == b + 0x80);
        Assert::IsTrue(DlgMainProbe::RgnSize() == 3 * sizeof(CRgn*));
        Assert::IsTrue(DlgMainProbe::CurTip() == b + 0x8C);
        Assert::IsTrue(DlgMainProbe::ActPnt() == b + 0x90);
        Assert::IsTrue(DlgMainProbe::ItemAct() == b + 0x98);
        Assert::IsTrue(DlgMainProbe::ActAmount() == b + 0x9C);
        Assert::IsTrue(DlgMainProbe::LastActTime() == b + 0xA0);
        Assert::IsTrue(DlgMainProbe::XpAct() == b + 0xA4);
        Assert::IsTrue(DlgMainProbe::LastXpTime() == b + 0xA8);
        Assert::IsTrue(DlgMainProbe::OwnerIni() == b + 0xAC);
        Assert::IsTrue(DlgMainProbe::OwnerIniSize() == 128);
        Assert::IsTrue(DlgMainProbe::Unknown1() == b + 0x12C);
        Assert::IsTrue(offsetof(CDlgMain, m_uCurDlgID) == b + 0x130);
        size_t o = b + 0x134;
        SFK_NEXT_MEMBER(m_DlgQuery, CDlgQuery)
        SFK_NEXT_MEMBER(m_DlgFriend, CDlgFriend)
        SFK_NEXT_MEMBER(m_DlgMpcGoods, CDlgMpcGoods)
        SFK_NEXT_MEMBER(m_DlgTrade, CDlgTrade)
        SFK_NEXT_MEMBER(m_DlgSetup, CDlgSetup)
        SFK_NEXT_MEMBER(m_DlgAct, CDlgAct)
        SFK_NEXT_MEMBER(m_DlgGroup, CDlgGroup)
        SFK_NEXT_MEMBER(m_DlgBag, CDlgBag)
        SFK_NEXT_MEMBER(m_DlgTalk, CDlgTalk)
        SFK_NEXT_MEMBER(m_DlgNpcBuy, CDlgNpcBuy)
        SFK_NEXT_MEMBER(m_DlgDepot, CDlgDepot)
        SFK_NEXT_MEMBER(m_DlgMagic, CDlgMagic)
        SFK_NEXT_MEMBER(m_DlgXp, CDlgXp)
        SFK_NEXT_MEMBER(m_DlgKongfu, CDlgKongfu)
        SFK_NEXT_MEMBER(m_DlgPk, CDlgPk)
        SFK_NEXT_MEMBER(m_DlgImprove, CDlgImprove)
        SFK_NEXT_MEMBER(m_DlgEnchant, CDlgEnchant)
        SFK_NEXT_MEMBER(m_DlgNpcFace, CDlgNpcFace)
        SFK_NEXT_MEMBER(m_DlgQuery_Faction, CDlgQuery_Faction)
        SFK_NEXT_MEMBER(m_DlgEmbed, CDlgEmbed)
        SFK_NEXT_MEMBER(m_DlgHotkey, CDlgHotkey)
        SFK_NEXT_MEMBER(m_DlgMsgSend, CDlgMsgSend)
        SFK_NEXT_MEMBER(m_DlgMsgReceive, CDlgMsgReceive)
        SFK_NEXT_MEMBER(m_DlgMpcBooth, CDlgMpcBooth)
        SFK_NEXT_MEMBER(m_DlgNpcBooth, CDlgNpcBooth)
        SFK_NEXT_MEMBER(m_DlgHawk, CDlgHawk)
        SFK_NEXT_MEMBER(m_DlgChat, CDlgChat)
        SFK_NEXT_MEMBER(m_DlgBlack, CDlgBlack)
        SFK_NEXT_MEMBER(m_DlgLevWord, CDlgLevWord)
        SFK_NEXT_MEMBER(m_DlgEffigy, CDlgEffigy)
        SFK_NEXT_MEMBER(m_DlgLay, CDlgLay)
        SFK_NEXT_MEMBER(m_DlgOther, CDlgOther)
        SFK_NEXT_MEMBER(m_DlgNpcEquip, CDlgNpcEquip)
        SFK_NEXT_MEMBER(m_DlgDice, CDlgDice)
        SFK_NEXT_MEMBER(m_DlgDiceList, CDlgDiceList)
        SFK_NEXT_MEMBER(m_DlgEnemy, CDlgEnemy)
        SFK_NEXT_MEMBER(m_DlgInsHelp, CDlgInsHelp)
        SFK_NEXT_MEMBER(m_DlgCompose, CDlgCompose)
        SFK_NEXT_MEMBER(m_DlgFuse, CDlgFuse)
        SFK_NEXT_MEMBER(m_DlgExeConfirm, CDlgExeConfirm)
        SFK_NEXT_MEMBER(m_DlgExercise, CDlgExercise)
        SFK_NEXT_MEMBER(m_BlessForcePrg, CMyProgress)
        SFK_NEXT_MEMBER(m_ForcePrg, CMyProgress)
        SFK_NEXT_MEMBER(m_FactionBtn, CMyButton)
        SFK_NEXT_MEMBER(m_GoodBox, CMyGrid)
        SFK_NEXT_MEMBER(m_ExpPrg, CMyProgress)
        SFK_NEXT_MEMBER(m_XpPrg, CMyProgress)
        SFK_NEXT_MEMBER(m_MpPrg, CMyProgress)
        SFK_NEXT_MEMBER(m_HpPrg, CMyProgress)
        SFK_NEXT_MEMBER(m_TalkChk, CMyCheck)
        SFK_NEXT_MEMBER(m_MapChk, CMyCheck)
        SFK_NEXT_MEMBER(m_RunChk, CMyCheck)
        SFK_NEXT_MEMBER(m_ScreenMoveChk, CMyCheck)
        SFK_NEXT_MEMBER(m_StandChk, CMyCheck)
        SFK_NEXT_MEMBER(m_PkBtn, CMyButton)
        SFK_NEXT_MEMBER(m_QueryBtn, CMyButton)
        SFK_NEXT_MEMBER(m_LevWordBtn, CMyButton)
        SFK_NEXT_MEMBER(m_GoodBtn, CMyButton)
        SFK_NEXT_MEMBER(m_SetupBtn, CMyButton)
        SFK_NEXT_MEMBER(m_ActBtn, CMyButton)
        SFK_NEXT_MEMBER(m_ChatBtn, CMyButton)
        SFK_NEXT_MEMBER(m_GroupBtn, CMyButton)
        SFK_NEXT_MEMBER(m_FriendBtn, CMyButton)
        SFK_NEXT_MEMBER(m_MagicImg, CMyImage)
        Assert::IsTrue(sizeof(CDlgMain) == RoundUp(o, __alignof(CDlgMain)));
    }
};
