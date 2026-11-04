#include "CppUnitTest.h"
#include <cstddef>
#include <cstring>
#include <cwchar>
#include "dmutil.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(CMusicManager) == 0x0C, "");
static_assert(sizeof(CMusicSegment) == 0x18, "");

struct MusicManagerProbe : CMusicManager
{
    static void Check()
    {
        static_assert(offsetof(MusicManagerProbe, m_bCleanupCOM) == 0x0, "");
        static_assert(offsetof(MusicManagerProbe, m_pLoader)     == 0x4, "");
        static_assert(offsetof(MusicManagerProbe, m_pPerformance)== 0x8, "");
    }
};

struct MusicSegmentProbe : CMusicSegment
{
    MusicSegmentProbe() : CMusicSegment(NULL, NULL, NULL) {}
    static void Check()
    {
        static_assert(offsetof(MusicSegmentProbe, m_pSegment)           == 0x04, "");
        static_assert(offsetof(MusicSegmentProbe, m_pLoader)            == 0x08, "");
        static_assert(offsetof(MusicSegmentProbe, m_pPerformance)       == 0x0C, "");
        static_assert(offsetof(MusicSegmentProbe, m_pEmbeddedAudioPath) == 0x10, "");
        static_assert(offsetof(MusicSegmentProbe, m_bDownloaded)        == 0x14, "");
    }
};

TEST_CLASS(DmutilHeader)
{
public:
    TEST_METHOD(ObjectSizesMatchBinary)
    {
        Assert::AreEqual<size_t>(0x0C, sizeof(CMusicManager));
        Assert::AreEqual<size_t>(0x18, sizeof(CMusicSegment));
    }
};

VOID DXUtil_ConvertAnsiStringToWide(WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar);
VOID DXUtil_ConvertGenericStringToWide(WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar);

TEST_CLASS(DmutilStrings)
{
public:
    TEST_METHOD(AnsiToWideConvertsAndTerminates)
    {
        WCHAR w[8];
        memset(w, 0xEE, sizeof(w));
        DXUtil_ConvertAnsiStringToWide(w, "abc", -1);
        Assert::AreEqual(0, wcscmp(L"abc", w));
        Assert::AreEqual<int>(0xEEEE, w[4]);
    }

    TEST_METHOD(AnsiToWideTruncatesToDestinationSize)
    {
        WCHAR w[4];
        DXUtil_ConvertAnsiStringToWide(w, "abcdef", 4);
        Assert::AreEqual(0, wcscmp(L"abc", w));
    }

    TEST_METHOD(ConvertersIgnoreNullArguments)
    {
        WCHAR w[4] = { L'x', 0, 0, 0 };
        DXUtil_ConvertAnsiStringToWide(NULL, "a", -1);
        DXUtil_ConvertAnsiStringToWide(w, NULL, -1);
        DXUtil_ConvertGenericStringToWide(w, "a", 0);
        Assert::AreEqual<int>(L'x', w[0]);
    }

    TEST_METHOD(GenericForwardsToAnsi)
    {
        WCHAR w[8];
        DXUtil_ConvertGenericStringToWide(w, "zf", -1);
        Assert::AreEqual(0, wcscmp(L"zf", w));
    }
};

TEST_CLASS(MusicManagerWithoutRuntime)
{
public:
    TEST_METHOD(ConstructorLeavesObjectsNull)
    {
        CMusicManager m;
        Assert::IsNull(m.GetLoader());
        Assert::IsNull(m.GetPerformance());
    }

    TEST_METHOD(SetSearchDirectoryWithoutLoaderIsUnexpected)
    {
        CMusicManager m;
        Assert::AreEqual<long>(E_UNEXPECTED, m.SetSearchDirectory(TEXT("Sound")));
    }

    TEST_METHOD(DefaultAudioPathWithoutPerformanceIsNull)
    {
        CMusicManager m;
        Assert::IsNull(m.GetDefaultAudioPath());
        m.CollectGarbage();
    }
};
