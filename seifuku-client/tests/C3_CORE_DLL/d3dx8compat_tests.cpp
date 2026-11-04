#include "CppUnitTest.h"
#include <cstddef>
#include "d3dx8compat.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(D3DXVECTOR2) == 8, "");
static_assert(sizeof(D3DXVECTOR3) == 12, "");
static_assert(sizeof(D3DXVECTOR4) == 16, "");
static_assert(sizeof(D3DXMATRIX) == 64, "");
static_assert(sizeof(D3DXMATRIXA16) == 64 && __alignof(D3DXMATRIXA16) == 4, "");
static_assert(sizeof(D3DXQUATERNION) == 16 && sizeof(D3DXPLANE) == 16 && sizeof(D3DXCOLOR) == 16, "");
static_assert(sizeof(D3DXIMAGE_INFO) == 28, "");
static_assert(offsetof(D3DXMATRIX, _41) == 48 && offsetof(D3DXMATRIX, _22) == 20, "");
static_assert(sizeof(D3DVIEWPORT9) == 24, "");

static const float EPS = 1e-5f;
static void Near(float expected, float actual, const wchar_t* what)
{
    Assert::IsTrue(fabsf(expected - actual) <= EPS, what);
}
static void NearV(float x, float y, float z, const D3DXVECTOR3& v, const wchar_t* what)
{
    Near(x, v.x, what); Near(y, v.y, what); Near(z, v.z, what);
}

