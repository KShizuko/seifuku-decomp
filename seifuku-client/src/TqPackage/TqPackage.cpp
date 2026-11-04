#pragma warning(disable:4786)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <string>
#include <vector>
#include <map>
#include "TqPackage.h"

struct TqPackHeader {
	DWORD id;
	int number;
	unsigned offset;
};

struct TqPackIndex {
	DWORD uid;
	DWORD offset;
	DWORD size;
	DWORD space;
};

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

static bool NormalizeName ( char *buffer, const char *filename )
{
	if ( !buffer )
		return false ;
	if ( !filename || !filename[0] )
	{
		buffer[0] = 0 ;
		return false ;
	}
	int len = strlen ( filename ) ;
	int i ;
	for ( i=0; i<len; i++ )
	{
		if ( filename[i] >= 'A' && filename[i] <= 'Z' )
			buffer[i] = filename[i] + 'a' - 'A' ;
		else if ( filename[i] == '\\' )
			buffer[i] = '/' ;
		else
			buffer[i] = filename[i] ;
	}
	buffer[i] = 0 ;
	return true ;
}

static void NormalizePath ( char *path )
{
	int len = strlen ( path ) ;
	for ( int i=0; i<len; i++ )
	{
		if ( path[i] >= 'A' && path[i] <= 'Z' )
			path[i] = path[i] + 'a' - 'A' ;
		else if ( path[i] == '\\' )
			path[i] = '/' ;
	}
}

static void PackageName ( char *buffer, const char *filename )
{
	int i = 0 ;
	if ( !filename[0] )
	{
		buffer[0] = 0 ;
		return ;
	}
	do
	{
		char c = filename[i] ;
		if ( c == '/' || c == 0 )
			break ;
		if ( c >= 'A' && c <= 'Z' )
			c = c + 'a' - 'A' ;
		buffer[i] = c ;
		i++ ;
	}
	while ( filename[i] ) ;
	buffer[i] = 0 ;
}

static DWORD PackageId ( const char *filename )
{
	char buffer[260] ;
	PackageName ( buffer, filename ) ;
	if ( strlen ( buffer ) == 0 )
		return 0 ;
	return stringtoid ( buffer ) ;
}

class ITqPack
{
public:
	virtual int			Release		() = 0 ;
	virtual bool		Check		( const char *filename, DWORD *size ) = 0 ;
	virtual int			Reserved2	( int ) = 0 ;
	virtual TQPRESULT	Open		( const char *filename, int ) = 0 ;
	virtual TQPRESULT	Load		( const char *filename, void *buffer, DWORD *size ) = 0 ;
	virtual TQPRESULT	Close		() = 0 ;
	virtual int			Reserved6	( int ) = 0 ;
	virtual int			Reserved7	() = 0 ;
	virtual int			Reserved8	( int ) = 0 ;
	virtual int			Reserved9	( int, int ) = 0 ;
	virtual DWORD		GetAmount	() = 0 ;
	virtual const char*	Reserved11	( int ) = 0 ;
};

class CTqWdfPack : public ITqPack
{
public:
	CTqWdfPack () ;
	virtual ~CTqWdfPack () ;

	virtual int			Release		() ;
	virtual bool		Check		( const char *filename, DWORD *size ) ;
	virtual int			Reserved2	( int )			{ return 0 ; }
	virtual TQPRESULT	Open		( const char *filename, int ) ;
	virtual TQPRESULT	Load		( const char *filename, void *buffer, DWORD *size ) ;
	virtual TQPRESULT	Close		() ;
	virtual int			Reserved6	( int )			{ return 0 ; }
	virtual int			Reserved7	()				{ return 0 ; }
	virtual int			Reserved8	( int )			{ return 0 ; }
	virtual int			Reserved9	( int, int )	{ return 0 ; }
	virtual DWORD		GetAmount	()				{ return m_mapIndex.size () ; }
	virtual const char*	Reserved11	( int )			{ return "" ; }

