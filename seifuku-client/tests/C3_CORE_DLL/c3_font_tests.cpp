#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <string>
#include <vector>
#include "c3_font.h"
#include "seifuku_fixtures.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(C3Font) == 0x40128, "");
static_assert(offsetof(C3Font, lpFace) == 0x000 && offsetof(C3Font, szName) == 0x004 && sizeof(((C3Font*) 0)->szName) == 256, "");
static_assert(offsetof(C3Font, nSize) == 0x104 && offsetof(C3Font, nRealSize) == 0x108, "");
static_assert(offsetof(C3Font, lpChar) == 0x10C && sizeof(((C3Font*) 0)->lpChar) == 0x40000, "");
static_assert(offsetof(C3Font, atlases) == 0x4010C && sizeof(C3TexList) == 0x10, "");
static_assert(offsetof(C3TexList, lpFirst) == 4 && offsetof(C3TexList, lpLast) == 8 && offsetof(C3TexList, lpEnd) == 0xC, "");
static_assert(offsetof(C3Font, nCursorX) == 0x4011C && offsetof(C3Font, nCursorY) == 0x40120 && offsetof(C3Font, bAntiAlias) == 0x40124, "");

static_assert(sizeof(C3Char) == 0x1C, "");
static_assert(offsetof(C3Char, Char) == 0 && offsetof(C3Char, lpTex) == 4 && offsetof(C3Char, nTop) == 8, "");
static_assert(offsetof(C3Char, u0) == 0xC && offsetof(C3Char, v0) == 0x10 && offsetof(C3Char, u1) == 0x14 && offsetof(C3Char, v1) == 0x18, "");

static_assert(sizeof(FontVertex) == 0x1C, "");
static_assert(offsetof(FontVertex, x) == 0 && offsetof(FontVertex, y) == 4 && offsetof(FontVertex, z) == 8 && offsetof(FontVertex, rhw) == 0xC, "");
static_assert(offsetof(FontVertex, color) == 0x10 && offsetof(FontVertex, u) == 0x14 && offsetof(FontVertex, v) == 0x18, "");
static_assert(FONT_VERTEX == 0x144, "");

static_assert(std::is_same<decltype(&Font_Create), BOOL (*)(C3Font**, LPCSTR, int, int, int)>::value, "");
static_assert(std::is_same<decltype(&Font_CreateDbcsSpec), BOOL (*)(C3Font**, WORD, LPCSTR)>::value, "");
static_assert(std::is_same<decltype(&Font_Save), void (*)(C3Font*, LPCSTR)>::value, "");
static int (* const kFontDrawStyled)(C3Font*, float, float, D3DCOLOR, LPCSTR, RENDER_TEXT_STYLE, DWORD, long, long) = &Font_Draw;
static BOOL (* const kFontDrawData)(C3Font*, float, float, D3DCOLOR, LPCSTR, const void*) = &Font_Draw;
static_assert(std::is_same<decltype(&Font_Release), void (*)(C3Font**)>::value && std::is_same<decltype(&Font_Prepare), void (*)(void)>::value, "");
static_assert(std::is_same<decltype(&Font_CheckPixel), BOOL (*)(C3Font*, LPCSTR, DWORD)>::value, "");
static_assert(std::is_same<decltype(&Font_Clear),void (*)(C3Font*)>::value && std::is_same<decltype(&Char_Clear), void (*)(C3Char*)>::value, "");
static_assert(std::is_same<decltype(&Font_DrawBegin), void (*)(void)>::value && std::is_same<decltype(&Font_DrawEnd), bool (*)(void)>::value, "");
static_assert(std::is_same<decltype(&Font_SetTextCornerColor), void (*)(DWORD, DWORD, DWORD, DWORD)>::value, "");
static_assert(RENDER_TEXT_NORMAL == 0 && RENDER_TEXT_SHADOW == 1 && RENDER_TEXT_OUTLINE == 2 && RENDER_TEXT_STROKE == 3 && RENDER_TEXT_GRADIENT == 4, "");

