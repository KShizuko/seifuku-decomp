#include "CppUnitTest.h"
#include <windows.h>
#include <cstring>
#include <string>
#include "StrRes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
    struct StrResCwd
    {
        char old[MAX_PATH];
        StrResCwd()
        {
            GetCurrentDirectoryA(MAX_PATH, old);
            std::string f = __FILE__;
            SetCurrentDirectoryA((f.substr(0, f.find_last_of("\\/")) + "\\fixtures").c_str());
        }
        ~StrResCwd() { SetCurrentDirectoryA(old); }
    };
}

TEST_CLASS(GraphicDataStrRes)
{
public:
    TEST_METHOD(InitReadsTheTableAndGetStrAnswers)
    {
        StrResCwd cwd;
        CStrRes res;
        Assert::AreEqual("SE:100", res.GetStr(100));
        res.Init();
        Assert::AreEqual("Hello", res.GetStr(100));
        Assert::AreEqual("two words = and a sign", res.GetStr(200));
        Assert::AreEqual("", res.GetStr(300));
        Assert::AreEqual("SE:999", res.GetStr(999));
        Assert::AreEqual("SE:0", res.GetStr(0));
        const char* missing = res.GetStr(4294967295u);
        Assert::AreEqual("SE:4294967295", missing);
        Assert::IsTrue(missing == res.GetStr(1));
        Assert::AreEqual("SE:1", missing);
        res.Init();
        Assert::AreEqual("Hello", res.GetStr(100));
        Assert::AreEqual("two words = and a sign", res.GetStr(200));
        res.Destroy();
        Assert::AreEqual("SE:100", res.GetStr(100));
        res.Init();
        Assert::AreEqual("Hello", res.GetStr(100));
    }
};
