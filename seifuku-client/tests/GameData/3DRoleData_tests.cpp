#include "CppUnitTest.h"
#include <windows.h>
#include <type_traits>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include "3DRoleData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(IRoleData) == 4, "");
static_assert(std::is_same<decltype(&RoleDataQuery), IRoleData* (*)(void)>::value, "");
static_assert(std::is_same<decltype(&RoleDataDestroy), void (*)(void)>::value, "");

namespace
{
    const char MONSTER_TEXT[] =
        "[Pheasant]\r\nSizeAdd=1\r\nZoomPercent=100\r\nMaxLife=33\r\nLevel=1\r\nBornAction=0\r\nBornEffect=none\r\nBornSound=none\r\n"
        "ActResCtrl=1\r\nASB=5\r\nADB=6\r\nBodyType=7\r\nTypeID=1\r\nAntiType=0\r\nArmet=0\r\nRWeapon=0\r\nLWeapon=0\r\nMisc=0\r\nMount=0\r\n"
        "\r\n"
        "[Guard]\nSizeAdd=-2\nZoomPercent=120\nMaxLife=5000\nLevel=50\nBornAction=3\nBornEffect=born\nBornSound=sound/born.wav\n"
        "ActResCtrl=0\nASB=5\nADB=2\nBodyType=9\nTypeID=900\nAntiType=4096\nArmet=111\nRWeapon=410\nLWeapon=900\nMisc=5\nMount=7\n";

    const char USERHELP_TEXT[] =
        "[Header]\nHelpInfoAmount=3\n"
        "[HelpInfo0]\nProfession=0\nMinLevel=0\nMaxLevel=200\nSyndicate=2\nMarriage=2\nMete=0\nHelpLineAmount=1\nLine0=default~help\n"
        "[HelpInfo1]\nProfession=10\nMinLevel=1\nMaxLevel=39\nSyndicate=2\nMarriage=0\nMete=0\nHelpLineAmount=2\nLine0=find~a~guild\nLine1=and~marry\n"
        "[HelpInfo2]\nProfession=10\nMinLevel=40\nMaxLevel=139\nSyndicate=1\nMarriage=2\nMete=1\nHelpLineAmount=1\nLine0=mete~help\n";

    void WriteEncrypted(const char* name, const char* text, int len)
    {
        unsigned char key[128];
        srand(0x2537);
        for (int i = 0; i < 128; i++) key[i] = (unsigned char) (rand() % 256);
        FILE* fp = fopen(name, "wb");
        if (!fp) return;
        for (int i = 0; i < len; i++)
        {
            unsigned char c = (unsigned char) text[i];
            int d = i % 8;
            c = (unsigned char) ((unsigned char) (c << d) + (unsigned char) (c >> (8 - d)));
            fputc(c ^ key[i % 128], fp);
        }
        fclose(fp);
    }
    void WriteMonsterDat()
    {
        WriteEncrypted("ini/Monster.dat", MONSTER_TEXT, (int) sizeof(MONSTER_TEXT) - 1);
        WriteEncrypted("ini/UserHelpInfo.dat", USERHELP_TEXT, (int) sizeof(USERHELP_TEXT) - 1);
    }

    void Put32(FILE* fp, DWORD d) { fwrite(&d, 4, 1, fp); }
    void PutStr(FILE* fp, const char* s) { Put32(fp, (DWORD) strlen(s) + 1); fwrite(s, 1, strlen(s) + 1, fp); }

    void WriteBinaryTables()
    {
        FILE* fp = fopen("ini/tips.dat", "wb");
        if (fp)
        {
            char header[32] = "TqTips"; fwrite(header, 1, 32, fp); Put32(fp, 1); Put32(fp, 2);
            Put32(fp, 2); PutStr(fp, "first tip, line one"); PutStr(fp, "first tip, line two");
            Put32(fp, 1); PutStr(fp, "second tip");
            fclose(fp);
        }
        fp = fopen("ini/pet.dat", "wb");
        if (fp)
        {
            Put32(fp, 1);
            Put32(fp, 2); Put32(fp, 3); Put32(fp, 4);
            Put32(fp, 5); Put32(fp, 6); Put32(fp, 7); Put32(fp, 8); Put32(fp, 9);
            Put32(fp, 2 * (3 + 4) + 5 + 2 * 2 + 6 + 8 * 9);
            fclose(fp);
        }
        fp = fopen("ini/GameMap.dat", "wb");
        if (fp)
        {
            Put32(fp, 2);
            Put32(fp, 1002); Put32(fp, 6); fwrite("Desert", 1, 6, fp); Put32(fp, 24);
            Put32(fp, 1000); Put32(fp, 5); fwrite("Plain", 1, 5, fp); Put32(fp, 32);
            fclose(fp);
        }
    }

