#pragma once
#include <windows.h>
#include <stdio.h>
#include <string>

namespace realmaps
{
	inline bool Enabled()
	{
		char sz[8] = "";
		return ::GetEnvironmentVariableA("SFK_REAL_MAPS", sz, sizeof(sz)) && strcmp(sz, "1") == 0;
	}

	inline bool ClearMapFolder(const std::string& strMap)
	{
		DWORD dwAttr = ::GetFileAttributesA(strMap.c_str());
		if(dwAttr == INVALID_FILE_ATTRIBUTES)
			return true;
		if(dwAttr & FILE_ATTRIBUTE_REPARSE_POINT)
			return ::RemoveDirectoryA(strMap.c_str()) != FALSE;
		std::string cmd = "cmd /c rmdir /s /q \"" + strMap + "\"";
		system(cmd.c_str());
		return ::GetFileAttributesA(strMap.c_str()) == INVALID_FILE_ATTRIBUTES;
	}

	inline std::string Use(const std::string& strRepo, const std::string& strRun, int& nDocs)
	{
		std::string strSrc = strRepo + "\\distributable\\GameServer";
		const char* files[] = { "\\ini\\GameMap.dat", "\\ini\\TerrainNpc.ini", "\\ini\\AdditiveSize.ini" };
		for(int i = 0; i < sizeof(files)/sizeof(files[0]); i++)
			if(!::CopyFileA((strSrc + files[i]).c_str(), (strRun + files[i]).c_str(), FALSE))
				return std::string("cannot copy ") + strSrc + files[i];
		if(::GetFileAttributesA((strSrc + "\\map").c_str()) == INVALID_FILE_ATTRIBUTES)
			return "no " + strSrc + "\\map";
		if(!ClearMapFolder(strRun + "\\map"))
			return "cannot clear " + strRun + "\\map";
		std::string cmd = "cmd /c mklink /J \"" + strRun + "\\map\" \"" + strSrc + "\\map\" >nul";
		system(cmd.c_str());
		if(!(::GetFileAttributesA((strRun + "\\map").c_str()) & FILE_ATTRIBUTE_REPARSE_POINT))
			return "cannot link " + strRun + "\\map";
		FILE* fp = fopen((strRun + "\\ini\\GameMap.dat").c_str(), "rb");
		nDocs = 0;
		if(fp)
		{
			fread(&nDocs, 4, 1, fp);
			fclose(fp);
		}
		return "";
	}
}
