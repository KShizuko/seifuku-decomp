#include "windows.h"
#include "Fact.h"
#include "FactSet.h"
#include "i_mydb.h"
#include "SymbolTable.h"
#include "Syntax.h"

inline bool IsSymbol(char ch)
{
	if(ch == UNDERLINE_CHAR || isalpha(ch))
		return true;

	return false;
}

inline bool IsNum(char ch)
{
	if(ch == NEGATE_CHAR || isdigit(ch))
		return true;

	return false;
}

inline bool IsIdentChar(char ch)
{
	if(ch == UNDERLINE_CHAR || isalnum(ch))
		return true;

	return false;
}

inline void IgnoreSpace(const char*& ptr)
{
	while(isspace(*ptr))
		ptr++;
}

bool GetWord(const char*& ptr, char* bufWord)
{
	IgnoreSpace(ptr);

	char* pBuf = bufWord;
	if(*ptr == STRING_CHAR)
	{
		do{
			*(pBuf++)	= *(ptr++);
		}while(*ptr && *ptr != STRING_CHAR);
		if(*ptr)
			*(pBuf++)	= *(ptr++);
	}
	else
	{
		if(*ptr == NEGATE_CHAR && isdigit(*(ptr+1)))
			*(pBuf++)	= *(ptr++);

		while(IsIdentChar(*ptr))
			*(pBuf++)	= *(ptr++);
	}

	*pBuf	= 0;

	return bufWord[0] != 0;
}

inline void CopyKeyParam(const char*& ptr, char*& bufWord)
{
	while(*ptr && *ptr != KEY_PARAM_CHAR2)
		*(bufWord++)	= *(ptr++);
	if(*ptr)
		*(bufWord++)	= *(ptr++);
}

inline void CopyDataParam(const char*& ptr, char*& bufWord)
{
	while(*ptr && *ptr != DATA_PARAM_CHAR2)
		*(bufWord++)	= *(ptr++);
	if(*ptr)
		*(bufWord++)	= *(ptr++);
}

inline void CopyString(const char*& ptr, char*& bufWord)
{
	while(*ptr && *ptr != STRING_CHAR)
		*(bufWord++)	= *(ptr++);
	if(*ptr)
		*(bufWord++)	= *(ptr++);
}
bool GetFact(const char*& ptr, char* bufWord)
{
	IgnoreSpace(ptr);
	char* pBuf = bufWord;
	while(*ptr && *ptr != SEPARATOR_CHAR)
	{
		if(*ptr == KEY_PARAM_CHAR)
		{
			*(pBuf++)	= *(ptr++);
			CopyKeyParam(ptr, pBuf);
		}
		else if(*ptr == DATA_PARAM_CHAR)
		{
			*(pBuf++)	= *(ptr++);
			CopyDataParam(ptr, pBuf);
		}
		else if(*ptr == STRING_CHAR)
		{
			*(pBuf++)	= *(ptr++);
			CopyString(ptr, pBuf);
		}
		else
			*(pBuf++)	= *(ptr++);
	}
	if(*ptr)
		ptr++;
	*pBuf	= 0;
	return bufWord[0] != 0;
}
bool AnalyzeFact(CSymbolTable* pSymbolTable, StringRef strFact,
				 bool* pbLogicNot, OBJID* pidType, VarTypeSet* psetParam, VarTypeSet* pKeyParam )
{
	CHECKF(pSymbolTable && pbLogicNot && pidType && psetParam);

	DEBUG_TRY
	FIELDBUF	buf;
	const char* ptr = strFact;

	IgnoreSpace(ptr);

	*pbLogicNot = (*ptr == LOGIC_NOT_CHAR);
	if(*pbLogicNot)
		ptr++;

	IgnoreSpace(ptr);

	IF_NOT(GetWord(ptr, buf) && isalpha(*buf))
	{
		LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
		return false;
	}
	*pidType	= pSymbolTable->GetSymbolID(buf);

	IgnoreSpace(ptr);

	if(*ptr == KEY_PARAM_CHAR)
	{
		ptr++;
		while(*ptr)
		{
			IF_NOT(GetWord(ptr, buf))
			{
				LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
				return false;
			}
			VarType	vData;
			if(IsSymbol(*buf))
				vData = VarType(buf, VarType::TYPE_SYMBOL);
			else if(IsNum(*buf))
				vData = VarType(atoi(buf), VarType::TYPE_INT);
			else if(*buf==STRING_CHAR)
			{
				if(strlen(buf) >= 2 && buf[strlen(buf)-1] == STRING_CHAR)
					buf[strlen(buf)-1]	= 0;
				vData = VarType(buf+1, VarType::TYPE_STRING);
			}
			else
			{
				LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
				return false;
			}
			psetParam->Push(vData);
			if(pKeyParam)
				pKeyParam->Push(vData);

			IgnoreSpace(ptr);

			if(*ptr == SEPARATOR_CHAR)
				ptr++;
			else
				break;
		}

		IF_NOT(*ptr == KEY_PARAM_CHAR2)
		{
			LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
			return false;
		}

		if(*ptr == KEY_PARAM_CHAR2)
			ptr++;
	}

	if(*ptr == DATA_PARAM_CHAR)
	{
		ptr++;
		while(*ptr)
		{
			IF_NOT(GetWord(ptr, buf))
			{
				LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
				return false;
			}
			VarType	vData;
			if(IsSymbol(*buf))
				vData = VarType(buf, VarType::TYPE_SYMBOL);
			else if(IsNum(*buf))
				vData = VarType(atoi(buf), VarType::TYPE_INT);
			else if(*buf==STRING_CHAR)
			{
				if(strlen(buf) >= 2 && buf[strlen(buf)-1] == STRING_CHAR)
					buf[strlen(buf)-1]	= 0;
				vData = VarType(buf+1, VarType::TYPE_STRING);
			}
			else
			{
				LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
				return false;
			}
			psetParam->Push(vData);
			if(pKeyParam)
				pKeyParam->Push(VarType());

			IgnoreSpace(ptr);

			if(*ptr == SEPARATOR_CHAR)
				ptr++;
			else
				break;
		}

		IF_NOT(*ptr == DATA_PARAM_CHAR2)
		{
			LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
			return false;
		}

		if(*ptr == DATA_PARAM_CHAR2)
			ptr++;
	}

	IgnoreSpace(ptr);

	IF_NOT(*ptr == 0)
	{
		LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
	}

	return *ptr == 0;

	DEBUG_CATCH("AnalyzeFact(1)")
	return false;
}

