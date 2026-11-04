#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "c3_phy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Motion) == 0x28, "");
static_assert(offsetof(C3Motion, dwBoneCount) == 0x00 && offsetof(C3Motion, dwFrames) == 0x04 && offsetof(C3Motion, dwKeyFrames) == 0x08, "");
static_assert(offsetof(C3Motion, lpKeyFrame) == 0x0C && offsetof(C3Motion, matrix) == 0x10, "");
static_assert(offsetof(C3Motion, dwMorphCount) == 0x14 && offsetof(C3Motion, lpMorph) == 0x18 && offsetof(C3Motion, nFrame) == 0x1C, "");
static_assert(offsetof(C3Motion, fWeight) == 0x20, "");
static_assert(offsetof(C3Motion, dwCacheID) == 0x24, "");
static_assert(sizeof(C3KeyFrame) == 8, "");
static_assert(sizeof(C3MotionTracker) == 8, "");

static_assert(sizeof(C3Phy) == 0x1A4, "");
static_assert(__alignof(C3Phy) == 4, "");
static_assert(offsetof(C3Phy, lpName) == 0x00 && offsetof(C3Phy, dwBlendCount) == 0x04 && offsetof(C3Phy, bNormal) == 0x08 && offsetof(C3Phy, bTwoSide) == 0x0C, "");
static_assert(offsetof(C3Phy, dwNVecCount) == 0x10 && offsetof(C3Phy, dwAVecCount) == 0x14 && offsetof(C3Phy, lpVB) == 0x18 && offsetof(C3Phy, vb) == 0x1C, "");
static_assert(offsetof(C3Phy, dwNTriCount) == 0x20 && offsetof(C3Phy, dwATriCount) == 0x24 && offsetof(C3Phy, lpIB) == 0x28 && offsetof(C3Phy, ib) == 0x2C, "");
static_assert(offsetof(C3Phy, lpTexName) == 0x30 && offsetof(C3Phy, nTex) == 0x34 && offsetof(C3Phy, nTex2) == 0x38, "");
static_assert(offsetof(C3Phy, bboxMin) == 0x3C && offsetof(C3Phy, bboxMax) == 0x48 && offsetof(C3Phy, lpMotion) == 0x54 && offsetof(C3Phy, lpBlendMotion) == 0x58, "");
static_assert(offsetof(C3Phy, fA) == 0x5C && offsetof(C3Phy, fB) == 0x68, "");
static_assert(offsetof(C3Phy, Key) == 0x6C && offsetof(C3Phy, bDraw) == 0x84 && offsetof(C3Phy, dwTexRow) == 0x88, "");
static_assert(offsetof(C3Phy, InitMatrix) == 0x8C && offsetof(C3Phy, matrix) == 0xCC, "");
static_assert(offsetof(C3Phy, bMatrix) == 0x10C && offsetof(C3Phy, matrixLocal) == 0x110 && offsetof(C3Phy, matrixWorld) == 0x150, "");
static_assert(offsetof(C3Phy, uvstep) == 0x190 && offsetof(C3Phy, dwStepCount) == 0x198, "");
static_assert(offsetof(C3Phy, nBillboard) == 0x19C && offsetof(C3Phy, lpCache) == 0x1A0, "");
static_assert(sizeof(PhyCacheItem) == 8 && offsetof(PhyCacheItem, dwSize) == 4, "");

static_assert(sizeof(PhyVertex) == 0x34, "");
static_assert(offsetof(PhyVertex, u) == 0x0C && offsetof(PhyVertex, color) == 0x14 && offsetof(PhyVertex, index) == 0x18 && offsetof(PhyVertex, weight) == 0x20 && offsetof(PhyVertex, normal) == 0x28, "");
static_assert(sizeof(PhyOutVertex) == 0x24 && PHY_OUT_VERTEX == 0x152, "");
static_assert(offsetof(PhyOutVertex, nx) == 0x0C && offsetof(PhyOutVertex, color) == 0x18 && offsetof(PhyOutVertex, u) == 0x1C && offsetof(PhyOutVertex, v) == 0x20, "");
static_assert(_BONE_MAX_ == 2, "");
static_assert(sizeof(D3DMATERIAL8) == 0x44 && sizeof(D3DMATERIAL8) == sizeof(D3DMATERIAL9) && offsetof(D3DMATERIAL8, Power) == offsetof(D3DMATERIAL9, Power), "");

static_assert(std::is_same<decltype(&Motion_GetMatrix), void (*)(C3Motion*, DWORD, D3DXMATRIXA16*)>::value, "");
static_assert(std::is_same<decltype(&Motion_GetMatrix_Blend), void (*)(C3MotionTracker, DWORD, D3DXMATRIXA16*)>::value, "");
static_assert(std::is_same<decltype(&Motion_Load), BOOL (*)(C3Motion**, void*)>::value, "");
static_assert(std::is_same<decltype(&Phy_Load), BOOL (*)(C3Phy**, void*, BOOL, BOOL, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Phy_Calculate), BOOL (*)(C3Phy*, BOOL, int, D3DXVECTOR3)>::value, "");
static_assert(std::is_same<decltype(&Phy_SetLight), void (*)(BOOL, float, float, float, DWORD, DWORD, float)>::value, "");
static_assert(std::is_same<decltype(&Phy_SetFluidReflectSpeed), void (*)(float, float)>::value, "");
static_assert(std::is_same<decltype(&Phy_DrawNormal), BOOL (*)(C3Phy*, D3DMATERIAL8, D3DXMATRIXA16*, BOOL, DWORD, DWORD, int, float)>::value, "");
static_assert(std::is_same<decltype(&Phy_Draw), BOOL (*)(PhyOutVertex*, DWORD, C3Texture*, const D3DMATERIAL8*, DWORD, DWORD, BOOL, BOOL)>::value, "");
static_assert(std::is_same<decltype(&Phy_DrawAlpha), BOOL (*)(C3Phy*, BOOL, DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Phy_ChangeTexture), void (*)(C3Phy*, int)>::value && std::is_same<decltype(&Phy_ChangeMixTexture), void (*)(C3Phy*, int)>::value, "");
static_assert(std::is_same<decltype(&Phy_Muliply), void (*)(C3Phy*, int, D3DXMATRIXA16*)>::value, "");
static_assert(std::is_same<decltype(&Phy_SetColor), void (*)(C3Phy*, float, float, float, float)>::value && std::is_same<decltype(&Phy_SetFrame), void (*)(C3Phy*, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Phy_SetBlendFrame), void (*)(C3Phy*, DWORD)>::value && std::is_same<decltype(&Phy_SetStepCount), void (*)(C3Phy*, DWORD)>::value, "");

