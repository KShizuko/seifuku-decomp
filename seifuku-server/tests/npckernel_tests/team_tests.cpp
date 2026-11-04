#include <cstddef>
#include "CppUnitTest.h"
#include "windows.h"
#include "define.h"
#include "Team.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static_assert(sizeof(TeamMemberInfo) == 0x1C, "");
static_assert(offsetof(TeamMemberInfo, id) == 0x10, "");
static_assert(_MAX_TEAMAMOUNT == 5, "");

TEST_CLASS(TeamLayoutTests)
{
public:
	TEST_METHOD(MemberNameFitsTheRecord)
	{
		Assert::AreEqual((size_t)_MAX_NAMESIZE, sizeof(((TeamMemberInfo*)0)->szName));
	}
};
