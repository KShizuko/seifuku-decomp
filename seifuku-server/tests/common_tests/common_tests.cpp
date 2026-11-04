#include "CppUnitTest.h"
#include <windows.h>
#include <string.h>
#include "common.h"
#include "MyLock.h"
#include "BaseFunc.h"
#include "IniFile.h"
#include "StringManager.h"
#include "MyHeap.h"
#include "ThreadBase.h"
#include "TimeOut.h"
#include "NetStringPacker.h"
#include "VarType.h"
#include <stdlib.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(SafeCopyTests)
{
public:
	TEST_METHOD(CopiesWhenItFits)
	{
		char buf[8] = "xxxxxxx";
		Assert::IsTrue(SafeCopy(buf, "abc", sizeof(buf)));
		Assert::AreEqual("abc", buf);
	}

	TEST_METHOD(ZeroLengthMeansUnbounded)
	{
		char buf[8] = "xxxxxxx";
		Assert::IsTrue(SafeCopy(buf, "abcdef"));
		Assert::AreEqual("abcdef", buf);
	}

	TEST_METHOD(TruncatesAtTheBufferLengthAndReturnsFalse)
	{
		char buf[8] = "xxxxxxx";
		Assert::IsFalse(SafeCopy(buf, "abcd", 4));
		Assert::AreEqual("abc", buf);
		Assert::AreEqual('x', buf[4]);
	}

	TEST_METHOD(OneShorterThanTheBufferFits)
	{
		char buf[8] = "xxxxxxx";
		Assert::IsTrue(SafeCopy(buf, "abc", 4));
		Assert::AreEqual("abc", buf);
	}

	TEST_METHOD(EmptySourceGivesEmptyTarget)
	{
		char buf[8] = "xxxxxxx";
		Assert::IsTrue(SafeCopy(buf, "", sizeof(buf)));
		Assert::AreEqual("", buf);
	}
};

class CExposedCriticalSection : public CMyCriticalSection
{
public:
	CRITICAL_SECTION*	Crit()	{ return &m_xCrit; }
};

static DWORD WINAPI TryFromOtherThread(LPVOID p)
{
	CRITICAL_SECTION* pCrit = (CRITICAL_SECTION*)p;
	if(!::TryEnterCriticalSection(pCrit))
		return 0;
	::LeaveCriticalSection(pCrit);
	return 1;
}

static DWORD TryOnOtherThread(CRITICAL_SECTION* pCrit)
{
	HANDLE h = ::CreateThread(NULL, 0, TryFromOtherThread, pCrit, 0, NULL);
	::WaitForSingleObject(h, INFINITE);
	DWORD dw = 0;
	::GetExitCodeThread(h, &dw);
	::CloseHandle(h);
	return dw;
}

TEST_CLASS(MyLockTests)
{
public:
	TEST_METHOD(LayoutIsVtableAndCriticalSection)
	{
		static_assert(sizeof(CMyCriticalSection) == 0x1C, "");
		CExposedCriticalSection x;
		Assert::AreEqual(4, (int)((char*)x.Crit() - (char*)&x));
	}

	TEST_METHOD(ScopedLockHoldsUntilTheEndOfTheScope)
	{
		CExposedCriticalSection x;
		{
			CMySingleLock xLock(&x, true);
			Assert::AreEqual((DWORD)0, TryOnOtherThread(x.Crit()));
		}
		Assert::AreEqual((DWORD)1, TryOnOtherThread(x.Crit()));
	}
};

bool IsBetween(int nCurr, int nBegin, int nEnd);

