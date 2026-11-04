#include "CppUnitTest.h"
#include <windows.h>
#include <string.h>
#include <mysql.h>
#include <mariadb_version.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ScaffoldTests)
{
public:
	TEST_METHOD(PointersAreFourBytes)
	{
		static_assert(sizeof(void*) == 4, "");
		Assert::AreEqual((size_t)4, sizeof(void*));
	}

	TEST_METHOD(LiteralsReachTheBinaryAsGbk)
	{
		static const unsigned char expected[] = { 0xD5, 0xF7, 0xB7, 0xFE, 0x00 };
		const char* title = "征服";
		Assert::AreEqual((size_t)4, strlen(title));
		Assert::AreEqual(0, memcmp(title, expected, sizeof(expected)));
	}

	TEST_METHOD(CharacterSetIsMbcs)
	{
#if defined(_UNICODE) || defined(UNICODE) || !defined(_MBCS)
		Assert::Fail(L"_MBCS must be defined and _UNICODE must not");
#endif
		Assert::AreEqual((size_t)1, sizeof(TCHAR));
	}

	TEST_METHOD(ConnectorIsLinkedStatically)
	{
		Assert::AreEqual("3.3.19", MARIADB_PACKAGE_VERSION);
		Assert::AreEqual("3.3.19", mysql_get_client_info());
	}
};
