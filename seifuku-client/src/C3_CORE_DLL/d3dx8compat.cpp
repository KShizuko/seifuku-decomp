#include "d3dx8compat.h"
#include <wincodec.h>
#include <stdio.h>
#include <vector>

namespace
{
    struct SeifukuImage
    {
        UINT width, height, mipLevels;
        D3DFORMAT format;
        D3DXIMAGE_FILEFORMAT fileFormat;
        bool compressed;
        std::vector<DWORD> argb;
        std::vector<BYTE> blocks;
        std::vector<size_t> levelOffset, levelSize;
        SeifukuImage() : width(0), height(0), mipLevels(1), format(D3DFMT_UNKNOWN), fileFormat(D3DXIFF_FORCE_DWORD), compressed(false) {}
    };

    inline WORD RdW(const BYTE* p) { return (WORD) (p[0] | (p[1] << 8)); }
    inline DWORD RdD(const BYTE* p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((DWORD) p[3] << 24); }

    inline UINT DxtLevelSize(UINT w, UINT h, UINT blockBytes)
    {
        UINT bw = (w + 3) / 4, bh = (h + 3) / 4;
        if (bw == 0) bw = 1;
        if (bh == 0) bh = 1;
        return bw * bh * blockBytes;
    }

    D3DFORMAT MaskedFormat(DWORD bits, DWORD flags, DWORD rm, DWORD gm, DWORD bm, DWORD am)
    {
        const bool alpha = (flags & 0x1) != 0;
        if (bits == 32 && rm == 0x00FF0000 && gm == 0x0000FF00 && bm == 0x000000FF)
            return alpha && am == 0xFF000000 ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
        if (bits == 16 && rm == 0xF800 && gm == 0x07E0 && bm == 0x001F)
            return D3DFMT_R5G6B5;
        if (bits == 16 && rm == 0x7C00 && gm == 0x03E0 && bm == 0x001F)
            return alpha ? D3DFMT_A1R5G5B5 : D3DFMT_X1R5G5B5;
        if (bits == 16 && rm == 0x0F00 && gm == 0x00F0 && bm == 0x000F)
            return alpha ? D3DFMT_A4R4G4B4 : D3DFMT_X4R4G4B4;
        if (bits == 24 && rm == 0xFF0000 && gm == 0x00FF00 && bm == 0x0000FF)
            return D3DFMT_R8G8B8;
        return D3DFMT_UNKNOWN;
    }

    inline BYTE Expand(DWORD v, DWORD mask)
    {
        if (!mask) return 0xFF;
        DWORD shift = 0, bits = 0;
        while (!((mask >> shift) & 1)) shift++;
        DWORD m = mask >> shift;
        while (m & 1) { bits++; m >>= 1; }
        DWORD x = (v & mask) >> shift;
        return (BYTE) (bits >= 8 ? x >> (bits - 8) : (x * 255) / ((1u << bits) - 1));
    }

    bool DecodeDDS(const BYTE* p, UINT n, SeifukuImage& img)
    {
        if (n < 128 || RdD(p) != 0x20534444 || RdD(p + 4) != 124)
            return false;
        const DWORD flags = RdD(p + 8);
        img.height = RdD(p + 12);
        img.width = RdD(p + 16);
        const DWORD mipCount = (flags & 0x20000) ? RdD(p + 28) : 0;
        const BYTE* pf = p + 76;
        const DWORD pfFlags = RdD(pf + 4), fourCC = RdD(pf + 8), bits = RdD(pf + 12);
        img.fileFormat = D3DXIFF_DDS;
        img.mipLevels = mipCount ? mipCount : 1;
        if (pfFlags & 0x4)
        {
            UINT blockBytes;
            switch (fourCC)
            {
            case D3DFMT_DXT1: blockBytes = 8; break;
            case D3DFMT_DXT2: case D3DFMT_DXT3: case D3DFMT_DXT4: case D3DFMT_DXT5: blockBytes = 16; break;
            default: return false;
            }
            img.format = (D3DFORMAT) fourCC;
            img.compressed = true;
            size_t off = 128;
            UINT w = img.width, h = img.height;
            for (UINT i = 0; i < img.mipLevels; i++)
            {
                size_t sz = DxtLevelSize(w, h, blockBytes);
                if (off + sz > n) { img.mipLevels = i; break; }
                img.levelOffset.push_back(off);
                img.levelSize.push_back(sz);
                off += sz;
                w = w > 1 ? w / 2 : 1;
                h = h > 1 ? h / 2 : 1;
            }
            if (img.mipLevels == 0) return false;
            img.blocks.assign(p + 128, p + off);
            for (size_t i = 0; i < img.levelOffset.size(); i++) img.levelOffset[i] -= 128;
            return true;
        }
        if (!(pfFlags & 0x40))
            return false;
        const DWORD rm = RdD(pf + 16), gm = RdD(pf + 20), bm = RdD(pf + 24), am = RdD(pf + 28);
        img.format = MaskedFormat(bits, pfFlags, rm, gm, bm, am);
        if (img.format == D3DFMT_UNKNOWN) return false;
        const UINT bpp = bits / 8, pitch = img.width * bpp;
        if (128 + (size_t) pitch * img.height > n) return false;
        img.mipLevels = 1;
        img.argb.resize((size_t) img.width * img.height);
        const bool hasA = (pfFlags & 0x1) && am;
        for (UINT y = 0; y < img.height; y++)
            for (UINT x = 0; x < img.width; x++)
            {
                const BYTE* s = p + 128 + (size_t) y * pitch + (size_t) x * bpp;
                DWORD v = 0;
                for (UINT k = 0; k < bpp; k++) v |= (DWORD) s[k] << (8 * k);
                BYTE a = hasA ? Expand(v, am) : 0xFF;
                img.argb[(size_t) y * img.width + x] = ((DWORD) a << 24) | ((DWORD) Expand(v, rm) << 16) | ((DWORD) Expand(v, gm) << 8) | Expand(v, bm);
            }
        return true;
    }