TEST_CLASS(BaseFuncTimeTests)
{
public:
	TEST_METHOD(DateTimeFormatsLocalTime)
	{
		time_t t = 1208476800;
		tm x = *localtime(&t);
		char expected[32];
		sprintf(expected, "%04d-%02d-%02d %02d:%02d:%02d", x.tm_year+1900, x.tm_mon+1, x.tm_mday, x.tm_hour, x.tm_min, x.tm_sec);
		char buf[20];
		DateTime(buf, t);
		Assert::AreEqual(expected, buf);
		sprintf(expected, "%04d%02d%02d%02d%02d%02d", x.tm_year+1900, x.tm_mon+1, x.tm_mday, x.tm_hour, x.tm_min, x.tm_sec);
		char buf15[15];
		DateTimeStamp(buf15, t);
		Assert::AreEqual(expected, buf15);
	}

	TEST_METHOD(DateStampIsYyyymmdd)
	{
		time_t t = 1208476800;
		tm x = *localtime(&t);
		Assert::AreEqual((x.tm_year+1900)*10000 + (x.tm_mon+1)*100 + x.tm_mday, DateStamp(t));
	}

	TEST_METHOD(DateStampPassAddsDaysAcrossMonths)
	{
		Assert::AreEqual(20080501, DateStampPass(20080418, 13));
		Assert::AreEqual(20090101, DateStampPass(20081231, 1));
		Assert::AreEqual(20080301, DateStampPass(20080229, 1));
	}

	TEST_METHOD(RandGetUsesTwoCrtDraws)
	{
		::srand(1);
		int a = ::rand();
		int b = ::rand();
		::srand(1);
		Assert::AreEqual(((a << 15) + b) % 1000, RandGet(1000));
		Assert::AreEqual(0, RandGet(0));
		Assert::AreEqual(0, RandGet(-5));
	}

	TEST_METHOD(IsBetweenWrapsWhenBeginIsAfterEnd)
	{
		Assert::IsTrue(IsBetween(5, 1, 10));
		Assert::IsFalse(IsBetween(10, 1, 10));
		Assert::IsTrue(IsBetween(1, 1, 10));
		Assert::IsTrue(IsBetween(12, 11, 3));
		Assert::IsTrue(IsBetween(2, 11, 3));
		Assert::IsFalse(IsBetween(5, 11, 3));
	}

	TEST_METHOD(IsActiveTimeMatchesDayOfMonthAndTime)
	{
		time_t t = 1208476800;
		tm x = *localtime(&t);
		unsigned long nDaily = x.tm_hour*10000 + x.tm_min*100 + x.tm_sec;
		Assert::IsTrue(IsActiveTime(t, nDaily));
		Assert::IsFalse(IsActiveTime(t + 1, nDaily));
		Assert::IsTrue(IsActiveTime(t, x.tm_mday*100000000UL + nDaily));
		Assert::IsTrue(IsActiveTime(t, (((x.tm_wday+6)%7)+1)*1000000UL + nDaily));
	}
};

TEST_CLASS(BaseFuncLineTests)
{
public:
	TEST_METHOD(DdaLineXMajorRoundsHalfAway)
	{
		vector<POINT> v;
		DDALineEx(0, 0, 4, 2, v);
		Assert::AreEqual((size_t)4, v.size());
		int ex[] = { 1,1, 2,1, 3,2, 4,2 };
		for(int i = 0; i < 4; i++) { Assert::AreEqual(ex[i*2], (int)v[i].x); Assert::AreEqual(ex[i*2+1], (int)v[i].y); }
	}

	TEST_METHOD(DdaLineYMajorNegative)
	{
		vector<POINT> v;
		DDALineEx(5, 5, 4, 2, v);
		Assert::AreEqual((size_t)3, v.size());
		int ex[] = { 5,4, 4,3, 4,2 };
		for(int i = 0; i < 3; i++) { Assert::AreEqual(ex[i*2], (int)v[i].x); Assert::AreEqual(ex[i*2+1], (int)v[i].y); }
	}

	TEST_METHOD(DdaLineSamePointIsEmpty)
	{
		vector<POINT> v(3);
		DDALineEx(7, 7, 7, 7, v);
		Assert::AreEqual((size_t)0, v.size());
	}
};

