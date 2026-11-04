#ifndef _c3_datafile_h_
#define _c3_datafile_h_

#ifdef	C3_CORE_DLL_EXPORTS
#define C3_CORE_DLL_API __declspec ( dllexport )
#else
#define C3_CORE_DLL_API __declspec ( dllimport )
#endif

#include "c3_common.h"

C3_CORE_DLL_API
DWORD string_id ( const char *filename );

C3_CORE_DLL_API
void DataFile_Open ( const char *filename );

C3_CORE_DLL_API
void DataFile_Close ();

C3_CORE_DLL_API
BOOL DataFile_UnpackSearch ( const char *filename );

C3_CORE_DLL_API
BOOL DataFile_Search ( const char *filename, FILE *&file, DWORD &size );

C3_CORE_DLL_API
BOOL DataFile_Load ( const char *filename, void *&buffer, DWORD &size );

C3_CORE_DLL_API
void DataFile_Free ( void *&buffer );

#endif