	TQPRESULT	Read	( const char *filename, void *buffer, DWORD *size, DWORD pos ) ;

private:
	TqPackIndex*	Find	( const char *filename ) ;

	char							m_Reserved[0x400] ;
	std::map<DWORD, TqPackIndex*>	m_mapIndex ;
	std::string						m_strName ;
	FILE*							m_File ;
	TqPackIndex*					m_Index ;
	int								m_Number ;
};

CTqWdfPack::CTqWdfPack ()
{
	m_File = 0 ;
	m_Index = 0 ;
	m_Number = 0 ;
}

CTqWdfPack::~CTqWdfPack ()
{
	Close () ;
}

int CTqWdfPack::Release ()
{
	if ( this )
		delete this ;
	return 0 ;
}

TqPackIndex* CTqWdfPack::Find ( const char *filename )
{
	DWORD id = string_id ( filename ) ;
	std::map<DWORD, TqPackIndex*>::iterator it = m_mapIndex.find ( id ) ;
	if ( it != m_mapIndex.end () )
		return it->second ;
	return 0 ;
}

bool CTqWdfPack::Check ( const char *filename, DWORD *size )
{
	DWORD id = string_id ( filename ) ;
	std::map<DWORD, TqPackIndex*>::iterator it = m_mapIndex.find ( id ) ;
	if ( it != m_mapIndex.end () )
	{
		if ( size )
			*size = it->second->size ;
		return true ;
	}
	return false ;
}

TQPRESULT CTqWdfPack::Open ( const char *filename, int )
{
	m_strName = filename ;

	m_File = fopen ( filename, "rb" ) ;
	if ( !m_File )
		return TQP_OPENFAIL ;

	TqPackHeader header ;
	if ( fread ( &header, sizeof ( header ), 1, m_File ) != 1 )
		return TQP_READFAIL ;

	m_Number = header.number ;
	m_Index = ( TqPackIndex* ) malloc ( header.number * sizeof ( TqPackIndex ) ) ;
	if ( !m_Index )
		return TQP_NOMEMORY ;

	fseek ( m_File, header.offset, SEEK_SET ) ;
	if ( fread ( m_Index, sizeof ( TqPackIndex ), m_Number, m_File ) != ( size_t ) m_Number )
		return TQP_READFAIL ;

	for ( int i = 0; i < header.number; i++ )
		m_mapIndex[m_Index[i].uid] = &m_Index[i] ;
	return TQP_OK ;
}

TQPRESULT CTqWdfPack::Load ( const char *filename, void *buffer, DWORD *size )
{
	TqPackIndex *pf = Find ( filename ) ;
	if ( !pf )
		return TQP_NOTFOUND ;
	if ( pf->size < *size )
		return TQP_RANGE ;
	fseek ( m_File, pf->offset, SEEK_SET ) ;
	if ( fread ( buffer, 1, *size, m_File ) != *size )
		return TQP_READFAIL ;
	return TQP_OK ;
}

TQPRESULT CTqWdfPack::Read ( const char *filename, void *buffer, DWORD *size, DWORD pos )
{
	TqPackIndex *pf = Find ( filename ) ;
	if ( !pf )
		return TQP_NOTFOUND ;
	if ( pf->size < *size + pos )
		return TQP_RANGE ;
	fseek ( m_File, pf->offset + pos, SEEK_SET ) ;
	if ( fread ( buffer, 1, *size, m_File ) != *size )
		return TQP_READFAIL ;
	return TQP_OK ;
}

TQPRESULT CTqWdfPack::Close ()
{
	if ( m_File )
	{
		fclose ( m_File ) ;
		m_File = 0 ;
	}
	if ( m_Index )
	{
		free ( m_Index ) ;
		m_Index = 0 ;
	}
	m_mapIndex.clear () ;
	return TQP_OK ;
}

class CTqPackage ;

