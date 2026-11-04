#ifndef SFK_D3DX8COMPAT_H
#define SFK_D3DX8COMPAT_H

#include <windows.h>
#include <d3d9.h>
#include <d3d9types.h>
#include <DirectXMath.h>
#include <math.h>

#define D3DX_PI    ((FLOAT) 3.141592654f)
#define D3DX_1BYPI ((FLOAT) 0.318309886f)

#define D3DXToRadian(degree) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree(radian) ((radian) * (180.0f / D3DX_PI))

typedef struct D3DXVECTOR2
{
    D3DXVECTOR2() {}
    D3DXVECTOR2(const float* pf) { x = pf[0]; y = pf[1]; }
    D3DXVECTOR2(float fx, float fy) { x = fx; y = fy; }

    operator float* () { return &x; }
    operator const float* () const { return &x; }

    D3DXVECTOR2& operator += (const D3DXVECTOR2& v) { x += v.x; y += v.y; return *this; }
    D3DXVECTOR2& operator -= (const D3DXVECTOR2& v) { x -= v.x; y -= v.y; return *this; }
    D3DXVECTOR2& operator *= (float f) { x *= f; y *= f; return *this; }
    D3DXVECTOR2& operator /= (float f) { float g = 1.0f / f; x *= g; y *= g; return *this; }

    D3DXVECTOR2 operator + () const { return *this; }
    D3DXVECTOR2 operator - () const { return D3DXVECTOR2(-x, -y); }

    D3DXVECTOR2 operator + (const D3DXVECTOR2& v) const { return D3DXVECTOR2(x + v.x, y + v.y); }
    D3DXVECTOR2 operator - (const D3DXVECTOR2& v) const { return D3DXVECTOR2(x - v.x, y - v.y); }
    D3DXVECTOR2 operator * (float f) const { return D3DXVECTOR2(x * f, y * f); }
    D3DXVECTOR2 operator / (float f) const { float g = 1.0f / f; return D3DXVECTOR2(x * g, y * g); }

    friend D3DXVECTOR2 operator * (float f, const D3DXVECTOR2& v) { return D3DXVECTOR2(f * v.x, f * v.y); }

    BOOL operator == (const D3DXVECTOR2& v) const { return x == v.x && y == v.y; }
    BOOL operator != (const D3DXVECTOR2& v) const { return x != v.x || y != v.y; }

    float x, y;
} D3DXVECTOR2, *LPD3DXVECTOR2;

