#include "CppUnitTest.h"
#include <windows.h>
#include <cstring>
#include <string>
#include <vector>
#include <type_traits>
#include "GameDataSet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(IGameDataSet) == 4 && sizeof(IDataIcon) == 4, "");
static_assert(std::is_same<decltype(&GameDataSetQuery), IGameDataSet* (*)(void)>::value, "");
static_assert(std::is_same<decltype(&GameDataSetDestroy), void (*)(void)>::value, "");
static_assert(EXIGENCE_NORMAL == 0 && EXIGENCE_IMMEDIATE == 2, "");

namespace
{
    struct DataSetCwd
    {
        char old[MAX_PATH];
        DataSetCwd()
        {
            GetCurrentDirectoryA(MAX_PATH, old);
            std::string f = __FILE__;
            SetCurrentDirectoryA((f.substr(0, f.find_last_of("\\/")) + "\\fixtures").c_str());
        }
        ~DataSetCwd() { SetCurrentDirectoryA(old); }
    };
}

static void InitAndDestroy(IGameDataSet* p)
{
    DataSetCwd cwd;
    p->Init(FALSE);
    GameDataSetDestroy();
}

TEST_CLASS(GraphicDataGameDataSet)
{
public:
    TEST_METHOD(TheExportsAreUndecorated)
    {
        HMODULE h = GetModuleHandleA("GraphicData.dll");
        Assert::IsNotNull(h);
        Assert::IsTrue(GetProcAddress(h, "GameDataSetQuery") == (FARPROC) &GameDataSetQuery);
        Assert::IsTrue(GetProcAddress(h, "GameDataSetDestroy") == (FARPROC) &GameDataSetDestroy);
    }

    TEST_METHOD(OneObjectWithTwoInterfaces)
    {
        IGameDataSet* p = GameDataSetQuery();
        Assert::IsNotNull(p);
        Assert::IsTrue(GameDataSetQuery() == p);
        void* icon = p->QueryInterface(0);
        Assert::IsTrue(icon == (BYTE*) p + 4);
        Assert::IsNull(p->QueryInterface(1));
        typedef void* (__fastcall *FnQuery)(void*, void*, int);
        Assert::IsTrue(((FnQuery) (*(void***) p)[23])(p, 0, 0) == icon);
        typedef const char* (__fastcall *FnText)(void*, void*);
        Assert::AreEqual(p->GetStr(10426), ((FnText) (*(void***) p)[13])(p, 0));
        Assert::AreEqual(p->GetStr(10426), p->GetVersionInfo());
        Assert::IsNull(p->Slot10(123));
        InitAndDestroy(p);
        GameDataSetDestroy();
        IGameDataSet* q = GameDataSetQuery();
        Assert::IsNotNull(q);
        InitAndDestroy(q);
    }

    TEST_METHOD(StringsAndEmotions)
    {
        DataSetCwd cwd;
        IGameDataSet* p = GameDataSetQuery();
        Assert::AreEqual("SE:100", p->GetStr(100));
        p->InitStrRes();
        Assert::AreEqual("Hello", p->GetStr(100));
        typedef const char* (__fastcall *FnStr)(void*, void*, OBJID);
        Assert::AreEqual("Hello", ((FnStr) (*(void***) p)[15])(p, 0, 100));
        Assert::AreEqual(0, p->GetEmotionAmount());
        Assert::IsNull(p->GetEmotionShutCut(0));
        IDataIcon* icon = (IDataIcon*) p->QueryInterface(0);
        Assert::IsNull(icon->GetIcon(3));
        InitAndDestroy(p);
    }

    TEST_METHOD(InitReadsTheTextTables)
    {
        DataSetCwd cwd;
        IGameDataSet* p = GameDataSetQuery();
        Assert::IsNull(p->GetSound(1));
        p->Init(FALSE);
        Assert::AreEqual("sound/one.wav", p->GetSound(1));
        Assert::IsNull(p->GetSound(3));
        Assert::IsNull(p->GetSound(2));
        typedef const char* (__fastcall *FnSound)(void*, void*, OBJID);
        Assert::AreEqual("sound/one.wav", ((FnSound) (*(void***) p)[12])(p, 0, 1));
        Assert::AreEqual(3, p->GetEmotionAmount());
        Assert::AreEqual("/smile", p->GetEmotionShutCut(0));
        Assert::AreEqual("/laugh", p->GetEmotionShutCut(2));
        Assert::IsNull(p->GetEmotionShutCut(3));
        Assert::IsNull(p->GetEmotionShutCut(-1));
        typedef int (__fastcall *FnInt)(void*, void*);
        Assert::AreEqual(3, ((FnInt) (*(void***) p)[24])(p, 0));
        C3DSimpleObjInfo* so = p->Get3DSimpleObjInfo(7);
        Assert::IsNotNull(so);
        Assert::AreEqual<DWORD>(7, so->idType); Assert::AreEqual(2, so->nParts);
        Assert::AreEqual<DWORD>(101, so->idMesh[0]); Assert::AreEqual<DWORD>(201, so->idTexture[0]);
        Assert::AreEqual<DWORD>(102, so->idMesh[1]); Assert::AreEqual<DWORD>(202, so->idTexture[1]);
        so = p->Get3DSimpleObjInfo(9);
        Assert::IsNotNull(so);
        Assert::AreEqual(0, so->nParts);
        typedef C3DSimpleObjInfo* (__fastcall *FnSimple)(void*, void*, OBJID);
        Assert::IsTrue(((FnSimple) (*(void***) p)[16])(p, 0, 7) == p->Get3DSimpleObjInfo(7));
        static_assert(sizeof(C3DSimpleObjExInfo) == 0x78, "");
        C3DSimpleObjExInfo* ex = p->Get3DSimpleObjExInfo(5);
        Assert::IsNotNull(ex);
        Assert::AreEqual<DWORD>(5, ex->idType); Assert::AreEqual(2, ex->part.nPart);
        Assert::AreEqual<DWORD>(1, ex->part.idMesh[0]); Assert::AreEqual<DWORD>(7, ex->part.idMixTex[0]); Assert::AreEqual(6, ex->part.nAdb[0]);
        Assert::AreEqual<DWORD>(2, ex->part.idMesh[1]); Assert::AreEqual(2, ex->part.nAdb[1]);
        Assert::IsNotNull(ex->part.pMaterial[0]);
        Assert::IsNotNull(p->Get3DSimpleObjExInfo(6));
        Assert::AreEqual(0, p->Get3DSimpleObjExInfo(6)->part.nPart);
        Assert::IsNull(p->Get3DSimpleObjExInfo(7));
        typedef C3DSimpleObjExInfo* (__fastcall *FnSimpleEx)(void*, void*, OBJID);
        Assert::IsTrue(((FnSimpleEx) (*(void***) p)[17])(p, 0, 5) == ex);
        Assert::AreEqual<DWORD>(12, p->GetMeshLod(1, 1)); Assert::AreEqual<DWORD>(13, p->GetMeshLod(1, 2));
        Assert::AreEqual<DWORD>(11, p->GetMeshLod(1, 0)); Assert::AreEqual<DWORD>(11, p->GetMeshLod(1, 3));
        Assert::AreEqual<DWORD>(22, p->GetMeshLod(2, 1));
        Assert::AreEqual<DWORD>(3, p->GetMeshLod(3, 1));
        typedef OBJID (__fastcall *FnLod)(void*, void*, OBJID, int);
        Assert::AreEqual<DWORD>(13, ((FnLod) (*(void***) p)[29])(p, 0, 1, 2));
        p->SetLoadAtOnce(500);
        ((void (__fastcall*)(void*, void*, DWORD)) (*(void***) p)[28])(p, 0, 0);
        Assert::IsTrue(((BOOL (__fastcall*)(void*, void*, BOOL, DWORD)) (*(void***) p)[30])(p, 0, FALSE, 0) != FALSE);
        Assert::IsTrue(p->Reload(FALSE, 0x200 | 0x40 | 0x80 | 0x20 | 0x100) != FALSE);
        Assert::IsNotNull(p->Get3DSimpleObjExInfo(5)); Assert::AreEqual<DWORD>(13, p->GetMeshLod(1, 2));
        static_assert(sizeof(CMy3DEffectInfo) == 0x180, "");
        static_assert(offsetof(CMy3DEffectInfo, nScale) == 0x124 && offsetof(CMy3DEffectInfo, nDelay) == 0x164, "");
        static_assert(sizeof(C3DSimpleObjInfo) == 0x28, "");
        CMy3DEffectInfo* fx = p->GetMy3DEffectInfo("Fire");
        Assert::IsNotNull(fx);
        Assert::AreEqual("Fire", fx->szIndex); Assert::AreEqual(2, fx->nAmount);
        Assert::AreEqual<DWORD>(11, fx->idEffect[0]); Assert::AreEqual<DWORD>(21, fx->idTexture[0]);
        Assert::AreEqual(5, fx->nASB[0]); Assert::AreEqual(6, fx->nADB[0]);
        Assert::AreEqual<DWORD>(12, fx->idEffect[1]); Assert::AreEqual(2, fx->nADB[1]);
        Assert::AreEqual(100, fx->nScale[0]); Assert::AreEqual(50, fx->nScale[1]);
        Assert::AreEqual(100, fx->nDelay); Assert::AreEqual(3, fx->nLoopTime); Assert::AreEqual(33, fx->nFrameInterval);
        Assert::AreEqual(0, fx->nLoopInterval); Assert::AreEqual(-4, fx->nOffsetX); Assert::AreEqual(8, fx->nOffsetZ);
        Assert::IsNull(p->GetMy3DEffectInfo("Water"));
        Assert::IsNull(p->GetMy3DEffectInfo(0));
        Assert::IsTrue(p->FindMy3DEffectInfo("Fire") == fx);
        Assert::IsNull(p->FindMy3DEffectInfo(0));
        CMy3DEffectInfo add;
        memset(&add, 0, sizeof(add));
        strcpy(add.szIndex, "Water"); add.nAmount = 1; add.idEffect[0] = 77;
        p->AddMy3DEffectInfo(0);
        p->AddMy3DEffectInfo(&add);
        CMy3DEffectInfo* got = p->GetMy3DEffectInfo("Water");
        Assert::IsNotNull(got);
        Assert::IsTrue(got != &add);
        Assert::AreEqual<DWORD>(77, got->idEffect[0]);
        typedef CMy3DEffectInfo* (__fastcall *FnFx)(void*, void*, const char*);
        Assert::IsTrue(((FnFx) (*(void***) p)[18])(p, 0, "Fire") == fx);
        Assert::IsTrue(((FnFx) (*(void***) p)[19])(p, 0, "Water") == got);
        Assert::IsTrue(p->IsHiddenPhy("v_pet") != FALSE);
        Assert::IsTrue(p->IsHiddenPhy("v_back") != FALSE);
        Assert::IsFalse(p->IsHiddenPhy("body") != FALSE);
        typedef BOOL (__fastcall *FnHidden)(void*, void*, const char*);
        Assert::IsTrue(((FnHidden) (*(void***) p)[22])(p, 0, "v_pet") != FALSE);
        static_assert(sizeof(RolePartInfo) == 0x74, "");
        RolePartInfo* rp = p->GetRolePartInfo("Armor", 1001);
        Assert::IsNotNull(rp);
        Assert::AreEqual(2, rp->nPart);
        Assert::AreEqual<DWORD>(11, rp->idMesh[0]); Assert::AreEqual<DWORD>(21, rp->idTexture[0]); Assert::AreEqual<DWORD>(31, rp->idMixTex[0]);
        Assert::AreEqual(1, rp->nMixOpt[0]); Assert::AreEqual(5, rp->nAsb[0]); Assert::AreEqual(6, rp->nAdb[0]);
        Assert::AreEqual<DWORD>(12, rp->idMesh[1]); Assert::AreEqual<DWORD>(22, rp->idTexture[1]); Assert::AreEqual(2, rp->nAdb[1]);
        D3DMATERIAL8* glow = rp->pMaterial[0];
        Assert::IsNotNull(glow);
        Assert::AreEqual(128.0f / 255, glow->Ambient.a, 1e-6f); Assert::AreEqual(1.0f, glow->Ambient.r, 1e-6f); Assert::AreEqual(0.0f, glow->Ambient.g);
        Assert::AreEqual(1.0f, glow->Diffuse.g, 1e-6f); Assert::AreEqual(0.0f, glow->Diffuse.r);
        Assert::AreEqual(1.0f, glow->Specular.b, 1e-6f);
        Assert::AreEqual(64.0f / 255, glow->Emissive.r, 1e-6f);
        Assert::AreEqual(0.0f, glow->Power);
        Assert::IsTrue(rp->pMaterial[1] == glow);
        RolePartInfo* rp2 = p->GetRolePartInfo("Armor", 1002);
        Assert::IsNotNull(rp2);
        Assert::AreEqual(1, rp2->nPart);
        Assert::IsTrue(rp2->pMaterial[0] != glow);
        Assert::AreEqual(128.0f / 255, rp2->pMaterial[0]->Ambient.r, 1e-6f); Assert::AreEqual(10.0f, rp2->pMaterial[0]->Power);
        Assert::IsNull(p->GetRolePartInfo("Armor", 5));
        Assert::IsNull(p->GetRolePartInfo("Helmet", 1001));
        Assert::IsNull(p->GetRolePartInfo(0, 1001));
        typedef RolePartInfo* (__fastcall *FnPart)(void*, void*, const char*, OBJID);
        Assert::IsTrue(((FnPart) (*(void***) p)[21])(p, 0, "Armor", 1001) == rp);
        Assert::AreEqual("gdmesh/obj.c3", p->GetMotionRes("Armor", 100));
        Assert::IsNull(p->GetMotionRes("Armor", 101));
        Assert::IsNull(p->GetMotionRes("Helmet", 100));
        Assert::IsNull(p->GetMotionRes(0, 100));
        Assert::AreEqual<DWORD>(300, p->GetActionTransTime(100, 101));
        Assert::AreEqual<DWORD>(450, p->GetActionTransTime(100, 102));
        Assert::AreEqual<DWORD>(0, p->GetActionTransTime(100, 103));
        Assert::AreEqual<DWORD>(0, p->GetActionTransTime(101, 100));
        Assert::AreEqual<DWORD>(20, p->GetActionTransTime(200, 100));
        typedef DWORD (__fastcall *FnTrans)(void*, void*, DWORD, DWORD);
        Assert::AreEqual<DWORD>(300, ((FnTrans) (*(void***) p)[27])(p, 0, 100, 101));
        typedef const char* (__fastcall *FnRes)(void*, void*, const char*, OBJID);
        Assert::AreEqual("gdmesh/obj.c3", ((FnRes) (*(void***) p)[14])(p, 0, "Armor", 100));
        GameDataSetDestroy();
    }
    static void WriteResBin(const char* name, const unsigned __int64* ids, const char* const* files, int n)
    {
        FILE* fp = fopen(name, "wb");
        Assert::IsNotNull(fp);
        char header[32] = "TqDigitalRes";
        fwrite(header, 32, 1, fp);
        DWORD version = 0x10000;
        fwrite(&version, 4, 1, fp);
        fwrite(&n, 4, 1, fp);
        for (int i = 0; i < n; i++)
        {
            BYTE rec[16] = { 0 };
            memcpy(rec, &ids[i], 8);
            rec[8] = (BYTE) strlen(files[i]);
            memset(rec + 9, 0xEE, 7);
            fwrite(rec, 16, 1, fp);
            fwrite(files[i], rec[8], 1, fp);
        }
        fclose(fp);
    }

    static void WriteText(const char* name, const char* text)
    {
        FILE* fp = fopen(name, "wb");
        Assert::IsNotNull(fp);
        fwrite(text, strlen(text), 1, fp);
        fclose(fp);
    }
    static std::vector<BYTE> ReadAll(const char* name)
    {
        std::vector<BYTE> v;
        FILE* fp = fopen(name, "rb");
        Assert::IsNotNull(fp);
        int c;
        while ((c = fgetc(fp)) != EOF) v.push_back((BYTE) c);
        fclose(fp);
        return v;
    }

    TEST_METHOD(BuildBinCompilesTheTablesAndInitReadsThemBack)
    {
        DataSetCwd fixtures;
        CreateDirectoryA("build", 0);
        CreateDirectoryA("build\\ini", 0);
        CreateDirectoryA("build\\gdb", 0);
        const char* const copied[] = { "common.ini", "Material.ini", "EmotionIco.ini", "ActionTransition.ini", "StrRes.ini" };
        for (int i = 0; i < 5; i++)
            Assert::IsTrue(CopyFileA((std::string("ini\\") + copied[i]).c_str(), (std::string("build\\ini\\") + copied[i]).c_str(), FALSE) != FALSE);
        SetCurrentDirectoryA("build");

        const DWORD pack[3] = { 0, 0, 12 };
        { FILE* fp = fopen("gdb.wdf", "wb"); if (fp) { fwrite(pack, 12, 1, fp); fclose(fp); } }
        const char* const loose[] = { "gdb\\mesh1.c3", "gdb\\mesh2.c3", "gdb\\tex1.dds", "gdb\\eff1.c3", "gdb\\snd1.wav", "gdb\\mo1.c3" };
        for (int i = 0; i < 6; i++) WriteText(loose[i], "x");
        WriteText("ini/package.ini", "gdb.wdf\n");
        WriteText("ini/3dobj.ini", "1=gdb/mesh1.c3\n2=gdb/mesh2.c3\n");
        WriteText("ini/3dtexture.ini", "1=gdb/tex1.dds\n");
        WriteText("ini/3deffectobj.ini", "1=gdb/eff1.c3\n");
        WriteText("ini/sound.ini", "1=gdb/snd1.wav\nnot-a-line\n4294967298=gdb/snd1.wav\n");
        WriteText("ini/3DSimpleObj.ini", "[ObjIDType7]\nPartAmount=2\nPart0=1\nTexture0=1\nPart1=2\nTexture1=1\n\n[ObjIDType9]\nPartAmount=1\nPart0=1\nTexture0=1\n");
        WriteText("ini/3DEffect.ini", "[Fire]\nAmount=1\nEffectId0=1\nTextureId0=1\nASB0=5\nADB0=6\nDelay=100\nLoopTime=3\nFrameInterval=33\nLoopInterval=0\nOffsetX=-4\nOffsetY=0\nOffsetZ=8\n");
        WriteText("ini/RolePart.ini", "[Config]\nCount=1\nPart0=Armor\nMeshIni0=ini/armor.ini\nMotionIni0=ini/armormotion.ini\n[Dumy]\nCount=1\nDumy0=v_pet\n");
        WriteText("ini/armor.ini", "[1]\nPart=0\n[2]\nPart=0\n[3]\nPart=0\n[4]\nPart=0\n"
                                   "[5]\nPart=2\nMesh0=1\nTexture0=1\nMixTex0=7\nMixOpt0=1\nAsb0=5\nAdb0=6\nMaterial0=glow\n"
                                   "Mesh1=2\nTexture1=1\nMixTex1=0\nMixOpt1=0\nAsb1=5\nAdb1=2\nMaterial1=stone\n");
        WriteText("ini/armormotion.ini", "100=gdb/mo1.c3\n");

        IGameDataSet* p = GameDataSetQuery();
        p->BuildBin();
        typedef void (__fastcall *FnBuild)(void*, void*);
        Assert::IsNotNull((void*) (FnBuild) (*(void***) p)[26]);

        std::vector<BYTE> v = ReadAll("ini/sound.bin");
        Assert::AreEqual<size_t>(36 + 4 + 2 * (16 + 12), v.size());
        Assert::AreEqual(0, memcmp(&v[0], "TqDigitalRes\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 32));
        Assert::AreEqual<DWORD>(0x10000, *(DWORD*) &v[32]);
        Assert::AreEqual<DWORD>(2, *(DWORD*) &v[36]);
        Assert::IsTrue(*(unsigned __int64*) &v[40] == 1);
        Assert::AreEqual<int>(12, v[48]);
        Assert::AreEqual(0, memcmp(&v[56], "gdb/snd1.wav", 12));
        Assert::IsTrue(*(unsigned __int64*) &v[68] == 4294967298ui64);
        v = ReadAll("ini/3DSimpleObj.bin");
        Assert::AreEqual<size_t>(40 + 2 * 0x28, v.size());
        Assert::AreEqual(0, strcmp((char*) &v[0], "TqDigital3DSimpleObj"));
        Assert::AreEqual<DWORD>(2, *(DWORD*) &v[36]);
        const C3DSimpleObjInfo* so = (const C3DSimpleObjInfo*) &v[40];
        Assert::AreEqual<OBJID>(7, so[0].idType);
        Assert::AreEqual<OBJID>(9, so[1].idType);
        Assert::AreEqual(1, so[1].nParts);
        Assert::AreEqual<OBJID>(2, so[1].idMesh[1]);
        v = ReadAll("ini/3DEffect.bin");
        Assert::AreEqual<size_t>(40 + 0x180, v.size());
        Assert::AreEqual(0, strcmp((char*) &v[0], "TqDigital3DEffect"));
        Assert::AreEqual(0, strcmp((char*) &v[40], "Fire"));
        Assert::AreEqual(0, ((const CMy3DEffectInfo*) &v[40])->nScale[0]);
        v = ReadAll("ini/armor.bin");
        Assert::AreEqual<size_t>(40 + 5 * 0xF8, v.size());
        Assert::AreEqual(0, strcmp((char*) &v[0], "TqDigitalarmor"));
        Assert::AreEqual<DWORD>(5, *(DWORD*) &v[36]);
        const BYTE* r5 = &v[40 + 4 * 0xF8];
        Assert::AreEqual<DWORD>(5, *(DWORD*) r5);
        const RolePartInfo* i5 = (const RolePartInfo*) (r5 + 4);
        Assert::AreEqual(2, i5->nPart);
        Assert::IsNull(i5->pMaterial[0]);
        Assert::IsNull(i5->pMaterial[1]);
        Assert::AreEqual(0, strcmp((char*) r5 + 0x78, "glow"));
        Assert::AreEqual(0, strcmp((char*) r5 + 0x98, "stone"));
        v = ReadAll("ini/armormotion.bin");
        Assert::AreEqual<DWORD>(1, *(DWORD*) &v[36]);
        Assert::AreEqual("gdb/snd1.wav", p->GetSound(1));

        p->Init(TRUE);
        GameDataSetDestroy();

        p = GameDataSetQuery();
        p->Init(TRUE);
        Assert::AreEqual("gdb/snd1.wav", p->GetSound(1));
        C3DSimpleObjInfo* s7 = p->Get3DSimpleObjInfo(7);
        Assert::IsNotNull(s7);
        Assert::AreEqual(2, s7->nParts);
        Assert::AreEqual<OBJID>(2, s7->idMesh[1]);
        Assert::IsNotNull(p->Get3DSimpleObjInfo(9));
        CMy3DEffectInfo* fire = p->GetMy3DEffectInfo("Fire");
        Assert::IsNotNull(fire);
        Assert::AreEqual(1, fire->nAmount);
        Assert::AreEqual(100, fire->nDelay);
        Assert::AreEqual(-4, fire->nOffsetX);
        Assert::AreEqual(8, fire->nOffsetZ);
        RolePartInfo* a5 = p->GetRolePartInfo("Armor", 5);
        Assert::IsNotNull(a5);
        Assert::AreEqual(2, a5->nPart);
        Assert::AreEqual<OBJID>(1, a5->idMesh[0]);
        Assert::AreEqual<OBJID>(2, a5->idMesh[1]);
        Assert::AreEqual<OBJID>(7, a5->idMixTex[0]);
        Assert::AreEqual(5, a5->nAsb[1]);
        Assert::AreEqual(2, a5->nAdb[1]);
        Assert::IsNotNull(a5->pMaterial[0]); Assert::IsNotNull(a5->pMaterial[1]);
        RolePartInfo* a1 = p->GetRolePartInfo("Armor", 1);
        Assert::IsNotNull(a1);
        Assert::AreEqual(0, a1->nPart);
        Assert::IsNotNull(p->GetRolePartInfo("Armor", 4));
        Assert::IsNull(p->GetRolePartInfo("Armor", 6));
        Assert::AreEqual("gdb/mo1.c3", p->GetMotionRes("Armor", 100));
        GameDataSetDestroy();
    }

    TEST_METHOD(InitReadsTheBinaryTables)
    {
        DataSetCwd cwd;
        const unsigned __int64 ids[3] = { 7, 0x100000002ui64, 9 };
        const char* const files[3] = { "bin/seven.wav", "bin/wide.wav", "" };
        WriteResBin("ini/sound.bin", ids, files, 3);
        WriteResBin("ini/3dobj.bin", ids, files, 1);
        WriteResBin("ini/3dtexture.bin", ids, files, 0);
        WriteResBin("ini/3deffectobj.bin", ids, files, 1);
        const unsigned __int64 motion[1] = { 300 };
        const char* const motionFile[1] = { "bin/motion300.c3" };
        WriteResBin("ini/armormotion.bin", motion, motionFile, 1);
        {
            FILE* fp = fopen("ini/armor.bin", "wb");
            Assert::IsNotNull(fp);
            char header[32] = "TqDigitalarmor";
            fwrite(header, 32, 1, fp);
            DWORD version = 0x10000;
            fwrite(&version, 4, 1, fp);
            int n = 0;
            fwrite(&n, 4, 1, fp);
            fclose(fp);
        }

        {
            C3DSimpleObjInfo rec[2];
            memset(rec, 0, sizeof(rec));
            rec[0].idType = 500; rec[0].nParts = 1; rec[0].idMesh[0] = 11; rec[0].idTexture[0] = 12;
            rec[1].idType = 400; rec[1].nParts = 4; rec[1].idMesh[3] = 43; rec[1].idTexture[3] = 44;
            FILE* fp = fopen("ini/3DSimpleObj.bin", "wb");
            Assert::IsNotNull(fp);
            char header[32] = "TqDigital3DSimpleObj";
            fwrite(header, 32, 1, fp);
            DWORD version = 0x10000;
            fwrite(&version, 4, 1, fp);
            int n = 2;
            fwrite(&n, 4, 1, fp);
            fwrite(rec, sizeof(C3DSimpleObjInfo), 2, fp);
            fclose(fp);
        }

        {
            static CMy3DEffectInfo rec[2];
            memset(rec, 0, sizeof(rec));
            strcpy(rec[0].szIndex, "BinFire"); rec[0].nAmount = 1; rec[0].idEffect[0] = 21; rec[0].idTexture[0] = 22;
            rec[0].nASB[0] = 5; rec[0].nADB[0] = 6; rec[0].nDelay = 70; rec[0].nOffsetZ = -3;
            strcpy(rec[1].szIndex, "BinIce"); rec[1].nAmount = 8; rec[1].idEffect[7] = 87; rec[1].nLoopTime = 9;
            FILE* fp = fopen("ini/3DEffect.bin", "wb");
            Assert::IsNotNull(fp);
            char header[32] = "TqDigital3DEffect";
            fwrite(header, 32, 1, fp);
            DWORD version = 0x10000;
            fwrite(&version, 4, 1, fp);
            int n = 2;
            fwrite(&n, 4, 1, fp);
            fwrite(rec, sizeof(CMy3DEffectInfo), 2, fp);
            fclose(fp);
        }

        IGameDataSet* p = GameDataSetQuery();
        p->Init(TRUE);
        CMy3DEffectInfo* fire = p->GetMy3DEffectInfo("BinFire");
        CMy3DEffectInfo* ice = p->GetMy3DEffectInfo("BinIce");
        Assert::IsNotNull(fire);
        Assert::IsNotNull(ice);
        Assert::AreEqual(1, fire->nAmount);
        Assert::AreEqual<OBJID>(21, fire->idEffect[0]);
        Assert::AreEqual<OBJID>(22, fire->idTexture[0]);
        Assert::AreEqual(5, fire->nASB[0]);
        Assert::AreEqual(6, fire->nADB[0]);
        Assert::AreEqual(70, fire->nDelay);
        Assert::AreEqual(-3, fire->nOffsetZ);
        Assert::AreEqual(8, ice->nAmount);
        Assert::AreEqual<OBJID>(87, ice->idEffect[7]);
        Assert::AreEqual(9, ice->nLoopTime);
        Assert::IsTrue((BYTE*) ice == (BYTE*) fire + 0x180);
        Assert::IsNull(p->FindMy3DEffectInfo("NoSuchBinEffect"));
        C3DSimpleObjInfo* s500 = p->Get3DSimpleObjInfo(500);
        C3DSimpleObjInfo* s400 = p->Get3DSimpleObjInfo(400);
        Assert::IsNotNull(s500);
        Assert::IsNotNull(s400);
        Assert::AreEqual(1, s500->nParts);
        Assert::AreEqual<OBJID>(11, s500->idMesh[0]);
        Assert::AreEqual<OBJID>(12, s500->idTexture[0]);
        Assert::AreEqual(4, s400->nParts);
        Assert::AreEqual<OBJID>(43, s400->idMesh[3]);
        Assert::AreEqual<OBJID>(44, s400->idTexture[3]);
        Assert::IsTrue((BYTE*) s400 == (BYTE*) s500 + 0x28);
        Assert::AreEqual("bin/seven.wav", p->GetSound(7));
        Assert::AreEqual("", p->GetSound(9));
        Assert::IsNull(p->GetSound(1));
        Assert::IsNull(p->GetSound(2));
        Assert::AreEqual("bin/motion300.c3", p->GetMotionRes("Armor", 300));
        Assert::IsNull(p->GetMotionRes("Armor", 100));
        Assert::IsNull(p->GetRolePartInfo("Armor", 1001));
        GameDataSetDestroy();
    }
};
