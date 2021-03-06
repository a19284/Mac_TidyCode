#ifndef __ANYFILES__H__
#define __ANYFILES__H__
#include "../safevcrt.h"
#include "miniarchive.h"
#include "vfs.h"

//class CAnyFile;
	class CStdFile;
	class CRamFile;
	class CVFSFile;

//////////////////////////////////////////////////////////////////////////
// 标准文件(注意不要使用任何赋值操作,包含文件指针)
class LITEFILEIO_IMP CStdFile : public CAnyFile
{
public:
	CStdFile(IN LPCSTR lpszFileName,IN LPCSTR lpszOpenMode);
	virtual~CStdFile();
public:
	virtual LONG GetLength();
	virtual BOOL Write(IN LPCVOID lpBuffer,IN LONG nLength);
	virtual BOOL Read(OUT LPVOID lpBuffer,IN LONG nLength);
	virtual BOOL Seek(IN LONG nTo,IN LONG nFlag);
	virtual LONG Tell();
	virtual BOOL SetLength(IN LONG nNewLength);
	virtual BOOL IsFileOpened();
protected:
	FILE* m_fpFile;
};


//////////////////////////////////////////////////////////////////////////
// 内存文件(注意不要使用任何赋值操作,包含指针,如果需要必须使用引用)
class LITEFILEIO_IMP CRamFile : public CAnyFile
{
public:
	CRamFile(IN LPCVOID lpBuffer,IN LONG nLength,IN LONG nAllocStep=ALLOCSTEP_SMALL);
	CRamFile(IN LONG nAllocStep=ALLOCSTEP_SMALL);
public:
	virtual~CRamFile();
	virtual LONG GetLength();
	virtual BOOL Write(IN LPCVOID lpBuffer,IN LONG nLength);
	virtual BOOL Read(OUT LPVOID lpBuffer,IN LONG nLength);
	virtual BOOL Seek(IN LONG nTo,IN LONG nFlag);
	virtual LONG Tell();
	virtual BOOL SetLength(IN LONG nNewLength);
	virtual BOOL IsFileOpened();
public:
	operator LPCVOID() const;
	operator LPCBYTE() const;
	operator LPCSTR() const;
	enum enumCONST
	{	ALLOCSTEP_SMALL=1024,
		ALLOCSTEP_LARGE=1024*1024,
	};
protected:
	LONG m_nAllocStep;
public:
	char* m_pStream;
	LONG m_nLength;
	LONG m_nAlloc;
	LONG m_nSeek;
};


//////////////////////////////////////////////////////////////////////////
// VFS文件(注意不要使用任何赋值操作,包含指针,如果需要必须使用引用)
class LITEFILEIO_IMP CVFSFile : public CAnyFile
{
public:
	CVFSFile(CVFS* pVFS,IN LPCSTR lpszFileName,IN LPCSTR lpszOpenMode);
	virtual~CVFSFile();
public:
	virtual LONG GetLength();
	virtual BOOL Write(IN LPCVOID lpBuffer,IN LONG nLength);
	virtual BOOL Read(OUT LPVOID lpBuffer,IN LONG nLength);
	virtual BOOL Seek(IN LONG nTo,IN LONG nFlag);
	virtual LONG Tell();
	virtual BOOL SetLength(IN LONG nNewLength);
	virtual BOOL IsFileOpened();
protected:
	CVFS* m_pVFS;
	HVFSITEM m_hVFSItem;
};


//////////////////////////////////////////////////////////////////////////
// 文件查找类
class LITEFILEIO_IMP CFileFinder
{
public:
	CFileFinder();
	virtual~CFileFinder();
	BOOL FindFile(LPCSTR pszPattern);
	BOOL FindNextFile();
	TClibStr GetFileName() { return m_CurrFile.name; }
	TClibStr GetFilePath() { return IsDirectory()?(m_strDir+TClibStr(m_CurrFile.name)+TClibStr("/")):(m_strDir+TClibStr(m_CurrFile.name)); }
	BOOL IsDirectory() { return (m_CurrFile.attrib&_A_SUBDIR)!=0; }
	time_t GetLastWriteTime() { return m_CurrFile.time_write; }
private:
	INT_PTR m_hFinder;
	TClibStr m_strPattern;
	TClibStr m_strDir;
	_finddata_t m_CurrFile;
	_finddata_t m_NextFile;
	BOOL m_bMoreFile;
};


#endif