TEST_CLASS(BaseFuncStringTests)
{
public:
	TEST_METHOD(StrStrChSkipsDoubleByteCharacters)
	{
		const char* s = "\xB0\x61" "bc" "abc";
		Assert::IsTrue(StrStrCh(s, "abc") == s + 4);
		Assert::IsTrue(StrStrCh(s, "xyz") == NULL);
		Assert::IsTrue(StrStrCh("ab", "abc") == NULL);
	}

	TEST_METHOD(StringCheckRejectsControlsAndLone0x80)
	{
		char a[] = "abc";		Assert::IsTrue(StringCheck(a) != FALSE);
		char b[] = "a\x01" "c";	Assert::IsFalse(StringCheck(b) != FALSE);
		char c[] = "a\x80";		Assert::IsFalse(StringCheck(c) != FALSE);
		char d[] = "a\r\nb";	Assert::IsFalse(StringCheck(d) != FALSE); Assert::IsTrue(StringCheck(d, true) != FALSE);
		char e[] = "a\xB0";		Assert::IsFalse(StringCheck(e) != FALSE);
	}

	TEST_METHOD(TalkStrCheckRejectsQuotesAndBackslash)
	{
		char a[] = "hello";		Assert::IsTrue(TalkStrCheck(a) != FALSE);
		char b[] = "it's";		Assert::IsFalse(TalkStrCheck(b) != FALSE);
		char c[] = "a\\b";		Assert::IsFalse(TalkStrCheck(c) != FALSE);
		char d[] = "法轮";		Assert::IsFalse(TalkStrCheck(d) != FALSE);
	}

	TEST_METHOD(NameStrCheckWordLists)
	{
		char ok[] = "Hero";			Assert::IsTrue(NameStrCheck(ok) != FALSE);
		char gm[] = "xGmx";			Assert::IsFalse(NameStrCheck(gm) != FALSE);
		char none[] = "NoNe1";		Assert::IsFalse(NameStrCheck(none) != FALSE);
		char guard[] = "guard";		Assert::IsFalse(NameStrCheck(guard) != FALSE);
		char fw[] = "a\xA3\xC7m";	Assert::IsFalse(NameStrCheck(fw) != FALSE);
		char pua[] = "a\xA3\xA0";	Assert::IsFalse(NameStrCheck(pua) != FALSE);
		char none2[] = "无";		Assert::IsFalse(NameStrCheck(none2) != FALSE);
		char nonex[] = "无敌";		Assert::IsTrue(NameStrCheck(nonex) != FALSE);
		char sp[] = "a b";			Assert::IsFalse(NameStrCheck(sp) != FALSE);
		char bracket[] = "a]";		Assert::IsFalse(NameStrCheck(bracket) != FALSE);
		char caret[] = "a^";		Assert::IsTrue(NameStrCheck(caret) != FALSE);
	}

	TEST_METHOD(DumpBufferGroupsBytes)
	{
		unsigned char buf[20];
		for(int i = 0; i < 20; i++) buf[i] = (unsigned char)(i * 17);
		String s = DumpBuffer((const char*)buf, 20);
		Assert::AreEqual("00112233-44556677-8899aabb-ccddeeff 10213243", (const char*)s);
	}
};

TEST_CLASS(BaseTypeTests)
{
public:
	TEST_METHOD(StringWrapsStdString)
	{
		String a("ab");
		String b = a + String("cd");
		b += 'e';
		Assert::AreEqual("abcde", (const char*)b);
		Assert::IsTrue(String("a") < String("b"));
		Assert::AreEqual(-12L, String("-12").ConvertLong());
		Assert::AreEqual("-2147483648", (const char*)String().FormatLong(-2147483647L - 1));
	}
};

static const char* Fixture(const char* pszName)
{
	static char buf[MAX_PATH];
	strcpy(buf, __FILE__);
	char* p = strrchr(buf, '\\');
	strcpy(p + 1, "fixtures\\");
	strcat(buf, pszName);
	return buf;
}

TEST_CLASS(IniFileTests)
{
public:
	TEST_METHOD(ReadsAStringTrimmedAtBothEnds)
	{
		CIniFile ini(Fixture("sample.ini"), "Section");
		char buf[64];
		Assert::IsTrue(ini.GetString(buf, "Name", sizeof(buf)));
		Assert::AreEqual("Hello World", buf);
		Assert::IsTrue(ini.GetString(buf, "Tab", sizeof(buf)));
		Assert::AreEqual("x", buf);
	}

	TEST_METHOD(ReadsAnInteger)
	{
		CIniFile ini(Fixture("sample.ini"), "Section");
		Assert::AreEqual(42, ini.GetInt("Num"));
	}

	TEST_METHOD(SectionAndKeyAreCaseInsensitive)
	{
		CIniFile ini(Fixture("sample.ini"), "sEcTiOn");
		Assert::AreEqual(42, ini.GetInt("nUM"));
	}

	TEST_METHOD(AKeyMustEndAtBlankOrEquals)
	{
		CIniFile ini(Fixture("sample.ini"), "Section");
		char buf[64];
		Assert::IsFalse(ini.GetString(buf, "Nam", sizeof(buf)));
		Assert::AreEqual("", buf);
	}

	TEST_METHOD(TheSearchStopsAtTheNextSection)
	{
		CIniFile ini(Fixture("sample.ini"), "Section");
		Assert::AreEqual(0, ini.GetInt("OnlyNext"));
		ini.SetSection("Next");
		Assert::AreEqual(7, ini.GetInt("OnlyNext"));
	}

	TEST_METHOD(TheBufferSizeTruncates)
	{
		CIniFile ini(Fixture("sample.ini"), "Section");
		char buf[6];
		Assert::IsTrue(ini.GetString(buf, "Name", sizeof(buf)));
		Assert::AreEqual("Hello", buf);
	}

	TEST_METHOD(AMissingFileGivesFalse)
	{
		CIniFile ini(Fixture("absent.ini"), "Section");
		char buf[8];
		Assert::IsFalse(ini.GetString(buf, "Name", sizeof(buf)));
		Assert::AreEqual(0, ini.GetInt("Num"));
	}
};

