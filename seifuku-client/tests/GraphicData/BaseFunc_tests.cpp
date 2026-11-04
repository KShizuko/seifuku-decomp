#include "CppUnitTest.h"
#include <windows.h>
#include <cstring>
#include <string>
#include "BaseFunc.h"
#include "MyIni.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
    std::string Fixture(const char* name)
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures\\" + name;
    }
}

TEST_CLASS(GraphicDataBaseFunc)
{
public:
    TEST_METHOD(TimeGetIsTheMultimediaClock)
    {
        DWORD before = timeGetTime();
        DWORD t = TimeGet();
        DWORD after = timeGetTime();
        Assert::IsTrue(t - before <= after - before);
    }

    TEST_METHOD(IniDataGetReadsANumberOfASection)
    {
        std::string ini = Fixture("gdbase.ini");
        int n = -1;
        Assert::IsTrue(IniDataGet(ini.c_str(), "Section", "Number", n) != FALSE);
        Assert::AreEqual(42, n);
        Assert::IsTrue(IniDataGet(ini.c_str(), "other", "Number", n) != FALSE);
        Assert::AreEqual(7, n);
        n = -1;
        Assert::IsFalse(IniDataGet(0, "Section", "Number", n) != FALSE);
        Assert::IsFalse(IniDataGet(ini.c_str(), 0, "Number", n) != FALSE);
        Assert::IsFalse(IniDataGet(ini.c_str(), "Section", 0, n) != FALSE);
        Assert::IsFalse(IniDataGet("no-such-file.ini", "Section", "Number", n) != FALSE);
        Assert::AreEqual(-1, n);
        Assert::IsFalse(IniDataGet(ini.c_str(), "Section", "Missing", n, FALSE) != FALSE);
        Assert::AreEqual(-1, n);
    }

    TEST_METHOD(MyIniGetStringInTheModule)
    {
        std::string ini = Fixture("gdbase.ini");
        CMyIni* pIni = CMyIni::CreateNew(ini.c_str(), false);
        Assert::IsNotNull(pIni);
        Assert::AreEqual("Value", pIni->GetString("Section", "Key"));
        Assert::AreEqual(42, pIni->GetInt("Section", "Number"));
        Assert::IsNull(pIni->GetString("Section", "Missing"));
        Assert::IsNull(pIni->GetString("Nowhere", "Key"));
        delete pIni;
    }
};
