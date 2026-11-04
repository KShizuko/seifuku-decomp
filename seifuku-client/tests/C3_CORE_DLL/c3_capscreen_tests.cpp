#include "CppUnitTest.h"
#include <type_traits>
#include "c3_capscreen.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(std::is_same<decltype(&CapScreen), void (*)(char*)>::value, "");
static_assert(std::is_same<decltype(&JPGEncode), BOOL (*)(char*, DWORD, DWORD, unsigned char*, IJL_COLOR, DWORD)>::value, "");
static_assert(IJL_RGB == 1 && IJL_BGR == 2, "");

TEST_CLASS(C3CapScreenHeader)
{
public:
    TEST_METHOD(ColourEnumMatchesIjl)
    {
        Assert::AreEqual(2, (int) IJL_BGR);
    }
};

#include <wincodec.h>
#include <string>
#include <vector>
#include "seifuku_jpegenc.h"
#include "seifuku_fixtures.h"

namespace
{
    struct TestCom
    {
        HRESULT hr;
        TestCom() { hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); }
        ~TestCom() { if (SUCCEEDED(hr)) CoUninitialize(); }
    };

    std::vector<BYTE> ReadAll(const std::string& path)
    {
        std::vector<BYTE> v;
        HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (h == INVALID_HANDLE_VALUE) return v;
        DWORD size = GetFileSize(h, 0), read = 0;
        v.resize(size);
        if (size) ReadFile(h, &v[0], size, &read, 0);
        CloseHandle(h);
        return v;
    }

    bool DecodeJpeg(const std::string& path, UINT& w, UINT& h, DWORD& centreBgra)
    {
        std::wstring wide(path.begin(), path.end());
        IWICImagingFactory* factory = 0;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**) &factory))) return false;
        IWICBitmapDecoder* decoder = 0;
        bool ok = SUCCEEDED(factory->CreateDecoderFromFilename(wide.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder));
        GUID container;
        ok = ok && SUCCEEDED(decoder->GetContainerFormat(&container)) && IsEqualGUID(container, GUID_ContainerFormatJpeg);
        IWICBitmapFrameDecode* frame = 0;
        ok = ok && SUCCEEDED(decoder->GetFrame(0, &frame)) && SUCCEEDED(frame->GetSize(&w, &h));
        IWICFormatConverter* conv = 0;
        ok = ok && SUCCEEDED(factory->CreateFormatConverter(&conv)) &&
             SUCCEEDED(conv->Initialize(frame, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom));
        if (ok)
        {
            WICRect rc = { (INT) w / 2, (INT) h / 2, 1, 1 };
            ok = SUCCEEDED(conv->CopyPixels(&rc, 4, 4, (BYTE*) &centreBgra));
        }
        if (conv) conv->Release();
        if (frame) frame->Release();
        if (decoder) decoder->Release();
        factory->Release();
        return ok;
    }

    bool IsRed(DWORD bgra) { return ((bgra >> 16) & 0xFF) > 200 && ((bgra >> 8) & 0xFF) < 60 && (bgra & 0xFF) < 60; }
}

TEST_CLASS(C3CapScreenFunctions)
{
public:
    TEST_METHOD(JpgEncodeWritesRedInBothChannelOrders)
    {
        TestCom com;
        std::string dir = SeifukuFixtures::Dir();
        CreateDirectoryA(dir.c_str(), 0);
        const UINT w = 16, h = 8;
        std::vector<BYTE> bgr(w * h * 3), rgb(w * h * 3);
        for (UINT i = 0; i < w * h; i++) { bgr[i * 3 + 2] = 0xFF; rgb[i * 3] = 0xFF; }

        std::string a = dir + "\\seifuku_cap_bgr.jpg", b = dir + "\\seifuku_cap_rgb.jpg";
        Assert::IsTrue(Seifuku_JpegEncode24(a.c_str(), w, h, &bgr[0], TRUE, 95) == TRUE);
        Assert::IsTrue(Seifuku_JpegEncode24(b.c_str(), w, h, &rgb[0], FALSE, 95) == TRUE);

        std::vector<BYTE> bytes = ReadAll(a);
        Assert::IsTrue(bytes.size() > 4);
        Assert::AreEqual<int>(0xFF, bytes[0]); Assert::AreEqual<int>(0xD8, bytes[1]);
        Assert::AreEqual<int>(0xFF, bytes[bytes.size() - 2]); Assert::AreEqual<int>(0xD9, bytes[bytes.size() - 1]);

        UINT dw = 0, dh = 0; DWORD px = 0;
        Assert::IsTrue(DecodeJpeg(a, dw, dh, px));
        Assert::AreEqual<UINT>(w, dw); Assert::AreEqual<UINT>(h, dh); Assert::IsTrue(IsRed(px));
        Assert::IsTrue(DecodeJpeg(b, dw, dh, px));
        Assert::AreEqual<UINT>(w, dw); Assert::IsTrue(IsRed(px));

        std::string c = dir + "\\seifuku_cap_q10.jpg";
        Assert::IsTrue(Seifuku_JpegEncode24(c.c_str(), w, h, &bgr[0], TRUE, 10) == TRUE);
        Assert::IsTrue(ReadAll(c).size() < bytes.size());
        Assert::IsTrue(Seifuku_JpegEncode24(NULL, w, h, &bgr[0], TRUE, 95) == FALSE);
        Assert::IsTrue(Seifuku_JpegEncode24(a.c_str(), 0, h, &bgr[0], TRUE, 95) == FALSE);
        Assert::IsTrue(Seifuku_JpegEncode24(a.c_str(), w, h, NULL, TRUE, 95) == FALSE);
        std::string bad = dir + "\\seifuku_no_such_dir\\x.jpg";
        Assert::IsTrue(Seifuku_JpegEncode24(bad.c_str(), w, h, &bgr[0], TRUE, 95) == FALSE);
        Assert::IsTrue(GetFileAttributesA(bad.c_str()) == INVALID_FILE_ATTRIBUTES);
    }

    TEST_METHOD(CapScreenWritesAJpegOfTheWindow)
    {
        TestCom com;
        std::string dir = SeifukuFixtures::Dir();
        CreateDirectoryA(dir.c_str(), 0);
        std::string path = dir + "\\seifuku_capscreen.jpg";
        DeleteFileA(path.c_str());
        g_DisplayMode.Width = 16; g_DisplayMode.Height = 8;
        g_hWnd = GetDesktopWindow();
        CapScreen((char*) path.c_str());
        g_hWnd = 0; g_DisplayMode.Width = 0; g_DisplayMode.Height = 0;
        UINT dw = 0, dh = 0; DWORD px = 0;
        Assert::IsTrue(DecodeJpeg(path, dw, dh, px));
        Assert::AreEqual<UINT>(16, dw); Assert::AreEqual<UINT>(8, dh);
    }
};
