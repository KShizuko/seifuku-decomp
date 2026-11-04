#include "CppUnitTest.h"
#include <string>
#include <vector>
#include <wincodec.h>
#include "d3dx8compat.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
    struct NullRefDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        NullRefDevice() : d3d(0), dev(0)
        {
            d3d = Direct3DCreate9(D3D_SDK_VERSION);
            if (!d3d) return;
            D3DPRESENT_PARAMETERS pp;
            ZeroMemory(&pp, sizeof(pp));
            pp.Windowed = TRUE;
            pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
            pp.BackBufferFormat = D3DFMT_UNKNOWN;
            pp.hDeviceWindow = GetDesktopWindow();
            if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, GetDesktopWindow(),
                                         D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dev)))
                dev = 0;
        }
        ~NullRefDevice()
        {
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    void Put32(std::vector<BYTE>& v, DWORD d) { v.push_back((BYTE) d); v.push_back((BYTE) (d >> 8)); v.push_back((BYTE) (d >> 16)); v.push_back((BYTE) (d >> 24)); }
    void Put16(std::vector<BYTE>& v, WORD w) { v.push_back((BYTE) w); v.push_back((BYTE) (w >> 8)); }

    std::vector<BYTE> MakeBmp24(int w, int h, const DWORD* colors)
    {
        std::vector<BYTE> v;
        const DWORD pitch = (w * 3 + 3) & ~3, size = 54 + pitch * h;
        v.push_back('B'); v.push_back('M'); Put32(v, size); Put32(v, 0); Put32(v, 54);
        Put32(v, 40); Put32(v, w); Put32(v, h); Put16(v, 1); Put16(v, 24); Put32(v, 0); Put32(v, pitch * h); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        for (int y = h - 1; y >= 0; y--)
        {
            for (int x = 0; x < w; x++) { DWORD c = colors[y * w + x]; v.push_back((BYTE) c); v.push_back((BYTE) (c >> 8)); v.push_back((BYTE) (c >> 16)); }
            for (DWORD p = w * 3; p < pitch; p++) v.push_back(0);
        }
        return v;
    }

    std::vector<BYTE> MakeTga32(int w, int h, const DWORD* argb, bool topDown)
    {
        std::vector<BYTE> v(18, 0);
        v[2] = 2; v[12] = (BYTE) w; v[13] = (BYTE) (w >> 8); v[14] = (BYTE) h; v[15] = (BYTE) (h >> 8); v[16] = 32; v[17] = (BYTE) (8 | (topDown ? 0x20 : 0));
        for (int yy = 0; yy < h; yy++)
        {
            int y = topDown ? yy : h - 1 - yy;
            for (int x = 0; x < w; x++) { DWORD c = argb[y * w + x]; v.push_back((BYTE) c); v.push_back((BYTE) (c >> 8)); v.push_back((BYTE) (c >> 16)); v.push_back((BYTE) (c >> 24)); }
        }
        return v;
    }

    std::vector<BYTE> MakeDdsDxt3(const BYTE* blocks, UINT blockBytes)
    {
        std::vector<BYTE> v;
        Put32(v, 0x20534444); Put32(v, 124); Put32(v, 0x81007); Put32(v, 4); Put32(v, 8); Put32(v, blockBytes); Put32(v, 0); Put32(v, 0);
        for (int i = 0; i < 11; i++) Put32(v, 0);
        Put32(v, 32); Put32(v, 4); Put32(v, D3DFMT_DXT3); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        Put32(v, 0x1000); Put32(v, 0); Put32(v, 0); Put32(v, 0); Put32(v, 0);
        v.insert(v.end(), blocks, blocks + blockBytes);
        return v;
    }

    DWORD Pixel(LPDIRECT3DTEXTURE9 tex, UINT level, UINT x, UINT y)
    {
        D3DLOCKED_RECT lr;
        tex->LockRect(level, &lr, NULL, D3DLOCK_READONLY);
        DWORD v = *(DWORD*) ((BYTE*) lr.pBits + y * lr.Pitch + x * 4);
        tex->UnlockRect(level);
        return v;
    }
}

