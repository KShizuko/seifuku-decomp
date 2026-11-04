#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/share/define.h"
#include "../../src/MsgServer/MapGroupKernel/Enemy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mapgroup_enemy_tests
{
	TEST_CLASS(MapGroupEnemyTests)
	{
	public:
		TEST_METHOD(EnemyInfoLayout)
		{
			Assert::AreEqual(10, MAX_ENEMYSIZE);
			Assert::AreEqual((size_t)0xC8, sizeof(CEnemyInfoStruct));
			Assert::AreEqual((size_t)0x00, offsetof(CEnemyInfoStruct, setEnemy));
			Assert::AreEqual((size_t)0x28, offsetof(CEnemyInfoStruct, setName));
		}
	};
}