class CTqFile
{
public:
	CTqFile ()	{ m_File = 0 ; m_Size = 0 ; m_Pos = 0 ; m_Type = 0 ; m_Package = 0 ; }
	virtual ~CTqFile () ;

	FILE*			m_File ;
	std::string		m_strName ;
	DWORD			m_Size ;
	DWORD			m_Pos ;
	int				m_Type ;
	CTqPackage*		m_Package ;
};

CTqFile::~CTqFile ()
{
	if ( m_File )
	{
		fclose ( m_File ) ;
		m_File = 0 ;
	}
}

class CTqPackage
{
public:
	CTqPackage () ;
	virtual ~CTqPackage () ;

	TQPRESULT	Open		( const char *filename ) ;
	bool		Check		( int type, const char *filename, DWORD *size ) ;
	TQPRESULT	OpenFile	( const char *filename, void *&file ) ;
	TQPRESULT	CloseFile	( CTqFile *file ) ;
	long		Read		( CTqFile *file, void *buffer, DWORD size ) ;
	static bool	Seek		( CTqFile *file, long offset, int mode ) ;
	static long	Tell		( CTqFile *file ) ;
	static bool	IsEof		( CTqFile *file ) ;
	TQPRESULT	Dump		( const char *filename, void *&buffer, DWORD &size, int type ) ;

	DWORD		GetId		()	{ return m_Id ; }

private:
	void		ProcessDir	( const char *pszDir ) ;
	TQPRESULT	DumpPack	( const char *filename, void *&buffer, DWORD &size ) ;
	TQPRESULT	DumpUnpack	( const char *filename, void *&buffer, DWORD &size ) ;

	DWORD							m_Id ;
	std::string						m_strName ;
	CTqWdfPack*						m_Pack ;
	std::map<std::string, DWORD>	m_mapUnpack ;
	std::map<CTqFile*, CTqFile*>	m_mapFile ;
};

CTqPackage::CTqPackage ()
{
	m_Id = 0 ;
	m_Pack = new CTqWdfPack ;
}

CTqPackage::~CTqPackage ()
{
	if ( m_Pack )
	{
		delete m_Pack ;
		m_Pack = 0 ;
	}
	for ( std::map<CTqFile*, CTqFile*>::iterator it = m_mapFile.begin (); it != m_mapFile.end (); ++it )
	{
		if ( it->second )
			delete it->second ;
	}
	m_mapFile.clear () ;
}

TQPRESULT CTqPackage::Open ( const char *filename )
{
	char szBase[260] = "" ;
	const char *pDot = strrchr ( filename, '.' ) ;
	if ( !pDot )
		strcpy ( szBase, filename ) ;
	else
	{
		int n = pDot - filename ;
		if ( n < 260 )
		{
			strncpy ( szBase, filename, n ) ;
			szBase[n] = 0 ;
		}
	}
	m_strName = szBase ;
	m_Id = string_id ( szBase ) ;
	m_Pack->Open ( filename, 0 ) ;
	ProcessDir ( szBase ) ;
	return TQP_OK ;
}

void CTqPackage::ProcessDir ( const char *pszDir )
{
	char szPath[260] ;
	struct _finddata_t fd ;

	if ( strlen ( pszDir ) == 0 )
		strcpy ( szPath, "*.*" ) ;
	else
	{
		strcpy ( szPath, pszDir ) ;
		strcat ( szPath, "/*.*" ) ;
	}

	long hFind = _findfirst ( szPath, &fd ) ;
	if ( hFind == -1 )
		return ;

	char szSub[260] = "" ;
	do
	{
		if ( strcmp ( fd.name, ".." ) != 0 && strcmp ( fd.name, "." ) != 0 )
		{
			if ( strlen ( pszDir ) == 0 )
				strcpy ( szSub, fd.name ) ;
			else
				sprintf ( szSub, "%s/%s", pszDir, fd.name ) ;

			if ( !( fd.attrib & _A_SUBDIR ) )
			{
				NormalizePath ( szSub ) ;
				std::string str = szSub ;
				m_mapUnpack[str] = fd.size ;
			}
			else
				ProcessDir ( szSub ) ;
		}
	}
	while ( _findnext ( hFind, &fd ) == 0 ) ;
	_findclose ( hFind ) ;
}

