#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "NetMsg.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CNetMsg) == 0x404, "");
static_assert(_MAX_MSGSIZE == 1024, "");

static_assert(sizeof(CNetwork) == 0x1C, "");
static_assert(offsetof(CNetwork, m_dwMsgCount) == 4 && offsetof(CNetwork, m_dwBytesReceived) == 8 && offsetof(CNetwork, m_dwBytesSend) == 0xC, "");

namespace
{
    struct SocketProbe : CLoginSocket
    {
        static size_t OutBuf() { return offsetof(SocketProbe, m_bufOutput); }
        static size_t OutLen() { return offsetof(SocketProbe, m_nOutbufLen); }
        static size_t Sock() { return offsetof(SocketProbe, m_sockClient); }
        static size_t State() { return offsetof(SocketProbe, m_bState); }
        static size_t InBuf() { return offsetof(SocketProbe, m_bufInput); }
        static size_t InLen() { return offsetof(SocketProbe, m_nInbufLen); }
        static size_t InStart() { return offsetof(SocketProbe, m_nInbufStart); }
        static size_t SendEnc() { return offsetof(SocketProbe, m_cSendEncrypt); }
        static size_t RecvEnc() { return offsetof(SocketProbe, m_cRecvEncrypt); }
    };

    typedef CEncryptClient<0xFA, 0x0F, 0x13, 0x9D, 0xA4, 0x79, 0x6D, 0x62> Cipher;

    struct CipherProbe : Cipher
    {
        const unsigned char* T1() const { return m_cGlobalEncrypt.m_bufEncrypt1; }
        const unsigned char* T2() const { return m_cGlobalEncrypt.m_bufEncrypt2; }
        int Pos1() const { return m_nPos1; }
        int Pos2() const { return m_nPos2; }
    };

    void BuildTables(unsigned char* t1, unsigned char* t2)
    {
        unsigned n = 0x9D;
        for (int i = 0; i < 256; i++) { t1[i] = (unsigned char) n; n = (((0xFA * n) % 256 + 0x0F) * n + 0x13) % 256; }
        unsigned char m = 0x62;
        for (int i = 0; i < 256; i++) { t2[i] = m; m = (unsigned char) ((0x79 - 0x5C * m) * m + 0x6D); }
    }
}

static_assert(sizeof(CLoginSocket) == 0x12424, "");
static_assert(sizeof(Cipher) == 0x208, "");

TEST_CLASS(ClientNetworkHeaders)
{
public:
    TEST_METHOD(SocketMembersLieWhere4267PutsThem)
    {
        Assert::AreEqual<size_t>(0x0, SocketProbe::OutBuf());
        Assert::AreEqual<size_t>(0x2000, SocketProbe::OutLen());
        Assert::AreEqual<size_t>(0x2004, SocketProbe::Sock());
        Assert::AreEqual<size_t>(0x2008, SocketProbe::State());
        Assert::AreEqual<size_t>(0x200C, SocketProbe::InBuf());
        Assert::AreEqual<size_t>(0x1200C, SocketProbe::InLen());
        Assert::AreEqual<size_t>(0x12010, SocketProbe::InStart());
        Assert::AreEqual<size_t>(0x12014, SocketProbe::SendEnc());
        Assert::AreEqual<size_t>(0x1221C, SocketProbe::RecvEnc());
    }

    TEST_METHOD(CipherTablesMatchTheBinaryBuilder)
    {
        CipherProbe c;
        unsigned char t1[256], t2[256];
        BuildTables(t1, t2);
        Assert::AreEqual<int>(0x9D, c.T1()[0]);
        Assert::AreEqual<int>(0x62, c.T2()[0]);
        for (int i = 0; i < 256; i++)
        {
            Assert::AreEqual<int>(t1[i], c.T1()[i]);
            Assert::AreEqual<int>(t2[i], c.T2()[i]);
        }
        Assert::AreEqual(0, c.Pos1()); Assert::AreEqual(0, c.Pos2());
    }

    TEST_METHOD(EncryptXorsSwapsNibblesAndXorsAB)
    {
        CipherProbe c;
        unsigned char b[3] = { 0, 0x5A, 0xFF };
        const unsigned char* t1 = c.T1(); const unsigned char* t2 = c.T2();
        c.Encrypt(b, 3);
        for (int i = 0; i < 3; i++)
        {
            unsigned char x = (unsigned char) ((i == 0 ? 0 : i == 1 ? 0x5A : 0xFF) ^ t1[i] ^ t2[0]);
            unsigned char expect = (unsigned char) ((((x & 0x0F) << 4) + (x >> 4)) ^ 0xAB);
            Assert::AreEqual<int>(expect, b[i]);
        }
        Assert::AreEqual<int>(0x54, (unsigned char) ((((0x9D ^ 0x62) & 0x0F) << 4 | ((0x9D ^ 0x62) >> 4)) ^ 0xAB));
        Assert::AreEqual(3, c.Pos1()); Assert::AreEqual(0, c.Pos2());

        CipherProbe d;
        unsigned char big[256] = { 0 };
        d.Encrypt(big, 256, false);
        Assert::AreEqual(0, d.Pos1()); Assert::AreEqual(0, d.Pos2());
        d.Encrypt(big, 256);
        Assert::AreEqual(0, d.Pos1()); Assert::AreEqual(1, d.Pos2());
        d.Encrypt(big, 1);
        Assert::AreEqual(1, d.Pos1()); Assert::AreEqual(1, d.Pos2());
    }

    TEST_METHOD(ChangeCodeRekeysBothTables)
    {
        unsigned char t1[256], t2[256];
        BuildTables(t1, t2);
        CipherProbe c;
        c.Encrypt(t1, 0);
        const DWORD key = 0x12345678;
        c.ChangeCode(key);
        const DWORD key2 = key * key;
        for (int i = 0; i < 256; i += 4)
        {
            Assert::AreEqual<DWORD>(*(const DWORD*) &t1[i] ^ key, *(const DWORD*) (c.T1() + i));
            Assert::AreEqual<DWORD>(*(const DWORD*) &t2[i] ^ key2, *(const DWORD*) (c.T2() + i));
        }
        Assert::AreEqual(0, c.Pos1()); Assert::AreEqual(0, c.Pos2());
        c.ChangeCode(key);
        Assert::AreEqual(0, memcmp(t1, c.T1(), 256)); Assert::AreEqual(0, memcmp(t2, c.T2(), 256));
    }
};
