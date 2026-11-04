#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <type_traits>
#include "ND_Bitmap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(I3DTexture) == 4 && sizeof(I3DMotion) == 4 && sizeof(I3DObj) == 4, "");
static_assert(std::is_same<decltype(&TextureCreate), I3DTexture* (*)(const char*)>::value && std::is_same<decltype(&TextureCreateEx), I3DTexture* (*)(C3Texture*)>::value, "");
static_assert(std::is_same<decltype(&MotionCreate), I3DMotion* (*)(char*, BOOL)>::value && std::is_same<decltype(&MeshCreate), I3DObj* (*)(char*)>::value, "");
static_assert(std::is_same<decltype(&MotionMatrixGet), D3DXMATRIXA16* (*)(C3Motion*, int)>::value, "");

namespace
{
    struct MeshDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        MeshDevice() : d3d(0), dev(0), slot(0)
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
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
                dev = 0;
            if (dev) *slot = dev;
        }
        ~MeshDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    struct Blob
    {
        std::vector<BYTE> v;
        void Raw(const void* p, size_t n) { const BYTE* b = (const BYTE*) p; v.insert(v.end(), b, b + n); }
        void U32(DWORD d) { Raw(&d, 4); }
        void U16(WORD w) { Raw(&w, 2); }
        void F32(float f) { Raw(&f, 4); }
        void Str(const char* s) { U32((DWORD) strlen(s)); Raw(s, strlen(s)); }
        void Mat(const D3DXMATRIX& m) { Raw(&m, 64); }
        void Chunk(const char* id, const Blob& body) { Raw(id, 4); U32((DWORD) body.v.size()); Raw(&body.v[0], body.v.size()); }
        void Vertex(float x, float y, float z, float u, float tv)
        {
            F32(x); F32(y); F32(z);
            for (int i = 0; i < 9; i++) F32(0.0f);
            F32(u); F32(tv); U32(0);
            U32(0); U32(0);
            F32(1.0f); F32(0.0f);
        }
    };

    void WriteFile(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD w = 0;
        ::WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    std::string FixtureDir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    void BuildFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        DWORD hdr[3] = { 0, 0, 12 };
        WriteFile(dir + "\\gmesh.wdf", hdr, sizeof(hdr));
        CreateDirectoryA((dir + "\\gmesh").c_str(), 0);

        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        D3DXMATRIX tr; D3DXMatrixIdentity(&tr); tr._41 = 2; tr._42 = 4; tr._43 = 6;
        Blob ph;
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
        Blob mo;
        mo.U32(1); mo.U32(4); mo.Raw("KKEY", 4); mo.U32(2);
        mo.U32(0); mo.Mat(id);
        mo.U32(2); mo.Mat(tr);
        mo.U32(0);

        Blob file;
        file.Raw("MAXFILE C3 00001", 16);
        file.Chunk("PHY ", ph);
        file.Chunk("MOTI", mo);
        WriteFile(dir + "\\gmesh\\obj.c3", &file.v[0], (DWORD) file.v.size());
        WriteFile(dir + "\\gmesh\\empty.c3", "MAXFILE C3 00001", 16);

        Blob bmp;
        const int w = 8, h = 4; const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        bmp.Raw("BM", 2); bmp.U32(size); bmp.U32(0); bmp.U32(54);
        bmp.U32(40); bmp.U32(w); bmp.U32(h); bmp.U16(1); bmp.U16(24); bmp.U32(0); bmp.U32(pitch * h); bmp.U32(0); bmp.U32(0); bmp.U32(0); bmp.U32(0);
        for (DWORD i = 0; i < pitch * h; i++) bmp.v.push_back(0x80);
        WriteFile(dir + "\\gmesh\\tex.bmp", &bmp.v[0], (DWORD) bmp.v.size());
    }

    struct CwdGuard
    {
        char old[MAX_PATH];
        CwdGuard(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~CwdGuard() { SetCurrentDirectoryA(old); }
    };

    void** Vtbl(void* p) { return *(void***) p; }

    DWORD MpqHash(const char* s, bool bPrepared)
    {
        static DWORD table[0x500];
        static bool bTable = false;
        if (!bPrepared)
            memset(table, 0, sizeof(table));
        else if (!bTable)
        {
            DWORD seed = 0x100001;
            for (DWORD i1 = 0; i1 < 0x100; i1++)
                for (DWORD i = 0, i2 = i1; i < 5; i++, i2 += 0x100)
                {
                    seed = (seed * 125 + 3) % 0x2AAAAB; DWORD t1 = (seed & 0xFFFF) << 16;
                    seed = (seed * 125 + 3) % 0x2AAAAB; DWORD t2 = seed & 0xFFFF;
                    table[i2] = t1 | t2;
                }
            bTable = true;
        }
        DWORD seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
        for (const BYTE* p = (const BYTE*) s; *p; p++)
        {
            DWORD ch = toupper(*p);
            seed1 = table[ch] ^ (seed1 + seed2);
            seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
        }
        return seed1;
    }
}