typedef struct D3DXVECTOR3 : public D3DVECTOR
{
    D3DXVECTOR3() {}
    D3DXVECTOR3(const float* pf) { x = pf[0]; y = pf[1]; z = pf[2]; }
    D3DXVECTOR3(const D3DVECTOR& v) { x = v.x; y = v.y; z = v.z; }
    D3DXVECTOR3(float fx, float fy, float fz) { x = fx; y = fy; z = fz; }

    operator float* () { return &x; }
    operator const float* () const { return &x; }

    D3DXVECTOR3& operator += (const D3DXVECTOR3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    D3DXVECTOR3& operator -= (const D3DXVECTOR3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    D3DXVECTOR3& operator *= (float f) { x *= f; y *= f; z *= f; return *this; }
    D3DXVECTOR3& operator /= (float f) { float g = 1.0f / f; x *= g; y *= g; z *= g; return *this; }

    D3DXVECTOR3 operator + () const { return *this; }
    D3DXVECTOR3 operator - () const { return D3DXVECTOR3(-x, -y, -z); }

    D3DXVECTOR3 operator + (const D3DXVECTOR3& v) const { return D3DXVECTOR3(x + v.x, y + v.y, z + v.z); }
    D3DXVECTOR3 operator - (const D3DXVECTOR3& v) const { return D3DXVECTOR3(x - v.x, y - v.y, z - v.z); }
    D3DXVECTOR3 operator * (float f) const { return D3DXVECTOR3(x * f, y * f, z * f); }
    D3DXVECTOR3 operator / (float f) const { float g = 1.0f / f; return D3DXVECTOR3(x * g, y * g, z * g); }

    friend D3DXVECTOR3 operator * (float f, const struct D3DXVECTOR3& v) { return D3DXVECTOR3(f * v.x, f * v.y, f * v.z); }

    BOOL operator == (const D3DXVECTOR3& v) const { return x == v.x && y == v.y && z == v.z; }
    BOOL operator != (const D3DXVECTOR3& v) const { return x != v.x || y != v.y || z != v.z; }
} D3DXVECTOR3, *LPD3DXVECTOR3;

typedef struct D3DXVECTOR4
{
    D3DXVECTOR4() {}
    D3DXVECTOR4(const float* pf) { x = pf[0]; y = pf[1]; z = pf[2]; w = pf[3]; }
    D3DXVECTOR4(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }

    operator float* () { return &x; }
    operator const float* () const { return &x; }

    D3DXVECTOR4& operator += (const D3DXVECTOR4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    D3DXVECTOR4& operator -= (const D3DXVECTOR4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    D3DXVECTOR4& operator *= (float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
    D3DXVECTOR4& operator /= (float f) { float g = 1.0f / f; x *= g; y *= g; z *= g; w *= g; return *this; }

    D3DXVECTOR4 operator + () const { return *this; }
    D3DXVECTOR4 operator - () const { return D3DXVECTOR4(-x, -y, -z, -w); }

    D3DXVECTOR4 operator + (const D3DXVECTOR4& v) const { return D3DXVECTOR4(x + v.x, y + v.y, z + v.z, w + v.w); }
    D3DXVECTOR4 operator - (const D3DXVECTOR4& v) const { return D3DXVECTOR4(x - v.x, y - v.y, z - v.z, w - v.w); }
    D3DXVECTOR4 operator * (float f) const { return D3DXVECTOR4(x * f, y * f, z * f, w * f); }
    D3DXVECTOR4 operator / (float f) const { float g = 1.0f / f; return D3DXVECTOR4(x * g, y * g, z * g, w * g); }

    friend D3DXVECTOR4 operator * (float f, const D3DXVECTOR4& v) { return D3DXVECTOR4(f * v.x, f * v.y, f * v.z, f * v.w); }

    BOOL operator == (const D3DXVECTOR4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    BOOL operator != (const D3DXVECTOR4& v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

    float x, y, z, w;
} D3DXVECTOR4, *LPD3DXVECTOR4;

typedef struct D3DXMATRIX : public D3DMATRIX
{
    D3DXMATRIX() {}
    D3DXMATRIX(const float* pf) { memcpy(&_11, pf, sizeof(D3DMATRIX)); }
    D3DXMATRIX(const D3DMATRIX& mat) { memcpy(&_11, &mat, sizeof(D3DMATRIX)); }
    D3DXMATRIX(float f11, float f12, float f13, float f14,
               float f21, float f22, float f23, float f24,
               float f31, float f32, float f33, float f34,
               float f41, float f42, float f43, float f44)
    {
        _11 = f11; _12 = f12; _13 = f13; _14 = f14;
        _21 = f21; _22 = f22; _23 = f23; _24 = f24;
        _31 = f31; _32 = f32; _33 = f33; _34 = f34;
        _41 = f41; _42 = f42; _43 = f43; _44 = f44;
    }

    float& operator () (UINT iRow, UINT iCol) { return m[iRow][iCol]; }
    float  operator () (UINT iRow, UINT iCol) const { return m[iRow][iCol]; }

    operator float* () { return &_11; }
    operator const float* () const { return &_11; }

    D3DXMATRIX& operator *= (const D3DXMATRIX& mat);
    D3DXMATRIX& operator += (const D3DXMATRIX& mat) { for (int i = 0; i < 16; i++) (&_11)[i] += (&mat._11)[i]; return *this; }
    D3DXMATRIX& operator -= (const D3DXMATRIX& mat) { for (int i = 0; i < 16; i++) (&_11)[i] -= (&mat._11)[i]; return *this; }
    D3DXMATRIX& operator *= (float f) { for (int i = 0; i < 16; i++) (&_11)[i] *= f; return *this; }
    D3DXMATRIX& operator /= (float f) { float g = 1.0f / f; for (int i = 0; i < 16; i++) (&_11)[i] *= g; return *this; }

    D3DXMATRIX operator + () const { return *this; }
    D3DXMATRIX operator - () const { D3DXMATRIX r; for (int i = 0; i < 16; i++) (&r._11)[i] = -(&_11)[i]; return r; }

    D3DXMATRIX operator * (const D3DXMATRIX& mat) const;
    D3DXMATRIX operator + (const D3DXMATRIX& mat) const { D3DXMATRIX r(*this); r += mat; return r; }
    D3DXMATRIX operator - (const D3DXMATRIX& mat) const { D3DXMATRIX r(*this); r -= mat; return r; }
    D3DXMATRIX operator * (float f) const { D3DXMATRIX r(*this); r *= f; return r; }
    D3DXMATRIX operator / (float f) const { D3DXMATRIX r(*this); r /= f; return r; }

    friend D3DXMATRIX operator * (float f, const D3DXMATRIX& mat) { return mat * f; }

    BOOL operator == (const D3DXMATRIX& mat) const { return 0 == memcmp(this, &mat, sizeof(D3DMATRIX)); }
    BOOL operator != (const D3DXMATRIX& mat) const { return 0 != memcmp(this, &mat, sizeof(D3DMATRIX)); }
} D3DXMATRIX, *LPD3DXMATRIX;

typedef struct _D3DXMATRIXA16 : public D3DXMATRIX
{
    _D3DXMATRIXA16() {}
    _D3DXMATRIXA16(const float* pf) : D3DXMATRIX(pf) {}
    _D3DXMATRIXA16(const D3DMATRIX& mat) : D3DXMATRIX(mat) {}
    _D3DXMATRIXA16(float f11, float f12, float f13, float f14,
                   float f21, float f22, float f23, float f24,
                   float f31, float f32, float f33, float f34,
                   float f41, float f42, float f43, float f44)
        : D3DXMATRIX(f11, f12, f13, f14, f21, f22, f23, f24, f31, f32, f33, f34, f41, f42, f43, f44) {}
    _D3DXMATRIXA16& operator = (const D3DXMATRIX& mat) { memcpy(&_11, &mat._11, sizeof(D3DMATRIX)); return *this; }
} D3DXMATRIXA16, *LPD3DXMATRIXA16;

typedef struct D3DXQUATERNION
{
    D3DXQUATERNION() {}
    D3DXQUATERNION(const float* pf) { x = pf[0]; y = pf[1]; z = pf[2]; w = pf[3]; }
    D3DXQUATERNION(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }

    operator float* () { return &x; }
    operator const float* () const { return &x; }

    BOOL operator == (const D3DXQUATERNION& q) const { return x == q.x && y == q.y && z == q.z && w == q.w; }
    BOOL operator != (const D3DXQUATERNION& q) const { return x != q.x || y != q.y || z != q.z || w != q.w; }

    float x, y, z, w;
} D3DXQUATERNION, *LPD3DXQUATERNION;

typedef struct D3DXPLANE
{
    D3DXPLANE() {}
    D3DXPLANE(const float* pf) { a = pf[0]; b = pf[1]; c = pf[2]; d = pf[3]; }
    D3DXPLANE(float fa, float fb, float fc, float fd) { a = fa; b = fb; c = fc; d = fd; }

    operator float* () { return &a; }
    operator const float* () const { return &a; }

    D3DXPLANE operator + () const { return *this; }
    D3DXPLANE operator - () const { return D3DXPLANE(-a, -b, -c, -d); }

    BOOL operator == (const D3DXPLANE& p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }
    BOOL operator != (const D3DXPLANE& p) const { return a != p.a || b != p.b || c != p.c || d != p.d; }

    float a, b, c, d;
} D3DXPLANE, *LPD3DXPLANE;

typedef struct D3DXCOLOR
{
    D3DXCOLOR() {}
    D3DXCOLOR(DWORD argb)
    {
        const float f = 1.0f / 255.0f;
        r = f * (float) (unsigned char) (argb >> 16);
        g = f * (float) (unsigned char) (argb >>  8);
        b = f * (float) (unsigned char) (argb >>  0);
        a = f * (float) (unsigned char) (argb >> 24);
    }
    D3DXCOLOR(const float* pf) { r = pf[0]; g = pf[1]; b = pf[2]; a = pf[3]; }
    D3DXCOLOR(const D3DCOLORVALUE& c) { r = c.r; g = c.g; b = c.b; a = c.a; }
    D3DXCOLOR(float fr, float fg, float fb, float fa) { r = fr; g = fg; b = fb; a = fa; }

    operator DWORD () const
    {
        DWORD dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (DWORD) (r * 255.0f + 0.5f);
        DWORD dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (DWORD) (g * 255.0f + 0.5f);
        DWORD dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (DWORD) (b * 255.0f + 0.5f);
        DWORD dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (DWORD) (a * 255.0f + 0.5f);
        return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
    }
    operator float* () { return &r; }
    operator const float* () const { return &r; }
    operator D3DCOLORVALUE* () { return (D3DCOLORVALUE*) &r; }
    operator const D3DCOLORVALUE* () const { return (const D3DCOLORVALUE*) &r; }
    operator D3DCOLORVALUE& () { return *(D3DCOLORVALUE*) &r; }
    operator const D3DCOLORVALUE& () const { return *(const D3DCOLORVALUE*) &r; }

    D3DXCOLOR& operator += (const D3DXCOLOR& c) { r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
    D3DXCOLOR& operator -= (const D3DXCOLOR& c) { r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
    D3DXCOLOR& operator *= (float f) { r *= f; g *= f; b *= f; a *= f; return *this; }
    D3DXCOLOR& operator /= (float f) { float k = 1.0f / f; r *= k; g *= k; b *= k; a *= k; return *this; }

    D3DXCOLOR operator + () const { return *this; }
    D3DXCOLOR operator - () const { return D3DXCOLOR(-r, -g, -b, -a); }

    D3DXCOLOR operator + (const D3DXCOLOR& c) const { return D3DXCOLOR(r + c.r, g + c.g, b + c.b, a + c.a); }
    D3DXCOLOR operator - (const D3DXCOLOR& c) const { return D3DXCOLOR(r - c.r, g - c.g, b - c.b, a - c.a); }
    D3DXCOLOR operator * (float f) const { return D3DXCOLOR(r * f, g * f, b * f, a * f); }
    D3DXCOLOR operator / (float f) const { float k = 1.0f / f; return D3DXCOLOR(r * k, g * k, b * k, a * k); }

    friend D3DXCOLOR operator * (float f, const D3DXCOLOR& c) { return D3DXCOLOR(f * c.r, f * c.g, f * c.b, f * c.a); }

    BOOL operator == (const D3DXCOLOR& c) const { return r == c.r && g == c.g && b == c.b && a == c.a; }
    BOOL operator != (const D3DXCOLOR& c) const { return r != c.r || g != c.g || b != c.b || a != c.a; }

    float r, g, b, a;
} D3DXCOLOR, *LPD3DXCOLOR;

namespace SeifukuD3DX
{
    inline DirectX::XMMATRIX LoadM(const D3DXMATRIX* pM)
    {
        return DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(pM));
    }
    inline D3DXMATRIX* StoreM(D3DXMATRIX* pOut, DirectX::CXMMATRIX M)
    {
        DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(pOut), M);
        return pOut;
    }
    inline DirectX::XMVECTOR LoadV3(const D3DXVECTOR3* pV)
    {
        return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(pV));
    }
    inline D3DXVECTOR3* StoreV3(D3DXVECTOR3* pOut, DirectX::FXMVECTOR V)
    {
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(pOut), V);
        return pOut;
    }
    inline DirectX::XMVECTOR LoadV4(const float* p)
    {
        return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(p));
    }
    inline void StoreV4(float* p, DirectX::FXMVECTOR V)
    {
        DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(p), V);
    }
}

inline D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX* pOut)
{
    pOut->_12 = pOut->_13 = pOut->_14 = pOut->_21 = pOut->_23 = pOut->_24 =
    pOut->_31 = pOut->_32 = pOut->_34 = pOut->_41 = pOut->_42 = pOut->_43 = 0.0f;
    pOut->_11 = pOut->_22 = pOut->_33 = pOut->_44 = 1.0f;
    return pOut;
}

inline BOOL D3DXMatrixIsIdentity(const D3DXMATRIX* pM)
{
    return pM->_11 == 1.0f && pM->_12 == 0.0f && pM->_13 == 0.0f && pM->_14 == 0.0f &&
           pM->_21 == 0.0f && pM->_22 == 1.0f && pM->_23 == 0.0f && pM->_24 == 0.0f &&
           pM->_31 == 0.0f && pM->_32 == 0.0f && pM->_33 == 1.0f && pM->_34 == 0.0f &&
           pM->_41 == 0.0f && pM->_42 == 0.0f && pM->_43 == 0.0f && pM->_44 == 1.0f;
}

inline D3DXMATRIX* D3DXMatrixMultiply(D3DXMATRIX* pOut, const D3DXMATRIX* pM1, const D3DXMATRIX* pM2)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixMultiply(SeifukuD3DX::LoadM(pM1), SeifukuD3DX::LoadM(pM2)));
}