    struct RoleDataCwd
    {
        char old[MAX_PATH];
        RoleDataCwd()
        {
            GetCurrentDirectoryA(MAX_PATH, old);
            std::string f = __FILE__;
            SetCurrentDirectoryA((f.substr(0, f.find_last_of("\\/")) + "\\fixtures").c_str());
            WriteMonsterDat();
            WriteBinaryTables();
        }
        ~RoleDataCwd() { SetCurrentDirectoryA(old); }
    };
}

TEST_CLASS(GameDataRoleData)
{
public:
    TEST_METHOD(ActionEffectsByLookActionPartAndVariable)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        Assert::AreEqual("exact", p->GetRolePart3DEffect(1, 100, 2, 0, 0));
        Assert::AreEqual("anylook", p->GetRolePart3DEffect(5, 100, 2, 0, 0));
        Assert::AreEqual("anylook-anyaction", p->GetRolePart3DEffect(5, 123, 3, 0, 0));
        Assert::AreEqual("anylook-anypart", p->GetRolePart3DEffect(5, 200, 77, 0, 0));
        Assert::AreEqual("anylook-anyvar", p->GetRolePart3DEffect(5, 300, 4, 9, 0));
        Assert::AreEqual("anyaction", p->GetRolePart3DEffect(7, 123, 5, 0, 0));
        Assert::AreEqual("anypart", p->GetRolePart3DEffect(7, 400, 77, 1, 0));
        Assert::AreEqual("anyvar", p->GetRolePart3DEffect(7, 500, 6, 9, 0));
        Assert::IsNull(p->GetRolePart3DEffect(7, 600, 6, 0, 0));
        Assert::AreEqual("wide", p->GetRolePart3DEffect(4294967, 1, 2, 3, 0));
        Assert::AreEqual("table-one", p->GetRolePart3DEffect(1, 100, 2, 0, 1));
        Assert::AreEqual("table-one-any", p->GetRolePart3DEffect(5, 100, 2, 0, 1));
        Assert::AreEqual("table-one-any", p->GetRolePart3DEffect(5, 100, 2, 999, 1));
        Assert::IsNull(p->GetRolePart3DEffect(5, 101, 2, 0, 1));
        Assert::IsNull(p->GetRolePart3DEffect(1, 100, 2, 0, 2));
        Assert::IsNull(p->GetRolePart3DEffect(999, 999, 999, 999, 2));
        Assert::AreEqual("exact", p->GetRolePart3DEffect(1, 100, 2, 0, 3));
        Assert::AreEqual("table-one", p->GetRolePart3DEffect(1, 100, 2, 0, 0xFFFFFFFD));
        typedef const char* (__fastcall *FnEffect)(void*, void*, DWORD, DWORD, DWORD, DWORD, DWORD);
        Assert::AreEqual("exact", ((FnEffect) (*(void***) p)[3])(p, 0, 1, 100, 2, 0, 0));
        Assert::AreEqual("table-one", ((FnEffect) (*(void***) p)[3])(p, 0, 1, 100, 2, 0, 1));
        RoleDataDestroy();
    }

    TEST_METHOD(WeaponPairsMapEffectsAndActionSounds)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();

        Assert::IsTrue(p->CanWeaponCombin(410, 420) != FALSE);
        Assert::IsFalse(p->CanWeaponCombin(420, 410) != FALSE);
        Assert::IsTrue(p->CanWeaponCombin(500, 0) != FALSE);
        Assert::IsFalse(p->CanWeaponCombin(500, 1) != FALSE);
        typedef BOOL (__fastcall *FnCombin)(void*, void*, DWORD, DWORD);
        Assert::AreEqual(1, (int) ((FnCombin) (*(void***) p)[8])(p, 0, 410, 420));

        const ActionMapEffectInfo* e = p->GetActionMapEffect(1, 100, 2);
        Assert::IsNotNull(e);
        Assert::AreEqual("dust", e->szEffect);
        Assert::AreEqual(300, e->nShowTime);
        Assert::AreEqual(1, (int) e->bDirEnable);
        Assert::AreEqual("anylook", p->GetActionMapEffect(7, 100, 3)->szEffect);
        Assert::AreEqual("anyaction", p->GetActionMapEffect(1, 123, 4)->szEffect);
        Assert::AreEqual("anyterrain", p->GetActionMapEffect(1, 200, 77)->szEffect);
        Assert::AreEqual("terrainonly", p->GetActionMapEffect(7, 123, 5)->szEffect);
        Assert::AreEqual("actiononly", p->GetActionMapEffect(7, 300, 77)->szEffect);
        Assert::AreEqual("lookonly", p->GetActionMapEffect(2, 123, 77)->szEffect);
        Assert::IsNull(p->GetActionMapEffect(7, 123, 77));
        typedef const ActionMapEffectInfo* (__fastcall *FnMap)(void*, void*, int, int, int);
        Assert::IsTrue(((FnMap) (*(void***) p)[2])(p, 0, 1, 100, 2) == e);

        Assert::AreEqual("sound/sword.wav", p->GetActionSound(1, 410, 200));
        Assert::AreEqual("sound/any.wav", p->GetActionSound(1, 410, 300));
        Assert::AreEqual("sound/standby.wav", p->GetActionSound(2, 0, 100));
        Assert::IsNull(p->GetActionSound(1, 410, 400));
        Assert::IsNull(p->GetActionSound(3, 410, 200));
        typedef const char* (__fastcall *FnSound)(void*, void*, DWORD, DWORD, DWORD);
        Assert::AreEqual("sound/sword.wav", ((FnSound) (*(void***) p)[4])(p, 0, 1, 410, 200));
        RoleDataDestroy();
    }

    TEST_METHOD(MonstersByNameAndByType)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        const CMonsterInfo* a = p->GetMonsterInfo("Pheasant");
        Assert::IsNotNull(a);
        Assert::AreEqual("Pheasant", a->strName.c_str());
        Assert::AreEqual(1, a->nAddSize);
        Assert::AreEqual(100, a->nZoomPercent);
        Assert::AreEqual(33, a->nMaxLife);
        Assert::AreEqual(7, a->nBodyType);
        Assert::AreEqual<OBJID>(1, a->idType);
        Assert::AreEqual("none", a->strBornEffect.c_str());
        const CMonsterInfo* g = p->GetMonsterInfo("Guard");
        Assert::IsNotNull(g);
        Assert::AreEqual(-2, g->nAddSize);
        Assert::AreEqual(5000, g->nMaxLife);
        Assert::AreEqual(50, g->nLevel);
        Assert::AreEqual(3, g->nBornAction);
        Assert::AreEqual("born", g->strBornEffect.c_str());
        Assert::AreEqual("sound/born.wav", g->strBornSound.c_str());
        Assert::AreEqual(0, g->nActResCtrl);
        Assert::AreEqual(5, g->nAsb);
        Assert::AreEqual(2, g->nAdb);
        Assert::AreEqual<OBJID>(900, g->idType);
        Assert::AreEqual<DWORD>(4096, g->dwAntiType);
        Assert::AreEqual<OBJID>(111, g->idArmet);
        Assert::AreEqual<OBJID>(410, g->idRWeapon);
        Assert::AreEqual<OBJID>(900, g->idLWeapon);
        Assert::AreEqual<OBJID>(5, g->idMisc);
        Assert::AreEqual<OBJID>(7, g->idMount);
        Assert::IsNull(p->GetMonsterInfo("NoSuchMonster"));
        Assert::IsNull(p->GetMonsterInfo(0));
        Assert::IsNull(p->GetMonsterInfo("AMonsterOf16Chars"));
        Assert::IsTrue(p->GetMonsterInfoByType(900) == g);
        Assert::IsTrue(p->GetMonsterInfoByType(1) == a);
        Assert::IsNull(p->GetMonsterInfoByType(2));
        typedef const CMonsterInfo* (__fastcall *FnByType)(void*, void*, OBJID);
        typedef const CMonsterInfo* (__fastcall *FnByName)(void*, void*, const char*);
        Assert::IsTrue(((FnByType) (*(void***) p)[5])(p, 0, 900) == g);
        Assert::IsTrue(((FnByName) (*(void***) p)[6])(p, 0, "Guard") == g);
        RoleDataDestroy();
    }

    TEST_METHOD(CommonIniFillsTheEightInts)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        Assert::AreEqual(11, p->GetMaleFaceAmount());
        Assert::AreEqual(12, p->GetFemaleFaceAmount());
        Assert::AreEqual(150, p->GetLeaderScale());
        Assert::AreEqual(130, p->GetAssistantScale());
        Assert::AreEqual(120, p->GetSubLeaderScale());
        Assert::AreEqual(30000, p->GetLeaderMaxLife());
        Assert::AreEqual(20000, p->GetAssistantMaxLife());
        Assert::AreEqual(10000, p->GetSubLeaderMaxLife());
        RoleDataDestroy();
    }

    TEST_METHOD(TransformsByTheirNumber)
    {
        static_assert(sizeof(CTransformInfo) == 0x24, "");
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        CTransformInfo* t = p->GetTransformInfo(98);
        Assert::IsNotNull(t);
        Assert::AreEqual(1, (int) t->bCanJump);
        Assert::AreEqual(-3, t->nAddSize);
        Assert::AreEqual(120, t->nScale);
        Assert::AreEqual(201, t->nLook);
        Assert::AreEqual<OBJID>(1, t->idArmet);
        Assert::AreEqual<OBJID>(2, t->idRWeapon);
        Assert::AreEqual<OBJID>(3, t->idLWeapon);
        Assert::AreEqual<OBJID>(4, t->idMisc);
        Assert::AreEqual<OBJID>(5, t->idMount);
        CTransformInfo* u = p->GetTransformInfo(7);
        Assert::IsNotNull(u);
        Assert::AreEqual(0, (int) u->bCanJump);
        Assert::AreEqual(350, u->nLook);
        Assert::IsNull(p->GetTransformInfo(201));
        typedef CTransformInfo* (__fastcall *FnTransform)(void*, void*, int);
        Assert::IsTrue(((FnTransform) (*(void***) p)[7])(p, 0, 98) == t);
        RoleDataDestroy();
    }

    TEST_METHOD(NpcTypesAndTheSecondNpcTable)
    {
        static_assert(sizeof(NpcTypeInfo) == 0x70 && sizeof(NpcXTypeInfo) == 0x44, "");
        static_assert(offsetof(NpcXTypeInfo, idArmor) == 0x2C && offsetof(NpcXTypeInfo, idArmet) == 0x30, "");
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        NpcTypeInfo* g = p->GetNpcTypeInfo(20);
        Assert::IsNotNull(g);
        Assert::AreEqual(20, g->nType);
        Assert::AreEqual("Guard", g->szNpcName);
        Assert::AreEqual<OBJID>(2000, g->idSimpleObj);
        Assert::AreEqual<OBJID>(2001, g->idMotionStandBy);
        Assert::AreEqual<OBJID>(2002, g->idMotionBlaze);
        Assert::AreEqual<OBJID>(2003, g->idMotionRest);
        Assert::AreEqual("guard_fx", g->szEffectIndex);
        Assert::AreEqual(5, g->nAsb);
        Assert::AreEqual(6, g->nAdb);
        Assert::AreEqual(1, (int) g->bFixDir);
        NpcTypeInfo* tree = p->GetNpcTypeInfo(3);
        Assert::IsNotNull(tree);
        Assert::AreEqual("Tree", tree->szNpcName);
        Assert::IsTrue(p->GetNpcTypeInfo(999) == tree);
        typedef NpcTypeInfo* (__fastcall *FnNpc)(void*, void*, OBJID);
        Assert::IsTrue(((FnNpc) (*(void***) p)[9])(p, 0, 20) == g);

        NpcXTypeInfo* m = p->GetNpcXTypeInfo(500);
        Assert::IsNotNull(m);
        Assert::AreEqual(500, m->nType);
        Assert::AreEqual("Merchant", m->szNpcName);
        Assert::AreEqual(1, m->nAddSize);
        Assert::AreEqual(100, m->nScale);
        Assert::AreEqual(1, (int) m->bFixDir);
        Assert::AreEqual(1003, m->nLook);
        Assert::AreEqual(7, m->nHead);
        Assert::AreEqual(315, m->nHair);
        Assert::AreEqual<OBJID>(1130200, m->idArmor);
        Assert::AreEqual<OBJID>(410, m->idRWeapon);
        NpcXTypeInfo* mayor = p->GetNpcXTypeInfo(40);
        Assert::IsNotNull(mayor);
        Assert::AreEqual<OBJID>(118000, mayor->idArmet);
        Assert::IsTrue(p->GetNpcXTypeInfo(1) == mayor);
        typedef NpcXTypeInfo* (__fastcall *FnNpcX)(void*, void*, int);
        Assert::IsTrue(((FnNpcX) (*(void***) p)[10])(p, 0, 500) == m);
        RoleDataDestroy();
    }

    TEST_METHOD(PkTitlesEffectsAndMapMagicItems)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        Assert::AreEqual("Peaceful", p->GetPkTitle(0));
        Assert::AreEqual("Peaceful", p->GetPkTitle(29));
        Assert::AreEqual("Red", p->GetPkTitle(30));
        Assert::IsNull(p->GetPkTitle(100));
        Assert::IsNull(p->GetPkTitle(-1));
        typedef char* (__fastcall *FnPk)(void*, void*, int);
        Assert::AreEqual("Red", ((FnPk) (*(void***) p)[28])(p, 0, 50));

        EffectInfo* e = p->GetEffectInfo("hit");
        Assert::IsNotNull(e);
        Assert::AreEqual("hit", e->szTitle);
        Assert::AreEqual("hit_ani", e->szAniTitle);
        Assert::AreEqual<DWORD>(1, e->dwLoopTime);
        Assert::AreEqual<DWORD>(50, e->dwFrameInterval);
        Assert::AreEqual<DWORD>(2, e->dwShowWay);
        Assert::AreEqual<DWORD>(0, e->dwLoopInterval);
        Assert::AreEqual(-4, e->nOffsetX);
        Assert::AreEqual(9, e->nOffsetY);
        Assert::AreEqual(1, e->nExigence);
        Assert::AreEqual<DWORD>(120, e->dwDelay);
        Assert::IsNotNull(p->GetEffectInfo("glow"));
        Assert::IsNull(p->GetEffectInfo("none"));
        Assert::IsNull(p->GetEffectInfo(0));
        typedef EffectInfo* (__fastcall *FnEffect)(void*, void*, char*);
        Assert::IsTrue(((FnEffect) (*(void***) p)[12])(p, 0, "hit") == e);

        MapMagicItemInfo* m = p->GetMapMagicItemInfo(12);
        Assert::IsNotNull(m);
        Assert::AreEqual<OBJID>(12, m->idType);
        Assert::AreEqual("s12", m->szStart);
        Assert::AreEqual("l12", m->szLast);
        Assert::AreEqual("e12", m->szEnd);
        Assert::AreEqual("start5", p->GetMapMagicItemInfo(5)->szStart);
        Assert::IsNull(p->GetMapMagicItemInfo(6));
        typedef MapMagicItemInfo* (__fastcall *FnMagic)(void*, void*, int);
        Assert::IsTrue(((FnMagic) (*(void***) p)[11])(p, 0, 12) == m);
        RoleDataDestroy();
    }

    TEST_METHOD(FlyingObjectsWeatherAndStatusEffects)
    {
        static_assert(sizeof(FlyingObjInfo) == 0x108 && sizeof(TerrainWeatherInfo) == 0x14 && sizeof(StatusEffectInfo) == 0x204, "");
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        FlyingObjInfo* f = p->GetFlyingObjInfo(1050, 410);
        Assert::IsNotNull(f);
        Assert::AreEqual<OBJID>(7001, f->idSimpleObj);
        Assert::AreEqual("arrow_fx", f->szFlyEffect);
        Assert::AreEqual("sound/fly.wav", f->szFlySound);
        Assert::AreEqual("sound/hit.wav", f->szHitSound);
        Assert::AreEqual("target_fx", f->szTargetEffect);
        Assert::AreEqual<OBJID>(0, f->idType);
        Assert::AreEqual<OBJID>(7002, p->GetFlyingObjInfo(1050, 420)->idSimpleObj);
        Assert::AreEqual<OBJID>(7003, p->GetFlyingObjInfo(2000, 500)->idSimpleObj);
        Assert::AreEqual<OBJID>(7001, p->GetFlyingObjInfo(2000, 600)->idSimpleObj);
        typedef FlyingObjInfo* (__fastcall *FnFly)(void*, void*, OBJID, OBJID);
        Assert::IsTrue(((FnFly) (*(void***) p)[13])(p, 0, 1050, 410) == f);

        TerrainWeatherInfo* w = p->GetWeatherInfo(7);
        Assert::IsNotNull(w);
        Assert::AreEqual(1, w->nType);
        Assert::AreEqual(0, w->nWindDir);
        Assert::AreEqual(2, w->nLevel);
        Assert::AreEqual<DWORD>(255, w->dwColor);
        Assert::AreEqual<DWORD>(4278190080u, p->GetWeatherInfo(1)->dwColor);
        Assert::IsNull(p->GetWeatherInfo(9));
        typedef TerrainWeatherInfo* (__fastcall *FnWeather)(void*, void*, int);
        Assert::IsTrue(((FnWeather) (*(void***) p)[26])(p, 0, 7) == w);

        Assert::AreEqual<DWORD>(2, p->GetStatusEffectInfoAmount());
        StatusEffectInfo* s = p->GetStatusEffectInfoByIndex(1);
        Assert::IsNotNull(s);
        Assert::AreEqual<DWORD>(0x80, s->dwStatus);
        Assert::AreEqual("shield3d", s->sz3DEffect);
        Assert::AreEqual("none", s->sz2DEffect);
        Assert::AreEqual<DWORD>(1, p->GetStatusEffectInfoByIndex(0)->dwStatus);
        Assert::IsNull(p->GetStatusEffectInfoByIndex(2));
        Assert::IsNull(p->GetStatusEffectInfoByIndex(-1));
        typedef DWORD (__fastcall *FnAmount)(void*, void*);
        typedef StatusEffectInfo* (__fastcall *FnStatus)(void*, void*, int);
        Assert::AreEqual<DWORD>(2, ((FnAmount) (*(void***) p)[14])(p, 0));
        Assert::IsTrue(((FnStatus) (*(void***) p)[15])(p, 0, 1) == s);
        RoleDataDestroy();
    }

    TEST_METHOD(TerrainNpcTypes)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        TerrainNpcTypeInfo* t = p->GetTerrainNpcInfo(60);
        Assert::IsNotNull(t);
        Assert::AreEqual(60, t->nType);
        Assert::AreEqual("Tree", t->szNpcName);
        Assert::AreEqual<size_t>(8, t->setStr.size());
        Assert::AreEqual("t/0.ani", t->setStr[0].c_str());
        Assert::AreEqual("t/7.ani", t->setStr[7].c_str());
        Assert::AreEqual("Rock", p->GetTerrainNpcInfo(61)->szNpcName);
        Assert::IsTrue(p->GetTerrainNpcInfo(99) == t);
        typedef TerrainNpcTypeInfo* (__fastcall *FnTerrain)(void*, void*, OBJID);
        Assert::IsTrue(((FnTerrain) (*(void***) p)[18])(p, 0, 61) != t);
        RoleDataDestroy();
    }

    TEST_METHOD(DelaysWeaponsActionControlMediaAndRestraints)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        CONST CActionDelayInfo* d = p->GetActionDelayInfo(1001);
        Assert::IsNotNull(d);
        Assert::AreEqual<DWORD>(300, d->tmWoundDelay);
        Assert::AreEqual<DWORD>(400, d->tmBlockDelay);
        Assert::AreEqual<DWORD>(1500, d->tmDieDelay);
        Assert::IsNull(p->GetActionDelayInfo(1002));

        CONST CWeaponInfo* w = p->GetWeaponInfo(410);
        Assert::IsNotNull(w);
        Assert::AreEqual("sword_hit", w->strHitEffect.c_str());
        Assert::AreEqual("", w->strHitSound.c_str());
        Assert::AreEqual("", w->strBlkEffect.c_str());
        Assert::AreEqual("sound/blk.wav", w->strBlkSound.c_str());
        Assert::IsNull(p->GetWeaponInfo(420));

        CONST CActionCtrlInfo* c = p->GetActionCtrlInfo(1410200);
        Assert::IsNotNull(c);
        Assert::AreEqual(2, c->nSection);
        Assert::AreEqual(30, c->nTimePercent[0]);
        Assert::AreEqual(100, c->nMovePercent[1]);
        Assert::AreEqual(1, (int) c->bVisible);
        Assert::AreEqual(50, p->GetActionCtrlInfo(7000300)->nMovePercent[0]);
        Assert::AreEqual(0, p->GetActionCtrlInfo(2999400)->nSection);
        Assert::AreEqual(1, (int) p->GetActionCtrlInfo(2999400)->bVisible);
        Assert::AreEqual(0, (int) p->GetActionCtrlInfo(2410400)->bVisible);
        Assert::IsTrue(p->GetActionCtrlInfo(2410400) == p->GetActionCtrlInfo(2410500));
        Assert::IsTrue(p->GetActionCtrlInfo(2410400) == p->GetActionCtrlInfo(999999999));

        CONST CMediaEffectInfo* m = p->GetMediaEffectInfo(100001);
        Assert::IsNotNull(m);
        Assert::AreEqual("levelup", m->strEffect.c_str());
        Assert::AreEqual("sound/levelup.wav", m->strSound.c_str());
        Assert::AreEqual("", p->GetMediaEffectInfo(100002)->strEffect.c_str());
        Assert::IsNull(p->GetMediaEffectInfo(100003));

        CONST RestrainInfo* r = p->GetRestrainInfo(2);
        Assert::IsNotNull(r);
        Assert::AreEqual<DWORD>(50, r->dwMultiple);
        Assert::AreEqual("water", r->strName.c_str());
        Assert::IsNull(p->GetRestrainInfo(3));

        typedef const void* (__fastcall *FnKey)(void*, void*, DWORD);
        void** vt = *(void***) p;
        Assert::IsTrue(((FnKey) vt[29])(p, 0, 1001) == d);
        Assert::IsTrue(((FnKey) vt[30])(p, 0, 410) == w);
        Assert::IsTrue(((FnKey) vt[31])(p, 0, 1410200) == c);
        Assert::IsTrue(((FnKey) vt[32])(p, 0, 100001) == m);
        Assert::IsTrue(((FnKey) vt[33])(p, 0, 2) == r);
        RoleDataDestroy();
    }

    TEST_METHOD(TipsPetsAndGameMaps)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        Assert::AreEqual(2, p->GetTipsAmount());
        Assert::AreEqual(2, p->GetTipLineAmount(0));
        Assert::AreEqual(1, p->GetTipLineAmount(1));
        Assert::AreEqual(0, p->GetTipLineAmount(2));
        Assert::AreEqual("first tip, line two", p->GetTipLine(0, 1));
        Assert::AreEqual("second tip", p->GetTipLine(1, 0));
        Assert::IsNull(p->GetTipLine(1, 1));
        Assert::IsNull(p->GetTipLine(-1, 0));
        typedef int (__fastcall *FnInt)(void*, void*);
        typedef char* (__fastcall *FnLine)(void*, void*, int, int);
        Assert::AreEqual(2, ((FnInt) (*(void***) p)[19])(p, 0));
        Assert::AreEqual("second tip", ((FnLine) (*(void***) p)[21])(p, 0, 1, 0));

        PetInfo* pet = p->GetPetInfo(0);
        Assert::IsNotNull(pet);
        Assert::AreEqual<size_t>(2, pet->setLocateTargetMethod.size());
        Assert::AreEqual(4, pet->setLocateTargetMethod[1]);
        Assert::AreEqual(5, pet->nEnvMethod);
        Assert::AreEqual(6, pet->nAddSize);
        Assert::AreEqual(7, pet->nScale);
        Assert::AreEqual(8, pet->nAttackRange);
        Assert::AreEqual<DWORD>(9, pet->dwAttackInterval);
        Assert::IsNull(p->GetPetInfo(1));
        typedef PetInfo* (__fastcall *FnPet)(void*, void*, int);
        Assert::IsTrue(((FnPet) (*(void***) p)[25])(p, 0, 0) == pet);

        GameMapInfo* gm = p->GetGameMapInfo(1002);
        Assert::IsNotNull(gm);
        Assert::AreEqual("Desert", gm->szName);
        Assert::AreEqual<DWORD>(24, gm->dwPuzzleGrideSize);
        Assert::AreEqual("Plain", p->GetGameMapInfo(1000)->szName);
        Assert::IsNull(p->GetGameMapInfo(1001));
        typedef GameMapInfo* (__fastcall *FnMap)(void*, void*, OBJID);
        Assert::IsTrue(((FnMap) (*(void***) p)[27])(p, 0, 1002) == gm);
        RoleDataDestroy();
    }

    TEST_METHOD(UserHelpByProfessionLevelAndState)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        Assert::AreEqual(2, p->GetUserHelpInfoLineAmount(10, 20, 0, 0, 0));
        Assert::AreEqual("find a guild", p->GetUserHelpInfoLineByIndex(10, 20, 1, 0, 0, 0));
        Assert::AreEqual("and marry", p->GetUserHelpInfoLineByIndex(10, 39, 0, 0, 0, 1));
        Assert::IsNull(p->GetUserHelpInfoLineByIndex(10, 20, 0, 0, 0, 2));
        Assert::AreEqual("mete help", p->GetUserHelpInfoLineByIndex(10, 40, 1, 1, 1, 0));
        Assert::AreEqual("default help", p->GetUserHelpInfoLineByIndex(10, 40, 0, 1, 1, 0));
        Assert::AreEqual("default help", p->GetUserHelpInfoLineByIndex(10, 20, 0, 1, 0, 0));
        Assert::AreEqual("default help", p->GetUserHelpInfoLineByIndex(10, 20, 0, 0, 1, 0));
        Assert::AreEqual(1, p->GetUserHelpInfoLineAmount(99, 1, 0, 0, 0));
        typedef const char* (__fastcall *FnLine)(void*, void*, int, int, int, int, int, int);
        typedef int (__fastcall *FnAmount)(void*, void*, int, int, int, int, int);
        Assert::AreEqual("and marry", ((FnLine) (*(void***) p)[16])(p, 0, 10, 20, 0, 0, 0, 1));
        Assert::AreEqual(2, ((FnAmount) (*(void***) p)[17])(p, 0, 10, 20, 0, 0, 0));
        RoleDataDestroy();
    }

    TEST_METHOD(ActionTimeIntervalsAndTheEmptyTextActions)
    {
        RoleDataCwd cwd;
        IRoleData* p = RoleDataQuery();
        p->Init();
        Assert::AreEqual<DWORD>(66, p->GetActionTimeInterval(100));
        Assert::AreEqual<DWORD>(66, p->GetActionTimeInterval(1000103));
        Assert::AreEqual<DWORD>(25, p->GetActionTimeInterval(110));
        Assert::AreEqual<DWORD>(33, p->GetActionTimeInterval(111));
        Assert::AreEqual<DWORD>(25, p->GetActionTimeInterval(121));
        Assert::AreEqual<DWORD>(10, p->GetActionTimeInterval(451));
        Assert::AreEqual<DWORD>(10, p->GetActionTimeInterval(470));
        Assert::AreEqual<DWORD>(33, p->GetActionTimeInterval(454));
        Assert::AreEqual<DWORD>(33, p->GetActionTimeInterval(99));
        Assert::AreEqual<DWORD>(25, p->GetActionTimeInterval(12345678901120i64));
        typedef DWORD (__fastcall *FnTime)(void*, void*, DWORD, DWORD);
        Assert::AreEqual<DWORD>(66, ((FnTime) (*(void***) p)[1])(p, 0, 102, 0));
        Assert::AreEqual(0, p->GetTextActionAmount());
        Assert::IsNull(p->GetTextActionTitle(0));
        Assert::IsNull(p->GetTextAction(0));
        RoleDataDestroy();
    }

    TEST_METHOD(TheTwoExportsAndTheObject)
    {
        HMODULE h = GetModuleHandleA("GameData.dll");
        Assert::IsNotNull(h);
        Assert::IsTrue(GetProcAddress(h, "RoleDataQuery") == (FARPROC) &RoleDataQuery);
        Assert::IsTrue(GetProcAddress(h, "RoleDataDestroy") == (FARPROC) &RoleDataDestroy);

        IRoleData* p = RoleDataQuery();
        Assert::IsNotNull(p);
        Assert::IsTrue(RoleDataQuery() == p);
        RoleDataDestroy();
        RoleDataDestroy();
        Assert::IsNotNull(RoleDataQuery());
        RoleDataDestroy();
    }

    TEST_METHOD(TheInlineGettersReadTheEightInts)
    {
        IRoleData* p = RoleDataQuery();
        int* words = (int*) p;
        for (int i = 1; i <= 8; i++) words[i] = 100 + i;
        typedef int (__fastcall *FnGet)(void*, void*);
        void** vt = *(void***) p;
        for (int i = 1; i <= 8; i++)
            Assert::AreEqual(100 + i, ((FnGet) vt[36 + i])(p, 0));
        Assert::AreEqual(101, p->GetMaleFaceAmount());
        Assert::AreEqual(102, p->GetFemaleFaceAmount());
        Assert::AreEqual(103, p->GetLeaderScale());
        Assert::AreEqual(104, p->GetAssistantScale());
        Assert::AreEqual(105, p->GetSubLeaderScale());
        Assert::AreEqual(106, p->GetLeaderMaxLife());
        Assert::AreEqual(107, p->GetAssistantMaxLife());
        Assert::AreEqual(108, p->GetSubLeaderMaxLife());
        RoleDataDestroy();
    }

    TEST_METHOD(AnItemIconWithoutAnId)
    {
        IRoleData* p = RoleDataQuery();
        Assert::IsNull(p->GetItemMinIcon(0, EXIGENCE_IMMEDIATE));
        typedef IAni* (__fastcall *FnIcon)(void*, void*, OBJID, int);
        Assert::IsNull(((FnIcon) (*(void***) p)[34])(p, 0, 0, EXIGENCE_NORMAL));
        RoleDataDestroy();
    }
};