TEST_CLASS(D3DXMatrices)
{
public:
    TEST_METHOD(IdentityAndIsIdentity)
    {
        D3DXMATRIX m;
        Assert::IsTrue(D3DXMatrixIdentity(&m) == &m);
        Assert::IsTrue(D3DXMatrixIsIdentity(&m));
        Near(1.0f, m._11, L"_11"); Near(0.0f, m._12, L"_12"); Near(1.0f, m._44, L"_44"); Near(0.0f, m._41, L"_41");
        m._43 = 2.0f;
        Assert::IsFalse(D3DXMatrixIsIdentity(&m));
    }

    TEST_METHOD(MultiplyIsRowVectorOrder)
    {
        D3DXMATRIX t, s, ts, st;
        D3DXMatrixTranslation(&t, 1.0f, 2.0f, 3.0f);
        D3DXMatrixScaling(&s, 2.0f, 2.0f, 2.0f);
        D3DXMatrixMultiply(&ts, &t, &s);
        D3DXMatrixMultiply(&st, &s, &t);
        Near(2.0f, ts._41, L"T*S _41"); Near(4.0f, ts._42, L"T*S _42"); Near(6.0f, ts._43, L"T*S _43"); Near(2.0f, ts._11, L"T*S _11");
        Near(1.0f, st._41, L"S*T _41"); Near(2.0f, st._11, L"S*T _11");
        D3DXMATRIX op = t * s;
        Assert::IsTrue(op == ts);
        t *= s;
        Assert::IsTrue(t == ts);
    }

    TEST_METHOD(MultiplyMayAliasItsOutput)
    {
        D3DXMATRIX a, b;
        D3DXMatrixTranslation(&a, 1.0f, 0.0f, 0.0f);
        D3DXMatrixTranslation(&b, 0.0f, 5.0f, 0.0f);
        D3DXMatrixMultiply(&a, &a, &b);
        Near(1.0f, a._41, L"_41"); Near(5.0f, a._42, L"_42");
    }

    TEST_METHOD(InverseOfTranslationAndSingular)
    {
        D3DXMATRIX t, inv;
        float det = 0.0f;
        D3DXMatrixTranslation(&t, 3.0f, -4.0f, 5.0f);
        Assert::IsTrue(D3DXMatrixInverse(&inv, &det, &t) == &inv);
        Near(1.0f, det, L"det");
        Near(-3.0f, inv._41, L"_41"); Near(4.0f, inv._42, L"_42"); Near(-5.0f, inv._43, L"_43"); Near(1.0f, inv._33, L"_33");
        D3DXMATRIX z; ZeroMemory(&z, sizeof(z));
        D3DXMATRIX untouched; D3DXMatrixIdentity(&untouched);
        Assert::IsNull(D3DXMatrixInverse(&untouched, NULL, &z));
        Assert::IsTrue(D3DXMatrixIsIdentity(&untouched));
        D3DXMatrixInverse(&t, NULL, &t);
        Near(-3.0f, t._41, L"in place");
    }

    TEST_METHOD(Vec2TransformCoord)
    {
        D3DXMATRIX r, t, m; D3DXVECTOR2 v(1, 0);
        D3DXMatrixRotationZ(&r, D3DXToRadian(90.0f));
        D3DXMatrixTranslation(&t, 5, 7, 0);
        D3DXMatrixMultiply(&m, &r, &t);
        D3DXVec2TransformCoord(&v, &v, &m);
        Assert::AreEqual(5.0f, v.x, 1e-4f); Assert::AreEqual(8.0f, v.y, 1e-4f);
        D3DXMatrixIdentity(&m); m._44 = 2.0f;
        D3DXVECTOR2 w(4, 6);
        D3DXVec2TransformCoord(&w, &w, &m);
        Assert::AreEqual(2.0f, w.x, 1e-4f); Assert::AreEqual(3.0f, w.y, 1e-4f);
    }

    TEST_METHOD(RotationsFollowD3DConventions)
    {
        D3DXMATRIX m; D3DXVECTOR3 v;
        const float q = D3DXToRadian(90.0f);
        D3DXMatrixRotationZ(&m, q); D3DXVec3TransformCoord(&v, &D3DXVECTOR3(1, 0, 0), &m);
        NearV(0, 1, 0, v, L"RotZ 90: x -> y");
        D3DXMatrixRotationX(&m, q); D3DXVec3TransformCoord(&v, &D3DXVECTOR3(0, 1, 0), &m);
        NearV(0, 0, 1, v, L"RotX 90: y -> z");
        D3DXMatrixRotationY(&m, q); D3DXVec3TransformCoord(&v, &D3DXVECTOR3(0, 0, 1), &m);
        NearV(1, 0, 0, v, L"RotY 90: z -> x");
        D3DXMATRIX a; D3DXMatrixRotationAxis(&a, &D3DXVECTOR3(0, 0, 2), q);
        D3DXMatrixRotationZ(&m, q);
        for (int i = 0; i < 16; i++) Near((&m._11)[i], (&a._11)[i], L"RotationAxis == RotationZ");
        D3DXQUATERNION qz(0.0f, 0.0f, sinf(q / 2), cosf(q / 2));
        D3DXMatrixRotationQuaternion(&a, &qz);
        for (int i = 0; i < 16; i++) Near((&m._11)[i], (&a._11)[i], L"RotationQuaternion == RotationZ");
    }

    TEST_METHOD(TranslationScalingFields)
    {
        D3DXMATRIX m;
        D3DXMatrixTranslation(&m, 1.0f, 2.0f, 3.0f);
        Near(1.0f, m._41, L""); Near(2.0f, m._42, L""); Near(3.0f, m._43, L""); Near(1.0f, m._11, L""); Near(1.0f, m._44, L"");
        D3DXMatrixScaling(&m, 2.0f, 3.0f, 4.0f);
        Near(2.0f, m._11, L""); Near(3.0f, m._22, L""); Near(4.0f, m._33, L""); Near(0.0f, m._41, L""); Near(1.0f, m._44, L"");
    }

    TEST_METHOD(LookAtLH)
    {
        D3DXMATRIX view; D3DXVECTOR3 v;
        D3DXVECTOR3 eye(0, 0, -5), at(0, 0, 0), up(0, 1, 0);
        D3DXMatrixLookAtLH(&view, &eye, &at, &up);
        D3DXVec3TransformCoord(&v, &eye, &view); NearV(0, 0, 0, v, L"eye -> origin");
        D3DXVec3TransformCoord(&v, &at, &view);  NearV(0, 0, 5, v, L"at -> +z (left-handed)");
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(1, 0, -5), &view); NearV(1, 0, 0, v, L"right stays right");
    }

    TEST_METHOD(PerspectiveFovLH)
    {
        D3DXMATRIX p;
        D3DXMatrixPerspectiveFovLH(&p, D3DXToRadian(90.0f), 1.0f, 1.0f, 100.0f);
        Near(1.0f, p._11, L"_11"); Near(1.0f, p._22, L"_22");
        Near(100.0f / 99.0f, p._33, L"_33"); Near(-100.0f / 99.0f, p._43, L"_43");
        Near(1.0f, p._34, L"_34"); Near(0.0f, p._44, L"_44");
        D3DXVECTOR3 v;
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(0, 0, 100), &p); Near(1.0f, v.z, L"far -> 1");
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(0, 0, 1), &p);   Near(0.0f, v.z, L"near -> 0");
    }

    TEST_METHOD(OrthoLH)
    {
        D3DXMATRIX o;
        D3DXMatrixOrthoLH(&o, 2.0f, 4.0f, 0.0f, 10.0f);
        Near(1.0f, o._11, L"2/w"); Near(0.5f, o._22, L"2/h"); Near(0.1f, o._33, L"1/(zf-zn)"); Near(0.0f, o._43, L"-zn/(zf-zn)"); Near(1.0f, o._44, L"");
    }

    TEST_METHOD(ShadowProjectsThroughPointLight)
    {
        D3DXMATRIX s; D3DXVECTOR3 v;
        D3DXVECTOR4 light(0.0f, 10.0f, 0.0f, 1.0f);
        D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
        D3DXMatrixShadow(&s, &light, &ground);
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(1, 5, 1), &s);
        NearV(2, 0, 2, v, L"shadow");
    }

    TEST_METHOD(TransposeAndOperators)
    {
        D3DXMATRIX m(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16), t;
        D3DXMatrixTranspose(&t, &m);
        Near(5.0f, t._12, L"_12 <- _21"); Near(13.0f, t._14, L"_14 <- _41");
        Near(7.0f, m(1, 2), L"operator()(row, col)");
        D3DXMATRIX twice = m * 2.0f;
        Near(32.0f, twice._44, L"scalar multiply");
        Assert::IsTrue(m != twice);
    }
};