TEST_CLASS(StringManagerTests)
{
public:
	TEST_METHOD(ReadsTheStringFileNamedInShellIni)
	{
		char szOld[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH, szOld);
		char szDir[MAX_PATH];
		strcpy(szDir, Fixture(""));
		::SetCurrentDirectory(szDir);
		{
			CStringManager mgr;
			Assert::AreEqual("Replaced", mgr.GetStr(std::string("KEY1")));
			Assert::AreEqual("  spaced value  ", mgr.GetStr(std::string("KEY2")));
			Assert::AreEqual("a=b", mgr.GetStr(std::string("KEY3")));
			Assert::AreEqual("", mgr.GetStr(std::string(";KEYX")));
			Assert::AreEqual("", mgr.GetStr(std::string("KEY9")));
		}
		::SetCurrentDirectory(szOld);
	}
};

class CExposedHeap : public CMyHeap
{
public:
	CExposedHeap() : CMyHeap("CExposedHeap") {}
	long	Count()		{ return m_lNumAlloc; }
};

TEST_CLASS(MyHeapTests)
{
public:
	TEST_METHOD(AllocZeroesAndCounts)
	{
		CExposedHeap heap;
		unsigned char* p = (unsigned char*)heap.Alloc(64);
		Assert::IsNotNull(p);
		for(int i = 0; i < 64; i++)
			Assert::AreEqual(0, (int)p[i]);
		Assert::AreEqual(1L, heap.Count());
		heap.Free(p);
		Assert::AreEqual(0L, heap.Count());
	}

	TEST_METHOD(ZeroSizeAndNullAreNoOps)
	{
		CExposedHeap heap;
		Assert::IsNull(heap.Alloc(0));
		Assert::AreEqual(0L, heap.Count());
		heap.Free(NULL);
		Assert::AreEqual(0L, heap.Count());
	}
};

class CCountingThread : public CThreadBase
{
public:
	CCountingThread() : m_nInit(0), m_nProcess(0), m_nDestroy(0), m_nStopAfter(-1) {}
	volatile long	m_nInit, m_nProcess, m_nDestroy, m_nStopAfter;
protected:
	virtual	void	OnInit()		{ m_nInit++; }
	virtual bool	OnProcess()		{ m_nProcess++; ::Sleep(1); return m_nProcess != m_nStopAfter; }
	virtual void	OnDestroy()		{ m_nDestroy++; }
};

TEST_CLASS(ThreadBaseTests)
{
public:
	TEST_METHOD(CloseThreadStopsTheLoop)
	{
		CCountingThread t;
		Assert::IsTrue(t.CreateThread());
		while(t.m_nProcess < 3)
			::Sleep(1);
		Assert::IsTrue(t.CloseThread(INFINITE));
		Assert::AreEqual(1L, (long)t.m_nInit);
		Assert::AreEqual(1L, (long)t.m_nDestroy);
		Assert::IsTrue(t.CloseThread());
	}

	TEST_METHOD(OnProcessFalseEndsTheThread)
	{
		CCountingThread t;
		t.m_nStopAfter = 2;
		Assert::IsTrue(t.CreateThread());
		Assert::IsTrue(t.CloseThread(INFINITE));
		Assert::AreEqual(1L, (long)t.m_nDestroy);
	}

	TEST_METHOD(ASuspendedThreadRunsAfterResume)
	{
		CCountingThread t;
		Assert::IsTrue(t.CreateThread(false));
		::Sleep(20);
		Assert::AreEqual(0L, (long)t.m_nInit);
		Assert::IsTrue(t.ResumeThread());
		while(t.m_nProcess < 1)
			::Sleep(1);
		Assert::IsTrue(t.CloseThread(INFINITE));
	}
};