    bool DecodeTGA(const BYTE* p, UINT n, SeifukuImage& img)
    {
        if (n < 18) return false;
        const BYTE idLen = p[0], cmapType = p[1], type = p[2], bpp = p[16], desc = p[17];
        if (cmapType != 0 || (type != 2 && type != 10) || (bpp != 24 && bpp != 32))
            return false;
        img.width = RdW(p + 12);
        img.height = RdW(p + 14);
        if (!img.width || !img.height) return false;
        img.fileFormat = D3DXIFF_TGA;
        img.format = bpp == 32 ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
        img.argb.resize((size_t) img.width * img.height);
        const bool topDown = (desc & 0x20) != 0;
        const UINT bytes = bpp / 8;
        const BYTE* s = p + 18 + idLen;
        const BYTE* end = p + n;
        size_t count = (size_t) img.width * img.height, i = 0;
        while (i < count)
        {
            UINT run = 1; bool rle = false;
            if (type == 10)
            {
                if (s >= end) return false;
                BYTE hdr = *s++;
                run = (hdr & 0x7F) + 1;
                rle = (hdr & 0x80) != 0;
            }
            DWORD px = 0;
            for (UINT r = 0; r < run && i < count; r++)
            {
                if (r == 0 || !rle)
                {
                    if (s + bytes > end) return false;
                    px = ((DWORD) (bytes == 4 ? s[3] : 0xFF) << 24) | ((DWORD) s[2] << 16) | ((DWORD) s[1] << 8) | s[0];
                    s += bytes;
                }
                UINT x = (UINT) (i % img.width), y = (UINT) (i / img.width);
                if (!topDown) y = img.height - 1 - y;
                img.argb[(size_t) y * img.width + x] = px;
                i++;
            }
        }
        return true;
    }

    bool DecodeBMP(const BYTE* p, UINT n, SeifukuImage& img)
    {
        if (n < 54 || p[0] != 'B' || p[1] != 'M') return false;
        const DWORD dataOff = RdD(p + 10), hdrSize = RdD(p + 14);
        if (hdrSize < 40) return false;
        const int w = (int) RdD(p + 18), hRaw = (int) RdD(p + 22);
        const WORD bpp = RdW(p + 28);
        const DWORD comp = RdD(p + 30);
        DWORD clrUsed = RdD(p + 46);
        if (w <= 0 || hRaw == 0 || comp != 0 || (bpp != 8 && bpp != 24 && bpp != 32))
            return false;
        const bool bottomUp = hRaw > 0;
        const UINT h = (UINT) (bottomUp ? hRaw : -hRaw);
        img.width = (UINT) w; img.height = h;
        img.fileFormat = D3DXIFF_BMP;
        img.format = D3DFMT_X8R8G8B8;
        const BYTE* pal = p + 14 + hdrSize;
        if (bpp == 8 && clrUsed == 0) clrUsed = 256;
        const UINT pitch = ((UINT) w * bpp / 8 + 3) & ~3u;
        if (dataOff + (size_t) pitch * h > n || (bpp == 8 && 14 + hdrSize + clrUsed * 4 > dataOff)) return false;
        img.argb.resize((size_t) w * h);
        for (UINT y = 0; y < h; y++)
        {
            const BYTE* row = p + dataOff + (size_t) (bottomUp ? h - 1 - y : y) * pitch;
            for (UINT x = 0; x < (UINT) w; x++)
            {
                DWORD px;
                if (bpp == 8)
                {
                    const BYTE* e = pal + (size_t) row[x] * 4;
                    px = row[x] < clrUsed ? (0xFF000000 | ((DWORD) e[2] << 16) | ((DWORD) e[1] << 8) | e[0]) : 0xFF000000;
                }
                else
                {
                    const BYTE* s = row + (size_t) x * bpp / 8;
                    px = 0xFF000000 | ((DWORD) s[2] << 16) | ((DWORD) s[1] << 8) | s[0];
                }
                img.argb[(size_t) y * w + x] = px;
            }
        }
        return true;
    }

