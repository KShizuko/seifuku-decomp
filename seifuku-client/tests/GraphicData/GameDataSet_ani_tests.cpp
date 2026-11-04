#include "CppUnitTest.h"
#include <windows.h>
#include <cstring>
#include <string>
#include <vector>
#include "GameDataSet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
    struct AniSetDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        AniSetDevice() : d3d(0), dev(0), slot(0)
        {
            HMODULE h = GetModuleHandleA("C3_CORE_DLL.dll");
            slot = h ? (LPDIRECT3DDEVICE9*) GetProcAddress(h, "?g_D3DDevice@@3PAUIDirect3DDevice8@@A") : 0;
            d3d = Direct3DCreate9(D3D_SDK_VERSION);
            if (!d3d || !slot) return;
            D3DPRESENT_PARAMETERS pp;
            ZeroMemory(&pp, sizeof(pp));
            pp.Windowed = TRUE;
            pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
            pp.hDeviceWindow = GetDesktopWindow();
            if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, GetDesktopWindow(),
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &pp, &dev)))
                dev = 0;
            if (dev) *slot = dev;
        }
        ~AniSetDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    void AniSetWrite(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        ::WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    std::string AniSetDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    void BuildAniSetFixtures(const std::string& dir)
    {
        DWORD hdr[3] = { 0, 0, 12 };
        AniSetWrite(dir + "\\gdani.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gdani").c_str(), 0);
        CreateDirectoryA((dir + "\\ani").c_str(), 0);
        std::vector<BYTE> v;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (DWORD i = 0; i < pitch * h; i++) v.push_back(0x60);
        AniSetWrite(dir + "\\gdani\\a.bmp", &v[0], (DWORD) v.size());
        AniSetWrite(dir + "\\gdani\\b.bmp", &v[0], (DWORD) v.size());
        const char ani[] =
            "[Two]\nFrameAmount=2\nFrame0=gdani/a.bmp\nFrame1=gdani/b.bmp\n\n"
            "[Three]\nFrameAmount=3\nFrame0=gdani/a.bmp\nFrame1=gdani/b.bmp\nFrame2=gdani/b.bmp\n";
        AniSetWrite(dir + "\\gdani\\test.ani", ani, sizeof(ani) - 1);
        const char ico[] =
            "[ICON0]\nFrameAmount=1\nFrame0=gdani/a.bmp\n\n"
            "[ICON1]\nFrameAmount=1\nFrame0=gdani/b.bmp\n\n"
            "[ICON2]\nFrameAmount=2\nFrame0=gdani/a.bmp\nFrame1=gdani/b.bmp\n";
        AniSetWrite(dir + "\\ani\\EmotionIco.ani", ico, sizeof(ico) - 1);
    }

    struct MeshBlob
    {
        std::vector<BYTE> v;
        void Raw(const void* p, size_t n) { const BYTE* b = (const BYTE*) p; v.insert(v.end(), b, b + n); }
        void U32(DWORD d) { Raw(&d, 4); }
        void U16(WORD w) { Raw(&w, 2); }
        void F32(float f) { Raw(&f, 4); }
        void Str(const char* s) { U32((DWORD) strlen(s)); Raw(s, strlen(s)); }
        void Mat(const D3DXMATRIX& m) { Raw(&m, 64); }
        void Chunk(const char* id, const MeshBlob& body) { Raw(id, 4); U32((DWORD) body.v.size()); Raw(&body.v[0], body.v.size()); }
        void Vertex(float x, float y, float z, float u, float tv)
        {
            F32(x); F32(y); F32(z);
            for (int i = 0; i < 9; i++) F32(0.0f);
            F32(u); F32(tv); U32(0);
            U32(0); U32(0);
            F32(1.0f); F32(0.0f);
        }
    };

    void BuildMeshSetFixtures(const std::string& dir)
    {
        DWORD hdr[3] = { 0, 0, 12 };
        AniSetWrite(dir + "\\gdmesh.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gdmesh").c_str(), 0);
        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        MeshBlob ph;
        ph.Str("seifuku"); ph.U32(0); ph.U32(3); ph.U32(3);
        ph.Vertex(0, 0, 0, 0.0f, 0.0f); ph.Vertex(1, 0, 0, 0.25f, 0.0f); ph.Vertex(0, 1, 0, 0.0f, 0.25f);
        ph.Vertex(0, 0, 1, 0.5f, 0.5f); ph.Vertex(1, 0, 1, 0.75f, 0.5f); ph.Vertex(0, 1, 1, 0.5f, 0.75f);
        ph.U32(1); ph.U32(1);
        for (WORD i = 0; i < 6; i++) ph.U16(i);
        ph.Str("none.bmp");
        ph.F32(-1); ph.F32(-1); ph.F32(-1); ph.F32(1); ph.F32(1); ph.F32(1);
        ph.Mat(id);
        ph.U32(1);
        ph.U32(0); ph.U32(0); ph.U32(0);
        ph.Raw("STEP", 4); ph.F32(0.0f); ph.F32(0.0f);
        MeshBlob mo;
        mo.U32(1); mo.U32(4); mo.Raw("KKEY", 4); mo.U32(2);
        mo.U32(0); mo.Mat(id);
        mo.U32(2); mo.Mat(id);
        mo.U32(0);
        MeshBlob file;
        file.Raw("MAXFILE C3 00001", 16);
        file.Chunk("PHY ", ph);
        file.Chunk("MOTI", mo);
        AniSetWrite(dir + "\\gdmesh\\obj.c3", &file.v[0], (DWORD) file.v.size());
        std::vector<BYTE> v;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (DWORD i = 0; i < pitch * h; i++) v.push_back(0x80);
        AniSetWrite(dir + "\\gdmesh\\tex.bmp", &v[0], (DWORD) v.size());
    }

    struct AniSetCwd
    {
        char old[MAX_PATH];
        AniSetCwd(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~AniSetCwd() { SetCurrentDirectoryA(old); }
    };

    void AniSetDestroy(AniSetDevice& dd)
    {
        dd.dev->AddRef();
        GameDataSetDestroy();
        *dd.slot = dd.dev;
        CMyBitmap::PrepareSprite();
    }
}

