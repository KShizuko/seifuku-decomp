#pragma warning(disable:4786)
#include "c3_datafile.h"
#include "TqPackage.h"

static CRITICAL_SECTION			g_csStringId;
static struct StringIdCriticalSectionInit
{
	StringIdCriticalSectionInit ()	{ InitializeCriticalSection ( &g_csStringId ); }
	~StringIdCriticalSectionInit ()	{ DeleteCriticalSection ( &g_csStringId ); }
} g_csStringIdInit;

static DWORD stringtoid ( const char* str )
{
	int i;
	unsigned int v;
	static unsigned m[70];
	EnterCriticalSection ( &g_csStringId );
	strncpy((char *)m,str,256);
	for (i=0;i<256/4 && m[i];i++) ;
	m[i++]=0x9BE74448,m[i++]=0x66F42C48;
	v=0xF4FA8928;

	__asm {
		mov esi,0x37A8470E		;x0=0x37A8470E
		mov edi,0x7758B42B		;y0=0x7758B42B
		xor ecx,ecx
_loop:
		mov ebx,0x267B0B11		;w=0x267B0B11
		rol v,1
		lea eax,m
		xor ebx,v

		mov eax,[eax+ecx*4]
		mov edx,ebx
		xor esi,eax
		xor edi,eax

		add edx,edi
		or edx,0x2040801		;a=0x2040801
		and edx,0xBFEF7FDF		;c=0xBFEF7FDF

		mov eax,esi
		mul edx
		adc eax,edx
		mov edx,ebx
		adc eax,0

		add edx,esi
		or edx,0x804021			;b=0x804021
		and edx,0x7DFEFBFF		;d=0x7DFEFBFF

		mov esi,eax
		mov eax,edi
		mul edx

		add edx,edx
		adc eax,edx
		jnc _skip
		add eax,2
_skip:
		inc ecx;
		mov edi,eax
		cmp ecx,i
		jnz _loop
		xor esi,edi
		mov v,esi
	}
	LeaveCriticalSection ( &g_csStringId );
	return v;
}

C3_CORE_DLL_API
DWORD string_id ( const char* filename )
{
	char buffer[256];
	int i;
	for (i=0;filename[i];i++) {
		if (filename[i]>='A' && filename[i]<='Z') buffer[i]=filename[i]+'a'-'A';
		else if (filename[i]=='\\') buffer[i]='/';
		else buffer[i]=filename[i];
	}
	buffer[i]=0;
	return stringtoid(buffer);
}

C3_CORE_DLL_API
void DataFile_Open ( const char *filename )
{
	TqPackagesOpen ( filename ) ;
}

C3_CORE_DLL_API
void DataFile_Close ()
{
	TqPackagesClose ( 0 ) ;
}

C3_CORE_DLL_API
BOOL DataFile_UnpackSearch ( const char *filename )
{
	return TqFCheck ( filename, FILETYPE_UNPACK ) ;
}

C3_CORE_DLL_API
BOOL DataFile_Search ( const char *filename, FILE *&file, DWORD &size )
{
	return TqFCheck ( filename, FILETYPE_PACK ) ;
}

C3_CORE_DLL_API
BOOL DataFile_Load ( const char *filename, void *&buffer, DWORD &size )
{
	return TqFDump ( filename, buffer, size ) == TQP_OK ;
}

C3_CORE_DLL_API
void DataFile_Free ( void *&buffer )
{
	TqFUndump ( buffer ) ;
}

C3_CORE_DLL_API
void* Common_OpenRes ( LPCSTR name, DWORD &size, int bPack )
{
	void *file ;
	if ( TqFOpen ( name, file ) != TQP_OK )
		return 0 ;
	size = TqFGetLen ( file ) ;
	return file ;
}

C3_CORE_DLL_API
void Common_CloseRes ( void *file )
{
	TqFClose ( file ) ;
}

C3_CORE_DLL_API
void Common_GetChunk ( void *file, ChunkHeader *chunk )
{
	TQFRead ( chunk, 1, sizeof ( ChunkHeader ), file );
}

C3_CORE_DLL_API
DWORD Common_ReadRes ( void *file, void *buffer, DWORD size )
{
	return TQFRead ( buffer, 1, size, file ) ;
}

C3_CORE_DLL_API
void Common_SeekRes ( void *file, int seek, int origin )
{
	TqFSeek ( file, seek, ( TQSEEKMODE ) origin );
}

C3_CORE_DLL_API
BOOL Common_IsEofRes ( void *file )
{
	return TqFIsEof ( file ) ;
}

C3_CORE_DLL_API
long Common_GetSeek ( void *file )
{
	return TqFTell ( file ) ;
}