bool CTqPackage::Check ( int type, const char *filename, DWORD *size )
{
	if ( type == FILETYPE_ANY )
	{
		std::string str = filename ;
		std::map<std::string, DWORD>::iterator it = m_mapUnpack.find ( str ) ;
		if ( it != m_mapUnpack.end () )
		{
			if ( size )
				*size = it->second ;
			return true ;
		}
		return m_Pack->Check ( filename, size ) ;
	}
	if ( type == FILETYPE_UNPACK )
	{
		std::string str = filename ;
		std::map<std::string, DWORD>::iterator it = m_mapUnpack.find ( str ) ;
		if ( it != m_mapUnpack.end () )
		{
			if ( size )
				*size = it->second ;
			return true ;
		}
		return false ;
	}
	if ( type == FILETYPE_PACK )
		return m_Pack->Check ( filename, size ) ;
	return false ;
}

TQPRESULT CTqPackage::OpenFile ( const char *filename, void *&file )
{
	DWORD size ;
	int type = FILETYPE_UNPACK ;
	if ( !Check ( FILETYPE_UNPACK, filename, &size ) )
	{
		if ( !Check ( FILETYPE_PACK, filename, &size ) )
			return TQP_NOTFOUND ;
		type = FILETYPE_PACK ;
	}

	CTqFile *pFile = new CTqFile ;
	pFile->m_strName = filename ;
	pFile->m_Type = type ;
	pFile->m_Size = size ;
	pFile->m_Package = this ;
	if ( type == FILETYPE_UNPACK )
	{
		pFile->m_File = fopen ( filename, "rb" ) ;
		if ( !pFile->m_File )
		{
			delete pFile ;
			return TQP_OPENFAIL ;
		}
	}
	m_mapFile[pFile] = pFile ;
	file = pFile ;
	return TQP_OK ;
}

TQPRESULT CTqPackage::CloseFile ( CTqFile *file )
{
	std::map<CTqFile*, CTqFile*>::iterator it = m_mapFile.find ( file ) ;
	if ( it != m_mapFile.end () )
	{
		m_mapFile.erase ( it ) ;
		if ( file )
			delete file ;
	}
	return TQP_OK ;
}

long CTqPackage::Read ( CTqFile *file, void *buffer, DWORD size )
{
	if ( file->m_Type == FILETYPE_UNPACK )
	{
		if ( !file->m_File )
			return -1 ;
		return fread ( buffer, 1, size, file->m_File ) ;
	}
	if ( file->m_Type == FILETYPE_PACK )
	{
		if ( file->m_Size < size + file->m_Pos )
			return -1 ;
		if ( m_Pack->Read ( file->m_strName.c_str (), buffer, &size, file->m_Pos ) == TQP_OK )
		{
			file->m_Pos += size ;
			return size ;
		}
	}
	return -1 ;
}

bool CTqPackage::Seek ( CTqFile *file, long offset, int mode )
{
	if ( file->m_Type == FILETYPE_UNPACK )
	{
		if ( !file->m_File )
			return false ;
		return fseek ( file->m_File, offset, mode ) == 0 ;
	}
	if ( file->m_Type != FILETYPE_PACK )
		return false ;
	if ( file->m_Size < file->m_Pos )
		return false ;
	if ( mode == TQSEEK_SET )
		file->m_Pos = offset ;
	else if ( mode == TQSEEK_CUR )
		file->m_Pos += offset ;
	else if ( mode == TQSEEK_END )
		file->m_Pos = file->m_Size + offset ;
	return true ;
}