TEST_CLASS(D3DXVectors)
{
public:
    TEST_METHOD(LengthDotCross)
    {
        D3DXVECTOR3 a(3, 4, 0), b(1, 0, 0), c(0, 1, 0), r;
        Near(5.0f, D3DXVec3Length(&a), L"length");
        Near(25.0f, D3DXVec3LengthSq(&a), L"length sq");
        Near(3.0f, D3DXVec3Dot(&a, &b), L"dot");
        D3DXVec3Cross(&r, &b, &c);
        NearV(0, 0, 1, r, L"x cross y = z");
        D3DXVec3Cross(&b, &b, &c);
        NearV(0, 0, 1, b, L"aliased cross");
    }

    TEST_METHOD(NormalizeIncludingZero)
    {
        D3DXVECTOR3 v(0, 3, 4), r;
        D3DXVec3Normalize(&r, &v);
        NearV(0, 0.6f, 0.8f, r, L"normalize");
        D3DXVec3Normalize(&v, &v);
        NearV(0, 0.6f, 0.8f, v, L"in place");
        D3DXVECTOR3 z(0, 0, 0);
        D3DXVec3Normalize(&r, &z);
        NearV(0, 0, 0, r, L"zero stays zero");
    }

    TEST_METHOD(LerpScaleAddSubtract)
    {
        D3DXVECTOR3 a(0, 0, 0), b(10, 20, 30), r;
        D3DXVec3Lerp(&r, &a, &b, 0.25f);   NearV(2.5f, 5.0f, 7.5f, r, L"lerp");
        D3DXVec3Scale(&r, &b, 0.5f);       NearV(5, 10, 15, r, L"scale");
        D3DXVec3Add(&r, &b, &b);           NearV(20, 40, 60, r, L"add");
        D3DXVec3Subtract(&r, &b, &D3DXVECTOR3(1, 1, 1)); NearV(9, 19, 29, r, L"subtract");
        D3DXVECTOR2 p(1, 2), q(3, 4), s;
        D3DXVec2Add(&s, &p, &q);           Near(4.0f, s.x, L"vec2 add x"); Near(6.0f, s.y, L"vec2 add y");
        D3DXVECTOR3 d = b - D3DXVECTOR3(1, 2, 3);
        NearV(9, 18, 27, d, L"operator-");
        NearV(-9, -18, -27, -d, L"unary -");
        NearV(18, 36, 54, 2.0f * d, L"scalar *");
    }

    TEST_METHOD(TransformCoordDividesByW)
    {
        D3DXMATRIX m; D3DXMatrixIdentity(&m);
        m._34 = 1.0f; m._44 = 0.0f;
        D3DXVECTOR3 v;
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(2, 4, 2), &m);
        NearV(1, 2, 1, v, L"perspective divide");
        D3DXMatrixTranslation(&m, 5, 6, 7);
        D3DXVec3TransformCoord(&v, &D3DXVECTOR3(1, 1, 1), &m);  NearV(6, 7, 8, v, L"coord");
        D3DXVec3TransformNormal(&v, &D3DXVECTOR3(1, 1, 1), &m); NearV(1, 1, 1, v, L"normal");
    }

    TEST_METHOD(ProjectToViewport)
    {
        D3DVIEWPORT9 vp = { 0, 0, 800, 600, 0.0f, 1.0f };
        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        D3DXVECTOR3 v;
        D3DXVec3Project(&v, &D3DXVECTOR3(0, 0, 0.5f), &vp, &id, &id, &id);
        NearV(400, 300, 0.5f, v, L"centre");
        D3DXVec3Project(&v, &D3DXVECTOR3(1, 1, 0), &vp, NULL, NULL, NULL);
        NearV(800, 0, 0, v, L"top right");
        D3DVIEWPORT9 off = { 10, 20, 100, 50, 0.0f, 1.0f };
        D3DXVec3Project(&v, &D3DXVECTOR3(-1, -1, 1), &off, &id, &id, &id);
        NearV(10, 70, 1, v, L"bottom left of an offset viewport");
    }

    TEST_METHOD(UnprojectIsTheInverseOfProject)
    {
        D3DVIEWPORT9 vp = { 0, 0, 800, 600, 0.0f, 1.0f };
        D3DXMATRIX id; D3DXMatrixIdentity(&id);
        D3DXVECTOR3 v;
        D3DXVec3Unproject(&v, &D3DXVECTOR3(400, 300, 0.5f), &vp, &id, &id, &id);
        NearV(0, 0, 0.5f, v, L"centre");
        D3DXVec3Unproject(&v, &D3DXVECTOR3(800, 0, 0), &vp, NULL, NULL, NULL);
        NearV(1, 1, 0, v, L"top right, NULL matrices");
        D3DXMATRIX view, proj;
        D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0, -800, -400), &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(0, 0, -1));
        D3DXMatrixPerspectiveFovLH(&proj, 0.7853982f, 800.0f / 600.0f, 100.0f, 8000.0f);
        D3DXVECTOR3 p(120, 35, -60), s, back;
        D3DXVec3Project(&s, &p, &vp, &proj, &view, NULL);
        D3DXVec3Unproject(&back, &s, &vp, &proj, &view, NULL);
        Assert::AreEqual(p.x, back.x, 0.05f); Assert::AreEqual(p.y, back.y, 0.05f); Assert::AreEqual(p.z, back.z, 0.05f);
    }

    TEST_METHOD(Vec3TransformKeepsAllFourComponents)
    {
        D3DXMATRIX m; D3DXMatrixIdentity(&m);
        m._41 = 5; m._42 = 6; m._43 = 7;
        m._14 = 0.5f; m._44 = 2;
        D3DXVECTOR4 r;
        Assert::IsTrue(&r == D3DXVec3Transform(&r, &D3DXVECTOR3(1, 2, 3), &m));
        Assert::AreEqual(6.0f, r.x); Assert::AreEqual(8.0f, r.y); Assert::AreEqual(10.0f, r.z);
        Assert::AreEqual(1 * 0.5f + 2.0f, r.w);
        D3DXMATRIX rz; D3DXMatrixRotationZ(&rz, 1.5707964f);
        D3DXVec3Transform(&r, &D3DXVECTOR3(1, 0, 0), &rz);
        Assert::AreEqual(0.0f, r.x, 1e-6f); Assert::AreEqual(1.0f, r.y, 1e-6f); Assert::AreEqual(0.0f, r.z, 1e-6f); Assert::AreEqual(1.0f, r.w);
    }
};