namespace
{
    struct DllDevice
    {
        LPDIRECT3D9 d3d;
        LPDIRECT3DDEVICE9 dev;
        LPDIRECT3DDEVICE9* slot;
        DllDevice() : d3d(0), dev(0), slot(0)
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
        ~DllDevice()
        {
            if (slot) *slot = 0;
            if (dev) dev->Release();
            if (d3d) d3d->Release();
        }
    };

    struct TqFontHead { char szTag[8]; DWORD dwVersion; DWORD dwChars; int nSize; };
    static_assert(sizeof(TqFontHead) == 0x14, "");

    std::vector<BYTE> ReadAll(const std::string& path)
    {
        std::vector<BYTE> v;
        HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (h == INVALID_HANDLE_VALUE) return v;
        DWORD n = GetFileSize(h, 0), r = 0;
        v.resize(n);
        if (n) ReadFile(h, &v[0], n, &r, 0);
        CloseHandle(h);
        return v;
    }

    int AtlasCount(const C3Font* f) { return f->atlases.lpFirst ? (int) (f->atlases.lpLast - f->atlases.lpFirst) : 0; }

    DWORD AtlasAlpha(LPDIRECT3DTEXTURE9 tex, int x, int y)
    {
        D3DSURFACE_DESC desc;
        tex->GetLevelDesc(0, &desc);
        D3DLOCKED_RECT lock;
        if (FAILED(tex->LockRect(0, &lock, 0, D3DLOCK_READONLY))) return 0xDEAD;
        DWORD a;
        if (desc.Format == D3DFMT_A8R8G8B8)
            a = ((const DWORD*) ((const BYTE*) lock.pBits + y * lock.Pitch))[x] >> 24;
        else
            a = ((const WORD*) ((const BYTE*) lock.pBits + y * lock.Pitch))[x] >> 12;
        tex->UnlockRect(0);
        return a;
    }
}