long CTqPackage::Tell ( CTqFile *file )
{
	if ( file->m_Type == FILETYPE_UNPACK )
	{
		if ( !file->m_File )
			return -1 ;
		return ftell ( file->m_File ) ;
	}
	if ( file->m_Type != FILETYPE_PACK )
		return -1 ;
	return file->m_Pos ;
}

bool CTqPackage::IsEof ( CTqFile *file )
{
	if ( file->m_Type == FILETYPE_UNPACK )
	{
		if ( !file->m_File )
			return false ;
		return feof ( file->m_File ) != 0 ;
	}
	if ( file->m_Type != FILETYPE_PACK )
		return false ;
	return file->m_Pos == file->m_Size ;
}

TQPRESULT CTqPackage::Dump ( const char *filename, void *&buffer, DWORD &size, int type )
{
	if ( type == FILETYPE_ANY )
	{
		TQPRESULT r = DumpUnpack ( filename, buffer, size ) ;
		if ( r == TQP_OK )
			return TQP_OK ;
		if ( r == TQP_NOTFOUND )
			r = DumpPack ( filename, buffer, size ) ;
		return r ;
	}
	if ( type == FILETYPE_UNPACK )
		return DumpUnpack ( filename, buffer, size ) ;
	if ( type == FILETYPE_PACK )
		return DumpPack ( filename, buffer, size ) ;
	return TQP_INVALID ;
}

TQPRESULT CTqPackage::DumpPack ( const char *filename, void *&buffer, DWORD &size )
{
	DWORD len ;
	if ( !m_Pack->Check ( filename, &len ) )
		return TQP_NOTFOUND ;
	void *p = malloc ( len ) ;
	TQPRESULT r = m_Pack->Load ( filename, p, &len ) ;
	if ( r == TQP_OK )
	{
		buffer = p ;
		size = len ;
		return r ;
	}
	if ( p )
		free ( p ) ;
	return r ;
}

TQPRESULT CTqPackage::DumpUnpack ( const char *filename, void *&buffer, DWORD &size )
{
	std::string str = filename ;
	std::map<std::string, DWORD>::iterator it = m_mapUnpack.find ( str ) ;
	if ( it == m_mapUnpack.end () )
		return TQP_NOTFOUND ;

	DWORD len = it->second ;
	FILE *f = fopen ( filename, "rb" ) ;
	if ( !f )
		return TQP_OPENFAIL ;
	void *p = malloc ( len ) ;
	DWORD n = fread ( p, 1, len, f ) ;
	fclose ( f ) ;
	if ( n == len )
	{
		buffer = p ;
		size = n ;
		return TQP_OK ;
	}
	if ( p )
		free ( p ) ;
	return TQP_READFAIL ;
}

static std::vector<CTqPackage*>	g_vecPackage;

static CTqPackage* FindPackage ( DWORD id )
{
	for ( DWORD n = 0; n < g_vecPackage.size (); n++ )
	{
		if ( g_vecPackage[n]->GetId () == id )
			return g_vecPackage[n] ;
	}
	return 0 ;
}

static CTqPackage* FindFilePackage ( const char *filename )
{
	return FindPackage ( PackageId ( filename ) ) ;
}

TQPACKAGE_API
TQPRESULT TqPackagesOpen ( const char *filename )
{
	char szName[260] ;
	NormalizeName ( szName, filename ) ;
	DWORD id = stringtoid ( szName ) ;
	if ( FindPackage ( id ) )
		return TQP_OK ;

	CTqPackage *pPackage = new CTqPackage ;
	TQPRESULT r = pPackage->Open ( szName ) ;
	if ( r == TQP_OK )
		g_vecPackage.push_back ( pPackage ) ;
	else if ( pPackage )
		delete pPackage ;
	return r ;
}