    struct ComScope
    {
        HRESULT hr;
        ComScope() { hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); }
        ~ComScope() { if (SUCCEEDED(hr)) CoUninitialize(); }
    };

    template <class T> struct ComPtr
    {
        T* p;
        ComPtr() : p(0) {}
        ~ComPtr() { if (p) p->Release(); }
        T** operator&() { return &p; }
        T* operator->() { return p; }
    };

    bool DecodeWIC(const BYTE* p, UINT n, SeifukuImage& img)
    {
        ComScope com;
        ComPtr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**) &factory)))
            return false;
        ComPtr<IWICStream> stream;
        if (FAILED(factory->CreateStream(&stream)) || FAILED(stream->InitializeFromMemory((BYTE*) p, n)))
            return false;
        ComPtr<IWICBitmapDecoder> decoder;
        if (FAILED(factory->CreateDecoderFromStream(stream.p, NULL, WICDecodeMetadataCacheOnDemand, &decoder)))
            return false;
        GUID container;
        img.fileFormat = D3DXIFF_FORCE_DWORD;
        if (SUCCEEDED(decoder->GetContainerFormat(&container)))
        {
            if (container == GUID_ContainerFormatJpeg) img.fileFormat = D3DXIFF_JPG;
            else if (container == GUID_ContainerFormatPng) img.fileFormat = D3DXIFF_PNG;
            else if (container == GUID_ContainerFormatBmp) img.fileFormat = D3DXIFF_BMP;
        }
        ComPtr<IWICBitmapFrameDecode> frame;
        if (FAILED(decoder->GetFrame(0, &frame))) return false;
        ComPtr<IWICFormatConverter> conv;
        if (FAILED(factory->CreateFormatConverter(&conv))) return false;
        if (FAILED(conv->Initialize(frame.p, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom)))
            return false;
        if (FAILED(conv->GetSize(&img.width, &img.height)) || !img.width || !img.height) return false;
        img.argb.resize((size_t) img.width * img.height);
        if (FAILED(conv->CopyPixels(NULL, img.width * 4, img.width * img.height * 4, (BYTE*) &img.argb[0])))
            return false;
        bool anyAlpha = false;
        for (size_t i = 0; i < img.argb.size() && !anyAlpha; i++) anyAlpha = (img.argb[i] >> 24) != 0xFF;
        img.format = anyAlpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8;
        return true;
    }

    bool DecodeImage(const BYTE* p, UINT n, SeifukuImage& img)
    {
        if (n >= 4 && RdD(p) == 0x20534444) return DecodeDDS(p, n, img);
        if (n >= 2 && p[0] == 'B' && p[1] == 'M') return DecodeBMP(p, n, img);
        if (n >= 18 && p[1] == 0 && (p[2] == 2 || p[2] == 10)) return DecodeTGA(p, n, img);
        return DecodeWIC(p, n, img);
    }

    inline bool HasAlpha(D3DFORMAT f)
    {
        return f == D3DFMT_A8R8G8B8 || f == D3DFMT_A1R5G5B5 || f == D3DFMT_A4R4G4B4 || f == D3DFMT_A8 ||
               f == D3DFMT_A8R3G3B2 || f == D3DFMT_DXT1 || f == D3DFMT_DXT2 || f == D3DFMT_DXT3 || f == D3DFMT_DXT4 || f == D3DFMT_DXT5;
    }

    inline bool PutPixel(BYTE* d, D3DFORMAT f, DWORD px)
    {
        const DWORD a = px >> 24, r = (px >> 16) & 0xFF, g = (px >> 8) & 0xFF, b = px & 0xFF;
        switch (f)
        {
        case D3DFMT_A8R8G8B8: *(DWORD*) d = px; return true;
        case D3DFMT_X8R8G8B8: *(DWORD*) d = px | 0xFF000000; return true;
        case D3DFMT_R5G6B5:   *(WORD*) d = (WORD) (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)); return true;
        case D3DFMT_X1R5G5B5: *(WORD*) d = (WORD) (0x8000 | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)); return true;
        case D3DFMT_A1R5G5B5: *(WORD*) d = (WORD) (((a >> 7) << 15) | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)); return true;
        case D3DFMT_A4R4G4B4: *(WORD*) d = (WORD) (((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4)); return true;
        default: return false;
        }
    }

    inline UINT BytesPerPixel(D3DFORMAT f)
    {
        switch (f)
        {
        case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: return 4;
        case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A4R4G4B4: return 2;
        default: return 0;
        }
    }

    inline DWORD Rgb565(WORD v)
    {
        DWORD r = (v >> 11) & 31, g = (v >> 5) & 63, b = v & 31;
        return 0xFF000000 | ((r * 255 / 31) << 16) | ((g * 255 / 63) << 8) | (b * 255 / 31);
    }
    inline DWORD MixRgb(DWORD a, DWORD b, DWORD wa, DWORD wb)
    {
        DWORD d = wa + wb, out = 0xFF000000;
        for (int s = 0; s < 24; s += 8)
            out |= ((((a >> s) & 0xFF) * wa + ((b >> s) & 0xFF) * wb) / d) << s;
        return out;
    }
    void DecodeDxtBlock(const BYTE* b, D3DFORMAT fmt, DWORD out[16])
    {
        const BYTE* c = fmt == D3DFMT_DXT1 ? b : b + 8;
        const WORD c0 = RdW(c), c1 = RdW(c + 2);
        const DWORD idx = RdD(c + 4);
        DWORD col[4];
        col[0] = Rgb565(c0);
        col[1] = Rgb565(c1);
        bool threeColour = fmt == D3DFMT_DXT1 && c0 <= c1;
        if (threeColour) { col[2] = MixRgb(col[0], col[1], 1, 1); col[3] = 0; }
        else { col[2] = MixRgb(col[0], col[1], 2, 1); col[3] = MixRgb(col[0], col[1], 1, 2); }
        BYTE alpha[16];
        if (fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3)
            for (int i = 0; i < 16; i++) alpha[i] = (BYTE) (((b[i / 2] >> ((i & 1) * 4)) & 0xF) * 17);
        else if (fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5)
        {
            BYTE a[8];
            a[0] = b[0]; a[1] = b[1];
            if (a[0] > a[1]) for (int i = 1; i < 7; i++) a[i + 1] = (BYTE) (((7 - i) * a[0] + i * a[1]) / 7);
            else { for (int i = 1; i < 5; i++) a[i + 1] = (BYTE) (((5 - i) * a[0] + i * a[1]) / 5); a[6] = 0; a[7] = 255; }
            unsigned __int64 bits = 0;
            for (int i = 0; i < 6; i++) bits |= (unsigned __int64) b[2 + i] << (8 * i);
            for (int i = 0; i < 16; i++) alpha[i] = a[(bits >> (3 * i)) & 7];
        }
        else
            for (int i = 0; i < 16; i++) alpha[i] = 0xFF;
        for (int i = 0; i < 16; i++)
        {
            DWORD k = (idx >> (2 * i)) & 3;
            out[i] = (col[k] & 0x00FFFFFF) | ((DWORD) (threeColour && k == 3 ? 0 : alpha[i]) << 24);
        }
    }
    void DecompressToArgb(SeifukuImage& img)
    {
        const UINT blockBytes = img.format == D3DFMT_DXT1 ? 8 : 16;
        const UINT bw = (img.width + 3) / 4, bh = (img.height + 3) / 4;
        img.argb.assign((size_t) img.width * img.height, 0);
        const BYTE* base = &img.blocks[img.levelOffset[0]];
        for (UINT by = 0; by < bh; by++)
            for (UINT bx = 0; bx < bw; bx++)
            {
                DWORD px[16];
                DecodeDxtBlock(base + ((size_t) by * bw + bx) * blockBytes, img.format, px);
                for (UINT y = 0; y < 4; y++)
                    for (UINT x = 0; x < 4; x++)
                    {
                        UINT X = bx * 4 + x, Y = by * 4 + y;
                        if (X < img.width && Y < img.height) img.argb[(size_t) Y * img.width + X] = px[y * 4 + x];
                    }
            }
        img.compressed = false;
        img.format = D3DFMT_A8R8G8B8;
        img.mipLevels = 1;
        img.blocks.clear();
        img.levelOffset.clear();
        img.levelSize.clear();
    }
    void BoxDown(const std::vector<DWORD>& src, UINT sw, UINT sh, std::vector<DWORD>& dst, UINT dw, UINT dh)
    {
        dst.resize((size_t) dw * dh);
        for (UINT y = 0; y < dh; y++)
            for (UINT x = 0; x < dw; x++)
            {
                UINT x0 = x * 2, y0 = y * 2, x1 = x0 + 1 < sw ? x0 + 1 : x0, y1 = y0 + 1 < sh ? y0 + 1 : y0;
                DWORD c[4] = { src[(size_t) y0 * sw + x0], src[(size_t) y0 * sw + x1], src[(size_t) y1 * sw + x0], src[(size_t) y1 * sw + x1] };
                DWORD acc[4] = { 0, 0, 0, 0 };
                for (int k = 0; k < 4; k++) { acc[0] += c[k] >> 24; acc[1] += (c[k] >> 16) & 0xFF; acc[2] += (c[k] >> 8) & 0xFF; acc[3] += c[k] & 0xFF; }
                dst[(size_t) y * dw + x] = ((acc[0] / 4) << 24) | ((acc[1] / 4) << 16) | ((acc[2] / 4) << 8) | (acc[3] / 4);
            }
    }

    HRESULT CreateFromImage(LPDIRECT3DDEVICE9 pDevice, SeifukuImage& img, UINT Width, UINT Height, UINT MipLevels, DWORD Usage,
                            D3DFORMAT Format, D3DPOOL Pool, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, LPDIRECT3DTEXTURE9* ppTexture)
    {
        if ((Width != 0 && Width != D3DX_DEFAULT && Width != img.width) || (Height != 0 && Height != D3DX_DEFAULT && Height != img.height))
            return E_NOTIMPL;
        if (pSrcInfo)
        {
            pSrcInfo->Width = img.width; pSrcInfo->Height = img.height; pSrcInfo->Depth = 1;
            pSrcInfo->MipLevels = img.mipLevels; pSrcInfo->Format = img.format;
            pSrcInfo->ResourceType = D3DRTYPE_TEXTURE; pSrcInfo->ImageFileFormat = img.fileFormat;
        }
        UINT full = 1;
        for (UINT w = img.width, h = img.height; w > 1 || h > 1; w = w > 1 ? w / 2 : 1, h = h > 1 ? h / 2 : 1) full++;
        UINT levels = (MipLevels == 0 || MipLevels == D3DX_DEFAULT) ? full : MipLevels;
        if (levels > full) levels = full;
        if (img.compressed && levels > img.mipLevels && (Format == D3DFMT_UNKNOWN || Format == img.format))
            DecompressToArgb(img);

        if (img.compressed)
        {
            if (Format != D3DFMT_UNKNOWN && Format != img.format)
                return E_NOTIMPL;
            LPDIRECT3DTEXTURE9 tex = 0;
            HRESULT hr = pDevice->CreateTexture(img.width, img.height, levels, Usage, img.format, Pool, &tex, NULL);
            if (FAILED(hr)) return hr;
            const UINT blockBytes = img.format == D3DFMT_DXT1 ? 8 : 16;
            UINT avail = levels < img.mipLevels ? levels : img.mipLevels;
            for (UINT i = 0; i < avail; i++)
            {
                D3DLOCKED_RECT lr;
                if (FAILED(tex->LockRect(i, &lr, NULL, 0))) { tex->Release(); return E_FAIL; }
                UINT w = img.width >> i, h = img.height >> i;
                if (!w) w = 1;
                if (!h) h = 1;
                UINT rows = (h + 3) / 4, rowBytes = ((w + 3) / 4) * blockBytes;
                const BYTE* s = &img.blocks[img.levelOffset[i]];
                for (UINT r = 0; r < rows; r++)
                    memcpy((BYTE*) lr.pBits + (size_t) r * lr.Pitch, s + (size_t) r * rowBytes, rowBytes);
                tex->UnlockRect(i);
            }
            *ppTexture = tex;
            return D3D_OK;
        }

        D3DFORMAT fmt = Format == D3DFMT_UNKNOWN ? img.format : Format;
        if (fmt == D3DFMT_R8G8B8) fmt = D3DFMT_X8R8G8B8;
        if (ColorKey && !HasAlpha(fmt)) fmt = D3DFMT_A8R8G8B8;
        if (!BytesPerPixel(fmt)) return E_NOTIMPL;
        if (ColorKey)
            for (size_t i = 0; i < img.argb.size(); i++)
                if (img.argb[i] == ColorKey) img.argb[i] = 0;

        LPDIRECT3DTEXTURE9 tex = 0;
        HRESULT hr = pDevice->CreateTexture(img.width, img.height, levels, Usage, fmt, Pool, &tex, NULL);
        if (FAILED(hr)) return hr;
        std::vector<DWORD> cur(img.argb), next;
        UINT w = img.width, h = img.height;
        for (UINT i = 0; i < levels; i++)
        {
            if (i > 0)
            {
                UINT nw = w > 1 ? w / 2 : 1, nh = h > 1 ? h / 2 : 1;
                BoxDown(cur, w, h, next, nw, nh);
                cur.swap(next);
                w = nw; h = nh;
            }
            D3DLOCKED_RECT lr;
            if (FAILED(tex->LockRect(i, &lr, NULL, 0))) { tex->Release(); return E_FAIL; }
            const UINT bpp = BytesPerPixel(fmt);
            for (UINT y = 0; y < h; y++)
                for (UINT x = 0; x < w; x++)
                    PutPixel((BYTE*) lr.pBits + (size_t) y * lr.Pitch + (size_t) x * bpp, fmt, cur[(size_t) y * w + x]);
            tex->UnlockRect(i);
        }
        *ppTexture = tex;
        return D3D_OK;
    }

    struct FormatRecord
    {
        DWORD fmt;
        DWORD cls;
        DWORD bpp;
        DWORD c[5];
        DWORD subst;
    };
    const FormatRecord s_fmtDefault = { 0, 0, 32, { 0, 8, 8, 8, 8 }, 0 };
    const FormatRecord s_fmtTable[] =
    {
        { 0x14, 0, 24, { 0, 0, 8, 8, 8 }, 0 },
        { 0x15, 0, 32, { 0, 8, 8, 8, 8 }, 1 },
        { 0x16, 0, 32, { 0, 0, 8, 8, 8 }, 1 },
        { 0x17, 0, 16, { 0, 0, 5, 6, 5 }, 1 },
        { 0x18, 0, 16, { 0, 0, 5, 5, 5 }, 1 },
        { 0x28, 1, 16, { 0, 8, 8, 8, 8 }, 1 },
        { 0x29, 1,  8, { 0, 8, 8, 8, 8 }, 1 },
        { 0x19, 0, 16, { 0, 1, 5, 5, 5 }, 1 },
        { 0x1A, 0, 16, { 0, 4, 4, 4, 4 }, 1 },
        { 0x1B, 0,  8, { 0, 0, 3, 3, 2 }, 1 },
        { 0x1C, 0,  8, { 0, 8, 0, 0, 0 }, 1 },
        { 0x1D, 0, 16, { 0, 8, 3, 3, 2 }, 1 },
        { 0x1E, 0, 16, { 0, 0, 4, 4, 4 }, 1 },
        { 0x1F, 0, 32, { 0, 2, 10, 10, 10 }, 1 },
        { 0x22, 0, 32, { 0, 0, 16, 16, 0 }, 1 },
        { 0x32, 2,  8, { 0, 0, 8, 8, 8 }, 1 },
        { 0x33, 2, 16, { 0, 8, 8, 8, 8 }, 1 },
        { 0x34, 2,  8, { 0, 4, 4, 4, 4 }, 1 },
        { 0x3C, 3, 16, { 0, 0, 0, 8, 8 }, 1 },
        { 0x3D, 3, 16, { 6, 0, 0, 5, 5 }, 1 },
        { 0x3E, 3, 32, { 8, 0, 0, 8, 8 }, 1 },
        { 0x3F, 3, 32, { 0, 8, 8, 8, 8 }, 1 },
        { 0x40, 3, 32, { 0, 0, 0, 16, 16 }, 1 },
        { 0x41, 3, 32, { 0, 0, 11, 11, 10 }, 1 },
        { 0x43, 3, 32, { 2, 0, 10, 10, 10 }, 1 },
        { MAKEFOURCC('U', 'Y', 'V', 'Y'), 0, 16, { 0, 0, 8, 8, 8 }, 0 },
        { MAKEFOURCC('Y', 'U', 'Y', '2'), 0, 16, { 0, 0, 8, 8, 8 }, 0 },
        { MAKEFOURCC('D', 'X', 'T', '1'), 0, 16, { 0, 1, 5, 6, 5 }, 0 },
        { MAKEFOURCC('D', 'X', 'T', '2'), 0, 16, { 0, 4, 5, 6, 5 }, 0 },
        { MAKEFOURCC('D', 'X', 'T', '3'), 0, 16, { 0, 4, 5, 6, 5 }, 0 },
        { MAKEFOURCC('D', 'X', 'T', '4'), 0, 16, { 0, 8, 5, 6, 5 }, 0 },
        { MAKEFOURCC('D', 'X', 'T', '5'), 0, 16, { 0, 8, 5, 6, 5 }, 0 },
        { 0x46, 4, 16, { 1, 0, 0, 0, 16 }, 1 },
        { 0x47, 4, 32, { 0, 0, 0, 0, 32 }, 1 },
        { 0x49, 4, 16, { 0, 0, 0, 1, 15 }, 1 },
        { 0x4B, 4, 32, { 0, 0, 0, 8, 24 }, 1 },
        { 0x50, 4, 16, { 0, 0, 0, 0, 16 }, 1 },
        { 0x4D, 4, 32, { 0, 0, 0, 0, 24 }, 1 },
        { 0x4F, 4, 32, { 0, 0, 0, 4, 24 }, 1 },
        { MAKEFOURCC(' ', 'L', '1', '6'), 2, 16, { 0, 0, 16, 16, 16 }, 0 },
        { MAKEFOURCC('A', 'L', '1', '6'), 2, 32, { 0, 16, 16, 16, 16 }, 0 },
        { MAKEFOURCC(' ', 'R', '1', '6'), 0, 48, { 0, 0, 16, 16, 16 }, 0 },
        { MAKEFOURCC('A', 'R', '1', '6'), 0, 64, { 0, 16, 16, 16, 16 }, 0 },
    };
    const DWORD s_fmtClassMatrix[25] =
    {
        1, 0, 0, 0, 0,
        1, 1, 0, 0, 0,
        1, 0, 1, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 0, 1,
    };

    const FormatRecord* FindFormatRecord(DWORD fmt)
    {
        for (size_t i = 0; i < sizeof(s_fmtTable) / sizeof(s_fmtTable[0]); i++)
            if (s_fmtTable[i].fmt == fmt)
                return &s_fmtTable[i];
        return &s_fmtDefault;
    }

    DWORD ScoreFormat(const FormatRecord& req, const FormatRecord& cand)
    {
        if (!s_fmtClassMatrix[req.cls * 5 + cand.cls])
            return (DWORD) -1;
        DWORD key = 0, present = 0;
        for (int i = 0; i < 5; i++)
        {
            DWORD r = req.c[i], c = cand.c[i];
            if (r > 0)
                present++;
            if (r < c)
                key += (r == 0) ? 0x100 : (c - r);
            else if (r > c)
                key += (c == 0) ? 0x1000000 : ((r - c) << 16);
        }
        if (present == (key >> 24))
            return (DWORD) -1;
        return key;
    }

    DWORD PickFormat(LPDIRECT3DDEVICE9 pDevice, DWORD usage, D3DRESOURCETYPE rtype, const FormatRecord& req)
    {
        LPDIRECT3D9 pD3D = NULL;
        D3DCAPS9 caps;
        D3DDISPLAYMODE mode;
        ZeroMemory(&caps, sizeof(caps));
        ZeroMemory(&mode, sizeof(mode));
        if (pDevice)
        {
            pDevice->GetDirect3D(&pD3D);
            pDevice->GetDeviceCaps(&caps);
            pDevice->GetDisplayMode(0, &mode);
        }
        DWORD best = (DWORD) -1;
        const FormatRecord* pick = &s_fmtDefault;
        for (size_t i = 0; i < sizeof(s_fmtTable) / sizeof(s_fmtTable[0]); i++)
        {
            const FormatRecord& r = s_fmtTable[i];
            if (r.fmt == 0)
                continue;
            if (pDevice && (!pD3D || FAILED(pD3D->CheckDeviceFormat(caps.AdapterOrdinal, caps.DeviceType, mode.Format, usage, rtype,
                                                                    (D3DFORMAT) r.fmt))))
                continue;
            if (r.fmt == req.fmt)
            {
                pick = &r;
                break;
            }
            if (!r.subst)
                continue;
            DWORD s = ScoreFormat(req, r);
            if (s == (DWORD) -1 || s > best)
                continue;
            if (s == best && r.bpp >= pick->bpp)
                continue;
            best = s;
            pick = &r;
        }
        if (pD3D)
            pD3D->Release();
        return pick->fmt;
    }

    UINT NextPow2(UINT v)
    {
        UINT p = 1;
        while (p < v)
            p <<= 1;
        return p;
    }

    UINT BitLength(UINT v)
    {
        UINT n = 0;
        while (v)
        {
            n++;
            v >>= 1;
        }
        return n;
    }

    HRESULT CheckTextureRequirements(LPDIRECT3DDEVICE9 pDevice, UINT* pWidth, UINT* pHeight, UINT* pMipLevels, DWORD usage,
                                     D3DFORMAT* pFormat, D3DPOOL pool)
    {
        if (!pDevice)
            return D3DERR_INVALIDCALL;
        if (usage == (DWORD) -1)
            usage = 0;
        if (usage & ~0x20Bu)
            return D3DERR_INVALIDCALL;
        if ((DWORD) pool > 3)
            return D3DERR_INVALIDCALL;

        DWORD fmt = pFormat ? (DWORD) *pFormat : 0;
        if (pool == D3DPOOL_SCRATCH)
        {
            if (fmt == 0)
                fmt = D3DFMT_A8R8G8B8;
        }
        else
        {
            fmt = PickFormat(pDevice, usage, D3DRTYPE_TEXTURE, *FindFormatRecord(fmt));
            if (fmt == 0)
                return D3DERR_NOTAVAILABLE;
        }

        UINT w = pWidth ? *pWidth : (UINT) -1;
        UINT h = pHeight ? *pHeight : (UINT) -1;
        if (w == (UINT) -1 && h == (UINT) -1)
            w = h = 256;
        else
        {
            if (w == (UINT) -1)
                w = h;
            if (h == (UINT) -1)
                h = w;
            if (w == 0)
                w = 1;
            if (h == 0)
                h = 1;
        }

        D3DCAPS9 caps;
        ZeroMemory(&caps, sizeof(caps));
        if (pool != D3DPOOL_SCRATCH)
        {
            pDevice->GetDeviceCaps(&caps);
            if (w > caps.MaxTextureWidth)
                w = caps.MaxTextureWidth;
            if (h > caps.MaxTextureHeight)
                h = caps.MaxTextureHeight;
            UINT a = (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) ? 1 : caps.MaxTextureAspectRatio;
            if (a)
            {
                if (w > a * h)
                    w = a * h;
                if (h > a * w)
                    h = a * w;
            }
            if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
            {
                w = NextPow2(w);
                h = NextPow2(h);
            }
        }
        if (fmt == D3DFMT_DXT1 || fmt == D3DFMT_DXT2 || fmt == D3DFMT_DXT3 || fmt == D3DFMT_DXT4 || fmt == D3DFMT_DXT5)
        {
            w = (w + 3) & ~3u;
            h = (h + 3) & ~3u;
        }

        UINT m = pMipLevels ? *pMipLevels : (UINT) -1;
        if (pool != D3DPOOL_SCRATCH && !(caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP))
            m = 1;
        else if (pool != D3DPOOL_SCRATCH && (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) && ((w & (w - 1)) || (h & (h - 1))))
            m = 1;
        else
        {
            UINT full = BitLength(w) > BitLength(h) ? BitLength(w) : BitLength(h);
            if (m > full || m == 0)
                m = full;
        }

        if (pWidth)
            *pWidth = w;
        if (pHeight)
            *pHeight = h;
        if (pMipLevels)
            *pMipLevels = m;
        if (pFormat)
            *pFormat = (D3DFORMAT) fmt;
        return D3D_OK;
    }
}