TEST_CLASS(D3DXTextureWrappers)
{
public:
    TEST_METHOD(CreateTextureHonoursExplicitArguments)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device on this machine; skipped"); return; }
        LPDIRECT3DTEXTURE9 tex = 0;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTexture(nr.dev, 64, 32, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &tex)));
        Assert::IsNotNull(tex);
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<unsigned>(64, desc.Width);
        Assert::AreEqual<unsigned>(32, desc.Height);
        Assert::AreEqual<int>(D3DFMT_A4R4G4B4, desc.Format);
        Assert::AreEqual<unsigned long>(1, tex->GetLevelCount());
        tex->Release();
    }

    TEST_METHOD(CreateTextureAppliesD3dxDefaults)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device on this machine; skipped"); return; }
        LPDIRECT3DTEXTURE9 tex = 0;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTexture(nr.dev, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, &tex)));
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<unsigned>(256, desc.Width);
        Assert::AreEqual<unsigned>(256, desc.Height);
        Assert::AreEqual<int>(D3DFMT_A8R8G8B8, desc.Format);
        Assert::AreEqual<unsigned long>(9, tex->GetLevelCount());
        tex->Release();
        Assert::IsTrue(FAILED(D3DXCreateTexture(0, 16, 16, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex)));
    }

    TEST_METHOD(CreateTextureReplacesAFormatTheDeviceRefuses)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device on this machine; skipped"); return; }
        LPDIRECT3D9 d3d = 0;
        nr.dev->GetDirect3D(&d3d);
        D3DCAPS9 caps;
        nr.dev->GetDeviceCaps(&caps);
        D3DDISPLAYMODE mode;
        nr.dev->GetDisplayMode(0, &mode);
        BOOL bR8G8B8 = SUCCEEDED(d3d->CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, mode.Format, D3DUSAGE_RENDERTARGET,
                                                        D3DRTYPE_TEXTURE, D3DFMT_R8G8B8));
        BOOL bX8R8G8B8 = SUCCEEDED(d3d->CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, mode.Format, D3DUSAGE_RENDERTARGET,
                                                          D3DRTYPE_TEXTURE, D3DFMT_X8R8G8B8));
        d3d->Release();
        if (!bR8G8B8 && !bX8R8G8B8) { Logger::WriteMessage("the device takes neither format; skipped"); return; }
        LPDIRECT3DTEXTURE9 tex = 0;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTexture(nr.dev, 64, 64, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &tex)));
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(bR8G8B8 ? D3DFMT_R8G8B8 : D3DFMT_X8R8G8B8, desc.Format);
        Assert::AreEqual<unsigned long>(1, tex->GetLevelCount());
        tex->Release();
    }
};