TQPACKAGE_API
void TqPackagesClose ( const char *filename )
{
	if ( !filename )
	{
		for ( DWORD n = 0; n < g_vecPackage.size (); n++ )
		{
			if ( g_vecPackage[n] )
				delete g_vecPackage[n] ;
		}
		g_vecPackage.erase ( g_vecPackage.begin (), g_vecPackage.end () ) ;
		return ;
	}

	char szName[260] ;
	NormalizeName ( szName, filename ) ;
	DWORD id = stringtoid ( szName ) ;
	for ( std::vector<CTqPackage*>::iterator it = g_vecPackage.begin (); it != g_vecPackage.end (); ++it )
	{
		CTqPackage *pPackage = *it ;
		if ( pPackage->GetId () == id )
		{
			g_vecPackage.erase ( it ) ;
			if ( pPackage )
				delete pPackage ;
			return ;
		}
	}
}

TQPACKAGE_API
bool TqFCheck ( const char *filename, FILETYPE type )
{
	if ( filename && filename[0] )
	{
		char szName[260] ;
		NormalizeName ( szName, filename ) ;
		CTqPackage *pPackage = FindFilePackage ( szName ) ;
		if ( pPackage )
			return pPackage->Check ( type, szName, 0 ) ;
	}
	return false ;
}

TQPACKAGE_API
TQPRESULT TqFDump ( const char *filename, void *&buffer, unsigned long &size )
{
	if ( filename && filename[0] )
	{
		char szName[260] ;
		NormalizeName ( szName, filename ) ;
		CTqPackage *pPackage = FindFilePackage ( szName ) ;
		if ( pPackage )
			return pPackage->Dump ( szName, buffer, size, FILETYPE_ANY ) ;
		return TQP_NOTFOUND ;
	}
	return TQP_INVALID ;
}

TQPACKAGE_API
TQPRESULT TqFUndump ( void *buffer )
{
	if ( buffer )
		free ( buffer ) ;
	return TQP_OK ;
}

TQPACKAGE_API
TQPRESULT TqFOpen ( const char *filename, void *&file )
{
	if ( filename && filename[0] )
	{
		char szName[260] ;
		NormalizeName ( szName, filename ) ;
		CTqPackage *pPackage = FindFilePackage ( szName ) ;
		if ( pPackage )
			return pPackage->OpenFile ( szName, file ) ;
		file = 0 ;
		return TQP_NOTFOUND ;
	}
	return TQP_INVALID ;
}

TQPACKAGE_API
TQPRESULT TqFClose ( void *file )
{
	CTqFile *pFile = ( CTqFile* ) file ;
	if ( pFile && pFile->m_Package )
		return pFile->m_Package->CloseFile ( pFile ) ;
	return TQP_INVALID ;
}

TQPACKAGE_API
long TQFRead ( void *buffer, unsigned long size, unsigned long count, void *file )
{
	CTqFile *pFile = ( CTqFile* ) file ;
	if ( !pFile || !pFile->m_Package )
		return -1 ;
	long n = pFile->m_Package->Read ( pFile, buffer, size * count ) ;
	if ( n > 0 )
		return ( unsigned long ) n / size ;
	return n ;
}

TQPACKAGE_API
bool TqFSeek ( void *file, long offset, TQSEEKMODE mode )
{
	CTqFile *pFile = ( CTqFile* ) file ;
	if ( pFile && pFile->m_Package )
		return CTqPackage::Seek ( pFile, offset, mode ) ;
	return false ;
}

TQPACKAGE_API
unsigned long TqFTell ( void *file )
{
	CTqFile *pFile = ( CTqFile* ) file ;
	if ( !pFile )
		return ( unsigned long ) -1 ;
	return CTqPackage::Tell ( pFile ) ;
}

TQPACKAGE_API
bool TqFIsEof ( void *file )
{
	CTqFile *pFile = ( CTqFile* ) file ;
	if ( !pFile )
		return true ;
	if ( pFile->m_Package )
		return CTqPackage::IsEof ( pFile ) ;
	return false ;
}

TQPACKAGE_API
long TqFGetLen ( void *file )
{
	CTqFile *pFile = ( CTqFile* ) file ;
	if ( pFile )
		return pFile->m_Size ;
	return -1 ;
}
