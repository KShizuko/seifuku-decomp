#include "stdafx.h"
#include "RC5_321216.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>

unsigned long	rotate_left(unsigned long nData, unsigned long nCount);
unsigned long	rotate_right(unsigned long nData, unsigned long nCount);

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");
static_assert(sizeof(CRc5_321216) == 0x78, "");
static_assert(RC5_SUB == 26 && RC5_KEY == 4, "");
static_assert(RC5_PW32 == 0xB7E15163 && RC5_QW32 == 0x9E3779B9, "");

static const unsigned char s_key2[16] = { 0x91, 0x5F, 0x46, 0x19, 0xBE, 0x41, 0xB2, 0x51, 0x63, 0x55, 0xA5, 0x01, 0x10, 0xA9, 0xCE, 0x91 };
static const unsigned char s_gameKey[16] = { 0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E, 0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE };

static void AssertBytes(const unsigned char* expected, const unsigned char* actual, int n)
{
    for (int i = 0; i < n; i++)
        Assert::AreEqual((int) expected[i], (int) actual[i]);
}

TEST_CLASS(Rc5Tests)
{
public:
    TEST_METHOD(ZeroKeyZeroBlockIsRivestsVector)
    {
        unsigned char key[16] = { 0 };
        unsigned char buf[8] = { 0 };
        CRc5_321216 rc5;
        rc5.Rc5InitKey(key);
        rc5.Rc5Encrypt(buf, 8);
        static const unsigned char expected[8] = { 0x21, 0xA5, 0xDB, 0xEE, 0x15, 0x4B, 0x8F, 0x6D };
        AssertBytes(expected, buf, 8);
    }

    TEST_METHOD(SecondKeyOnFirstCiphertextIsRivestsVector)
    {
        unsigned char buf[8] = { 0x21, 0xA5, 0xDB, 0xEE, 0x15, 0x4B, 0x8F, 0x6D };
        CRc5_321216 rc5;
        rc5.Rc5InitKey(s_key2);
        rc5.Rc5Encrypt(buf, 8);
        static const unsigned char expected[8] = { 0xF7, 0xC0, 0x13, 0xAC, 0x5B, 0x2B, 0x89, 0x52 };
        AssertBytes(expected, buf, 8);
    }

    TEST_METHOD(GameKeyOnPasswordBuffer)
    {
        unsigned char buf[16] = { 0 };
        memcpy(buf, "password", 8);
        CRc5_321216 rc5;
        rc5.Rc5InitKey(s_gameKey);
        rc5.Rc5Encrypt(buf, 16);
        static const unsigned char expected[16] = { 0x19, 0x02, 0xF8, 0x7F, 0x6B, 0x21, 0x2A, 0xF7, 0x0D, 0xA6, 0x08, 0x4D, 0xF3, 0x67, 0xEB, 0x73 };
        AssertBytes(expected, buf, 16);

        unsigned char block[8] = { 0 };
        rc5.Rc5Encrypt(block, 8);
        AssertBytes(expected + 8, block, 8);
    }

    TEST_METHOD(LengthRoundsDownToBlocks)
    {
        unsigned char buf[12] = { 0 };
        buf[8] = 0xAA; buf[9] = 0xBB; buf[10] = 0xCC; buf[11] = 0xDD;
        CRc5_321216 rc5;
        unsigned char key[16] = { 0 };
        rc5.Rc5InitKey(key);
        rc5.Rc5Encrypt(buf, 12);
        static const unsigned char expected[8] = { 0x21, 0xA5, 0xDB, 0xEE, 0x15, 0x4B, 0x8F, 0x6D };
        AssertBytes(expected, buf, 8);
        Assert::IsTrue(buf[8] == 0xAA && buf[9] == 0xBB && buf[10] == 0xCC && buf[11] == 0xDD);

        unsigned char part[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        rc5.Rc5Encrypt(part, 7);
        static const unsigned char untouched[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        AssertBytes(untouched, part, 8);
    }

    TEST_METHOD(KeyScheduleIsDeterministic)
    {
        unsigned char bufA[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
        unsigned char bufB[8] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
        CRc5_321216 rc5A, rc5B;
        rc5A.Rc5InitKey(s_gameKey);
        rc5B.Rc5InitKey(s_gameKey);
        rc5A.Rc5Encrypt(bufA, 8);
        rc5B.Rc5Encrypt(bufB, 8);
        AssertBytes(bufA, bufB, 8);
        Assert::IsFalse(memcmp(bufA, "abcdefgh", 8) == 0);
    }

    TEST_METHOD(RotateRightMirrorsRotateLeft)
    {
        Assert::AreEqual(0xC0000000ul, rotate_right(0x80000001ul, 1));
        Assert::AreEqual(0x18000000ul, rotate_right(0x80000001ul, 4));
        Assert::AreEqual(0x18000000ul, rotate_right(0x80000001ul, 36));
        Assert::AreEqual(0x12345678ul, rotate_right(rotate_left(0x12345678ul, 13), 13));
        Assert::AreEqual(0x12345678ul, rotate_left(rotate_right(0x12345678ul, 7), 7));
    }
};