TEST_CLASS(GraphicDataGameDataSetAni)
{
public:
    TEST_METHOD(GetDataAniImmediateQueuedAndTheIcons)
    {
        AniSetDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = AniSetDir();
        BuildAniSetFixtures(dir);
        AniSetCwd cwd(dir);
        CMyBitmap::PackFileOpen("gdani.wdf");

        IGameDataSet* p = GameDataSetQuery();
        p->Init(FALSE);

        Assert::IsNull(p->GetDataAni(0, "Two", EXIGENCE_IMMEDIATE, 30000));
        Assert::IsNull(p->GetDataAni("gdani/test.ani", 0, EXIGENCE_IMMEDIATE, 30000));
        Assert::IsNull(p->GetDataAni("", "Two", EXIGENCE_IMMEDIATE, 30000));
        Assert::IsNull(p->GetDataAni("gdani/test.ani", "", EXIGENCE_IMMEDIATE, 30000));

        IAni* two = p->GetDataAni("gdani/test.ani", "Two", EXIGENCE_IMMEDIATE, 30000);
        Assert::IsNotNull(two);
        Assert::AreEqual(2, two->GetFrameAmount());
        Assert::AreEqual<int>(ANISTATUS_LOADED, two->GetStatus());
        Assert::AreEqual<DWORD>(30000, two->GetLife());
        Assert::IsTrue(p->GetDataAni("gdani/test.ani", "Two", EXIGENCE_NORMAL, 1) == two);
        Assert::AreEqual<DWORD>(30000, two->GetLife());
        typedef IAni* (__fastcall *FnAni)(void*, void*, const char*, const char*, int, DWORD);
        Assert::IsTrue(((FnAni) (*(void***) p)[4])(p, 0, "gdani/test.ani", "Two", EXIGENCE_IMMEDIATE, 30000) == two);
        Assert::IsNull(p->GetDataAni("gdani/test.ani", "NoSuchTitle", EXIGENCE_IMMEDIATE, 30000));

        Assert::IsNull(p->GetDataAni("gdani/test.ani", "Three", EXIGENCE_NORMAL, 30000));
        IAni* three = 0;
        for (int i = 0; i < 250 && !three; i++) { Sleep(20); three = p->GetDataAni("gdani/test.ani", "Three", EXIGENCE_NORMAL, 30000); }
        Assert::IsNotNull(three);
        Assert::AreEqual(3, three->GetFrameAmount());
        Assert::IsTrue(three != two);

        IDataIcon* icon = (IDataIcon*) p->QueryInterface(0);
        IAni* i2 = icon->GetIcon(2);
        Assert::IsNotNull(i2);
        Assert::AreEqual(2, i2->GetFrameAmount());
        Assert::IsTrue(icon->GetIcon(5) == i2);
        IAni* i0 = icon->GetIcon(3);
        Assert::IsNotNull(i0);
        Assert::AreEqual(1, i0->GetFrameAmount());

        p->Process();
        Assert::IsTrue(p->GetDataAni("gdani/test.ani", "Two", EXIGENCE_IMMEDIATE, 30000) == two);

        AniSetDestroy(dd);
    }

    TEST_METHOD(MeshesTexturesAndMotions)
    {
        AniSetDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = AniSetDir();
        BuildMeshSetFixtures(dir);
        AniSetCwd cwd(dir);
        CMyBitmap::PackFileOpen("gdmesh.wdf");

        IGameDataSet* p = GameDataSetQuery();
        p->Init(FALSE);

        I3DObj* mesh = p->Get3DObj(1);
        Assert::IsNotNull(mesh);
        Assert::IsTrue(p->Get3DObj(1) == mesh);
        Assert::AreEqual<DWORD>(1, mesh->GetPhyAmount());
        Assert::IsTrue(mesh->IsPhyHidden(0) != FALSE);
        C3DObjInfo* info = p->Get3DObjInfo(1);
        Assert::IsNotNull(info);
        Assert::AreEqual<DWORD>(1, info->id);
        Assert::IsTrue(info->p3DObj == mesh);
        Assert::IsNotNull(info->pDefaultMotion);
        Assert::IsNull(p->Get3DObjInfo(2));
        typedef I3DObj* (__fastcall *FnObj)(void*, void*, OBJID, int, DWORD);
        Assert::IsTrue(((FnObj) (*(void***) p)[5])(p, 0, 1, 0, 0) == mesh);
        typedef C3DObjInfo* (__fastcall *FnInfo)(void*, void*, OBJID);
        Assert::IsTrue(((FnInfo) (*(void***) p)[9])(p, 0, 1) == info);

        I3DTexture* tex = p->Get3DTexture(1);
        Assert::IsNotNull(tex);
        Assert::IsTrue(p->Get3DTexture(1) == tex);
        Assert::IsNull(p->Get3DTexture(0));
        typedef I3DTexture* (__fastcall *FnTex)(void*, void*, OBJID, int, DWORD);
        Assert::IsTrue(((FnTex) (*(void***) p)[6])(p, 0, 1, 0, 0) == tex);

        Assert::IsNull(p->Get3DEffect(0));

        I3DMotion* mo = p->Get3DMotion("Armor", 100);
        Assert::IsNotNull(mo);
        Assert::IsTrue(p->Get3DMotion("Armor", 100) == mo);
        Assert::AreEqual<DWORD>(4, mo->GetFrameAmount());
        Assert::IsNull(p->Get3DMotion("Armor", 0));
        Assert::IsNull(p->Get3DMotion(0, 100));
        Assert::IsNull(p->Get3DMotion("Helmet", 100));
        Assert::IsNull(p->Get3DMotion("Armor", 555));
        typedef I3DMotion* (__fastcall *FnMotion)(void*, void*, const char*, DWORD, DWORD, int, DWORD);
        Assert::IsTrue(((FnMotion) (*(void***) p)[8])(p, 0, "Armor", 100, 0, 0, 0) == mo);

        p->Process();
        Assert::IsTrue(p->Get3DObjInfo(1) == info);
        Assert::IsTrue(p->Get3DTexture(1) == tex);
        Assert::IsTrue(p->Get3DMotion("Armor", 100) == mo);
        Sleep(1500);
        p->Process();
        Assert::IsTrue(p->Get3DObjInfo(1) == info);
        Assert::IsTrue(info->p3DObj == mesh);
        Assert::IsTrue(p->Get3DTexture(1) == tex);
        Assert::IsTrue(p->Get3DMotion("Armor", 100) == mo);
        Assert::IsTrue(p->Reload(FALSE, 0x02 | 0x08) != FALSE);
        Assert::IsNull(p->Get3DObjInfo(1));
        Assert::IsTrue(p->Get3DTexture(1) == tex);
        p->SetLoadAtOnce(0);
        Sleep(1);
        p->Process();
        Assert::IsNull(p->Get3DObj(1));
        info = p->Get3DObjInfo(1);
        Assert::IsNotNull(info);
        Assert::IsNull(info->p3DObj);
        for (int i = 0; i < 250 && !info->p3DObj; i++) { p->Process(); Sleep(20); }
        Assert::IsNotNull(info->p3DObj);
        Assert::IsNotNull(info->pDefaultMotion);
        Assert::IsTrue(p->Get3DObj(1) == info->p3DObj);
        mo = p->Get3DMotion("Armor", 100);
        Assert::IsNull(mo);
        for (int i = 0; i < 250 && !mo; i++) { p->Process(); Sleep(20); mo = p->Get3DMotion("Armor", 100); }
        Assert::IsNotNull(mo);
        Assert::AreEqual<DWORD>(4, mo->GetFrameAmount());
        Assert::IsTrue(p->Get3DObjInfo(1) == info);

        AniSetDestroy(dd);
        CMyBitmap::PackFileOpen("gdmesh.wdf");
        p = GameDataSetQuery();
        p->Init(FALSE);
        Assert::IsNull(p->Get3DObjInfo(1));
        mesh = p->Get3DObj(1);
        Assert::IsNotNull(mesh);
        Assert::AreEqual<DWORD>(1, mesh->GetPhyAmount());
        AniSetDestroy(dd);
    }
};
