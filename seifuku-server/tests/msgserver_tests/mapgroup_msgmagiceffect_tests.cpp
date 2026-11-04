#include "CppUnitTest.h"
#include <windows.h>
#include <stddef.h>
#include "../../src/MsgServer/MapGroupKernel/Network/MsgMagicEffect.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace
{
	struct MagicEffectProbe : public CMsgMagicEffect
	{
		typedef CMsgMagicEffect::MSG_Info	Body;
	};
	typedef MagicEffectProbe::Body	Body;
}

namespace mapgroup_msgmagiceffect_tests
{
	TEST_CLASS(MapGroupMsgMagicEffectTests)
	{
	public:
		TEST_METHOD(MagicEffectLayout)
		{
			Assert::AreEqual((size_t)0x00, offsetof(Body, idUser));
			Assert::AreEqual((size_t)0x04, offsetof(Body, idTarget));
			Assert::AreEqual((size_t)0x04, offsetof(Body, usPosX));
			Assert::AreEqual((size_t)0x06, offsetof(Body, usPosY));
			Assert::AreEqual((size_t)0x08, offsetof(Body, usType));
			Assert::AreEqual((size_t)0x0A, offsetof(Body, usLevel));
			Assert::AreEqual((size_t)0x0C, offsetof(Body, ucEffectNum));
			Assert::AreEqual((size_t)0x10, offsetof(Body, setEffectRole));
			Assert::AreEqual((size_t)0x18, sizeof(Body));
		}
		TEST_METHOD(EffectRoleLayout)
		{
			Assert::AreEqual((size_t)0x00, offsetof(EffectRoleStruct, idRole));
			Assert::AreEqual((size_t)0x04, offsetof(EffectRoleStruct, dwData));
			Assert::AreEqual((size_t)0x08, sizeof(EffectRoleStruct));
		}
	};
}