TEST_CLASS(D3DXPlanesColorsAngles)
{
public:
    TEST_METHOD(PlaneFromPointNormalAndDots)
    {
        D3DXPLANE p;
        D3DXPlaneFromPointNormal(&p, &D3DXVECTOR3(0, 5, 0), &D3DXVECTOR3(0, 1, 0));
        Near(0.0f, p.a, L"a"); Near(1.0f, p.b, L"b"); Near(0.0f, p.c, L"c"); Near(-5.0f, p.d, L"d = -n.p");
        Near(3.0f, D3DXPlaneDotCoord(&p, &D3DXVECTOR3(9, 8, 7)), L"signed distance");
        Near(2.0f, D3DXPlaneDotNormal(&p, &D3DXVECTOR3(0, 2, 0)), L"normal dot");
        Near(-3.0f, D3DXPlaneDot(&p, &D3DXVECTOR4(0, 2, 0, 1)), L"plane dot");
    }

    TEST_METHOD(PlaneIntersectLine)
    {
        D3DXPLANE p; D3DXVECTOR3 r;
        D3DXPlaneFromPointNormal(&p, &D3DXVECTOR3(0, 5, 0), &D3DXVECTOR3(0, 1, 0));
        Assert::IsTrue(D3DXPlaneIntersectLine(&r, &p, &D3DXVECTOR3(1, 0, 1), &D3DXVECTOR3(1, 10, 1)) == &r);
        NearV(1, 5, 1, r, L"hit");
        Assert::IsNull(D3DXPlaneIntersectLine(&r, &p, &D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(1, 0, 0)));
    }

    TEST_METHOD(ColorPacking)
    {
        D3DXCOLOR c(0x80FF0000);
        Near(1.0f, c.r, L"r"); Near(0.0f, c.g, L"g"); Near(0.0f, c.b, L"b"); Near(128.0f / 255.0f, c.a, L"a");
        Assert::AreEqual<unsigned long>(0x80FF0000, (DWORD) c);
        D3DXCOLOR over(2.0f, -1.0f, 0.5f, 1.0f);
        Assert::AreEqual<unsigned long>(0xFFFF0080, (DWORD) over);
    }

    TEST_METHOD(Angles)
    {
        Near(3.141592654f, D3DXToRadian(180.0f), L"pi");
        Near(45.0f, D3DXToDegree(D3DX_PI / 4), L"45");
        Near(3.141592654f, D3DX_PI, L"D3DX_PI");
    }

    TEST_METHOD(BoxBoundProbe)
    {
        D3DXVECTOR3 lo(-10.0f, 0.0f, -10.0f), hi(10.0f, 5.0f, 10.0f);
        D3DXVECTOR3 from(0.0f, -1000.0f, 0.0f), down(0.0f, 1.0f, 0.0f), aside(0.0f, 1.0f, 0.5f), away(0.0f, -1.0f, 0.0f);
        Assert::IsTrue(D3DXBoxBoundProbe(&lo, &hi, &from, &down) == TRUE, L"straight down");
        Assert::IsTrue(D3DXBoxBoundProbe(&lo, &hi, &from, &aside) == FALSE, L"past the box");
        Assert::IsTrue(D3DXBoxBoundProbe(&lo, &hi, &from, &away) == FALSE, L"behind the origin");
        D3DXVECTOR3 inside(0.0f, 2.0f, -50.0f), alongZ(0.0f, 0.0f, 1.0f);
        Assert::IsTrue(D3DXBoxBoundProbe(&lo, &hi, &inside, &alongZ) == TRUE, L"axis-parallel through the slab");
    }
};