inline D3DXMATRIX* D3DXMatrixTranspose(D3DXMATRIX* pOut, const D3DXMATRIX* pM)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixTranspose(SeifukuD3DX::LoadM(pM)));
}

inline D3DXMATRIX* D3DXMatrixInverse(D3DXMATRIX* pOut, float* pDeterminant, const D3DXMATRIX* pM)
{
    DirectX::XMVECTOR det;
    DirectX::XMMATRIX inv = DirectX::XMMatrixInverse(&det, SeifukuD3DX::LoadM(pM));
    float fDet = DirectX::XMVectorGetX(det);
    if (pDeterminant)
        *pDeterminant = fDet;
    if (fDet == 0.0f)
        return NULL;
    return SeifukuD3DX::StoreM(pOut, inv);
}

inline D3DXMATRIX* D3DXMatrixScaling(D3DXMATRIX* pOut, float sx, float sy, float sz)
{
    D3DXMatrixIdentity(pOut);
    pOut->_11 = sx; pOut->_22 = sy; pOut->_33 = sz;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixTranslation(D3DXMATRIX* pOut, float x, float y, float z)
{
    D3DXMatrixIdentity(pOut);
    pOut->_41 = x; pOut->_42 = y; pOut->_43 = z;
    return pOut;
}

inline D3DXMATRIX* D3DXMatrixRotationX(D3DXMATRIX* pOut, float Angle)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixRotationX(Angle));
}