TEST_CLASS(C3PhyHeader)
{
public:
    TEST_METHOD(ClearImage)
    {
        C3Phy p;
        memset(&p, 0xCD, sizeof(p));
        Phy_Clear(&p);
        const DWORD* w = reinterpret_cast<const DWORD*>(&p);
        const float* f = reinterpret_cast<const float*>(&p);
        for (int i = 0; i < 0x34 / 4; i++) Assert::AreEqual<DWORD>(0, w[i]);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, w[0x34 / 4]);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, w[0x38 / 4]);
        for (int i = 0x3C / 4; i <= 0x44 / 4; i++) Assert::AreEqual(999999.0f, f[i]);
        for (int i = 0x48 / 4; i <= 0x50 / 4; i++) Assert::AreEqual(-999999.0f, f[i]);
        Assert::AreEqual<DWORD>(0, w[0x54 / 4]); Assert::AreEqual<DWORD>(0, w[0x58 / 4]);
        for (int i = 0x5C / 4; i <= 0x68 / 4; i++) Assert::AreEqual<DWORD>(0x3F800000, w[i]);
        for (int i = 0x6C / 4; i <= 0x80 / 4; i++) Assert::AreEqual<DWORD>(0, w[i]);
        Assert::AreEqual<DWORD>(1, w[0x84 / 4]);
        Assert::AreEqual<DWORD>(1, w[0x88 / 4]);
        for (int i = 0; i < 16; i++) Assert::AreEqual<DWORD>(i % 5 == 0 ? 0x3F800000 : 0, w[0x8C / 4 + i]);
        for (int i = 0; i < 16; i++) Assert::AreEqual<DWORD>(i % 5 == 0 ? 0x3F800000 : 0, w[0xCC / 4 + i]);
        for (int i = 0x10C / 4; i <= 0x18C / 4; i++) Assert::AreEqual<DWORD>(0xCDCDCDCD, w[i]);
        Assert::AreEqual<DWORD>(0, w[0x190 / 4]);
        Assert::AreEqual<DWORD>(0, w[0x194 / 4]);
        Assert::AreEqual<DWORD>(0, w[0x198 / 4]);
        Assert::AreEqual<DWORD>(0, w[0x19C / 4]);
        Assert::AreEqual<DWORD>(0, w[0x1A0 / 4]);
    }
};

#include <string>
#include <vector>
#include <cstring>
#include "c3_datafile.h"
#include "seifuku_fixtures.h"
#include "c3_key.h"
#include "c3_main.h"

namespace
{
    struct PhyDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        D3DCAPS9* caps;
        PhyDevice() : d3d(0), dev(0), slot(0), caps(0)
        {
            HMODULE h = GetModuleHandleA("C3_CORE_DLL.dll");
            slot = h ? (LPDIRECT3DDEVICE9*) GetProcAddress(h, "?g_D3DDevice@@3PAUIDirect3DDevice8@@A") : 0;
            caps = h ? (D3DCAPS9*) GetProcAddress(h, "?g_D3DCaps@@3U_D3DCAPS8@@A") : 0;
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
        ~PhyDevice()
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
        void Vertex(bool morph, bool normal, float x, float y, float z, float u, float v, float nx = 0, float ny = 0, float nz = 0)
        {
            F32(x); F32(y); F32(z);
            if (morph) for (int i = 0; i < 9; i++) F32(0.0f);
            F32(u); F32(v); U32(0);
            U32(0); U32(0);
            F32(1.0f); F32(0.0f);
            if (normal) { F32(nx); F32(ny); F32(nz); }
        }
        void Frame(int nFrame, float f, BOOL b, int n) { U32((DWORD) nFrame); F32(f); U32((DWORD) b); U32((DWORD) n); }
        void Write(const std::string& path, bool header)
        {
            std::vector<BYTE> out;
            if (header) { const char* h = "MAXFILE C3 00001"; out.insert(out.end(), h, h + 16); }
            out.insert(out.end(), v.begin(), v.end());
            SeifukuFixtures::WriteBlob(path, out.empty() ? "" : (const void*) &out[0], (DWORD) out.size());
        }
    };

    D3DXMATRIX Translation(float x, float y, float z)
    {
        D3DXMATRIX m;
        D3DXMatrixIdentity(&m);
        m._41 = x; m._42 = y; m._43 = z;
        return m;
    }

