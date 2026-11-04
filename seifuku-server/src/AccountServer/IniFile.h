#ifndef INIFILE_H
#define INIFILE_H

#define INIWORDSIZE		512

class	CIniFile
{
public:
	CIniFile(const char * pFilename, const char * pSection);
	~CIniFile();
public:
	void	SetSection(const char * pSect);
	bool	GetString(const char * pKey, char * bufString);
	int		GetInt(const char * pKey);
protected:
	char m_bufFilename[INIWORDSIZE];
	char m_bufSection[INIWORDSIZE];
};

#endif