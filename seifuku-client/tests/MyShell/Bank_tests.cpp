#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include "Bank.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct BankProbe : CBank
{
    static size_t Id() { return offsetof(BankProbe, m_idBank); }
    static size_t Money() { return offsetof(BankProbe, m_dwMoney); }
};

static_assert(sizeof(CBank) == 0xC, "");
static_assert(std::has_virtual_destructor<CBank>::value, "");

TEST_CLASS(ClientBankHeader)
{
public:
    TEST_METHOD(IdThenMoney)
    {
        Assert::IsTrue(BankProbe::Id() == 0x4);
        Assert::IsTrue(BankProbe::Money() == 0x8);
    }
};

TEST_CLASS(ClientBankBodies)
{
public:
    TEST_METHOD(ConstructedBankIsClosed)
    {
        CBank bank;
        Assert::AreEqual((OBJID) ID_NONE, bank.GetId());
        Assert::AreEqual((DWORD) 0, bank.GetMoney());
        bank.Open(17);
        bank.SetMoney(250);
        Assert::AreEqual((OBJID) 17, bank.GetId());
        Assert::AreEqual((DWORD) 250, bank.GetMoney());
        bank.Close();
        Assert::AreEqual((OBJID) ID_NONE, bank.GetId());
        Assert::AreEqual((DWORD) 0, bank.GetMoney());
    }
};