    void PhyTail(Blob& ph, bool twoSide, const char* billboard = 0, const char* texture = "none.bmp", bool step = true)
    {
        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        ph.U32(1); ph.U32(1);
        for (WORD i = 0; i < 6; i++) ph.U16(i);
        ph.Str(texture);
        ph.F32(-1); ph.F32(-1); ph.F32(-1); ph.F32(1); ph.F32(1); ph.F32(1);
        ph.Mat(id);
        ph.U32(2);
        ph.U32(0);
        ph.U32(0);
        ph.U32(1); ph.Frame(3, 0, 0, 3);
        if (step) { ph.Raw("STEP", 4); ph.F32(0.25f); ph.F32(0.5f); }
        if (twoSide) ph.Raw("2SID", 4);
        if (billboard) ph.Raw(billboard, 4);
        ph.Raw("END ", 4);
    }

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    std::vector<BYTE> MakeBmp24(int w, int h, DWORD color)
    {
        std::vector<BYTE> v;
        const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++) { v.push_back((BYTE) color); v.push_back((BYTE) (color >> 8)); v.push_back((BYTE) (color >> 16)); }
            for (DWORD p = w * 3; p < pitch; p++) v.push_back(0);
        }
        return v;
    }

    void BuildPhyFixtures(const std::string& dir)
    {
        CreateDirectoryA(dir.c_str(), 0);
        C3DataFileHeader hdr; hdr.id = 0; hdr.number = 0; hdr.offset = sizeof(hdr);
        SeifukuFixtures::WriteBlob(dir + "\\phy.wdf", &hdr, sizeof(hdr));
        std::string sub = dir + "\\phy";
        CreateDirectoryA(sub.c_str(), 0);

        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        Blob mo;
        mo.U32(1); mo.U32(4); mo.Raw("KKEY", 4); mo.U32(2);
        mo.U32(0); mo.Mat(id);
        mo.U32(2); mo.Mat(Translation(2, 4, 6));
        mo.U32(0);
        mo.Write(sub + "\\motion.c3", true);

        Blob mz;
        mz.U32(1); mz.U32(2); mz.Raw("ZKEY", 4); mz.U32(2);
        mz.U16(0); mz.F32(0); mz.F32(0); mz.F32(0); mz.F32(1); mz.F32(0); mz.F32(0); mz.F32(0);
        mz.U16(1); mz.F32(0); mz.F32(0); mz.F32(0.70710678f); mz.F32(0.70710678f); mz.F32(5); mz.F32(0); mz.F32(0);
        mz.U32(0);
        mz.Write(sub + "\\motion_z.c3", true);

        Blob mx;
        mx.U32(1); mx.U32(2); mx.Raw("XKEY", 4); mx.U32(2);
        mx.U16(0); mx.F32(1); mx.F32(0); mx.F32(0); mx.F32(0); mx.F32(1); mx.F32(0); mx.F32(0); mx.F32(0); mx.F32(1); mx.F32(0); mx.F32(0); mx.F32(0);
        mx.U16(1); mx.F32(0); mx.F32(1); mx.F32(0); mx.F32(-1); mx.F32(0); mx.F32(0); mx.F32(0); mx.F32(0); mx.F32(1); mx.F32(5); mx.F32(0); mx.F32(0);
        mx.U32(0);
        mx.Write(sub + "\\motion_x.c3", true);

        Blob mp;
        mp.U32(1); mp.U32(2); mp.Mat(id); mp.Mat(Translation(5, 0, 0)); mp.U32(0);
        mp.Write(sub + "\\motion_raw.c3", true);

        Blob ph;
        ph.Str("seifuku"); ph.U32(0); ph.U32(3); ph.U32(3);
        ph.Vertex(true, false, 0, 0, 0, 0.0f, 0.0f); ph.Vertex(true, false, 1, 0, 0, 0.25f, 0.0f); ph.Vertex(true, false, 0, 1, 0, 0.0f, 0.25f);
        ph.Vertex(true, false, 0, 0, 1, 0.5f, 0.5f); ph.Vertex(true, false, 1, 0, 1, 0.75f, 0.5f); ph.Vertex(true, false, 0, 1, 1, 0.5f, 0.75f);
        PhyTail(ph, false);
        ph.Write(sub + "\\phy.c3", true);

        Blob pn;
        pn.Str("normals"); pn.U32(0); pn.U32(3); pn.U32(3);
        pn.Vertex(true, true, 0, 0, 0, 0.0f, 0.0f, 1, 0, 0); pn.Vertex(true, true, 1, 0, 0, 0.25f, 0.0f, 0, 1, 0); pn.Vertex(true, true, 0, 1, 0, 0.0f, 0.25f, 0, 0, 1);
        pn.Vertex(true, true, 0, 0, 1, 0.5f, 0.5f, 1, 0, 0); pn.Vertex(true, true, 1, 0, 1, 0.75f, 0.5f, 0, 1, 0); pn.Vertex(true, true, 0, 1, 1, 0.5f, 0.75f, 0, 0, 1);
        PhyTail(pn, true);
        pn.Write(sub + "\\phy_mn.c3", true);

        Blob pm;
        pm.Str("merge"); pm.U32(0); pm.U32(3); pm.U32(3);
        pm.Vertex(false, false, 0, 0, 0, 0, 0); pm.Vertex(false, false, 1, 0, 0, 0, 0); pm.Vertex(false, false, 0, 1, 0, 0, 0);
        pm.Vertex(false, false, 0, 0, 0, 0, 0); pm.Vertex(false, false, 0, 0, 1, 0, 0); pm.Vertex(false, false, 1, 0, 0, 0, 0);
        PhyTail(pm, false);
        pm.Write(sub + "\\phy_merge.c3", true);

        Blob pb;
        pb.Str("bilb"); pb.U32(0); pb.U32(3); pb.U32(3);
        pb.Vertex(true, true, 0, 0, 0, 0.0f, 0.0f, 1, 0, 0); pb.Vertex(true, true, 1, 0, 0, 0.25f, 0.0f, 0, 1, 0); pb.Vertex(true, true, 0, 1, 0, 0.0f, 0.25f, 0, 0, 1);
        pb.Vertex(true, true, 0, 0, 1, 0.5f, 0.5f, 1, 0, 0); pb.Vertex(true, true, 1, 0, 1, 0.75f, 0.5f, 0, 1, 0); pb.Vertex(true, true, 0, 1, 1, 0.5f, 0.75f, 0, 0, 1);
        PhyTail(pb, true, "BILB");
        pb.Write(sub + "\\phy_bilb.c3", true);
        Blob pb2;
        pb2.Str("bib2"); pb2.U32(0); pb2.U32(3); pb2.U32(3);
        pb2.Vertex(true, true, 0, 0, 0, 0.0f, 0.0f, 1, 0, 0); pb2.Vertex(true, true, 1, 0, 0, 0.25f, 0.0f, 0, 1, 0); pb2.Vertex(true, true, 0, 1, 0, 0.0f, 0.25f, 0, 0, 1);
        pb2.Vertex(true, true, 0, 0, 1, 0.5f, 0.5f, 1, 0, 0); pb2.Vertex(true, true, 1, 0, 1, 0.75f, 0.5f, 0, 1, 0); pb2.Vertex(true, true, 0, 1, 1, 0.5f, 0.75f, 0, 0, 1);
        PhyTail(pb2, false, "BIB2", "none.bmp", false);
        pb2.Write(sub + "\\phy_bib2.c3", true);

        std::vector<BYTE> bmp = MakeBmp24(4, 4, 0x0080FF);
        SeifukuFixtures::WriteBlob(sub + "\\fluid.bmp", &bmp[0], (DWORD) bmp.size());
        Blob pt;
        pt.Str("fluid"); pt.U32(0); pt.U32(3); pt.U32(3);
        pt.Vertex(true, true, 0, 0, 0, 0.0f, 0.0f, 1, 0, 0); pt.Vertex(true, true, 1, 0, 0, 0.25f, 0.0f, 0, 1, 0); pt.Vertex(true, true, 0, 1, 0, 0.0f, 0.25f, 0, 0, 1);
        pt.Vertex(true, true, 0, 0, 1, 0.5f, 0.5f, 1, 0, 0); pt.Vertex(true, true, 1, 0, 1, 0.75f, 0.5f, 0, 1, 0); pt.Vertex(true, true, 0, 1, 1, 0.5f, 0.75f, 0, 0, 1);
        PhyTail(pt, false, 0, "phy/fluid.bmp", false);
        pt.Write(sub + "\\phy_tex.c3", true);

        Blob empty;
        empty.Write(sub + "\\phy_save.c3", true);
        empty.Write(sub + "\\motion_save.c3", true);
    }

    D3DMATERIAL8 White()
    {
        D3DMATERIAL8 m;
        ZeroMemory(&m, sizeof(m));
        m.Diffuse.a = m.Diffuse.r = m.Diffuse.g = m.Diffuse.b = 1.0f;
        m.Ambient = m.Diffuse;
        return m;
    }
}

