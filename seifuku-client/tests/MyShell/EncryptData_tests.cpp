#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "EncryptData.h"
#include "ImageString.h"
#include "FDword.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CEncryptData) == 0xC4 && std::has_virtual_destructor<CEncryptData>::value, "");
static_assert(std::is_same<decltype(((CEncryptData*) 0)->set_key((const DWORD*) 0, 0)), DWORD*>::value, "");

struct EncryptDataProbe
{
    static DWORD* Key(CEncryptData& d) { return d.l_key; }
    static const DWORD* Data(const CEncryptData& d) { return d.m_dwValue; }
    static DWORD* SetKey(CEncryptData& d, const DWORD* key, DWORD bits) { return d.set_key(key, bits); }
    static void Encrypt(CEncryptData& d, const DWORD* in, DWORD* out) { d.encrypt(in, out); }
    static void Decrypt(CEncryptData& d, const DWORD* in, DWORD* out) { d.decrypt(in, out); }
    static size_t KeyOffset() { return offsetof(CEncryptData, l_key); }
    static size_t DataOffset() { return offsetof(CEncryptData, m_dwValue); }
};

struct ImageStringProbe : CImageString
{
    static size_t Scale() { return offsetof(ImageStringProbe, m_fScale); }
    static size_t Pos() { return offsetof(ImageStringProbe, m_posShow); }
    static size_t Color() { return offsetof(ImageStringProbe, m_dwColor); }
    static size_t ImgString() { return offsetof(ImageStringProbe, m_objImgString); }
};
static_assert(sizeof(CImageString) == 0x14 + sizeof(CImageStringEx) && std::has_virtual_destructor<CImageString>::value, "");
static_assert(std::is_same<decltype(&CImageString::SetColor), void (CImageString::*)(DWORD, DWORD)>::value, "");
static_assert(std::is_same<decltype(&CImageString::SetScalePercent), void (CImageString::*)(DWORD)>::value, "");
static_assert(std::is_same<decltype(CImageString::CreateNew(0, CMyPos())), CImageString*>::value, "");

static_assert(sizeof(CFDword) == 0x10 && std::has_virtual_destructor<CFDword>::value, "");
static_assert(std::is_same<decltype(CFDword::CreateNew(0, 0)), CFDword*>::value, "");

TEST_CLASS(ClientPlayerHelperHeaders)
{
public:
    TEST_METHOD(PlayerTailIsImageStringThenFDwordPointer)
    {
        Assert::IsTrue(0xD44 + 0x70 == 0xDB4 && 0x14 + 0x5C == 0x70);
        Assert::IsTrue(0xC74 + 0xC0 == 0xD34 && 0x14 + 0xAC == 0xC0);
        Assert::IsTrue(0xDB4 + sizeof(CFDword*) == 0xDB8);
    }

    TEST_METHOD(ImageStringLayoutMatchesTheConstructor)
    {
        Assert::IsTrue(ImageStringProbe::Scale() == 0x4);
        Assert::IsTrue(ImageStringProbe::Pos() == 0x8);
        Assert::IsTrue(ImageStringProbe::Color() == 0x10);
        Assert::IsTrue(ImageStringProbe::ImgString() == 0x14);
    }
};

TEST_CLASS(ClientEncryptDataBodies)
{
public:
    TEST_METHOD(SetValueGetValueRoundTrips)
    {
        CEncryptData data;
        Assert::AreEqual(0, data.GetValue());
        data.SetValue(12345);
        Assert::AreEqual(12345, data.GetValue());
        data.SetValue(-1);
        Assert::AreEqual(-1, data.GetValue());
        data.SetValue(0x7FFFFFFF);
        Assert::AreEqual(0x7FFFFFFF, data.GetValue());
        data.SetValue(0x01020304);
        Assert::AreEqual(0x01020304, data.GetValue());
    }

    TEST_METHOD(TwoInstancesKeepTheirOwnValues)
    {
        CEncryptData a1;
        CEncryptData a2;
        a1.SetValue(100);
        a2.SetValue(200);
        Assert::AreEqual(100, a1.GetValue());
        Assert::AreEqual(200, a2.GetValue());
    }

    TEST_METHOD(TheCipherIsRC6)
    {
        Assert::IsTrue(EncryptDataProbe::KeyOffset() == 0x4);
        Assert::IsTrue(EncryptDataProbe::DataOffset() == 0xB4);
        CEncryptData data;
        const DWORD key[4] = { 0, 0, 0, 0 };
        DWORD* s = EncryptDataProbe::SetKey(data, key, 128);
        Assert::IsTrue(s == EncryptDataProbe::Key(data));
        const DWORD plain[4] = { 0, 0, 0, 0 };
        DWORD cipher[4] = { 0, 0, 0, 0 };
        EncryptDataProbe::Encrypt(data, plain, cipher);
        Assert::IsTrue(cipher[0] == 0x36A5C38F);
        Assert::IsTrue(cipher[1] == 0x78F7B156);
        Assert::IsTrue(cipher[2] == 0x4EDF29C1);
        Assert::IsTrue(cipher[3] == 0x1EA44898);
        DWORD back[4] = { 1, 1, 1, 1 };
        EncryptDataProbe::Decrypt(data, cipher, back);
        Assert::IsTrue(back[0] == 0 && back[1] == 0 && back[2] == 0 && back[3] == 0);
    }

    TEST_METHOD(AStoredValueIsNotItsPlainBytes)
    {
        CEncryptData data;
        data.SetValue(0x01020304);
        const DWORD* d = EncryptDataProbe::Data(data);
        Assert::IsFalse(d[0] == 4 && d[1] == 3 && d[2] == 2 && d[3] == 1);
    }

    TEST_METHOD(AProtectedPosCopiesThroughCMyPos)
    {
        CMyProtectPos pos;
        CMyPos in = { 123, -45 };
        pos = in;
        CMyPos out = pos;
        Assert::IsTrue(out.x == 123 && out.y == -45);
        Assert::IsTrue(pos.x - 3 == 120);
        pos.y = 7;
        Assert::IsTrue(pos.y.GetValue() == 7);
    }
};
