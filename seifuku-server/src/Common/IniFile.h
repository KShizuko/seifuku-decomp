#ifndef INIFILE_H
#define INIFILE_H

#define INIWORDSIZE		512
#define	INIDATASIZE		8192

class	CIniFile
{
public:
	CIniFile(const char * pFilename, const char * pSection);
	~CIniFile();
public:
	void	SetSection(const char * pSect);

	bool	GetString(char * bufString, const char * pKey, int nBufSize);

	int		GetInt(const char * pKey);

protected:
	char m_bufFilename[INIWORDSIZE];
	char m_bufSection[INIWORDSIZE];
};

#endif