#include "CppUnitTest.h"
#include <type_traits>
#include <cmath>
#include "Physics.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(std::is_same<decltype(&CPhysics::S2DGetDistance), DWORD (*)(CMyPos, CMyPos)>::value, "");
static_assert(std::is_same<decltype(&CPhysics::S2DBMGetPos), BOOL (*)(int, float, float, CMyPos, CMyPos, CMyPos&)>::value, "");
static_assert(std::is_same<decltype(&CPhysics::S3Get3DDir), void (*)(CMy3DPos, CMy3DPos, CMy3DDir&)>::value, "");
static_assert(std::is_same<decltype(&CPhysics::S2DR45VP), void (*)(CMyPos, CMyPos&)>::value, "");
static_assert(std::is_polymorphic<CPhysics>::value && std::is_same<decltype(&CPhysics::IsPointInRect), BOOL (*)(CMyPos, CMyRect)>::value, "");

static bool Near(float a, float b) { return std::fabs(a - b) < 1e-4f; }

TEST_CLASS(ClientPhysics)
{
public:
    TEST_METHOD(DistanceIsTheTruncatedRoot)
    {
        CMyPos a = {0, 0}, b = {3, 4}, c = {1, 1};
        Assert::IsTrue(CPhysics::S2DGetDistance(a, b) == 5);
        Assert::IsTrue(CPhysics::S2DGetDistance(a, c) == 1);
        Assert::IsTrue(CPhysics::S2DGetDistance(b, a) == 5);
    }
    TEST_METHOD(BeelineMoveInIntegers)
    {
        CMyPos b = {0, 0}, e = {1000, 0}, cur = {-1, -1};
        Assert::IsTrue(CPhysics::S2DBMGetPos(1600, 0.0f, 0.5f, b, b, cur) == TRUE);
        Assert::IsTrue(cur.x == 0 && cur.y == 0);
        Assert::IsTrue(CPhysics::S2DBMGetPos(1600, 0.0f, 0.5f, b, e, cur) == FALSE);
        Assert::IsTrue(cur.x == 800 && cur.y == 0);
        CMyPos e2 = {0, 2000};
        Assert::IsTrue(CPhysics::S2DBMGetPos(1600, 400.0f, 0.5f, b, e2, cur) == FALSE);
        Assert::IsTrue(cur.x == 0 && cur.y == 850);
        Assert::IsTrue(CPhysics::S2DBMGetPos(1600, 0.0f, 1.0f, b, e, cur) == TRUE);
        Assert::IsTrue(cur.x == 1000 && cur.y == 0);
    }
    TEST_METHOD(DirectionByQuadrant)
    {
        CMy3DPos o = {0, 0, 0};
        CMy3DDir d;
        CMy3DPos q1 = {10, -10, 0};
        CPhysics::S3Get3DDir(o, q1, d);
        Assert::IsTrue(Near(d.fHorizontal, 0.785398f + 1.570795f) && Near(d.fVertical, 0.0f));
        CMy3DPos q2 = {-10, 10, 0};
        CPhysics::S3Get3DDir(o, q2, d);
        Assert::IsTrue(Near(d.fHorizontal, 0.785398f - 1.570795f));
        CMy3DPos q3 = {-10, -10, 0};
        CPhysics::S3Get3DDir(o, q3, d);
        Assert::IsTrue(Near(d.fHorizontal, -1.570795f - 0.785398f));
        CMy3DPos q4 = {10, 0, 10};
        CPhysics::S3Get3DDir(o, q4, d);
        Assert::IsTrue(Near(d.fHorizontal, 1.570795f) && Near(d.fVertical, 0.785398f));
        CMy3DPos q5 = {10, 0, -10};
        CPhysics::S3Get3DDir(o, q5, d);
        Assert::IsTrue(Near(d.fVertical, -0.785398f));
    }
    TEST_METHOD(RotateInThe45DegreeView)
    {
        CMyPos t = {0, 10}, p = {32, 0};
        CPhysics::S2DR45VP(t, p);
        Assert::IsTrue(p.x == 0 && p.y == 16);
        CMyPos t2 = {10, 0}, p2 = {16, 0};
        CPhysics::S2DR45VP(t2, p2);
        Assert::IsTrue(p2.x == 16 && p2.y == 0);
        CMyPos t3 = {30, 40}, p3 = {32, 0};
        CPhysics::S2DR45VP(t3, p3);
        Assert::IsTrue(p3.x == 13 && p3.y == 17);
        CMyPos t4 = {0, 0}, p4 = {32, 5};
        CPhysics::S2DR45VP(t4, p4);
        Assert::IsTrue(p4.x == 0 && p4.y == 0);
    }
};
