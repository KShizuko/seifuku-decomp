#ifndef _TQPACKAGE_H_
#define _TQPACKAGE_H_

#ifdef	TQPACKAGE_EXPORTS
#define TQPACKAGE_API __declspec ( dllexport )
#else
#define TQPACKAGE_API __declspec ( dllimport )
#endif

enum TQPRESULT
{
	TQP_OK			= 0,
	TQP_INVALID		= 1,
	TQP_RANGE		= 2,
	TQP_NOTFOUND	= 3,
	TQP_OPENFAIL	= 4,
	TQP_READFAIL	= 5,
	TQP_NOMEMORY	= 0x11,
};

enum FILETYPE
{
	FILETYPE_ANY	= 0,
	FILETYPE_UNPACK	= 1,
	FILETYPE_PACK	= 2,
};

enum TQSEEKMODE
{
	TQSEEK_SET		= 0,
	TQSEEK_CUR		= 1,
	TQSEEK_END		= 2,
};

TQPACKAGE_API
TQPRESULT TqPackagesOpen ( const char *filename );

TQPACKAGE_API
void TqPackagesClose ( const char *filename );

TQPACKAGE_API
bool TqFCheck ( const char *filename, FILETYPE type );

TQPACKAGE_API
TQPRESULT TqFDump ( const char *filename, void *&buffer, unsigned long &size );

TQPACKAGE_API
TQPRESULT TqFUndump ( void *buffer );

TQPACKAGE_API
TQPRESULT TqFOpen ( const char *filename, void *&file );

TQPACKAGE_API
TQPRESULT TqFClose ( void *file );

TQPACKAGE_API
long TQFRead ( void *buffer, unsigned long size, unsigned long count, void *file );

TQPACKAGE_API
bool TqFSeek ( void *file, long offset, TQSEEKMODE mode );

TQPACKAGE_API
unsigned long TqFTell ( void *file );

TQPACKAGE_API
bool TqFIsEof ( void *file );

TQPACKAGE_API
long TqFGetLen ( void *file );

#endif