TEST_CLASS(C3FontHeader)
{
public:
    TEST_METHOD(GlyphTableIndexIsLeadByteTimes256PlusTrail)
    {
        C3Font* f = (C3Font*) 0;
        Assert::AreEqual<size_t>(0x10C + (0xD6 * 256 + 0xD0) * 4, (size_t) &f->lpChar[0xD6][0xD0]);
        Assert::AreEqual<size_t>(0x10C + 'A' * 4, (size_t) &f->lpChar[0]['A']);
    }

    TEST_METHOD(CharClearAndFontClear)
    {
        C3Char c;
        memset(&c, 0xCD, sizeof(c));
        Char_Clear(&c);
        Assert::AreEqual<char>(0, c.Char[0]); Assert::AreEqual<char>(0, c.Char[1]); Assert::IsNull(c.lpTex);
        Assert::AreEqual<int>((int) 0xCDCDCDCD, c.nTop);

        C3Font* f = new C3Font;
        memset(f, 0xCD, sizeof(*f));
        f->atlases.lpFirst = (LPDIRECT3DTEXTURE9*) 0x1234;
        f->atlases.lpLast = (LPDIRECT3DTEXTURE9*) 0x1240;
        f->atlases.lpEnd = (LPDIRECT3DTEXTURE9*) 0x1250;
        Font_Clear(f);
        Assert::AreEqual<DWORD>(0xCDCDCDCD, (DWORD) f->lpFace);
        Assert::AreEqual<char>((char) 0xCD, f->szName[0]);
        Assert::AreEqual(0, f->nSize); Assert::AreEqual(0, f->nRealSize);
        Assert::IsTrue(f->atlases.lpLast == f->atlases.lpFirst && f->atlases.lpFirst == (LPDIRECT3DTEXTURE9*) 0x1234);
        Assert::IsTrue(f->atlases.lpEnd == (LPDIRECT3DTEXTURE9*) 0x1250);
        Assert::AreEqual(0, f->nCursorX); Assert::AreEqual(0, f->nCursorY); Assert::AreEqual<BYTE>(0, f->bAntiAlias);
        for (int i = 0; i < 256; i += 85) for (int j = 0; j < 256; j += 51) Assert::IsNull(f->lpChar[i][j]);
        delete f;
    }

    TEST_METHOD(CreateResolvesTheFontAndSizes)
    {
        C3Font* f = 0;
        Assert::IsTrue(Font_Create(&f, "Arial", 18, 1, 1));
        Assert::IsNotNull(f);
        Assert::AreEqual(32, f->nSize);
        Assert::AreEqual(18, f->nRealSize);
        Assert::AreEqual("Arial", f->szName);
        Assert::IsNotNull(f->lpFace);
        Assert::AreEqual<BYTE>(1, f->bAntiAlias);
        Assert::AreEqual(0, AtlasCount(f)); Assert::AreEqual(0, f->nCursorX);
        Font_Release(&f);
        Assert::IsNull(f);
        Font_Release(&f);

        Assert::IsTrue(Font_Create(&f, "Arial", 12, 0, 1));
        Assert::AreEqual<BYTE>(0, f->bAntiAlias); Assert::AreEqual(16, f->nSize);
        Font_Release(&f);

        Assert::IsTrue(Font_Create(&f, "Arial", 70, 1, 0));
        Assert::IsNotNull(f);
        Assert::AreEqual(128, f->nSize); Assert::AreEqual(70, f->nRealSize); Assert::AreEqual("Arial", f->szName);
        Assert::IsNull(f->lpFace); Assert::AreEqual<BYTE>(0, f->bAntiAlias);
        Font_Release(&f);
        Assert::IsNull(f);

        f = (C3Font*) 0x1;
        Assert::IsFalse(Font_Create(&f, "Seifuku No Such Font", 16, 1, 1));
        Assert::IsTrue(f == (C3Font*) 0x1);
    }

    TEST_METHOD(DrawPacksGlyphsIntoAtlasesAndAnswersTheAdvance)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        C3Font* f = 0;
        Assert::IsTrue(Font_Create(&f, "Arial", 18, 1, 1));
        Assert::IsTrue(Begin3D());
        DWORD nTri0 = g_dwTriTotal;

        int nAdvance = Font_Draw(f, 10.0f, 20.0f, 0xFFFFFFFF, "A", RENDER_TEXT_NORMAL, 0, 0, 0);
        Assert::IsTrue(nAdvance >= 9);
        Assert::AreEqual<DWORD>(2, g_dwTriTotal - nTri0);
        Assert::AreEqual(1, AtlasCount(f));
        C3Char* a = f->lpChar[0]['A'];
        Assert::IsNotNull(a);
        Assert::IsTrue(a->lpTex == f->atlases.lpFirst[0]);
        D3DSURFACE_DESC desc;
        a->lpTex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_A8R8G8B8, desc.Format); Assert::AreEqual<UINT>(256, desc.Width); Assert::AreEqual<UINT>(256, desc.Height);
        Assert::AreEqual(0.0f, a->u0); Assert::AreEqual(0.0f, a->v0);
        Assert::IsTrue(a->u1 > a->u0 && a->v1 > a->v0);
        int nWidth = (int) ((a->u1 - a->u0) * 256.0f + 0.5f), nRows = (int) ((a->v1 - a->v0) * 256.0f + 0.5f);
        Assert::IsTrue(nWidth > 0 && nWidth <= 18 && nRows > 0 && nRows <= 18);
        Assert::AreEqual(nWidth + 1 > 9 ? nWidth + 1 : 9, nAdvance);
        Assert::IsTrue(a->nTop >= 0 && a->nTop < 18);
        Assert::AreEqual(nWidth + 2, f->nCursorX); Assert::AreEqual(0, f->nCursorY);
        bool bInk = false;
        for (int y = 0; y < nRows; y++) for (int x = 0; x < nWidth; x++) if (AtlasAlpha(a->lpTex, x, y) > 0) bInk = true;
        Assert::IsTrue(bInk);
        Assert::AreEqual<DWORD>(0, AtlasAlpha(a->lpTex, nWidth + 2, 0));

        Assert::AreEqual(nAdvance, Font_Draw(f, 10.0f, 20.0f, 0xFF00FF00, "A", RENDER_TEXT_NORMAL, 0, 0, 0));
        Assert::AreEqual<DWORD>(4, g_dwTriTotal - nTri0);
        Assert::IsTrue(f->lpChar[0]['A'] == a);

        int nX = f->nCursorX;
        Assert::AreEqual(9, Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, " ", RENDER_TEXT_NORMAL, 0, 0, 0));
        C3Char* sp = f->lpChar[0][' '];
        Assert::IsNotNull(sp); Assert::AreEqual(sp->u0, sp->u1); Assert::AreEqual(nX + 2, f->nCursorX);

        char s[2] = { 0, 0 };
        for (int c = '!'; c <= '~'; c++) { s[0] = (char) c; Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, s, RENDER_TEXT_NORMAL, 0, 0, 0) > 0); }
        Assert::IsTrue(f->nCursorY > 0 && f->nCursorY % 20 == 0);
        Assert::AreEqual(1, AtlasCount(f));
        for (int c = 0xA1; c <= 0xFE; c++)
        {
            s[0] = (char) c;
            if (IsDBCSLeadByte((BYTE) c)) continue;
            Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, s, RENDER_TEXT_NORMAL, 0, 0, 0);
        }
        for (int c = 0xA1; c <= 0xFE && AtlasCount(f) < 2; c++)
        {
            f->nCursorY = 240; f->nCursorX = 250;
            s[0] = 'W';
            if (f->lpChar[0]['W']) { delete f->lpChar[0]['W']; f->lpChar[0]['W'] = 0; }
            Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, s, RENDER_TEXT_NORMAL, 0, 0, 0);
        }
        Assert::AreEqual(2, AtlasCount(f));
        Assert::AreEqual(0, f->nCursorY);
        Assert::IsTrue(f->lpChar[0]['W']->lpTex == f->atlases.lpFirst[1]);
        End3D();
        Font_Release(&f);

        Assert::IsTrue(Font_Create(&f, "Arial", 16, 0, 1));
        Assert::IsTrue(Begin3D());
        Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "H", RENDER_TEXT_NORMAL, 0, 0, 0) > 0);
        C3Char* h = f->lpChar[0]['H'];
        h->lpTex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_A4R4G4B4, desc.Format);
        Assert::AreEqual<DWORD>(0xF, AtlasAlpha(h->lpTex, 0, 0));
        End3D();
        Font_Release(&f);
    }

    TEST_METHOD(DrawStylesCountTheirTriangles)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        C3Font* f = 0;
        Assert::IsTrue(Font_Create(&f, "Arial", 16, 1, 1));
        Assert::IsTrue(Begin3D());
        DWORD nTri0 = g_dwTriTotal;
        Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "B", RENDER_TEXT_SHADOW, 0xFF000000, 1, 1) > 0);
        Assert::AreEqual<DWORD>(4, g_dwTriTotal - nTri0);
        Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "B", RENDER_TEXT_OUTLINE, 0xFF000000, 0, 0) > 0);
        Assert::AreEqual<DWORD>(22, g_dwTriTotal - nTri0);
        Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "B", RENDER_TEXT_STROKE, 0xFF000000, 2, -1) > 0);
        Assert::AreEqual<DWORD>(30, g_dwTriTotal - nTri0);
        Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "B", RENDER_TEXT_STROKE, 0xFF000000, 0, 0) > 0);
        Assert::AreEqual<DWORD>(32, g_dwTriTotal - nTri0);
        Font_SetTextCornerColor(0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF);
        Assert::IsTrue(Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "B", RENDER_TEXT_GRADIENT, 0, 0, 0) > 0);
        Assert::AreEqual<DWORD>(34, g_dwTriTotal - nTri0);
        Font_SetTextCornerColor(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
        Assert::AreEqual(0, Font_Draw(f, 0.0f, 0.0f, 0xFFFFFFFF, "B", (RENDER_TEXT_STYLE) 7, 0, 0, 0));
        Assert::AreEqual<DWORD>(34, g_dwTriTotal - nTri0);
        Font_DrawBegin();
        Assert::IsTrue(Font_DrawEnd());
        End3D();
        Font_Release(&f);
    }

    TEST_METHOD(CheckPixelCountsTheInkOfACharacter)
    {
        C3Font* f = 0;
        Assert::IsTrue(Font_Create(&f, "Arial", 16, 1, 1));
        Assert::IsTrue(Font_CheckPixel(f, "A", 0));
        Assert::IsTrue(Font_CheckPixel(f, "A", 4));
        Assert::IsFalse(Font_CheckPixel(f, "A", 100000));
        Assert::IsFalse(Font_CheckPixel(f, " ", 0));
        Font_Release(&f);
        Assert::IsTrue(Font_Create(&f, "Arial", 16, 1, 0));
        Assert::IsFalse(Font_CheckPixel(f, "A", 0));
        Font_Release(&f);
    }

    TEST_METHOD(PrepareSetsTheStateList)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        dd.dev->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
        dd.dev->SetRenderState(D3DRS_LIGHTING, TRUE);
        dd.dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        dd.dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
        Font_Prepare();
        DWORD st = 0;
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_SHADEMODE, &st))); Assert::AreEqual<DWORD>(D3DSHADE_GOURAUD, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_FILLMODE, &st))); Assert::AreEqual<DWORD>(D3DFILL_SOLID, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_LIGHTING, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_DITHERENABLE, &st))); Assert::AreEqual<DWORD>(FALSE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_CULLMODE, &st))); Assert::AreEqual<DWORD>(D3DCULL_NONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetRenderState(D3DRS_ALPHABLENDENABLE, &st))); Assert::AreEqual<DWORD>(TRUE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(0, D3DTSS_COLOROP, &st))); Assert::AreEqual<DWORD>(D3DTOP_SELECTARG2, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(0, D3DTSS_ALPHAOP, &st))); Assert::AreEqual<DWORD>(D3DTOP_MODULATE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MINFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_POINT, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetSamplerState(0, D3DSAMP_MIPFILTER, &st))); Assert::AreEqual<DWORD>(D3DTEXF_NONE, st);
        Assert::IsTrue(SUCCEEDED(dd.dev->GetTextureStageState(1, D3DTSS_COLOROP, &st))); Assert::AreEqual<DWORD>(D3DTOP_DISABLE, st);
    }

    TEST_METHOD(SaveWritesTheTqFontFileTheDataDrawReads)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir() + "\\font";
        CreateDirectoryA(dir.c_str(), 0);
        std::string path = dir + "\\arial16.fnt";
        DeleteFileA(path.c_str());

        C3Font* f = 0;
        Assert::IsTrue(Font_Create(&f, "Arial", 16, 0, 1));
        Font_Save(f, path.c_str());
        Font_Release(&f);

        std::vector<BYTE> file = ReadAll(path);
        Assert::IsTrue(file.size() > sizeof(TqFontHead));
        TqFontHead head;
        memcpy(&head, &file[0], sizeof(head));
        Assert::AreEqual("TQFONT", head.szTag);
        Assert::AreEqual<DWORD>(0x1000, head.dwVersion);
        Assert::AreEqual(16, head.nSize);
        Assert::IsTrue(head.dwChars > 0);
        const DWORD glyph = 16 * 16 / 2;
        Assert::AreEqual<size_t>(sizeof(head) + head.dwChars * (glyph + 8), file.size());
        const int* table = (const int*) &file[sizeof(head) + head.dwChars * glyph];
        long offA = -1, offSpace = -1; int nLast = -1; bool bZero = false;
        for (DWORD i = 0; i < head.dwChars; i++)
        {
            int code = table[2 * i]; long off = table[2 * i + 1];
            Assert::IsTrue(code > nLast); nLast = code;
            Assert::IsTrue(off >= (long) sizeof(head) && (off - (long) sizeof(head)) % glyph == 0);
            if (code == 'A') offA = off;
            if (code == ' ') offSpace = off;
            if (code == 0) bZero = true;
        }
        Assert::IsTrue(offA >= 0); Assert::IsTrue(offSpace >= 0);
        Assert::IsTrue(bZero);
        bool bInk = false;
        for (DWORD i = 0; i < glyph; i++) { Assert::AreEqual<BYTE>(0, file[offSpace + i]); if (file[offA + i]) bInk = true; }
        Assert::IsTrue(bInk);
        C3Font* g = 0;
        Assert::IsTrue(Font_Create(&g, "Arial", 16, 0, 0));
        Assert::IsTrue(Begin3D());
        Assert::IsTrue(Font_Draw(g, 0.0f, 0.0f, 0xFFFFFFFF, "A", &file[offA]));
        Assert::IsNotNull(g->lpChar[0]['A']);
        D3DSURFACE_DESC desc;
        g->lpChar[0]['A']->lpTex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_A4R4G4B4, desc.Format); Assert::AreEqual<UINT>(16, desc.Width);
        Assert::AreEqual(0, AtlasCount(g));
        End3D();
        Font_Release(&g);
    }

    TEST_METHOD(CreateDbcsSpecLoadsAGlyphFromAnImageFile)
    {
        DllDevice dd;
        if (!dd.dev) { Logger::WriteMessage("no NULLREF device; skipped"); return; }
        std::string dir = SeifukuFixtures::Dir();
        if (ReadAll(dir + "\\seifuku_tiny.bmp").empty())
        {
            BYTE bmp[54 + 4 * 2 * 2];
            memset(bmp, 0, sizeof(bmp));
            bmp[0] = 'B'; bmp[1] = 'M'; *(DWORD*) &bmp[2] = sizeof(bmp); *(DWORD*) &bmp[10] = 54;
            *(DWORD*) &bmp[14] = 40; *(int*) &bmp[18] = 2; *(int*) &bmp[22] = 2; *(WORD*) &bmp[26] = 1; *(WORD*) &bmp[28] = 32;
            SeifukuFixtures::WriteBlob(dir + "\\seifuku_tiny.bmp", bmp, sizeof(bmp));
        }
        C3Font* f = 0;
        Assert::IsTrue(Font_Create(&f, "Arial", 16, 0, 0));
        Assert::IsTrue(Font_CreateDbcsSpec(&f, 0xA1B0, (dir + "\\seifuku_tiny.bmp").c_str()));
        C3Char* c = f->lpChar[0xB0][0xA1];
        Assert::IsNotNull(c); Assert::IsNotNull(c->lpTex);
        D3DSURFACE_DESC desc;
        c->lpTex->GetLevelDesc(0, &desc);
        Assert::AreEqual<int>(D3DFMT_A4R4G4B4, desc.Format);
        Assert::IsTrue(Font_CreateDbcsSpec(&f, 0xA1B0, (dir + "\\seifuku_tiny.bmp").c_str()));
        Assert::IsNotNull(f->lpChar[0xB0][0xA1]);
        Assert::IsFalse(Font_CreateDbcsSpec(&f, 0xA1B0, (dir + "\\missing_glyph.bmp").c_str()));
        Assert::IsNull(f->lpChar[0xB0][0xA1]);
        Assert::IsFalse(Font_CreateDbcsSpec(&f, 0xA1B0, (LPCSTR) 0));
        Assert::IsFalse(Font_CreateDbcsSpec((C3Font**) 0, 0xA1B0, "x.bmp"));
        Font_Release(&f);
    }
};
