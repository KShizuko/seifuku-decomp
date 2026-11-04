#pragma once

#include <windows.h>
#include <stdio.h>
#include <string>
#include "CppUnitTest.h"
#include "c3_common.h"

struct C3DataFileIndex {
    DWORD uid;
    DWORD offset;
    DWORD size;
    DWORD space;
};

struct C3DataFileHeader {
    DWORD id;
    int number;
    unsigned offset;
};

namespace SeifukuFixtures
{
    inline std::string Dir()
    {
        std::string f = __FILE__;
        return f.substr(0, f.find_last_of("\\/")) + "\\fixtures";
    }

    inline void WriteBlob(const std::string& path, const void* data, DWORD size)
    {
        HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(h != INVALID_HANDLE_VALUE, L"fixture write");
        DWORD w = 0;
        WriteFile(h, data, size, &w, 0);
        CloseHandle(h);
    }

    inline void* OpenC3(const char* name)
    {
        DWORD size = 0;
        void* f = Common_OpenRes(name, size, 1);
        if (f) Common_SeekRes(f, 16, SEEK_CUR);
        return f;
    }

    struct CwdGuard
    {
        char old[MAX_PATH];
        CwdGuard(const std::string& dir) { GetCurrentDirectoryA(MAX_PATH, old); SetCurrentDirectoryA(dir.c_str()); }
        ~CwdGuard() { SetCurrentDirectoryA(old); }
    };
}