TEST_CLASS(TimeOutTests)
{
public:
	TEST_METHOD(MillisecondClockRunsOnTimeGet)
	{
		CTimeOutMS t;
		Assert::IsFalse(t.IsActive());
		t.Startup(100000);
		Assert::IsTrue(t.IsActive());
		Assert::IsFalse(t.IsTimeOut());
		Assert::IsFalse(t.ToNextTime());
		Assert::IsTrue(t.IsTimeOut(0));
		Assert::IsTrue(t.GetRemain() > 99000 && t.GetRemain() <= 100000);
		t.Clear();
		Assert::IsFalse(t.IsActive());
	}

	struct AgedClock : public CTimeOut { void Age(int nSecs) { m_tUpdateTime -= nSecs; } };
	TEST_METHOD(SecondClockRemain)
	{
		AgedClock t;
		Assert::AreEqual(0, t.GetRemain());
		t.Startup(10);
		t.Age(3);
		int nRemain = t.GetRemain();
		Assert::IsTrue(nRemain == 7 || nRemain == 6);
	}

	TEST_METHOD(MillisecondTickAccumulates)
	{
		CTimeOutMS t;
		t.Startup(10);
		::Sleep(15);
		Assert::IsTrue(t.ToNextTick(10));
		Assert::IsFalse(t.ToNextTick(100000));
	}

	TEST_METHOD(SecondClockRunsOnTimeGet)
	{
		CTimeOut t;
		t.Startup(1000);
		Assert::IsFalse(t.IsTimeOut());
		Assert::IsTrue(t.IsTimeOut(0));
		Assert::IsFalse(t.ToNextTick(1000));
		Assert::IsFalse(t.TimeOver());
		Assert::AreEqual(1000, t.GetInterval());
	}
};

TEST_CLASS(NetStringPackerTests)
{
public:
	TEST_METHOD(PacksCountLengthAndBytes)
	{
		char buf[32];
		CNetStringPacker packer;
		packer.SetBuf(buf, sizeof(buf));
		Assert::IsTrue(packer.AddString("ab") != FALSE);
		Assert::IsTrue(packer.AddString(NULL) != FALSE);
		Assert::IsTrue(packer.AddString("xyz") != FALSE);
		Assert::AreEqual(3u, packer.GetStrAmount());
		Assert::AreEqual(1u + 3 + 1 + 4, packer.GetSize());
		static const unsigned char expected[] = { 3, 2, 'a', 'b', 0, 3, 'x', 'y', 'z' };
		Assert::AreEqual(0, memcmp(buf, expected, sizeof(expected)));
		char s[8];
		Assert::IsTrue(packer.GetString(2, s, sizeof(s)) != FALSE);
		Assert::AreEqual("xyz", s);
		Assert::IsTrue(packer.GetString(1, s, sizeof(s)) != FALSE);
		Assert::AreEqual("", s);
		Assert::IsFalse(packer.GetString(3, s, sizeof(s)) != FALSE);
		Assert::IsFalse(packer.GetString(2, s, 3) != FALSE);
	}

	TEST_METHOD(RefusesWithoutBufferOrOverLong)
	{
		CNetStringPacker packer;
		Assert::IsFalse(packer.AddString("a") != FALSE);
		char buf[8];
		packer.SetBuf(buf, sizeof(buf));
		Assert::IsFalse(packer.AddString("0123456789") != FALSE);
		char big[300];
		memset(big, 'a', 256); big[256] = 0;
		Assert::IsFalse(packer.AddString(big) != FALSE);
	}
};

TEST_CLASS(VarTypeTests)
{
public:
	TEST_METHOD(AssignmentCopiesTheString)
	{
		VarType a("abc");
		VarType b(5);
		b = a;
		Assert::IsTrue(b.IsStringType());
		Assert::AreEqual("abc", (const char*)(const String&)b);
		VarType c(7);
		b = c;
		Assert::IsTrue(b.IsIntType());
		Assert::AreEqual(7, (int)b);
	}

	TEST_METHOD(EqualityNeedsTheSameKind)
	{
		Assert::IsTrue(VarType("x") == VarType("x"));
		Assert::IsFalse(VarType("x") == VarType("y"));
		Assert::IsTrue(VarType(3) == VarType(3));
		Assert::IsFalse(VarType(3) == VarType(3, VarType::TYPE_OBJID));
		Assert::IsFalse(VarType() == VarType());
		Assert::IsFalse(VarType("x", VarType::TYPE_SYMBOL) == VarType("x", VarType::TYPE_SYMBOL));
	}
};

TEST_CLASS(ArrayTests)
{
public:
	TEST_METHOD(IteratesAndDeletesDuringIteration)
	{
		Array<int> set;
		for(int i = 0; i < 5; i++)
			set.Push(i);
		Assert::AreEqual(5, set.Size());
		int nSum = 0;
		for(Array<int>::Iterator it = set.NewEnum(); it.Next(); )
		{
			if(*(int*)it % 2)
				set.DelObj(it);
			else
				nSum += *(int*)it;
		}
		Assert::AreEqual(3, set.Size());
		Assert::AreEqual(0 + 2 + 4, nSum);
		Assert::AreEqual(4, set[2]);
	}
};
