#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include "MD5.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MD5Probe : CMD5
{
    static size_t Buffer() { return offsetof(MD5Probe, m_buffer); }
    static size_t Count() { return offsetof(MD5Probe, m_count); }
    static size_t State() { return offsetof(MD5Probe, m_state); }
    static void Decode(CMD5& m, unsigned long* pOut, const unsigned char* pIn, unsigned int uLen) { m.Decode(pOut, pIn, uLen); }
};

static_assert(sizeof(CMD5) == 0x5C, "");

static const char* Digest(const char* text, char* out)
{
    CMD5 md5;
    md5.Update((const unsigned char*) text, (unsigned int) strlen(text));
    md5.Final(out);
    return out;
}

TEST_CLASS(ClientMD5)
{
public:
    TEST_METHOD(MembersFollowTheVtableInTheCtorOrder)
    {
        Assert::IsTrue(MD5Probe::Buffer() == 0x4);
        Assert::IsTrue(MD5Probe::Count() == 0x44);
        Assert::IsTrue(MD5Probe::State() == 0x4C);
    }

    TEST_METHOD(DecodeGuardsItsArguments)
    {
        CMD5 md5;
        unsigned long out[2] = { 0xAAAAAAAA, 0xAAAAAAAA };
        const unsigned char in[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        MD5Probe::Decode(md5, NULL, in, 8);
        MD5Probe::Decode(md5, out, NULL, 8);
        MD5Probe::Decode(md5, out, in, 0);
        Assert::IsTrue(out[0] == 0xAAAAAAAA && out[1] == 0xAAAAAAAA);
        MD5Probe::Decode(md5, out, in, 8);
        Assert::IsTrue(out[0] == 0x04030201 && out[1] == 0x08070605);
    }

    TEST_METHOD(DigestsTheRfc1321Vectors)
    {
        char out[64];
        Assert::AreEqual("d41d8cd98f00b204e9800998ecf8427e", Digest("", out));
        Assert::AreEqual("900150983cd24fb0d6963f7d28e17f72", Digest("abc", out));
        Assert::AreEqual("f96b697d7cb7938d525a2f31aaf161d0", Digest("message digest", out));
    }

    TEST_METHOD(UpdateInPiecesEqualsOneUpdate)
    {
        char text[200];
        for (int i = 0; i < 199; i++) text[i] = (char) ('a' + i % 26);
        text[199] = 0;
        char whole[64], pieces[64];
        Digest(text, whole);
        CMD5 md5;
        md5.Update((const unsigned char*) text, 10);
        md5.Update((const unsigned char*) text + 10, 70);
        md5.Update((const unsigned char*) text + 80, 119);
        md5.Final(pieces);
        Assert::AreEqual(whole, pieces);
    }

    TEST_METHOD(FinalRefusesANullOutput)
    {
        CMD5 md5;
        Assert::IsFalse(md5.Final(NULL));
    }

    TEST_METHOD(MD5StringAppendsTheSalt)
    {
        Assert::AreEqual("ce58d22043b87fd07a93f113b79baa1c", MD5String("", 0));
        Assert::AreEqual("20aea26d2d8df0a20aa8d38179bed6e3", MD5String("abc", 3));
        Assert::AreEqual("20aea26d2d8df0a20aa8d38179bed6e3", MD5String("abc", 0));
        Assert::IsNull(MD5String(NULL, 0));
    }

    TEST_METHOD(MD5PasswordCopiesTheSaltedDigest)
    {
        char out[256] = "";
        MD5Password(out, "123456");
        Assert::AreEqual("80fba977d063a6f7262a8a9c95f61140", out);
    }
};