inline D3DXMATRIX* D3DXMatrixRotationY(D3DXMATRIX* pOut, float Angle)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixRotationY(Angle));
}

inline D3DXMATRIX* D3DXMatrixRotationZ(D3DXMATRIX* pOut, float Angle)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixRotationZ(Angle));
}

inline D3DXMATRIX* D3DXMatrixRotationAxis(D3DXMATRIX* pOut, const D3DXVECTOR3* pV, float Angle)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixRotationAxis(SeifukuD3DX::LoadV3(pV), Angle));
}

inline D3DXMATRIX* D3DXMatrixRotationQuaternion(D3DXMATRIX* pOut, const D3DXQUATERNION* pQ)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixRotationQuaternion(SeifukuD3DX::LoadV4(&pQ->x)));
}

inline D3DXMATRIX* D3DXMatrixLookAtLH(D3DXMATRIX* pOut, const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt, const D3DXVECTOR3* pUp)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixLookAtLH(SeifukuD3DX::LoadV3(pEye), SeifukuD3DX::LoadV3(pAt), SeifukuD3DX::LoadV3(pUp)));
}

inline D3DXMATRIX* D3DXMatrixPerspectiveFovLH(D3DXMATRIX* pOut, float fovy, float Aspect, float zn, float zf)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixPerspectiveFovLH(fovy, Aspect, zn, zf));
}

