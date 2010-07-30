
#ifndef _DATAFILE_H_
#define _DATAFILE_H_

#include <Windows.h>
#include <stdio.h>
#include <string>


using namespace std;

class CDataFile
{
public:

	CDataFile() : m_szFileName("") { m_pFile = NULL; }
	~CDataFile() { Close(); }

  #if 1	//+++
	BOOL Open(const CString& rszFileName)
	{
		m_szFileName 	= rszFileName;
		m_pFile 		= ::fopen(LPCTSTR(rszFileName), "r");
		if(m_pFile==NULL) return FALSE;
		return TRUE;
	}
  #else
	BOOL Open(const string& rszFileName)
	{
		m_szFileName 	= rszFileName;
		m_pFile 		= ::fopen(rszFileName.c_str(), "r");
		if(m_pFile==NULL) return FALSE;
		return TRUE;
	}
  #endif

	void Close()
	{
		if(m_pFile!=NULL) ::fclose(m_pFile);
		m_pFile = NULL;
	}

  #if 1	//+++
	BOOL ReadLine(CString& str)
	{
		char buf[1200];
		if(fgets(buf,1200,m_pFile) == NULL){
			return FALSE;
		}else{
			str = buf;
			return TRUE;
		}
	}
  #else
	BOOL ReadLine(string *pszBuf)
	{
		char buf[1200];
		if(fgets(buf,1200,m_pFile) == NULL){
			return FALSE;
		}else{
			*pszBuf = buf;
			return TRUE;
		}
	}
  #endif

	BOOL IsEOF()
	{
		if(feof(m_pFile) == 0)
			return FALSE;
		else
			return TRUE;
	}


private:
	CDataFile(const CDataFile& rdat) {}
	const CDataFile& operator = (const CDataFile& rdat) {return *this;}

private:
  #if 1 //+++
	CString	m_szFileName;
  #else
	string 	m_szFileName;
  #endif
	FILE* 	m_pFile;
};


#endif