TEST_CLASS(C3PhyFunctions)
{
public:
    TEST_METHOD(FrameArithmeticAndMatrices)
    {
        C3Motion m;
        Motion_Clear(&m);
        Assert::AreEqual<DWORD>(0, m.dwBoneCount); Assert::IsNull(m.lpKeyFrame); Assert::IsNull(m.lpMorph);
        D3DXMATRIXA16 bone[2];
        D3DXMatrixIdentity(&bone[0]); D3DXMatrixIdentity(&bone[1]);
        m.dwBoneCount = 2; m.dwFrames = 5; m.matrix = bone;
        C3Motion m2;
        Motion_Clear(&m2);
        m2.dwFrames = 4;
        C3Phy p;
        Phy_Clear(&p);

        Phy_SetFrame(&p, 12);
        Phy_SetBlendFrame(&p, 12);
        p.lpMotion = &m;

        Phy_NextFrame(&p, 3); Assert::AreEqual(3, m.nFrame);
        Phy_NextFrame(&p, 3); Assert::AreEqual(1, m.nFrame);
        Phy_SetFrame(&p, 12); Assert::AreEqual(2, m.nFrame);
        Phy_SetBlendFrame(&p, 7); Assert::AreEqual(0, m2.nFrame);
        p.lpBlendMotion = &m2;
        Phy_SetBlendFrame(&p, 7); Assert::AreEqual(3, m2.nFrame);
        Assert::AreEqual(2, m.nFrame);

        Phy_SetStepCount(&p, 5); Assert::AreEqual<DWORD>(5, p.dwStepCount);
        Phy_SetStepCount(0, 5);

        D3DXMATRIXA16 t; D3DXMatrixIdentity(&t); t._41 = 7;
        Phy_Muliply(&p, 1, &t);
        Assert::AreEqual(0.0f, bone[0]._41); Assert::AreEqual(7.0f, bone[1]._41);
        Phy_Muliply(&p, -1, &t);
        Assert::AreEqual(7.0f, bone[0]._41); Assert::AreEqual(14.0f, bone[1]._41);
        p.bMatrix = TRUE; p.matrix._41 = 3.0f; p.matrixLocal = t; p.matrixWorld = t;
        Phy_ClearMatrix(&p);
        Assert::AreEqual(0.0f, bone[0]._41); Assert::AreEqual(0.0f, bone[1]._41); Assert::AreEqual(1.0f, bone[1]._44);
        Assert::IsTrue(p.bMatrix == FALSE);
        Assert::AreEqual(0.0f, p.matrix._41); Assert::AreEqual(0.0f, p.matrixLocal._41); Assert::AreEqual(0.0f, p.matrixWorld._41);
        Assert::AreEqual(1.0f, p.matrix._11);

        Phy_SetColor(&p, 0.5f, 0.25f, 0.125f, 1.0f);
        Assert::AreEqual(0.5f, p.fA); Assert::AreEqual(0.25f, p.fR); Assert::AreEqual(0.125f, p.fG); Assert::AreEqual(1.0f, p.fB);
        Phy_ChangeTexture(&p, 9);
        Assert::AreEqual(9, p.nTex); Assert::AreEqual(-1, p.nTex2);
        Phy_ChangeMixTexture(&p, 4);
        Assert::AreEqual(9, p.nTex); Assert::AreEqual(4, p.nTex2);
    }

    TEST_METHOD(LoadCalculateDrawSave)
    {
        PhyDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        BuildPhyFixtures(dir);
        SeifukuFixtures::CwdGuard cwd(dir);
        DataFile_Open("phy.wdf");
        int nMotion0 = g_nMotionCount;

        void* f = SeifukuFixtures::OpenC3("phy/motion.c3");
        Assert::IsNotNull(f);
        C3Motion* m = 0;
        Assert::IsTrue(Motion_Load(&m, f));
        Common_CloseRes(f);
        Assert::AreEqual<DWORD>(1, m->dwBoneCount); Assert::AreEqual<DWORD>(4, m->dwFrames);
        Assert::AreEqual<DWORD>(2, m->dwKeyFrames); Assert::AreEqual<DWORD>(0, m->dwMorphCount);
        Assert::AreEqual<DWORD>(0, m->lpKeyFrame[0].pos); Assert::AreEqual<DWORD>(2, m->lpKeyFrame[1].pos);
        Assert::AreEqual(6.0f, m->lpKeyFrame[1].matrix[0]._43);
        Assert::AreEqual(1.0f, m->matrix[0]._11);
        Assert::AreEqual<DWORD>(0, m->dwCacheID);
        D3DXMATRIXA16 mm;
        m->nFrame = 1; Motion_GetMatrix(m, 0, &mm);
        Assert::AreEqual(1.0f, mm._41); Assert::AreEqual(2.0f, mm._42); Assert::AreEqual(3.0f, mm._43); Assert::AreEqual(1.0f, mm._11);
        m->nFrame = 3; Motion_GetMatrix(m, 0, &mm);
        Assert::AreEqual(2.0f, mm._41);
        m->nFrame = 0; Motion_GetMatrix(m, 0, &mm);
        Assert::AreEqual(0.0f, mm._41); Assert::AreEqual(1.0f, mm._22);
        Assert::AreEqual(1, g_nMotionCount - nMotion0);
        mm._41 = 9.0f;
        Motion_GetMatrix(0, 0, &mm);
        Assert::AreEqual(9.0f, mm._41);

        f = SeifukuFixtures::OpenC3("phy/motion_raw.c3");
        Assert::IsNotNull(f);
        C3Motion* mp = 0;
        Assert::IsTrue(Motion_Load(&mp, f));
        Common_CloseRes(f);
        Assert::AreEqual<DWORD>(2, mp->dwKeyFrames); Assert::AreEqual<DWORD>(1, mp->lpKeyFrame[1].pos);
        Assert::AreEqual(5.0f, mp->lpKeyFrame[1].matrix[0]._41); Assert::AreEqual(0.0f, mp->lpKeyFrame[0].matrix[0]._41);
        Assert::AreEqual(2, g_nMotionCount - nMotion0);

        C3MotionTracker tracker;
        tracker.lpMotion = 0; tracker.lpBlendMotion = mp;
        mm._41 = 9.0f;
        Motion_GetMatrix_Blend(tracker, 0, &mm);
        Assert::AreEqual(9.0f, mm._41);
        tracker.lpMotion = m; tracker.lpBlendMotion = 0;
        m->nFrame = 1;
        Motion_GetMatrix_Blend(tracker, 0, &mm);
        Assert::AreEqual(1.0f, mm._41); Assert::AreEqual(3.0f, mm._43);
        tracker.lpBlendMotion = mp;
        mp->nFrame = 1; mp->fWeight = 0.25f;
        Motion_GetMatrix_Blend(tracker, 0, &mm);
        Assert::AreEqual(0.75f * 1.0f + 0.25f * 5.0f, mm._41, 1e-5f); Assert::AreEqual(1.5f, mm._42, 1e-5f); Assert::AreEqual(2.25f, mm._43, 1e-5f);
        Assert::AreEqual(1.0f, mm._11, 1e-5f); Assert::AreEqual(1.0f, mm._44, 1e-5f);
        m->nFrame = 0;

        f = SeifukuFixtures::OpenC3("phy/motion_z.c3");
        Assert::IsNotNull(f);
        Assert::IsTrue(Motion_Load(&mp, f));
        Common_CloseRes(f);
        Assert::AreEqual(2, g_nMotionCount - nMotion0);
        Assert::AreEqual<DWORD>(2, mp->dwKeyFrames); Assert::AreEqual<DWORD>(0, mp->lpKeyFrame[0].pos); Assert::AreEqual<DWORD>(1, mp->lpKeyFrame[1].pos);
        Assert::AreEqual(1.0f, mp->lpKeyFrame[0].matrix[0]._11); Assert::AreEqual(0.0f, mp->lpKeyFrame[0].matrix[0]._41);
        Assert::AreEqual(0.0f, mp->lpKeyFrame[1].matrix[0]._11, 1e-5f); Assert::AreEqual(1.0f, mp->lpKeyFrame[1].matrix[0]._12, 1e-5f);
        Assert::AreEqual(-1.0f, mp->lpKeyFrame[1].matrix[0]._21, 1e-5f); Assert::AreEqual(1.0f, mp->lpKeyFrame[1].matrix[0]._33, 1e-5f);
        Assert::AreEqual(5.0f, mp->lpKeyFrame[1].matrix[0]._41); Assert::AreEqual(0.0f, mp->lpKeyFrame[1].matrix[0]._34); Assert::AreEqual(1.0f, mp->lpKeyFrame[1].matrix[0]._44);
        Assert::IsNull(mp->lpMorph);

        f = SeifukuFixtures::OpenC3("phy/motion_x.c3");
        Assert::IsNotNull(f);
        Assert::IsTrue(Motion_Load(&mp, f));
        Common_CloseRes(f);
        Assert::AreEqual(2, g_nMotionCount - nMotion0);
        Assert::AreEqual<DWORD>(1, mp->lpKeyFrame[1].pos);
        Assert::AreEqual(1.0f, mp->lpKeyFrame[0].matrix[0]._22); Assert::AreEqual(0.0f, mp->lpKeyFrame[0].matrix[0]._14);
        Assert::AreEqual(1.0f, mp->lpKeyFrame[1].matrix[0]._12); Assert::AreEqual(-1.0f, mp->lpKeyFrame[1].matrix[0]._21);
        Assert::AreEqual(5.0f, mp->lpKeyFrame[1].matrix[0]._41); Assert::AreEqual(0.0f, mp->lpKeyFrame[1].matrix[0]._24); Assert::AreEqual(1.0f, mp->lpKeyFrame[1].matrix[0]._44);
        Motion_Unload(&mp);
        Assert::IsNull(mp);
        Assert::AreEqual(1, g_nMotionCount - nMotion0);

        f = SeifukuFixtures::OpenC3("phy/phy.c3");
        Assert::IsNotNull(f);
        C3Phy* p = 0;
        Assert::IsTrue(Phy_Load(&p, f, FALSE, FALSE, FALSE, TRUE));
        char end[4] = { 0 };
        Common_ReadRes(f, end, 4);
        Assert::IsTrue(memcmp(end, "END ", 4) == 0);
        Common_CloseRes(f);
        Assert::AreEqual("seifuku", p->lpName); Assert::AreEqual("none.bmp", p->lpTexName);
        Assert::AreEqual<DWORD>(3, p->dwNVecCount); Assert::AreEqual<DWORD>(3, p->dwAVecCount);
        Assert::AreEqual<DWORD>(1, p->dwNTriCount); Assert::AreEqual<DWORD>(1, p->dwATriCount);
        Assert::AreEqual(-1, p->nTex); Assert::AreEqual(-1, p->nTex2); Assert::IsNotNull(p->vb); Assert::IsNotNull(p->ib);
        Assert::IsNotNull(p->lpIB); Assert::AreEqual<WORD>(5, p->lpIB[5]);
        Assert::IsTrue(p->bNormal == FALSE); Assert::IsTrue(p->bTwoSide == FALSE);
        Assert::AreEqual(1.0f, p->bboxMax.y); Assert::AreEqual(-1.0f, p->bboxMin.z);
        Assert::AreEqual<DWORD>(2, p->dwTexRow); Assert::AreEqual<DWORD>(1, p->Key.dwChangeTexs);
        Assert::AreEqual(3, p->Key.lpChangeTexs[0].nFrame); Assert::AreEqual(3, p->Key.lpChangeTexs[0].nParam[0]);
        Assert::AreEqual(0.25f, p->uvstep.x); Assert::AreEqual(0.5f, p->uvstep.y); Assert::AreEqual<DWORD>(0, p->dwStepCount);
        Assert::AreEqual(1.0f, p->fA); Assert::IsTrue(p->bDraw == TRUE);
        Assert::AreEqual(0.75f, p->lpVB[4].u); Assert::AreEqual(1.0f, p->lpVB[4].pos.z); Assert::AreEqual(1.0f, p->lpVB[4].weight[0]);
        Assert::AreEqual(0.0f, p->lpVB[4].normal.x);
        Assert::IsNull(p->Key.lpAlphas); Assert::IsNull(p->Key.lpDraws);
        Assert::IsTrue(p->bMatrix == FALSE); Assert::AreEqual(1.0f, p->matrixLocal._11); Assert::AreEqual(1.0f, p->matrixWorld._44);
        Assert::AreEqual(0, p->nBillboard); Assert::IsNotNull(p->lpCache); Assert::IsTrue(p->lpCache->empty());
        PhyOutVertex* ov = 0;
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(1.0f, ov[4].x); Assert::AreEqual(1.0f, ov[4].z); Assert::AreEqual(0.75f, ov[4].u); Assert::AreEqual<DWORD>(0xFFFFFFFF, ov[4].color);
        p->vb->Unlock();

        f = SeifukuFixtures::OpenC3("phy/phy_mn.c3");
        Assert::IsNotNull(f);
        C3Phy* pn = 0;
        Assert::IsTrue(Phy_Load(&pn, f, TRUE, FALSE, FALSE, TRUE));
        Common_ReadRes(f, end, 4);
        Assert::IsTrue(memcmp(end, "END ", 4) == 0);
        Common_CloseRes(f);
        Assert::IsTrue(pn->bNormal == TRUE); Assert::IsTrue(pn->bTwoSide == TRUE);
        Assert::AreEqual(0.75f, pn->lpVB[4].u); Assert::AreEqual(1.0f, pn->lpVB[4].normal.y); Assert::AreEqual(1.0f, pn->lpVB[5].normal.z);
        Assert::AreEqual(1.0f, pn->lpVB[3].normal.x); Assert::AreEqual(1.0f, pn->lpVB[5].pos.y);
        Assert::AreEqual(0.5f, pn->uvstep.y);
        Assert::AreEqual(0, pn->nBillboard);

        f = SeifukuFixtures::OpenC3("phy/phy_bilb.c3");
        Assert::IsNotNull(f);
        C3Phy* pb = 0;
        Assert::IsTrue(Phy_Load(&pb, f, TRUE, FALSE, FALSE, TRUE));
        Common_ReadRes(f, end, 4);
        Assert::IsTrue(memcmp(end, "END ", 4) == 0);
        Common_CloseRes(f);
        Assert::IsTrue(pb->bTwoSide == TRUE); Assert::AreEqual(1, pb->nBillboard); Assert::AreEqual(0.25f, pb->uvstep.x);
        Phy_Unload(&pb);
        Assert::IsNull(pb);
        f = SeifukuFixtures::OpenC3("phy/phy_bib2.c3");
        Assert::IsNotNull(f);
        Assert::IsTrue(Phy_Load(&pb, f, TRUE, FALSE, FALSE, TRUE));
        Common_ReadRes(f, end, 4);
        Assert::IsTrue(memcmp(end, "END ", 4) == 0);
        Common_CloseRes(f);
        Assert::IsTrue(pb->bTwoSide == FALSE); Assert::AreEqual(2, pb->nBillboard); Assert::AreEqual(0.0f, pb->uvstep.x);
        Phy_Unload(&pb);

        f = SeifukuFixtures::OpenC3("phy/phy_merge.c3");
        Assert::IsNotNull(f);
        C3Phy* pc = 0;
        Assert::IsTrue(Phy_Load(&pc, f, FALSE, TRUE, FALSE, FALSE));
        Common_CloseRes(f);
        Assert::IsTrue(pc->bNormal == TRUE);
        const float h = 0.70710678f;
        Assert::AreEqual(0.0f, pc->lpVB[2].normal.x, 1e-5f); Assert::AreEqual(0.0f, pc->lpVB[2].normal.y, 1e-5f); Assert::AreEqual(-1.0f, pc->lpVB[2].normal.z, 1e-5f);
        Assert::AreEqual(0.0f, pc->lpVB[4].normal.x, 1e-5f); Assert::AreEqual(-1.0f, pc->lpVB[4].normal.y, 1e-5f); Assert::AreEqual(0.0f, pc->lpVB[4].normal.z, 1e-5f);
        const int shared[4] = { 0, 1, 3, 5 };
        for (int i = 0; i < 4; i++)
        {
            Assert::AreEqual(0.0f, pc->lpVB[shared[i]].normal.x, 1e-5f);
            Assert::AreEqual(-h, pc->lpVB[shared[i]].normal.y, 1e-5f);
            Assert::AreEqual(-h, pc->lpVB[shared[i]].normal.z, 1e-5f);
        }
        Assert::IsTrue(SUCCEEDED(pc->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(-h, ov[3].ny, 1e-5f); Assert::AreEqual(-1.0f, ov[2].nz, 1e-5f);
        pc->vb->Unlock();

        D3DXVECTOR3 nolight(0.0f, 0.0f, 0.0f);
        p->lpMotion = m;
        m->nFrame = 1;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual<DWORD>(0xFFFFFFFF, ov[0].color); Assert::AreEqual<DWORD>(0xFFFFFFFF, ov[5].color);
        Assert::AreEqual(1.0f + 1.0f, ov[1].x); Assert::AreEqual(2.0f, ov[1].y); Assert::AreEqual(3.0f, ov[1].z);
        Assert::AreEqual(0.25f, ov[1].u); Assert::AreEqual(0.0f, ov[1].v);
        Assert::AreEqual(0.0f + 1.0f, ov[5].x); Assert::AreEqual(1.0f + 2.0f, ov[5].y); Assert::AreEqual(1.0f + 3.0f, ov[5].z);
        p->vb->Unlock();
        Phy_SetStepCount(p, 2);
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(0.25f + 2 * 0.25f, ov[1].u); Assert::AreEqual(0.0f + 2 * 0.5f, ov[1].v);
        Assert::AreEqual(0.5f + 2 * 0.25f, ov[5].u); Assert::AreEqual(0.75f + 2 * 0.5f, ov[5].v);
        p->vb->Unlock();
        m->nFrame = 3;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(1.0f + 2.0f, ov[1].x);
        Assert::AreEqual(0.25f + 0.5f, ov[1].u); Assert::AreEqual(0.0f + 0.5f, ov[1].v);
        p->vb->Unlock();
        Phy_SetStepCount(p, 0);
        D3DXMATRIXA16 shift; D3DXMatrixIdentity(&shift); shift._41 = 7.0f;
        Phy_Muliply(p, 0, &shift);
        m->nFrame = 1;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(1.0f + 1.0f + 7.0f, ov[1].x);
        p->vb->Unlock();
        p->bMatrix = TRUE; p->matrixLocal._41 = 2.0f; p->matrixWorld._42 = 3.0f;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(1.0f + 1.0f, ov[1].x);
        p->vb->Unlock();
        Phy_ClearMatrix(p);
        Assert::IsTrue(p->bMatrix == FALSE); Assert::AreEqual(0.0f, p->matrixLocal._41); Assert::AreEqual(0.0f, p->matrixWorld._42); Assert::AreEqual(0.0f, m->matrix[0]._41);
        p->bDraw = FALSE;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        p->bDraw = TRUE;

        pc->lpMotion = m;
        m->nFrame = 0;
        D3DXVECTOR3 light(0.0f, 0.0f, 3.0f);
        Assert::IsTrue(Phy_Calculate(pc, TRUE, 0x15, light));
        Assert::IsTrue(SUCCEEDED(pc->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(-1.0f, ov[2].nz, 1e-5f); Assert::AreEqual(-h, ov[0].ny, 1e-5f);
        Assert::AreEqual(0.0f, ov[2].u); Assert::AreEqual(0.0f, ov[2].v);
        pc->vb->Unlock();
        pc->InitMatrix._22 = 2.0f;
        D3DXVECTOR3 back(0.0f, 0.0f, -1.0f);
        Assert::IsTrue(Phy_Calculate(pc, TRUE, 0x16, back));
        Assert::IsTrue(SUCCEEDED(pc->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(-2.0f, ov[4].ny, 1e-5f); Assert::AreEqual(-2.0f * h, ov[0].ny, 1e-5f); Assert::AreEqual(-h, ov[0].nz, 1e-5f);
        Assert::AreEqual(2.0f, ov[2].y, 1e-5f);
        ov[2].nz = 9.0f;
        pc->vb->Unlock();
        pc->InitMatrix._22 = 1.0f;
        Assert::IsTrue(Phy_Calculate(pc, FALSE, 0, light));
        Assert::IsTrue(SUCCEEDED(pc->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(9.0f, ov[2].nz);
        pc->vb->Unlock();

        m->dwCacheID = 7; p->bMatrix = TRUE; p->lpBlendMotion = 0;
        m->nFrame = 1;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::AreEqual<size_t>(1, p->lpCache->size());
        std::vector<PhyCacheItem>& slots = (*p->lpCache)[7];
        Assert::AreEqual<size_t>(4, slots.size());
        Assert::IsNull(slots[0].lpVertex); Assert::IsNotNull(slots[1].lpVertex); Assert::IsNull(slots[2].lpVertex);
        Assert::AreEqual<DWORD>(6 * sizeof(PhyOutVertex), slots[1].dwSize);
        Assert::AreEqual(1.0f + 1.0f, slots[1].lpVertex[1].x); Assert::AreEqual<DWORD>(0xFFFFFFFF, slots[1].lpVertex[1].color);
        p->lpVB[1].pos.x += 100.0f;
        Phy_SetColor(p, 1, 0.5f, 1, 1);
        Phy_SetStepCount(p, 1);
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(1.0f + 1.0f, ov[1].x);
        Assert::AreEqual<DWORD>(0xFF7FFFFF, ov[1].color); Assert::AreEqual<DWORD>(0xFF7FFFFF, ov[5].color);
        Assert::AreEqual(0.25f + 0.25f, ov[1].u); Assert::AreEqual(0.0f + 0.5f, ov[1].v);
        p->vb->Unlock();
        Assert::AreEqual<DWORD>(0xFFFFFFFF, slots[1].lpVertex[1].color);
        m->nFrame = 2;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(100.0f + 1.0f + 2.0f, ov[1].x); Assert::AreEqual<DWORD>(0xFF7FFFFF, ov[1].color);
        p->vb->Unlock();
        Assert::IsNotNull(slots[2].lpVertex); Assert::AreEqual(100.0f + 1.0f + 2.0f, slots[2].lpVertex[1].x);
        m->nFrame = 1;
        m->dwCacheID = 0;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(p->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(100.0f + 1.0f + 1.0f, ov[1].x);
        p->vb->Unlock();
        m->dwCacheID = 9; p->bMatrix = FALSE;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::AreEqual<size_t>(1, p->lpCache->size());
        p->bMatrix = TRUE; p->lpBlendMotion = m;
        Assert::IsTrue(Phy_Calculate(p, FALSE, 0, nolight));
        Assert::AreEqual<size_t>(1, p->lpCache->size());
        p->lpBlendMotion = 0; p->bMatrix = FALSE; m->dwCacheID = 0;
        p->lpVB[1].pos.x -= 100.0f;
        Phy_SetColor(p, 1, 1, 1, 1);
        Phy_SetStepCount(p, 0);
        pc->bMatrix = TRUE; m->dwCacheID = 3; m->nFrame = 0;
        Assert::IsTrue(Phy_Calculate(pc, FALSE, 0, nolight));
        Assert::AreEqual<size_t>(2, pc->lpCache->size() + p->lpCache->size());
        Phy_SetColor(pc, 0.5f, 1, 1, 1);
        Assert::IsTrue(Phy_Calculate(pc, FALSE, 0, nolight));
        Assert::IsTrue(SUCCEEDED(pc->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual<DWORD>(0x7FFFFFFF, ov[0].color); Assert::AreEqual<DWORD>(0x7FFFFFFF, ov[5].color);
        Assert::AreEqual<DWORD>(0xFFFFFFFF, (*pc->lpCache)[3][0].lpVertex[0].color);
        pc->vb->Unlock();
        Phy_SetColor(pc, 1, 1, 1, 1);
        pc->bMatrix = FALSE; m->dwCacheID = 0;

        dd.dev->SetRenderState(D3DRS_DITHERENABLE, TRUE);
        dd.dev->SetRenderState(D3DRS_ALPHAREF, 0);
        if (dd.caps) dd.caps->MaxAnisotropy = 0;
        Phy_Prepare();
        DWORD st = 1;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DITHERENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHATESTENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHAREF, &st))); Assert::AreEqual<DWORD>(8, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHAFUNC, &st))); Assert::AreEqual<DWORD>(D3DCMP_GREATEREQUAL, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_CW, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MINFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_LINEAR, st);
        if (dd.caps)
        {
            dd.caps->MaxAnisotropy = 4;
            Phy_Prepare();
            Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MINFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_ANISOTROPIC, st);
            Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MAXANISOTROPY, &st))); Assert::AreEqual<DWORD>(2, st);
            Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MIPFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_LINEAR, st);
            dd.caps->MaxAnisotropy = 0;
        }

        Assert::IsTrue(Begin3D());
        DWORD nTri0 = g_dwTriTotal;
        dd.dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        dd.dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);
        D3DMATERIAL9 got, marker;
        ZeroMemory(&marker, sizeof(marker)); marker.Emissive.g = 0.75f;
        dd.dev->SetMaterial(&marker);
        Assert::IsTrue(Phy_DrawNormal(p, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::AreEqual<DWORD>(2, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ZWRITEENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_CW, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_COLOR2, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetMaterial(&got)));
        Assert::AreEqual(0.75f, got.Emissive.g);
        D3DXMATRIX tr;
        D3DXMatrixIdentity(&g_ViewMatrix);
        D3DXMatrixIdentity(&g_IDMatrix);
        Phy_SetColor(p, 0.5f, 0.25f, 1, 1);
        Assert::IsTrue(Phy_DrawNormal(p, White(), &shift, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.125f));
        Assert::AreEqual<DWORD>(6, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ZWRITEENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_SRCALPHA, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DESTBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_INVSRCALPHA, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ZFUNC, &st))); Assert::AreEqual<DWORD>(D3DCMP_LESSEQUAL, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetMaterial(&got)));
        Assert::AreEqual(0.125f, got.Diffuse.a); Assert::AreEqual(0.0f, got.Diffuse.r); Assert::AreEqual(0.0f, got.Ambient.a);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTransform(D3DTS_VIEW, (D3DMATRIX*) &tr)));
        Assert::IsTrue(memcmp(&tr, &g_ViewMatrix, sizeof(tr)) == 0);
        {
            D3DXMATRIX want, flat;
            D3DXMatrixMultiply(&want, &p->matrixLocal, &shift);
            D3DXMatrixMultiply(&want, &want, &p->matrixWorld);
            D3DXMatrixMultiply(&want, &want, &g_ViewMatrix);
            D3DXMatrixIdentity(&flat); flat._33 = 0.0f; flat._43 = g_ViewMatrix._43 * 1.5f;
            D3DXMatrixMultiply(&want, &want, &flat);
            Assert::IsTrue(SUCCEEDED(dd.dev->GetTransform(D3DTS_WORLD, (D3DMATRIX*) &tr)));
            Assert::AreEqual(want._41, tr._41, 1e-5f); Assert::AreEqual(want._33, tr._33, 1e-5f); Assert::AreEqual(want._43, tr._43, 1e-5f);
            Assert::AreEqual(7.0f, tr._41, 1e-5f); Assert::AreEqual(0.0f, tr._33, 1e-5f); Assert::AreEqual(0.0f, tr._43, 1e-5f);
        }
        Assert::IsTrue(Phy_DrawNormal(p, White(), NULL, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::AreEqual<DWORD>(8, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetMaterial(&got)));
        Assert::AreEqual(0.125f, got.Diffuse.a); Assert::AreEqual(0.0f, got.Diffuse.r);
        Phy_SetColor(p, 0, 1, 1, 1);
        dd.dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        Assert::IsTrue(Phy_DrawNormal(p, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::AreEqual<DWORD>(8, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ZERO, st);
        Phy_SetColor(p, 1, 1, 1, 1);
        p->bDraw = FALSE;
        Assert::IsTrue(Phy_DrawNormal(p, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::AreEqual<DWORD>(8, g_dwTriTotal - nTri0);
        p->bDraw = TRUE;
        Assert::IsTrue(Phy_DrawAlpha(p, TRUE));
        Assert::AreEqual<DWORD>(8, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SRCBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ZERO, st);

        pn->lpMotion = m;
        Assert::IsTrue(Phy_Calculate(pn, TRUE, 0, light));
        nTri0 = g_dwTriTotal;
        Assert::IsTrue(Phy_DrawNormal(pn, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, 0, 0.0f));
        Assert::AreEqual<DWORD>(2, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_NONE, st);
        Assert::IsTrue(Phy_DrawNormal(pn, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, 0x14, 0.0f));
        Assert::AreEqual<DWORD>(6, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_CCW, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetMaterial(&got)));
        Assert::AreEqual(1.0f, got.Diffuse.a); Assert::AreEqual(0.0f, got.Diffuse.g);
        Assert::IsTrue(SUCCEEDED(pn->vb->Lock(0, 0, (void**) &ov, 0)));
        Assert::AreEqual(1.0f, ov[1].x, 1e-5f); Assert::AreEqual(2.0f, ov[1].y, 1e-5f);
        Assert::AreEqual(1.0f + 2.0f, ov[5].z, 1e-5f);
        pn->vb->Unlock();

        Phy_SetLight(TRUE, 0.0f, 0.0f, 2.0f, 0x00FF8000, 0x00204080, 0.0f);
        dd.dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);
        dd.dev->SetRenderState(D3DRS_AMBIENT, 0x12345678);
        Assert::IsTrue(Phy_DrawNormal(p, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_COLOR2, st);
        Phy_SetColor(p, 0.5f, 0.25f, 1, 0.5f);
        Assert::IsTrue(Phy_DrawNormal(p, White(), NULL, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_MATERIAL, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SPECULARMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_MATERIAL, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_MATERIAL, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SPECULARENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_AMBIENT, &st))); Assert::AreEqual<DWORD>(0x12345678, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetMaterial(&got)));
        Assert::AreEqual(0.5f, got.Diffuse.a); Assert::AreEqual(1.0f, got.Diffuse.r); Assert::AreEqual(1.0f, got.Diffuse.b); Assert::AreEqual(1.0f, got.Ambient.r);
        D3DLIGHT9 l;
        if (SUCCEEDED(dd.dev->GetLight(0, &l)))
        {
            Assert::IsTrue(l.Type == D3DLIGHT_DIRECTIONAL);
            Assert::AreEqual(0.0f, l.Direction.x); Assert::AreEqual(1.0f, l.Direction.z, 1e-5f);
            Assert::AreEqual(1.0f, l.Diffuse.r, 1e-5f); Assert::AreEqual(128.0f / 255.0f, l.Diffuse.g, 1e-5f); Assert::AreEqual(0.0f, l.Diffuse.b);
            Assert::AreEqual(128.0f / 255.0f, l.Specular.g, 1e-5f); Assert::AreEqual(1.0f, l.Specular.a);
            Assert::AreEqual(32.0f / 255.0f * 0.25f, l.Ambient.r, 1e-5f); Assert::AreEqual(64.0f / 255.0f, l.Ambient.g, 1e-5f);
            Assert::AreEqual(128.0f / 255.0f * 0.5f, l.Ambient.b, 1e-5f); Assert::AreEqual(1.0f, l.Ambient.a);
            Assert::AreEqual(1000.0f, l.Range); Assert::AreEqual(1.0f, l.Attenuation0);
        }
        Phy_SetColor(p, 1, 1, 1, 1);
        D3DMATERIAL8 shiny = White();
        shiny.Power = 4.0f;
        Assert::IsTrue(Phy_DrawNormal(p, shiny, NULL, TRUE, D3DBLEND_ONE, D3DBLEND_ONE, 0, 0.0f));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SPECULARENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        if (SUCCEEDED(dd.dev->GetLight(0, &l))) Assert::AreEqual(32.0f / 255.0f, l.Ambient.r, 1e-5f);

        f = SeifukuFixtures::OpenC3("phy/phy_tex.c3");
        Assert::IsNotNull(f);
        C3Phy* pt = 0;
        Assert::IsTrue(Phy_Load(&pt, f, TRUE, FALSE, TRUE, TRUE));
        Common_CloseRes(f);
        Assert::IsTrue(pt->nTex >= 0); Assert::AreEqual("phy/fluid.bmp", pt->lpTexName);
        Phy_ChangeMixTexture(pt, pt->nTex);
        pt->lpMotion = m;
        Assert::IsTrue(Phy_Calculate(pt, FALSE, 0, nolight));
        Phy_SetFluidReflectSpeed(0.0f, 0.0f);
        nTri0 = g_dwTriTotal;
        Assert::IsTrue(Phy_DrawNormal(pt, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, 0x17, 0.0f));
        Assert::AreEqual<DWORD>(2, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTransform(D3DTS_TEXTURE1, (D3DMATRIX*) &tr)));
        Assert::AreEqual(0.0f, tr._41); Assert::AreEqual(0.0f, tr._42); Assert::AreEqual(1.0f, tr._11); Assert::AreEqual(1.0f, tr._44); Assert::AreEqual(0.0f, tr._43);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, &st))); Assert::AreEqual<DWORD>(D3DTTFF_DISABLE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(1, D3DTSS_TEXCOORDINDEX, &st))); Assert::AreEqual<DWORD>(D3DTSS_TCI_CAMERASPACENORMAL, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(1, D3DTSS_COLOROP, &st))); Assert::AreEqual<DWORD>(D3DTOP_MODULATE2X, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(0, D3DTSS_COLORARG2, &st))); Assert::AreEqual<DWORD>(D3DTA_DIFFUSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(2, D3DTSS_COLOROP, &st))); Assert::AreEqual<DWORD>(D3DTOP_DISABLE, st);
        Phy_SetFluidReflectSpeed(2.0f, -1.0f);
        Assert::IsTrue(Phy_DrawNormal(pt, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, 0x17, 0.0f));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTransform(D3DTS_TEXTURE1, (D3DMATRIX*) &tr)));
        Assert::IsTrue(tr._41 > 0.0f); Assert::IsTrue(tr._42 < 0.0f); Assert::AreEqual(-tr._41, tr._42 * 2.0f);
        Phy_SetFluidReflectSpeed(1.0f, 0.0f);
        Phy_ChangeMixTexture(pt, -1);
        Assert::IsTrue(Phy_DrawNormal(pt, White(), NULL, FALSE, D3DBLEND_ONE, D3DBLEND_ZERO, 0x12, 0.0f));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(0, D3DTSS_COLORARG2, &st))); Assert::AreEqual<DWORD>(D3DTA_DIFFUSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(1, D3DTSS_COLOROP, &st))); Assert::AreEqual<DWORD>(D3DTOP_DISABLE, st);
        pt->lpMotion = 0;
        Phy_Unload(&pt);
        Assert::IsNull(pt);

        PhyOutVertex tri3[3];
        ZeroMemory(tri3, sizeof(tri3));
        tri3[1].x = 1.0f; tri3[2].y = 1.0f;
        Assert::IsFalse(Phy_Draw(0, 1, 0, 0, D3DBLEND_ONE, D3DBLEND_ZERO, TRUE, FALSE) != FALSE);
        nTri0 = g_dwTriTotal;
        Assert::IsTrue(Phy_Draw(tri3, 1, 0, 0, D3DBLEND_ONE, D3DBLEND_ZERO, TRUE, TRUE));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_NONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DESTBLEND, &st))); Assert::AreEqual<DWORD>(D3DBLEND_ZERO, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SPECULARENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        D3DMATERIAL8 white = White();
        Assert::IsTrue(Phy_Draw(tri3, 1, 0, &white, D3DBLEND_ONE, D3DBLEND_ZERO, FALSE, TRUE));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_CW, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SPECULARENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::AreEqual<DWORD>(0, g_dwTriTotal - nTri0);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_AMBIENTMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_MATERIAL, st);
        Phy_SetLight(FALSE, 1.0f, 1.0f, 1.0f, 0x00FFFFFF, 0x00666666, 0.0f);
        dd.dev->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR2);
        Assert::IsTrue(Phy_Draw(tri3, 1, 0, &white, D3DBLEND_ONE, D3DBLEND_ZERO, FALSE, TRUE));
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &st))); Assert::AreEqual<DWORD>(D3DMCS_COLOR2, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        End3D();

        Assert::IsTrue(Phy_Save((char*) "phy\\phy_save.c3", pn, FALSE));
        f = SeifukuFixtures::OpenC3("phy/phy_save.c3");
        Assert::IsNotNull(f);
        Common_SeekRes(f, 8, SEEK_CUR);
        C3Phy* p2 = 0;
        Assert::IsTrue(Phy_Load(&p2, f, TRUE, FALSE, FALSE, FALSE));
        Common_CloseRes(f);
        Assert::AreEqual("normals", p2->lpName); Assert::AreEqual<DWORD>(3, p2->dwAVecCount); Assert::AreEqual<DWORD>(1, p2->dwATriCount);
        Assert::AreEqual(0.75f, p2->lpVB[4].u); Assert::AreEqual(1.0f, p2->lpVB[4].normal.y); Assert::AreEqual(0.5f, p2->uvstep.y);
        Assert::AreEqual(3, p2->Key.lpChangeTexs[0].nParam[0]);
        Assert::IsTrue(p2->bTwoSide == FALSE);
        WORD* tri = 0;
        Assert::IsTrue(SUCCEEDED(p2->ib->Lock(0, 0, (void**) &tri, 0)));
        Assert::AreEqual<WORD>(5, tri[5]);
        p2->ib->Unlock();
        Phy_Unload(&p2);
        Assert::IsNull(p2);

        Assert::IsTrue(Motion_Save((char*) "phy\\motion_save.c3", m, FALSE));
        f = SeifukuFixtures::OpenC3("phy/motion_save.c3");
        Assert::IsNotNull(f);
        Common_SeekRes(f, 8, SEEK_CUR);
        C3Motion* m2 = 0;
        Assert::IsTrue(Motion_Load(&m2, f));
        Common_CloseRes(f);
        Assert::AreEqual<DWORD>(2, m2->dwKeyFrames); Assert::AreEqual<DWORD>(2, m2->lpKeyFrame[1].pos); Assert::AreEqual(4.0f, m2->lpKeyFrame[1].matrix[0]._42);
        Motion_Unload(&m2);

        p->lpMotion = 0; pn->lpMotion = 0; pc->lpMotion = 0;
        Phy_Unload(&p);
        Assert::IsNull(p);
        Phy_Unload(&pn);
        Phy_Unload(&pc);
        Motion_Unload(&m);
        Assert::IsNull(m);
        Assert::AreEqual(0, g_nMotionCount - nMotion0);
    }
};