inline D3DXMATRIX* D3DXMatrixOrthoLH(D3DXMATRIX* pOut, float w, float h, float zn, float zf)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixOrthographicLH(w, h, zn, zf));
}

inline D3DXMATRIX* D3DXMatrixShadow(D3DXMATRIX* pOut, const D3DXVECTOR4* pLight, const D3DXPLANE* pPlane)
{
    return SeifukuD3DX::StoreM(pOut, DirectX::XMMatrixShadow(SeifukuD3DX::LoadV4(&pPlane->a), SeifukuD3DX::LoadV4(&pLight->x)));
}

inline D3DXMATRIX& D3DXMATRIX::operator *= (const D3DXMATRIX& mat)
{
    D3DXMatrixMultiply(this, this, &mat);
    return *this;
}

inline D3DXMATRIX D3DXMATRIX::operator * (const D3DXMATRIX& mat) const
{
    D3DXMATRIX r;
    D3DXMatrixMultiply(&r, this, &mat);
    return r;
}

inline D3DXVECTOR2* D3DXVec2Add(D3DXVECTOR2* pOut, const D3DXVECTOR2* pV1, const D3DXVECTOR2* pV2)
{
    pOut->x = pV1->x + pV2->x; pOut->y = pV1->y + pV2->y;
    return pOut;
}

