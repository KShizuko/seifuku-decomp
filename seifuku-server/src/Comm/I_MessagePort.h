#ifndef	I_MESSAGEPORT_H
#define I_MESSAGEPORT_H

#include <windows.h>
#include "common.h"
#include "LogFile.h"

#define MSGPORT_BEGIN	namespace message_port {
#define MSGPORT_END		};

#define	MAX_PORT_ID			59999
#define	MAX_PACKET_ID		59999
#define	MAX_MSG_TYPE		59999

enum VAR_TYPE {	
		VARTYPE_NONE = MAX_MSG_TYPE+1,
		VARTYPE_CHAR, VARTYPE_UCHAR, VARTYPE_SHORT, VARTYPE_USHORT, 
		VARTYPE_LONG, VARTYPE_ULONG, VARTYPE_INT, VARTYPE_UINT, 
		VARTYPE_FLOAT, VARTYPE_DOUBLE };
#define	STRUCT_TYPE(x)							((VAR_TYPE)sizeof(x))
#define	STRING_TYPE(x)							((VAR_TYPE)(strlen(x)+1))
#define	BUFFER_TYPE(x)							((VAR_TYPE)(x))

const int	PORT_ANY		= -1;
const int	INVALID_PORT	= MAX_PORT_ID+1;
const int	PACKET_ANY		= -1;

enum {	STATUS_FLAG_OK			= 0,
		STATUS_FLAG_CLOSE,
		STATUS_FLAG_ERROR,
		STATUS_FLAG_FATAL,
};

struct	CMessageStatus
{
	int		m_nPortFrom;
	int		m_nPacket;
	VAR_TYPE		m_nVarType;
	int 	m_nError;
};

class	IMessagePort
{
public:
	IMessagePort			() {}
	virtual ~IMessagePort	() {}

	virtual bool	IsOpen	() PURE_VIRTUAL_FUNCTION_0;

public:
	virtual int		GetID	() PURE_VIRTUAL_FUNCTION_0;
	virtual int		GetSize	() PURE_VIRTUAL_FUNCTION_0;

	virtual bool	Open	() PURE_VIRTUAL_FUNCTION_0;
	virtual bool	Close	() PURE_VIRTUAL_FUNCTION_0;

	virtual bool	Send	(int nPort, int nPacket, VAR_TYPE nVarType, const void* buf) PURE_VIRTUAL_FUNCTION_0;
	virtual bool	Recv	(int nPort, int nPacket, VAR_TYPE nVarType, void* buf, CMessageStatus* pStatus) PURE_VIRTUAL_FUNCTION_0;

	virtual bool	WaitMsg	(int nMilliSec = INFINITE) PURE_VIRTUAL_FUNCTION_0;
};

#endif