HRESULT WINAPI D3DXCreateTexture(LPDIRECT3DDEVICE9 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage,
                                 D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9* ppTexture)
{
    if (!pDevice || !ppTexture)
        return D3DERR_INVALIDCALL;
    if (Usage == (DWORD) -1)
        Usage = 0;
    HRESULT hr = CheckTextureRequirements(pDevice, &Width, &Height, &MipLevels, Usage, &Format, Pool);
    if (FAILED(hr))
        return hr;
    hr = pDevice->CreateTexture(Width, Height, MipLevels, Usage, Format, Pool, ppTexture, NULL);
    if (FAILED(hr))
        return hr;
    return D3D_OK;
}

HRESULT WINAPI D3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize,
                                                   UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format,
                                                   D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey,
                                                   D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                   LPDIRECT3DTEXTURE9* ppTexture)
{
    (void) Filter; (void) MipFilter; (void) pPalette;
    if (!pDevice || !pSrcData || !SrcDataSize || !ppTexture)
        return D3DERR_INVALIDCALL;
    SeifukuImage img;
    if (!DecodeImage((const BYTE*) pSrcData, SrcDataSize, img))
        return D3DXERR_INVALIDDATA;
    return CreateFromImage(pDevice, img, Width, Height, MipLevels, Usage, Format, Pool, ColorKey, pSrcInfo, ppTexture);
}

HRESULT WINAPI D3DXCreateTextureFromFileExA(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, UINT Width, UINT Height,
                                            UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter,
                                            DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
                                            PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture)
{
    if (!pSrcFile)
        return D3DERR_INVALIDCALL;
    FILE* f = fopen(pSrcFile, "rb");
    if (!f)
        return D3DXERR_INVALIDDATA;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return D3DXERR_INVALIDDATA; }
    std::vector<BYTE> buf((size_t) len);
    size_t got = fread(&buf[0], 1, (size_t) len, f);
    fclose(f);
    if (got != (size_t) len)
        return D3DXERR_INVALIDDATA;
    return D3DXCreateTextureFromFileInMemoryEx(pDevice, &buf[0], (UINT) len, Width, Height, MipLevels, Usage, Format, Pool,
                                               Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
}