TEST_CLASS(D3DXTextureLoaders)
{
public:
    TEST_METHOD(BmpLoadsAsX8R8G8B8WithOneLevel)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        const DWORD px[8] = { 0xFF0000, 0x00FF00, 0x0000FF, 0x123456, 0x000000, 0xFFFFFF, 0x808080, 0xABCDEF };
        std::vector<BYTE> bmp = MakeBmp24(4, 2, px);
        LPDIRECT3DTEXTURE9 tex = 0;
        D3DXIMAGE_INFO info;
        HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(nr.dev, &bmp[0], (UINT) bmp.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                         D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &tex);
        Assert::IsTrue(SUCCEEDED(hr));
        Assert::AreEqual<unsigned>(4, info.Width); Assert::AreEqual<unsigned>(2, info.Height);
        Assert::AreEqual<int>(D3DFMT_X8R8G8B8, info.Format);
        Assert::AreEqual<int>(D3DXIFF_BMP, info.ImageFileFormat);
        Assert::AreEqual<int>(D3DRTYPE_TEXTURE, info.ResourceType);
        Assert::AreEqual<unsigned long>(1, tex->GetLevelCount());
        Assert::AreEqual<DWORD>(0xFFFF0000, Pixel(tex, 0, 0, 0));
        Assert::AreEqual<DWORD>(0xFF123456, Pixel(tex, 0, 3, 0));
        Assert::AreEqual<DWORD>(0xFFABCDEF, Pixel(tex, 0, 3, 1));
        tex->Release();
    }

    TEST_METHOD(ColorKeyForcesAlphaAndBlanksThePixel)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        const DWORD px[4] = { 0x000000, 0xFF00FF, 0x000000, 0x0000FF };
        std::vector<BYTE> bmp = MakeBmp24(2, 2, px);
        LPDIRECT3DTEXTURE9 tex = 0;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &bmp[0], (UINT) bmp.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                                     D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR, D3DX_FILTER_LINEAR,
                                                                     0xFF000000, NULL, NULL, &tex)));
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_A8R8G8B8, desc.Format);
        Assert::AreEqual<DWORD>(0x00000000, Pixel(tex, 0, 0, 0));
        Assert::AreEqual<DWORD>(0xFFFF00FF, Pixel(tex, 0, 1, 0));
        tex->Release();
    }

    TEST_METHOD(FullMipChainIsBoxFiltered)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        DWORD px[16];
        for (int i = 0; i < 16; i++) px[i] = (i % 2) ? 0x000000 : 0xFFFFFF;
        std::vector<BYTE> bmp = MakeBmp24(4, 4, px);
        LPDIRECT3DTEXTURE9 tex = 0;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &bmp[0], (UINT) bmp.size(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                                                     D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex)));
        Assert::AreEqual<unsigned long>(3, tex->GetLevelCount());
        Assert::AreEqual<DWORD>(0xFF7F7F7F, Pixel(tex, 1, 0, 0));
        Assert::AreEqual<DWORD>(0xFF7F7F7F, Pixel(tex, 2, 0, 0));
        tex->Release();
    }

    TEST_METHOD(TgaKeepsAlphaAndHonoursOrigin)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        const DWORD px[4] = { 0x80FF0000, 0xFF00FF00, 0x000000FF, 0x40FFFFFF };
        for (int topDown = 0; topDown < 2; topDown++)
        {
            std::vector<BYTE> tga = MakeTga32(2, 2, px, topDown != 0);
            LPDIRECT3DTEXTURE9 tex = 0;
            D3DXIMAGE_INFO info;
            Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &tga[0], (UINT) tga.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                                         D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &tex)));
            Assert::AreEqual<int>(D3DFMT_A8R8G8B8, info.Format);
            Assert::AreEqual<int>(D3DXIFF_TGA, info.ImageFileFormat);
            Assert::AreEqual<DWORD>(0x80FF0000, Pixel(tex, 0, 0, 0));
            Assert::AreEqual<DWORD>(0x40FFFFFF, Pixel(tex, 0, 1, 1));
            tex->Release();
        }
    }

    TEST_METHOD(DdsDxt3PassesThroughUntouched)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        BYTE blocks[32];
        for (int i = 0; i < 32; i++) blocks[i] = (BYTE) (i * 7 + 3);
        std::vector<BYTE> dds = MakeDdsDxt3(blocks, 32);
        LPDIRECT3DTEXTURE9 tex = 0;
        D3DXIMAGE_INFO info;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &dds[0], (UINT) dds.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                                     D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &tex)));
        Assert::AreEqual<int>(D3DFMT_DXT3, info.Format);
        Assert::AreEqual<int>(D3DXIFF_DDS, info.ImageFileFormat);
        Assert::AreEqual<unsigned>(8, info.Width); Assert::AreEqual<unsigned>(4, info.Height);
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_DXT3, desc.Format);
        D3DLOCKED_RECT lr;
        tex->LockRect(0, &lr, NULL, D3DLOCK_READONLY);
        Assert::AreEqual(0, memcmp(lr.pBits, blocks, 32));
        tex->UnlockRect(0);
        tex->Release();
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &dds[0], (UINT) dds.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                                     D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, NULL, NULL, &tex)));
        tex->Release();
        Assert::IsTrue(FAILED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &dds[0], (UINT) dds.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                                  D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex)));
    }

    TEST_METHOD(DdsDxt3WithMoreLevelsThanTheFileIsDecodedAndFiltered)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device on this machine; skipped"); return; }
        BYTE blocks[32];
        memset(blocks, 0xFF, 8); *(WORD*) (blocks + 8) = 0xF800; *(WORD*) (blocks + 10) = 0xF800; memset(blocks + 12, 0, 4);
        memset(blocks + 16, 0xFF, 8); *(WORD*) (blocks + 24) = 0x001F; *(WORD*) (blocks + 26) = 0x001F; memset(blocks + 28, 0, 4);
        std::vector<BYTE> dds = MakeDdsDxt3(blocks, 32);
        LPDIRECT3DTEXTURE9 tex = 0;
        D3DXIMAGE_INFO info;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &dds[0], (UINT) dds.size(), D3DX_DEFAULT, D3DX_DEFAULT, 2, 0,
            D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &tex)));
        Assert::AreEqual<int>(D3DFMT_DXT3, info.Format);
        Assert::AreEqual<unsigned long>(2, tex->GetLevelCount());
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_A8R8G8B8, desc.Format);
        Assert::AreEqual<unsigned long>(0xFFFF0000, Pixel(tex, 0, 1, 1));
        Assert::AreEqual<unsigned long>(0xFF0000FF, Pixel(tex, 0, 5, 2));
        Assert::AreEqual<unsigned long>(0xFFFF0000, Pixel(tex, 1, 0, 0));
        Assert::AreEqual<unsigned long>(0xFF0000FF, Pixel(tex, 1, 3, 1));
        tex->Release();
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &dds[0], (UINT) dds.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
            D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &tex)));
        tex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_DXT3, desc.Format);
        tex->Release();
    }

    TEST_METHOD(FromFileReadsTheFileAndRejectsGarbage)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        CreateDirectoryA(dir.c_str(), 0);
        const DWORD px[2] = { 0x112233, 0x445566 };
        std::vector<BYTE> bmp = MakeBmp24(2, 1, px);
        SeifukuFixtures::WriteBlob(dir + "\\seifuku_tiny.bmp", &bmp[0], (DWORD) bmp.size());
        SeifukuFixtures::WriteBlob(dir + "\\seifuku_garbage.bin", "not an image at all, really not", 31);
        LPDIRECT3DTEXTURE9 tex = 0;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileEx(nr.dev, (dir + "\\seifuku_tiny.bmp").c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                             D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex)));
        Assert::AreEqual<DWORD>(0xFF445566, Pixel(tex, 0, 1, 0));
        tex->Release();
        Assert::IsTrue(FAILED(D3DXCreateTextureFromFileEx(nr.dev, (dir + "\\seifuku_garbage.bin").c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                          D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex)));
        Assert::IsTrue(FAILED(D3DXCreateTextureFromFileEx(nr.dev, (dir + "\\seifuku_missing.bmp").c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                          D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &tex)));
    }

    TEST_METHOD(PngGoesThroughWic)
    {
        NullRefDevice nr;
        if (!nr.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        IWICImagingFactory* factory = 0;
        Assert::IsTrue(SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**) &factory)));
        IStream* mem = 0;
        CreateStreamOnHGlobal(NULL, TRUE, &mem);
        IWICBitmapEncoder* enc = 0;
        Assert::IsTrue(SUCCEEDED(factory->CreateEncoder(GUID_ContainerFormatPng, NULL, &enc)));
        enc->Initialize(mem, WICBitmapEncoderNoCache);
        IWICBitmapFrameEncode* frame = 0;
        enc->CreateNewFrame(&frame, NULL);
        frame->Initialize(NULL);
        frame->SetSize(2, 1);
        WICPixelFormatGUID fmt = GUID_WICPixelFormat32bppBGRA;
        frame->SetPixelFormat(&fmt);
        DWORD src[2] = { 0x80112233, 0xFF445566 };
        frame->WritePixels(1, 8, 8, (BYTE*) src);
        frame->Commit(); enc->Commit();
        HGLOBAL hg = 0; GetHGlobalFromStream(mem, &hg);
        SIZE_T size = GlobalSize(hg);
        std::vector<BYTE> png((BYTE*) GlobalLock(hg), (BYTE*) GlobalLock(hg) + size);
        GlobalUnlock(hg); GlobalUnlock(hg);
        frame->Release(); enc->Release(); mem->Release(); factory->Release();
        if (SUCCEEDED(hrCo)) CoUninitialize();

        LPDIRECT3DTEXTURE9 tex = 0;
        D3DXIMAGE_INFO info;
        Assert::IsTrue(SUCCEEDED(D3DXCreateTextureFromFileInMemoryEx(nr.dev, &png[0], (UINT) png.size(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0,
                                                                     D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, NULL, &tex)));
        Assert::AreEqual<int>(D3DXIFF_PNG, info.ImageFileFormat);
        Assert::AreEqual<int>(D3DFMT_A8R8G8B8, info.Format);
        Assert::AreEqual<DWORD>(0x80112233, Pixel(tex, 0, 0, 0));
        Assert::AreEqual<DWORD>(0xFF445566, Pixel(tex, 0, 1, 0));
        tex->Release();
    }
};