bool AnalyzeFact(CSymbolTable* pSymbolTable, StringRef strFact,
				 CFact* pFact, CFact* pPattern )
{
	CHECKF(pSymbolTable && pFact);

	DEBUG_TRY
	bool		bLogicNot;
	OBJID		idType;
	VarTypeSet	setParam;
	VarTypeSet	setPattern;
	if(AnalyzeFact(pSymbolTable, strFact, &bLogicNot, &idType, &setParam, &setPattern))
	{
		pFact->Create(bLogicNot, idType, setParam);
		if(pPattern)
			pPattern->Create(true, idType, setPattern);
		return true;
	}

	DEBUG_CATCH("AnalyzeFact(2)")
	return false;
}

bool AnalyzeFactSet(CSymbolTable* pSymbolTable, StringRef strFact,
									CFactArray* psetFacts)
{
	CHECKF(pSymbolTable && psetFacts);

	DEBUG_TRY
	const char*	ptr	= strFact;
	FIELDBUF	buf;
	while(GetFact(ptr, buf))
	{
		bool		bLogicNot;
		OBJID		idType;
		VarTypeSet	setParam;
		MYASSERT(setParam.Size() == 0);
		if(AnalyzeFact(pSymbolTable, buf, &bLogicNot, &idType, &setParam, NULL))
		{
			CFact fact;
			if(fact.Create(bLogicNot, idType, setParam))
				psetFacts->Push(fact);
		}
	}

	IgnoreSpace(ptr);

	IF_NOT(*ptr == 0)
	{
		LOGERROR("Syntax error: [%s] near [%s]", (LPCTSTR)strFact, ptr);
	}

	return *ptr == 0;

	DEBUG_CATCH("AnalyzeFactSet()")
	return false;
}

bool ReplaceSymbol(CFact* pFact,
					ARGUMENT_SET_REF setArgument)
{
	VarTypeSet&	setParam = pFact->GetParamSet();
	VarTypeSet::Iterator pParam = setParam.NewEnum();
	while(pParam.Next())
	{
		if(pParam->IsSymbolType())
		{
			String strSymbol = static_cast<String>(*pParam);
			IF_NOT(setArgument.IsIn(strSymbol))
				return false;
			*pParam	= setArgument[strSymbol];
		}
	}
	return true;
}
