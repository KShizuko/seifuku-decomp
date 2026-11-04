#include "stdafx.h"
#include "WebBrowser.h"
#include "CppUnitTest.h"
#include <cstddef>
#include <type_traits>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(void*) == 4, "");

static_assert(sizeof(CWebBrowser) == sizeof(CWnd), "");
static_assert(std::is_base_of<CWnd, CWebBrowser>::value, "");
static_assert(std::is_polymorphic<CWebBrowser>::value, "");
static_assert(std::is_same<decltype(&CWebBrowser::Refresh2), void (CWebBrowser::*)(VARIANT*)>::value, "");
static_assert(std::is_same<decltype(&CWebBrowser::GetLocationURL), CString (CWebBrowser::*)()>::value, "");

struct WebBrowserProbe
{
    static CLSID const& Clsid(CWebBrowser& w) { return w.GetClsid(); }
};

TEST_CLASS(ClientWebBrowserHeader)
{
public:
    TEST_METHOD(TheRuntimeClassIsTheBinarys)
    {
        CRuntimeClass* pClass = RUNTIME_CLASS(CWebBrowser);
        Assert::IsTrue(0 == strcmp(pClass->m_lpszClassName, "CWebBrowser"));
        Assert::IsTrue(pClass->m_nObjectSize == (int)sizeof(CWebBrowser));
        Assert::IsTrue(pClass->m_wSchema == 0xFFFF);
        Assert::IsTrue(pClass->m_pfnCreateObject != NULL);
        Assert::IsTrue(pClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
    }

    TEST_METHOD(CreateObjectMakesAWebBrowser)
    {
        CObject* pObj = RUNTIME_CLASS(CWebBrowser)->CreateObject();
        Assert::IsTrue(pObj != NULL);
        Assert::IsTrue(pObj->IsKindOf(RUNTIME_CLASS(CWebBrowser)));
        Assert::IsTrue(pObj->IsKindOf(RUNTIME_CLASS(CWnd)));
        delete pObj;
    }

    TEST_METHOD(TheClsidIsTheWebBrowserControls)
    {
        CWebBrowser w;
        const CLSID& c = WebBrowserProbe::Clsid(w);
        Assert::IsTrue(c.Data1 == 0xEAB22AC3);
        Assert::IsTrue(c.Data2 == 0x30C1);
        Assert::IsTrue(c.Data3 == 0x11CF);
        static const BYTE d4[8] = { 0xA7, 0xEB, 0x00, 0x00, 0xC0, 0x5B, 0xAE, 0x0B };
        Assert::IsTrue(0 == memcmp(c.Data4, d4, 8));
    }
};