inline float D3DXVec3Length(const D3DXVECTOR3* pV)
{
    return sqrtf(pV->x * pV->x + pV->y * pV->y + pV->z * pV->z);
}

inline float D3DXVec3LengthSq(const D3DXVECTOR3* pV)
{
    return pV->x * pV->x + pV->y * pV->y + pV->z * pV->z;
}

inline float D3DXVec3Dot(const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2)
{
    return pV1->x * pV2->x + pV1->y * pV2->y + pV1->z * pV2->z;
}

inline D3DXVECTOR3* D3DXVec3Cross(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2)
{
    D3DXVECTOR3 v;
    v.x = pV1->y * pV2->z - pV1->z * pV2->y;
    v.y = pV1->z * pV2->x - pV1->x * pV2->z;
    v.z = pV1->x * pV2->y - pV1->y * pV2->x;
    *pOut = v;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Add(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2)
{
    pOut->x = pV1->x + pV2->x; pOut->y = pV1->y + pV2->y; pOut->z = pV1->z + pV2->z;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Subtract(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2)
{
    pOut->x = pV1->x - pV2->x; pOut->y = pV1->y - pV2->y; pOut->z = pV1->z - pV2->z;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Scale(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, float s)
{
    pOut->x = pV->x * s; pOut->y = pV->y * s; pOut->z = pV->z * s;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Lerp(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2, float s)
{
    pOut->x = pV1->x + s * (pV2->x - pV1->x);
    pOut->y = pV1->y + s * (pV2->y - pV1->y);
    pOut->z = pV1->z + s * (pV2->z - pV1->z);
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3Normalize(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV)
{
    return SeifukuD3DX::StoreV3(pOut, DirectX::XMVector3Normalize(SeifukuD3DX::LoadV3(pV)));
}

inline D3DXVECTOR2* D3DXVec2TransformCoord(D3DXVECTOR2* pOut, const D3DXVECTOR2* pV, const D3DXMATRIX* pM)
{
    float x = pV->x * pM->_11 + pV->y * pM->_21 + pM->_41;
    float y = pV->x * pM->_12 + pV->y * pM->_22 + pM->_42;
    float w = pV->x * pM->_14 + pV->y * pM->_24 + pM->_44;
    pOut->x = x / w;
    pOut->y = y / w;
    return pOut;
}

inline D3DXVECTOR3* D3DXVec3TransformCoord(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM)
{
    return SeifukuD3DX::StoreV3(pOut, DirectX::XMVector3TransformCoord(SeifukuD3DX::LoadV3(pV), SeifukuD3DX::LoadM(pM)));
}

inline D3DXVECTOR3* D3DXVec3TransformNormal(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM)
{
    return SeifukuD3DX::StoreV3(pOut, DirectX::XMVector3TransformNormal(SeifukuD3DX::LoadV3(pV), SeifukuD3DX::LoadM(pM)));
}

inline D3DXVECTOR3* D3DXVec3Project(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DVIEWPORT9* pViewport,
                                    const D3DXMATRIX* pProjection, const D3DXMATRIX* pView, const D3DXMATRIX* pWorld)
{
    using namespace DirectX;
    XMMATRIX P = pProjection ? SeifukuD3DX::LoadM(pProjection) : XMMatrixIdentity();
    XMMATRIX V = pView       ? SeifukuD3DX::LoadM(pView)       : XMMatrixIdentity();
    XMMATRIX W = pWorld      ? SeifukuD3DX::LoadM(pWorld)      : XMMatrixIdentity();
    XMVECTOR r = XMVector3Project(SeifukuD3DX::LoadV3(pV),
                                  (float) pViewport->X, (float) pViewport->Y,
                                  (float) pViewport->Width, (float) pViewport->Height,
                                  pViewport->MinZ, pViewport->MaxZ, P, V, W);
    return SeifukuD3DX::StoreV3(pOut, r);
}

inline D3DXVECTOR3* D3DXVec3Unproject(D3DXVECTOR3* pOut, const D3DXVECTOR3* pV, const D3DVIEWPORT9* pViewport,
                                      const D3DXMATRIX* pProjection, const D3DXMATRIX* pView, const D3DXMATRIX* pWorld)
{
    using namespace DirectX;
    XMMATRIX P = pProjection ? SeifukuD3DX::LoadM(pProjection) : XMMatrixIdentity();
    XMMATRIX V = pView       ? SeifukuD3DX::LoadM(pView)       : XMMatrixIdentity();
    XMMATRIX W = pWorld      ? SeifukuD3DX::LoadM(pWorld)      : XMMatrixIdentity();
    XMVECTOR r = XMVector3Unproject(SeifukuD3DX::LoadV3(pV),
                                    (float) pViewport->X, (float) pViewport->Y,
                                    (float) pViewport->Width, (float) pViewport->Height,
                                    pViewport->MinZ, pViewport->MaxZ, P, V, W);
    return SeifukuD3DX::StoreV3(pOut, r);
}

inline D3DXVECTOR4* D3DXVec3Transform(D3DXVECTOR4* pOut, const D3DXVECTOR3* pV, const D3DXMATRIX* pM)
{
    float x = pV->x, y = pV->y, z = pV->z;
    pOut->x = x * pM->_11 + y * pM->_21 + z * pM->_31 + pM->_41;
    pOut->y = x * pM->_12 + y * pM->_22 + z * pM->_32 + pM->_42;
    pOut->z = x * pM->_13 + y * pM->_23 + z * pM->_33 + pM->_43;
    pOut->w = x * pM->_14 + y * pM->_24 + z * pM->_34 + pM->_44;
    return pOut;
}

inline float D3DXPlaneDot(const D3DXPLANE* pP, const D3DXVECTOR4* pV)
{
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d * pV->w;
}

inline float D3DXPlaneDotCoord(const D3DXPLANE* pP, const D3DXVECTOR3* pV)
{
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z + pP->d;
}

inline float D3DXPlaneDotNormal(const D3DXPLANE* pP, const D3DXVECTOR3* pV)
{
    return pP->a * pV->x + pP->b * pV->y + pP->c * pV->z;
}

inline D3DXPLANE* D3DXPlaneFromPointNormal(D3DXPLANE* pOut, const D3DXVECTOR3* pPoint, const D3DXVECTOR3* pNormal)
{
    pOut->a = pNormal->x;
    pOut->b = pNormal->y;
    pOut->c = pNormal->z;
    pOut->d = -D3DXVec3Dot(pPoint, pNormal);
    return pOut;
}

inline D3DXVECTOR3* D3DXPlaneIntersectLine(D3DXVECTOR3* pOut, const D3DXPLANE* pP, const D3DXVECTOR3* pV1, const D3DXVECTOR3* pV2)
{
    D3DXVECTOR3 dir;
    D3DXVec3Subtract(&dir, pV2, pV1);
    float fDenom = D3DXPlaneDotNormal(pP, &dir);
    if (fDenom == 0.0f)
        return NULL;
    float t = -D3DXPlaneDotCoord(pP, pV1) / fDenom;
    pOut->x = pV1->x + t * dir.x;
    pOut->y = pV1->y + t * dir.y;
    pOut->z = pV1->z + t * dir.z;
    return pOut;
}

inline BOOL D3DXBoxBoundProbe(const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax, const D3DXVECTOR3* pRayPosition, const D3DXVECTOR3* pRayDirection)
{
    const float* lo = &pMin->x;
    const float* hi = &pMax->x;
    const float* pos = &pRayPosition->x;
    const float* dir = &pRayDirection->x;
    float tNear = -3.402823466e+38F, tFar = 3.402823466e+38F;
    for (int i = 0; i < 3; i++)
    {
        if (dir[i] == 0.0f)
        {
            if (pos[i] < lo[i] || pos[i] > hi[i])
                return FALSE;
            continue;
        }
        float t1 = (lo[i] - pos[i]) / dir[i];
        float t2 = (hi[i] - pos[i]) / dir[i];
        if (t1 > t2) { float t = t1; t1 = t2; t2 = t; }
        if (t1 > tNear) tNear = t1;
        if (t2 < tFar) tFar = t2;
        if (tNear > tFar || tFar < 0.0f)
            return FALSE;
    }
    return TRUE;
}

#define D3DX_DEFAULT         ((UINT) -1)
#define D3DXERR_INVALIDDATA  MAKE_HRESULT(1, 0x876, 2900)
#define D3DX_FILTER_NONE     (1 << 0)
#define D3DX_FILTER_POINT    (2 << 0)
#define D3DX_FILTER_LINEAR   (3 << 0)
#define D3DX_FILTER_TRIANGLE (4 << 0)
#define D3DX_FILTER_BOX      (5 << 0)
#define D3DX_FILTER_MIRROR_U (1 << 16)
#define D3DX_FILTER_MIRROR_V (2 << 16)
#define D3DX_FILTER_MIRROR_W (4 << 16)
#define D3DX_FILTER_MIRROR   (7 << 16)
#define D3DX_FILTER_DITHER   (1 << 19)

typedef enum _D3DXIMAGE_FILEFORMAT
{
    D3DXIFF_BMP = 0,
    D3DXIFF_JPG = 1,
    D3DXIFF_TGA = 2,
    D3DXIFF_PNG = 3,
    D3DXIFF_DDS = 4,
    D3DXIFF_PPM = 5,
    D3DXIFF_DIB = 6,
    D3DXIFF_FORCE_DWORD = 0x7fffffff
} D3DXIMAGE_FILEFORMAT;

typedef struct _D3DXIMAGE_INFO
{
    UINT                    Width;
    UINT                    Height;
    UINT                    Depth;
    UINT                    MipLevels;
    D3DFORMAT               Format;
    D3DRESOURCETYPE         ResourceType;
    D3DXIMAGE_FILEFORMAT    ImageFileFormat;
} D3DXIMAGE_INFO;

HRESULT WINAPI D3DXCreateTexture(LPDIRECT3DDEVICE9 pDevice, UINT Width, UINT Height, UINT MipLevels, DWORD Usage,
                                 D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9* ppTexture);

HRESULT WINAPI D3DXCreateTextureFromFileExA(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, UINT Width, UINT Height,
                                            UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter,
                                            DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo,
                                            PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture);
#define D3DXCreateTextureFromFileEx D3DXCreateTextureFromFileExA

HRESULT WINAPI D3DXCreateTextureFromFileInMemoryEx(LPDIRECT3DDEVICE9 pDevice, LPCVOID pSrcData, UINT SrcDataSize,
                                                   UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format,
                                                   D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey,
                                                   D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette,
                                                   LPDIRECT3DTEXTURE9* ppTexture);

#endif
