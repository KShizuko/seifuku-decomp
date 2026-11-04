#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <type_traits>
#include "MyIni.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

struct MyIniProbe : CMyIni
{
    MyIniProbe() : CMyIni(false) {}
    static size_t Sections() { return offsetof(MyIniProbe, m_mapSection); }
    static size_t Iter() { return offsetof(MyIniProbe, m_iterSection); }
    static size_t ShowError() { return offsetof(MyIniProbe, m_bShowError); }
    static size_t File() { return offsetof(MyIniProbe, m_strFile); }
    BOOL Valid(char* s) { return this->IsValidLine(s); }
    BOOL Section(char* s, CMyString& out) { return this->ParseSection(s, out); }
    BOOL KeyValue(char* s, CMyString& k, CMyString& v) { return this->ParseKeyValue(s, k, v); }
};

static_assert(std::is_base_of<string, CMyString>::value && std::has_virtual_destructor<CMyString>::value, "");
static_assert(std::has_virtual_destructor<CMyIni>::value, "");
static_assert(std::is_same<decltype(static_cast<CMyIni* (*)(const char*, bool)>(&CMyIni::CreateNew)), CMyIni* (*)(const char*, bool)>::value, "");
static_assert(std::is_same<decltype(&CMyIni::GetInt), int (CMyIni::*)(const char*, const char*)>::value, "");
static_assert(std::is_same<decltype(&CMyIni::HasSection), BOOL (CMyIni::*)(const char*)>::value, "");
static_assert(std::is_same<decltype(static_cast<BOOL (CMyIni::*)(const char*)>(&CMyIni::Load)), BOOL (CMyIni::*)(const char*)>::value, "");

static const char* FixturePath(char* buf, size_t cb, const char* name)
{
    const char* file = __FILE__;
    const char* slash = strrchr(file, '\\');
    const char* slash2 = strrchr(file, '/');
    if (slash2 > slash) slash = slash2;
    size_t n = slash ? (size_t)(slash - file + 1) : 0;
    if (n + strlen("fixtures\\") + strlen(name) + 1 > cb) { buf[0] = 0; return buf; }
    memcpy(buf, file, n);
    buf[n] = 0;
    strcat(buf, "fixtures\\");
    strcat(buf, name);
    return buf;
}

TEST_CLASS(ClientMyIni)
{
public:
    TEST_METHOD(MembersFollowInOrder)
    {
        Assert::IsTrue(MyIniProbe::Sections() == 4);
        Assert::IsTrue(MyIniProbe::Iter() == 4 + sizeof(MAP_INISECTION));
        Assert::IsTrue(MyIniProbe::ShowError() == MyIniProbe::Iter() + sizeof(MAP_INISECTION::iterator));
        Assert::IsTrue(MyIniProbe::File() == MyIniProbe::ShowError() + 4);
        Assert::IsTrue(sizeof(CMyIni) == MyIniProbe::File() + sizeof(string));
    }
    TEST_METHOD(TheParsersReadALine)
    {
        MyIniProbe ini;
        char s1[] = "[Screen]";       CMyString sec;
        Assert::IsTrue(ini.Section(s1, sec) && sec == "Screen");
        char s2[] = "[Screen";
        Assert::IsFalse(ini.Section(s2, sec));
        char s3[] = "x = 12 ;c";      CMyString k, v;
        Assert::IsTrue(ini.KeyValue(s3, k, v) && k == "x" && v == "12");
        char s4[] = "key=";           CMyString k4, v4;
        Assert::IsTrue(ini.KeyValue(s4, k4, v4) && k4 == "key" && v4.length() == 0);
        char s5[] = "=5";
        Assert::IsFalse(ini.Valid(s5));
        char s6[] = ";x=1";
        Assert::IsFalse(ini.Valid(s6));
        char s7[] = "ab";
        Assert::IsFalse(ini.Valid(s7));
        Assert::IsTrue(CMyString("%d-%d", 800, 600) == "800-600");
        char s8[] = "[a%%b%s]";       CMyString sec8;
        Assert::IsTrue(ini.Section(s8, sec8) && sec8 == "a%%b%s");
        char s9[] = "k%d=v%%x";       CMyString k9, v9;
        Assert::IsTrue(ini.KeyValue(s9, k9, v9) && k9 == "k%d" && v9 == "v%%x");
    }
    TEST_METHOD(LoadAndLookUpTheFixture)
    {
        char path[MAX_PATH];
        CMyIni* pIni = CMyIni::CreateNew(FixturePath(path, sizeof(path), "myini.ini"), false);
        Assert::IsNotNull(pIni);
        Assert::IsTrue(pIni->HasSection("800-600"));
        Assert::IsFalse(pIni->HasSection("640-480"));
        Assert::IsTrue(pIni->GetInt("800-600", "y") == 20);
        Assert::IsTrue(pIni->GetInt("800-600", "h") == 40);
        Assert::IsTrue(pIni->GetInt("800-600", "zz") == 0);
        Assert::IsTrue(pIni->GetInt("1024-768", "x") == 100);
        Assert::IsTrue(pIni->GetInt("nowhere", "x") == 0);
        Assert::AreEqual("20", pIni->GetString("800-600", "y"));
        Assert::IsNull(pIni->GetString("800-600", "zz"));
        Assert::IsNull(pIni->GetString("nowhere", "x"));
        Assert::IsFalse(pIni->SearchSection("640-480"));
        Assert::IsTrue(pIni->SearchSection("1024-768"));
        Assert::IsTrue(pIni->GetContent("x") == "100");
        Assert::IsTrue(pIni->GetContent("zz").empty());
        Assert::IsTrue(pIni->SearchSection(0));
        Assert::IsFalse(pIni->GetContent("x").empty());
        delete pIni;
    }
};