TEST_CLASS(GraphicTextureMotionMesh)
{
public:
    TEST_METHOD(CreateUseAndRelease)
    {
        MeshDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = FixtureDir();
        BuildFixtures(dir);
        CwdGuard cwd(dir);
        CMyBitmap::PackFileOpen("gmesh.wdf");

        Assert::IsNull(MotionCreate((char*) "gmesh/missing.c3", FALSE));
        Assert::IsNull(MotionCreate((char*) "gmesh/empty.c3", FALSE));
        I3DMotion* mo = MotionCreate((char*) "gmesh/obj.c3", FALSE);
        Assert::IsNotNull(mo);
        Assert::AreEqual<DWORD>(1, mo->GetMotionAmount());
        Assert::AreEqual<DWORD>(4, mo->GetFrameAmount());
        C3Motion* c3mo = mo->GetMotion(0);
        Assert::IsNotNull(c3mo);
        Assert::AreEqual<DWORD>(1, c3mo->dwBoneCount);
        Assert::AreEqual<DWORD>(0, c3mo->dwCacheID);
        I3DMotion* moc = MotionCreate((char*) "gmesh/obj.c3", TRUE);
        Assert::IsNotNull(moc);
        Assert::AreEqual<DWORD>(MpqHash("gmesh/obj.c30", false), moc->GetMotion(0)->dwCacheID);
        Assert::IsTrue(moc->GetMotion(0)->dwCacheID != 0);
        Assert::AreEqual<DWORD>(0, moc->Release());
        Assert::AreEqual<DWORD>(2, mo->GetKeyFrameAmount(0));
        Assert::AreEqual(6.0f, mo->GetKeyFrameMatrix(0, 1)->_43);
        Assert::IsTrue(mo->GetMatrix(0, 0) == c3mo->matrix);
        Assert::IsTrue(mo->GetMatrix(5, 7) == c3mo->matrix);
        Assert::IsTrue(MotionMatrixGet(c3mo, 0) == c3mo->matrix);
        typedef DWORD (__fastcall *FnDword)(void*, void*);
        typedef C3Motion* (__fastcall *FnMotion)(void*, void*, DWORD);
        Assert::AreEqual<DWORD>(4, ((FnDword) Vtbl(mo)[3])(mo, 0));
        Assert::AreEqual<DWORD>(1, ((FnDword) Vtbl(mo)[4])(mo, 0));
        Assert::IsTrue(c3mo == ((FnMotion) Vtbl(mo)[5])(mo, 0, 0));

        Assert::IsNull(TextureCreate("gmesh/missing.bmp"));
        Assert::IsNull(TextureCreate(0));
        Assert::IsNull(TextureCreateEx(0));
        I3DTexture* tex = TextureCreate("gmesh/tex.bmp");
        Assert::IsNotNull(tex);
        Assert::IsNotNull(tex->GetTexture());
        Assert::IsTrue(tex->GetTextureID() >= 0);
        typedef int (__fastcall *FnInt)(void*, void*);
        Assert::AreEqual(tex->GetTextureID(), ((FnInt) Vtbl(tex)[3])(tex, 0));
        typedef BOOL (__fastcall *FnLock)(void*, void*, RECT);
        typedef void (__fastcall *FnPixel)(void*, void*, int, int, DWORD);
        typedef void (__fastcall *FnVoid)(void*, void*);
        RECT lock = { 0, 0, 2, 2 };
        if (((FnLock) Vtbl(tex)[5])(tex, 0, lock))
        {
            Assert::IsFalse(tex->LockRect(lock) != FALSE);
            ((FnPixel) Vtbl(tex)[6])(tex, 0, 1, 1, 0xFFFF0000);
            tex->SetPixel(2, 1, 0xFF00FF00);
            ((FnVoid) Vtbl(tex)[7])(tex, 0);
            Assert::IsTrue(tex->LockRect(lock) != FALSE);
            tex->Unlock();
        }
        else
            Logger::WriteMessage("the texture is not a 32-bit format: the lock slots not exercised");
        tex->Unlock();

        Assert::IsNull(MeshCreate((char*) "gmesh/missing.c3"));
        Assert::IsNull(MeshCreate((char*) "gmesh/empty.c3"));
        I3DObj* obj = MeshCreate((char*) "gmesh/obj.c3");
        Assert::IsNotNull(obj);
        Assert::IsTrue(obj->IsValid());
        Assert::AreEqual<DWORD>(1, obj->GetPhyAmount());
        Assert::AreEqual("seifuku", obj->GetPhyName(0));
        Assert::AreEqual(0, obj->GetIndexByName((char*) "SEIFUKU"));
        Assert::AreEqual(-1, obj->GetIndexByName((char*) "other"));
        Assert::AreEqual(-1, obj->GetIndexByName(0));
        D3DXVECTOR3 vMin(0, 0, 0), vMax(0, 0, 0);
        obj->GetBoundingBox(0, &vMin, &vMax);
        Assert::AreEqual(-1.0f, vMin.x); Assert::AreEqual(1.0f, vMax.z);
        Assert::IsNull(obj->GetMotion(0));
        Assert::AreEqual<DWORD>(1, ((FnDword) Vtbl(obj)[12])(obj, 0));

        obj->SetMotion(mo);
        Assert::IsTrue(obj->GetMotion(0) == c3mo);
        Assert::IsTrue(obj->GetBoneMatrix(0, 0) == c3mo->matrix);
        obj->SetBlendMotion(mo, 0.25f);
        Assert::IsTrue(obj->GetBlendMotion(0) == c3mo);
        Assert::AreEqual(0.25f, c3mo->fWeight); Assert::AreEqual(0, c3mo->nFrame);
        obj->SetBlendMotion(0, 0.0f);
        Assert::IsNull(obj->GetBlendMotion(0));

        obj->SetFrame(6);
        Assert::AreEqual(2, c3mo->nFrame);
        obj->NextFrame(1);
        Assert::AreEqual(3, c3mo->nFrame);
        obj->SetBlendFrame(1);
        obj->SetFrame(0);

        obj->Move(3.0f, 0.0f, 0.0f);
        Assert::AreEqual(3.0f, c3mo->matrix[0]._41);
        D3DXMATRIXA16 m; D3DXMatrixIdentity(&m); m._42 = 5.0f;
        obj->Muliply(&m);
        Assert::AreEqual(5.0f, c3mo->matrix[0]._42);
        obj->ClearMatrix();
        Assert::AreEqual(0.0f, c3mo->matrix[0]._41); Assert::AreEqual(0.0f, c3mo->matrix[0]._42);
        D3DXMATRIXA16 local; D3DXMatrixIdentity(&local); local._41 = 7.0f;
        D3DXMATRIXA16 world; D3DXMatrixIdentity(&world); world._42 = 8.0f;
        obj->SetPhyMatrix(&local, &world);
        Assert::IsNotNull(obj->GetPhyMatrix(0));

        Assert::IsTrue(CMyBitmap::Begin3D() != FALSE);
        DWORD nTri0 = CMyBitmap::GetShowTriTotal();
        obj->Draw(0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        Assert::AreEqual<DWORD>(2, CMyBitmap::GetShowTriTotal() - nTri0);
        Assert::IsTrue(obj->IsPhyHidden(0) == FALSE);
        obj->SetPhyHidden(0, TRUE);
        Assert::IsTrue(obj->IsPhyHidden(0) != FALSE);
        obj->Draw(0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        Assert::AreEqual<DWORD>(2, CMyBitmap::GetShowTriTotal() - nTri0);
        obj->SetPhyHidden(0, FALSE);
        obj->Draw(2, 100.0f, 100.0f, -500.0f, 0.5f, 1.0f, 1.0f, 1.0f, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        Assert::AreEqual<DWORD>(6, CMyBitmap::GetShowTriTotal() - nTri0);
        obj->ChangeTexture(tex);
        obj->ChangeMixTexture(tex, (char*) "nosuchpart");
        obj->SetDrawType(0);
        obj->SetDrawType(2);
        D3DMATERIAL8 mat;
        ZeroMemory(&mat, sizeof(mat));
        mat.Diffuse.a = mat.Diffuse.r = 1.0f;
        obj->SetMaterial(&mat);
        obj->SetARGB(1.0f, 1.0f, 1.0f, 1.0f);
        obj->Draw(0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        Assert::AreEqual<DWORD>(8, CMyBitmap::GetShowTriTotal() - nTri0);
        obj->ChangeTexture(0);
        obj->ChangeMixTexture(0);
        Assert::IsTrue(CMyBitmap::End3D() != FALSE);

        I3DTexture* wrap = TextureCreateEx(tex->GetTexture());
        Assert::IsNotNull(wrap);
        Assert::IsTrue(wrap->GetTexture() == tex->GetTexture());
        Assert::AreEqual<DWORD>(0, wrap->Release());

        Assert::AreEqual<DWORD>(0, obj->Release());
        Assert::AreEqual<DWORD>(0, mo->Release());
        CMyBitmap::PrepareSprite();
    }
};
