//////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2006 - All Rights Reserved
//  ģ������: SafeVCRT
//  ��������: 2006/07/28
//  �����д: zhanglijun
//  ����˵��: ��ȫ��VC���п�
//            ��ǰ����Ҫ��װVCRT/STD_C++��ĳ��ò���,Ϊ��Ӧ�Թ㷺�����еĴ����Ľ����������
//            ��ǰ����þ�̬�ⷽʽ,��ǰ���Ա���ֲ��linux��
//  ����ʾ��: 
//  �޸ļ�¼:
//////////////////////////////////////////////////////////////////////////
#ifndef __SAFE_VCRT__H__
#define __SAFE_VCRT__H__


// ����ģʽ,���ݾɰ�
#if defined(COMPATIBLE_5X_SAFEVCRT)
#include "safevcrt_5x.h"
#else
#define WITH_SAFEVCRT


#if !defined(SAFEVCRT_LIB)

	// Ĭ��ʹ�þ��ɰ�SafeVCRT(��ע:���󲻿����ڽӿ�)
	#if !defined(MINI_SAFEVCRT) && !defined(FULL_SAFEVCRT) && !defined(DYNA_SAFEVCRT)
		#define MINI_SAFEVCRT
	#endif

	// ����ģʽ(����ģʽ,��̬��)
	#if defined(MINI_SAFEVCRT)
		#define SAFEVCRT_STATIC
		#define SAFEVCRT_MINIMODE

	// ����ģʽ(����ģʽ,��̬��)
	#elif defined(FULL_SAFEVCRT)
		#define SAFEVCRT_STATIC
		#define SAFEVCRT_FULLMODE

	// ��̬ģʽ(����ģʽ,��̬��̬�Զ�ѡ��)
	#elif defined(DYNA_SAFEVCRT)
		#if defined(WIN32)
			#define SAFEVCRT_DYNAMIC
		#else
			#define SAFEVCRT_STATIC
		#endif
		#define SAFEVCRT_FULLMODE

	// ģʽδ����
	#else
		#error SafeVCRT.h must be use one mode:  DYNA_SAFEVCRT,MINI_SAFEVCRT,FULL_SAFEVCRT
	#endif
#endif


// Ƕ������ģʽ֧��
#include "safevcrt_os.h"
#include "safevcrt_types.h"

// �ӿڵ���
#ifdef PLATFORM_WINDOWS
	#define DLLEXPORT __declspec(dllexport)
	#define DLLIMPORT __declspec(dllimport)
	#define LIBEXPORT
	#define LIBIMPORT
#else
	#define DLLEXPORT
	#define DLLIMPORT
	#define LIBEXPORT
	#define LIBIMPORT
#endif

#ifndef SAFEVCRT_DYNAMIC
	#ifdef SAFEVCRT_LIB
		#define SAFEVCRT_IMP LIBEXPORT
	#else
		#define SAFEVCRT_IMP LIBIMPORT
	#endif
#else
	#ifdef SAFEVCRT_LIB
		#define SAFEVCRT_IMP DLLEXPORT
	#else
		#define SAFEVCRT_IMP DLLIMPORT
	#endif
#endif



//////////////////////////////////////////////////////////////////////////
// ���÷�ʽ
#if defined(PLATFORM_WINDOWS)
	// ������÷�ʽ
	#define CLIBAPI __stdcall
	#define CLIBCDECL __cdecl
	#define CLIBFASTCALL __fastcall
#else
	// ������÷�ʽ
	#define CLIBAPI
	#define CLIBCDECL
	#define CLIBFASTCALL	
#endif






	


#if defined(_ARCH_AMD64_)
	#if defined(PLATFORM_WINDOWS)
		#define UNALIGNED	__unaligned
	#else
		#define UNALIGNED
	#endif
#else
	#define UNALIGNED
#endif

// INLINE�궨��
#define CLIB_INLINE	inline

// ���Ƹ�ֵ
#define NO_ASSIGN_OPERATOR(CLASS)						\
	private:											\
		const CLASS& operator=(const CLASS& Src);		\


// �źŴ���(ע���źŴ�����WINDOWS��LINUX��������)
//		WINDOWS�����ź�
//			SIGINT          2       �û��жϳ�������
//			SIGILL          4       �Ƿ�ָ��(�Ƿ����̾���)
//			SIGFPE          8       ��������쳣
//			SIGSEGV         11      ��ַ(��)��ͻ
//			SIGTERM         15      �ⲿǿ���ж�
//			SIGBREAK(WIN)   21      CTRL+BREAK�жϳ�������
//			SIGABRT         22      �������abort����׼���쳣�˳�
//		LINUX�����ź�
//			SIGHUP			1		����ն˽����⵽һ�����ӶϿ����򽫴��ź��͸�����ն���صĿ��ƽ���(��
//									�����׽���)���μ�ͼ9.11�����ź��͸�session�ṹ��s-leader�ֶ���ָ��Ľ��̡�
//									�����ն˵�CLOCAL��־û������ʱ�������������²Ų������źš�(��������ӵ�
//									�ն��Ǳ��صģ������ø��ն˵�CLOCAL��־���������ն���������������е��ƽ�
//									������״̬�С��ڵ�ʮһ�½�˵��������ô˱�־)ע�⣬�ӵ����źŵĶԻ����׽�
//									�̿����ں�̨����Ϊһ�����Ӽ�ͼ9��7����������ͨ�����ն˲������ź�(�жϡ���
//									���͹���)����Щ�ź����Ǵ��ݸ�ǰ̨�����顣����Ի���ǰ������ֹ����Ҳ������
//									�źš���������������ź��͸�ǰ̨�������е�ÿһ�����̡�ͨ���ô��ź�֪ͨ��
//									�����(��ʮ����)���ٶ����ǵ������ļ���Ϊ��ѡ��SIGHUP�������ǣ���Ϊһ������
//									���̲�����һ�������նˣ�����ͨ����������յ������źš�SIGKILL ���ź�ָʾ��
//									����ִ��һ���Ƿ�Ӳ��ָ�4.3BSD��abort�����������źš�SIGABRT������������
//									�����
//			SIGINT(WIN)  	2		���û����жϼ�(������DELETE��Control-C)ʱ���ն���������������ź�  
//									������ǰ̨�������е�ÿһ������(��ͼ9��8)����һ������������ʱʧ�أ��ر�  
//									����������Ļ�ϲ�����������Ҫ�����ʱ�����ô��ź���ֹ����
//			SIGQUIT			3		���û����ն��ϰ��˳���(������Control-��)ʱ���������źţ�������ǰ̨����  
//									���е����н���(��ͼ9.8)�����źŲ�����ֹǰ̨������(��SIGINT����������),  
//									��Ҳ����һ��core�ļ���
//			SIGILL(WIN)  	4		�Ƿ�ָ��(�Ƿ����̾���)
//			SIGTRAP			5		��ָʾһ��ʵ�ֶ����Ӳ�����ϡ�
//									���ź���������PPP-11��TRAPָ���  
//			SIGABRT(WIN)  	6		����abort����ʱ(10��17��)�������źš��ý����쳣��ֹ��
//			SIGBUS			7		��ָʾһ��ʵ�ֶ����Ӳ�����ϡ�
//			SIGFPE(WIN)  	8		���źű�ʾһ�����������쳣���������0����������ȡ�
//			SIGKILL			9		�����������ܱ���׽������ź��е�һ��������ϵͳ����Ա�ṩ��һ�ֿ�������  
//									��һ���̵Ŀɿ�������
//			SIGUSR1			10		����һ���û�������źţ�������Ӧ�ó���
//			SIGSEGV(WIN)  	11		���ź�ָʾ���̽�����һ����Ч�Ĵ洢���ʡ�����SEG��ʾ"��Υ��"
//									("segmentation violation")��
//			SIGUSR2			12		����һ���û�������źţ�������Ӧ�ó���
//			SIGPIPE			13		����ڶ���������ֹʱд�ܵ���������ź�SIGPIPE����14.2���н�˵���ܵ���  
//									���׽ӿڵ�һ���Ѿ���ֹʱ��һ������д�ò��Ҳ�������źš�
//			SIGALRM			14		����alarm����ʱ���õ�ʱ���Ѿ�����.��ϸ�����10.10��.����setitimer(2)
//									�������õļ��ʱ���Ѿ���ʱ����ôҲ�������źš�
//			SIGTERM(WIN)  	15		������kill(1)����͵�ϵͳĬ����ֹ�źš�
//			SIGSTKFLT		16
//			SIGCHLD			17		��һ��������ֹ��ֹͣʱ��SIGCHLD�źű��͸��丸���̡���ϵͳĬ�ϣ�������  
//									���źš����������ϣ���˽����ӽ��̵�����״̬�ı䣬��Ӧ��׽���źš���  
//									�Ų�׽������ͨ��Ҫ����wait������ȡ���ӽ���ID������ֹ״̬��ϵͳV������  
//									�汾��һ����ΪSIGCLD(��H)�������źš���һ�źž��зǱ�׼�����壬��SVR2  
//									���ֲ�ҳ�;������µĳ����о���СҪʹ�������źš�Ӧ�ó���Ӧ��ʹ�ñ�׼��  
//									SIGCHLD�źš���10.7���������������źš�
//			SIGCONT			18		����ҵ�����ź��͸���Ҫ��������еĴ���ֹͣ״̬�Ľ��̡�������յ����ź�  
//									�Ľ��̴���ֹͣ״̬����ϵͳĬ�϶�����ʹ�ý��̼������У�����Ĭ�϶����Ǻ�  
//									�Դ��źš����磬vi�༭�����ڲ�׽�����źź��ػ��ն���Ļ�����ڽ�һ����  
//									�����10��20�ڡ�
//			SIGSTOP			19		����һ����ҵ�����źţ���ֹͣһ�����̡��������ڽ���ֹͣ�ź�(SIGTSTP)��  
//									����SIGSTOP���ܱ����ٻ���ԡ�
//			SIGTSTP			20		SIGTSTP ���ǽ���ֹͣ�źţ����û����ն��ϰ������*(������Control-Z)ʱ���ն�
//									��������������źš�
//			SIGTTIN			21		��һ����̨�����������ͼ��������ն�ʱ���ն���������������źš�(��μ�9
//									8���жԴ����������)�����������������£����������źţ���ʱ�������������أ�
//									errno����ΪEIO��(a)�����̺��Ի��������źţ�ʽ(b)�����������Ľ������ǹ¶���
//									���顣
//			SIGTTOU			22		��һ����̨�����������ͼд������ն�ʱ�������ź�(��μ�9.8�ڶԴ���������ۡ�)
//									������������SIGTTIN�źŲ�ͬ��һ�����̿���ѡ��Ϊ������̨����д�����նˡ��ڵ�
//									ʮһ���н�������θ��Ĵ�ѡ��������������̨����д������SIGTTIN����Ҳ������
//									���������(a)д���̺��Ի��������źţ�ʽ(b)д���������������ǹ¶������顣������
//									������²��������źţ�д�����������أ�errno����ΪEIO�������Ƿ�������̨����д��
//									ĳЩ��д����������ն˲���Ҳ�ܲ������źţ�tcsetatlr,tcsendbreak,tcdrain,tcflush,
//									tcflow�Լ�tcsetpgrp���ڵ�ʮһ�½�˵����Щ�ն˲�����
//			SIGURG			23		���ź�֪ͨ�����Ѿ�����һ����������������������ϣ��ӵ��ǹ涨�����ʵ�����ʱ����
//									�ź��ǿ�ѡ��ز����ġ�
//			SIGXCPU			24		SVR4��4��3+BSD֧����Դ���Ƶĸ���(��7��11��)��������̳���������CPUʱ�����ƣ���
//									����SIGXCPU�źš�
//			SIGXFSZ			25		������̳����������ļ���������(��7��11��)����SVR4��4��3+BSD�������źš�
//			SIGVTALRM		26		��һ����setitimer(2)�������õ�������ʱ���Ѿ�����ʱ�������źš�
//			SIGPROF			27		��setitimer(2)�������õĹ���ͳ�� ���ʱ���Ѿ�����ʱ�������źš�
//			SIGWINCH		28		SVR4��4��3+BSDϵͳ�˱�����ÿ���ն˻�α�ն��������*���ҵ�������ֹͣ(stop)�в�ͬ��
//									���塣��������ҵ���ƺ��ź�ʱ�������ἰֹͣ(stopping)�ͼ�����ҵ�������ն���������һ
//									ֱ������ֹͣ��ʾ��Control-S��Control-Q�ַ�ֹͣ�������������ˣ��ն��������򽫲�
//									������ֹͣ�źź��ַ���֮Ϊ�����ַ�(suspend)����ֹͣ�ַ���
//									���ڵĴ�С��һ�����̿�����ioctl����(��11,12��)�õ������ô��ڵĴ�С�����һ��������
//									ioctl������-����-��С��������˴��ڴ�С����ϵͳ�˽�SIGWINCH�ź�������ǰ̨�����顣
//			SIGIO			29		���ź�ָʾһ���첽I��O�¼�����12.6.2�н��Դ˽������ۡ���ͼ10.1��,��SIGIO��ϵͳĬ��
//									��������ֹ�����.���ҵ���,��������ϵͳ.��SVR4��,SIGIO��SIGPOLL��ͬ����Ĭ�϶�������
//									ֹ�˽��̡���4��3+BSD��(���ź���Դ��4.2BSD)����Ĭ�϶����Ǻ�������
//			SIGPOLL			SIGIO	����һ��SVR4�źţ�����һ������ѯ�豸�Ϸ���һ�ض��¼�ʱ�������źš���12.5.2���н�˵
//									��poll�����ʹ��ź�.����4.3+BSD��SIGIO��SIGURG�ź���ӽ�.
// ��WINDOWS��������Ҫ�������źŽ���һЩ����,��Ϊ��Щ�źŲ���֧��
#if defined(PLATFORM_WINDOWS)
#define SIGALRM	14
#define SIGPIPE 13
#define SIGUSR2	12
#define SIGCHLD 17
#endif
typedef void (* SIGNAL_ROUTINE)(int nSignalNo);
SIGNAL_ROUTINE SAFEVCRT_IMP SetSignalRoutine(int nSignalNo,SIGNAL_ROUTINE pfnSignalRoutine);



//////////////////////////////////////////////////////////////////////////
// �̴߳���
#if defined(PLATFORM_WINDOWS)
	typedef HANDLE HTHREAD;
	typedef UINT THREADID;
	typedef UINT HRTHREAD;
	#define HTHREAD_NULL		NULL
	#define HRTHREAD_EXCEPTION	0XFFFFFFFF
#else
	typedef pthread_t HTHREAD;
	typedef pthread_t THREADID;
	typedef VOID* HRTHREAD;
	#define HTHREAD_NULL		0
	#define HRTHREAD_EXCEPTION	(LPVOID)(~0)
#endif
typedef HRTHREAD (WINAPI * LPTHREAD_ENTRY_ROUTE)(LPVOID);
SAFEVCRT_IMP HTHREAD clibCreateThread(LPTHREAD_ENTRY_ROUTE lpStartAddress,LPVOID lpParameter,THREADID* pnThreadId);
SAFEVCRT_IMP THREADID clibGetThreadId();
SAFEVCRT_IMP BOOL clibJoinThread(HTHREAD hThread,HRTHREAD* pnExitCode);
SAFEVCRT_IMP VOID clibExitThread(HRTHREAD nExitCode);
SAFEVCRT_IMP BOOL clibTerminateThread(HTHREAD hThread,HRTHREAD nExitCode);
SAFEVCRT_IMP VOID clibExitProcess();




#if defined(_COMPILE_BY_GCC_)
	// ѭ����λ
	inline unsigned long _lrotl(unsigned long value, int shift)
		{	unsigned long nSeg1 = value>>(size_of(unsigned long)-shift);
			unsigned long nSeg2 = value<<shift;
			return nSeg1+nSeg2;
		}
	inline unsigned long _lrotr(unsigned long value, int shift)
		{	unsigned long nSeg1 = value>>shift;
			unsigned long nSeg2 = value<<(size_of(unsigned long)-shift);
			return nSeg1+nSeg2;
		}
		inline unsigned int _rotl(unsigned int value, int shift)
		{	unsigned int nSeg1 = value>>(size_of(unsigned int)-shift);
			unsigned int nSeg2 = value<<shift;
			return nSeg1+nSeg2;
		}
		inline unsigned int _rotr(unsigned int value, int shift)
		{	unsigned int nSeg1 = value>>shift;
			unsigned int nSeg2 = value<<(size_of(unsigned int)-shift);
			return nSeg1+nSeg2;
		}



	// WINDOWS���������
	#define _alloca alloca
	#define strcmpi stricmp
	#define	_vsnprintf vsnprintf
	#define	_snprintf snprintf
	#define	wcsnicmp wcsncasecmp
	#define	wcsicmp wcscasecmp
	#define	strnicmp strncasecmp
	#define	stricmp strcasecmp
	#define lstrlenW wcslen
	#define lstrlenA strlen
	#define stricmp strcasecmp
	#define _snprintf snprintf
	#define _vsnprintf vsnprintf
	#define memicmp strnicmp
	// �ַ�������
	#ifdef UNICODE
		#define _vstprintf	vswprintf
		#define _stprintf   swprintf
		#define _vsntprintf	_vsnwprintf
		#define _sntprintf	_snwprintf

		#define lstrlen		lstrlenW
		#define _tcspbrk	wcspbrk
		#define _tcsspn     wcsspn
		#define _tcscspn	wcscspn
		#define _tcsstr		wcsstr
		#define _tcschr		wcschr
		#define _tcsrchr	wcsrchr
		#define _tcslen		wcslen
		#define _tcsncmp	wcsncmp
		#define _tcsdup		wcsdup
		#define _ttoi       _wtoi
		#define _istspace   iswspace
		#define _istdigit   iswdigit
		#define _tcsupr		wcsupr
		#define _tcslwr		wcslwr
		#define _tcsrev		wcsrev
		#define _tcsinc		wcsinc
	#else
		#define _vstprintf	vsprintf
		#define _stprintf	sprintf
		#define _vsntprintf	_vsnprintf
		#define _sntprintf	_snprintf

		#define lstrlen		lstrlenA
		#define _tcspbrk	strpbrk
		#define _tcsspn     strspn
		#define _tcscspn	strcspn
		#define _tcsstr		strstr
		#define _tcschr		strchr
		#define _tcsrchr	strrchr
		#define _tcslen		strlen
		#define _tcsncmp	strncmp
		#define _tcsdup		strdup
		#define _ttoi		atoi
		#define _istspace	isspace
		#define _istdigit	isdigit
		#define _tcsupr		strupr
		#define _tcslwr		strlwr
		#define _tcsrev		strrev
		#define _tcsinc		strinc
	#endif
	#if defined(_ARCH_AMD64_)
		#define _atoi64		atol
	#else
		#define _atoi64		atoll
	#endif
	#include "gnu2win.h"
	#include "profile.h"
#endif

// X32��X64����
#ifndef strlen32
	#define strlen32(p)	((unsigned int)strlen(p))
#endif

#if defined(PLATFORM_WINDOWS) && defined(_ARCH_AMD64_)
	#define time32_t	long
	#define time32(p)	((long)time(p))
#elif defined(_ARCH_AMD64_)
	#define time32_t	LONG
	#define time32(p)	((LONG)time(p))
#else
	#define time32_t	time_t
	#define time32(p)	(time(p))
#endif




//////////////////////////////////////////////////////////////////////////
// ����ָ��
#define DECLARE_SMART_CLASS(SmartClassName,RefClassName)																						\
	class SmartClassName																														\
	{																																			\
	public:																																		\
		SmartClassName(RefClassName* pInterface) { m_pInterface=pInterface; }																	\
		SmartClassName(const SmartClassName& Ref) { m_pInterface=Ref.m_pInterface;  if(m_pInterface!=NULL) m_pInterface->QueryInterface(); }	\
		virtual~SmartClassName() { if(m_pInterface!=NULL) m_pInterface->ReleaseInterface(); }													\
		const SmartClassName& operator=(const SmartClassName& Ref)																				\
			{	if(m_pInterface!=NULL) m_pInterface->ReleaseInterface();																		\
				m_pInterface=Ref.m_pInterface;																									\
				if(m_pInterface!=NULL) m_pInterface->QueryInterface();																			\
				return *this;																													\
			}																																	\
		VOID SetHandler(RefClassName* pInterface)																								\
			{	if(m_pInterface!=NULL) m_pInterface->ReleaseInterface();																		\
				m_pInterface=pInterface;																										\
			}																																	\
		BOOL IsValid() { return m_pInterface!=NULL; }																							\
		operator RefClassName* () const { return m_pInterface; }																				\
		RefClassName* operator->() { VERIFY(m_pInterface!=NULL); return m_pInterface; }															\
	protected:																																	\
		RefClassName* m_pInterface;																												\
	};																																			\


#define DECLARE_INTERFACE_CLASS(RefClassName)																									\
	RefClassName* QueryInterface()																												\
	{	InterlockedIncrement(&m_nRefCount);																										\
		return this;																															\
	}																																			\
	LONG ReleaseInterface()																														\
	{	LONG nRef=InterlockedDecrement(&m_nRefCount);																							\
		if(nRef==0)																																\
		{	delete this;																														\
		}																																		\
		return nRef;																															\
	}																																			\


//////////////////////////////////////////////////////////////////////////
// ��ȫ�ڴ����
#ifndef TMALLOC
	inline void* mem_init(void* pMemory,size_t cbSize)
	{	if(pMemory!=NULL&&cbSize>0)
			memset(pMemory,0,cbSize);
		return pMemory;
	}
	#define TMALLOC(n,T)		((T*)mem_init(((n)<=0)?NULL:malloc(size_of(T)*(n)),size_of(T)*(n)))
#endif
#ifndef TFREE
	#define TFREE(a)			if(a!=NULL) { free(a); a=NULL; }
#endif
#ifndef TDEL
	#define TDEL(a)				if(a!=NULL) { delete a; a=NULL; }
#endif
#ifndef TDELARRAY
	#define TDELARRAY(a)		if(a!=NULL) { delete[] a; a=NULL; }
#endif
#ifndef TDELOBJECT
	#define TDELOBJECT(a)		if(a!=NULL) { ::DeleteObject(a); a=NULL; }
#endif
#ifndef TMEMCPY
	inline LONG __memcpy(void* pDest,LONG nDestCount,void* pSrc,LONG nSrcCount,size_t cbUnit)
	{	LONG nCopyCount=min_of(nDestCount,nSrcCount);
		if(nCopyCount>0) memcpy(pDest,pSrc,nCopyCount*cbUnit);
		return nCopyCount;
	}
	#define TMEMCPY(pDest,nDestCount,pSrc,nSrcCount) __memcpy(pDest,nDestCount,pSrc,nSrcCount,size_of(*pDest))
#endif
#ifndef TZEROMEMORY
	#define TZEROMEMORY(pBuffer,cbBuffer) if((pBuffer)!=NULL&&(cbBuffer)!=0) memset(pBuffer,0,cbBuffer)
#endif


// ������(����: ASEStdDef.h)
#ifndef EMBED_OBJECT
#define EMBED_OBJECT
	typedef VOID (* DESTRUCTOR)(VOID*);
	#define DECLARE_DESTRUCT(CLASS)												\
		public:																	\
		static VOID __Destructor(LPVOID pvSelf)									\
		{	CLASS* pSelf=(CLASS*)pvSelf;										\
			if(pSelf!=NULL) pSelf->~CLASS();									\
		}
	typedef VOID* (* CONNECTOR)(VOID*);
	#define DECLARE_CONNECTOR(CLASS,TYPE)										\
		public:																	\
		static VOID* __Connector(LPVOID pvSelf)									\
		{	CLASS* pSelf=(CLASS*)pvSelf;										\
			if(pSelf!=NULL) return (VOID*)((TYPE*)pSelf);						\
			return NULL;														\
		}
	#define DECLARE_CONNECTOR_EX(CLASS,BYCLASS,TYPE)							\
		public:																	\
		static VOID* __Connector(LPVOID pvSelf)									\
		{	CLASS* pSelf=(CLASS*)pvSelf;										\
			if(pSelf!=NULL) return (VOID*)(TYPE*)static_cast<BYCLASS*>(pvSelf);	\
			return NULL;														\
		}
#endif


//////////////////////////////////////////////////////////////////////////
// �ļ�������ļ�·������
#ifndef FFILELENGTH
	#define FFILELENGTH(a)		((fileno(a)>=0)?filelength(fileno(a)):0)
#endif
#ifndef TFCLOSE
	#define TFCLOSE(a)			if(a!=NULL) { fclose(a); a=NULL; }
#endif
// ��ȡ�ļ�����
SAFEVCRT_IMP long filelength2(int fileno);
// �õ�ģ�����ڵ�·��
SAFEVCRT_IMP LPCSTR GetModulePath(HMODULE hModule=NULL,LPSTR lpszBuffer=NULL,LONG nSize=MAX_PATH);
// ת��Ϊ����·��
SAFEVCRT_IMP LPCSTR ConvertAbsolutePath(LPCSTR lpszPath,LPSTR lpszAbsPath,LONG nSize);
// ת��Ϊ����·��
SAFEVCRT_IMP LPCSTR ConvertAbsolutePathAt(LPCSTR lpszAt,LPCSTR lpszPath,LPSTR lpszAbsPath,LONG nSize);
// ת��Ϊ·����
SAFEVCRT_IMP LPCSTR ConvertToDir(LPSTR lpszPath);
// �õ��ļ�·��
SAFEVCRT_IMP LPCSTR GetFileDir(LPCSTR lpszPath,LPSTR lpszDir,LONG nSize);
// �ж��ļ��Ƿ����
SAFEVCRT_IMP BOOL IsPathExist(LPCSTR lpszPath);
// �ж��ļ��Ƿ����
SAFEVCRT_IMP BOOL IsPathExistEx(LPCSTR lpszPath,BOOL bNeedWrite);
// ��ȡ�ļ�ʱ��
SAFEVCRT_IMP time_t GetFileModifyTime(LPCSTR lpszFilePath);
// ��ȡ�ļ�������չ��
SAFEVCRT_IMP LPCSTR GetFileName(LPCSTR pszPath,LPSTR pszTemp,LONG nSize);
SAFEVCRT_IMP LPCSTR GetFileExt(LPCSTR pszPath,LPSTR pszTemp,LONG nSize);
// ɾ��ƥ����ļ�
SAFEVCRT_IMP void DeleteMatchFile(LPCTSTR pszPath,LPCTSTR pszPattern);
SAFEVCRT_IMP void DeleteDirInfo(LPCTSTR pszPath,BOOL bCheckSafe=TRUE);
SAFEVCRT_IMP void DeleteDirs(LPCTSTR pszPath,BOOL bCheckSafe=TRUE);
SAFEVCRT_IMP void CreateDirs(LPCTSTR pszPath);
SAFEVCRT_IMP void CopyMatchFile(LPCTSTR pszFrom,LPCTSTR pszTo,BOOL bIgnoreExists,BOOL bResetAttrib);
// ��̬��װ��
SAFEVCRT_IMP HMODULE LoadLibraryWithPath(LPCTSTR pszDll);
#if defined(PLATFORM_WINDOWS)
	#define fncmp stricmp
#else
	#define fncmp strcmp
#endif


//////////////////////////////////////////////////////////////////////////
// ϵͳ�ַ���������������
#define sprintf0(a,b)			sprintf(a,b)
#define sprintf1(a,b,c)			sprintf(a,b,c)
#define sprintf2(a,b,c,d)		sprintf(a,b,c,d)
#define sprintf3(a,b,c,d,e)		sprintf(a,b,c,d,e)
#define sprintf4(a,b,c,d,e,f)	sprintf(a,b,c,d,e,f)
#define sprintf5(a,b,c,d,e,f,g)	sprintf(a,b,c,d,e,f,g)

#ifndef nsprintf
	inline int __nsprintf(char* lpszString,int nSize,const char* lpszFmt,...)
	{	if(nSize<=0) return 0;
		va_list args;
		va_start(args,lpszFmt);
		int nRet=_vsnprintf(lpszString,nSize,lpszFmt,args);
		if(nRet<0||nRet==nSize)
		{	lpszString[nSize-1]=0;
			nRet=nSize-1;
		}
		va_end(args);
		return nRet;
	}
	#define nsprintf __nsprintf
#endif
#ifndef nvsprintf
	inline int __nvsprintf(char* lpszString,int nSize,const char* lpszFmt,va_list ap)
	{	if(nSize<=0) return 0;
		int nRet=_vsnprintf(lpszString,nSize,lpszFmt,ap);
		if(nRet<0||nRet==nSize)
		{	lpszString[nSize-1]=0;
			nRet=nSize-1;
		}
		return nRet;
	}
	#define nvsprintf __nvsprintf
#endif
#ifndef nscatprintf
	inline int __nscatprintf(char* lpszString,int nSize,const char* lpszFmt,...)
	{	if(nSize<=0) return 0;
		va_list args;
		va_start(args,lpszFmt);
		int nOldLen=strlen32(lpszString);
		if(nOldLen+1>=nSize) return 0;
		int nRet=_vsnprintf(lpszString+nOldLen,nSize-nOldLen,lpszFmt,args);
		if(nRet<0||nRet==nSize-nOldLen)
		{	lpszString[nSize-1]=0;
			nRet=nSize-1-nOldLen;
		}
		va_end(args);
		return nRet;
	}
	#define nscatprintf __nscatprintf
#endif
#ifndef nvscatprintf
	inline int __nvscatprintf(char* lpszString,int nSize,const char* lpszFmt,va_list ap)
	{	if(nSize<=0) return 0;
		int nOldLen=strlen32(lpszString);
		if(nOldLen+1>=nSize) return 0;
		int nRet=_vsnprintf(lpszString+nOldLen,nSize-nOldLen,lpszFmt,ap);
		if(nRet<0||nRet==nSize-nOldLen)
		{	lpszString[nSize-1]=0;
			nRet=nSize-1-nOldLen;
		}
		return nRet;
	}
	#define nvscatprintf __nvscatprintf
#endif
#ifndef nstrcpy
	inline void __nstrcpy(char* lpszString,const char* lpszSrc,int nSize)
	{	if(nSize<=0) return;
		if(lpszSrc!=NULL)
		{	if(lpszSrc[0]==0) { lpszString[0]='\0'; return; }
			strncpy(lpszString,lpszSrc,nSize);
			lpszString[nSize-1]=0;
		}
		else
		{	lpszString[0]=0;
		}
	}
	#define nstrcpy __nstrcpy
#endif
#ifndef nstrcat
	inline void __nstrcat(char* lpszFront,const char* lpszBack,int nSize)
	{	strncat(lpszFront,lpszBack,nSize);
		lpszFront[nSize-1]=0;
	}
	#define nstrcat __nstrcat
#endif

// �ַ�������
#ifndef SAFESTRING
	#define SAFESTRING(a)	(a[size_of(a)-1]=0)
#endif

// �ַ�������
#ifndef COPYSTRARRAY
	#define COPYSTRARRAY(a,b)	nstrcpy((char*)(a),(b),ARRAYSIZE(a))
#endif
#ifndef CATSTRARRAY
	#define CATSTRARRAY(a,b)	nstrcat((char*)(a),(b),ARRAYSIZE(a))
#endif

// ����������
#ifndef COPYBINARRAY
	#define COPYBINARRAY(a,b)		memcpy((a),(b),size_of(a))
#endif

#ifndef COPYBINARRAY2
	#define COPYBINARRAY2(a,b,c)	if(min_of(size_of(a),(c))>0) { memcpy((a),(b),min_of(size_of(a),(c))); }
#endif

// ������ӳ��
#define FUNC_WRAPPER1(T,R,F,T1,P1)							virtual R F(T1 P1)							{ return T->F(P1); }
#define FUNC_WRAPPER2(T,R,F,T1,P1,T2,P2)					virtual R F(T1 P1,T2 P2)					{ return T->F(P1,P2); }
#define FUNC_WRAPPER3(T,R,F,T1,P1,T2,P2,T3,P3)				virtual R F(T1 P1,T2 P2,T3 P3)				{ return T->F(P1,P2,P3); }
#define FUNC_WRAPPER4(T,R,F,T1,P1,T2,P2,T3,P3,T4,P4)		virtual R F(T1 P1,T2 P2,T3 P3,T4 P4)		{ return T->F(P1,P2,P3,P4); }
#define FUNC_WRAPPER5(T,R,F,T1,P1,T2,P2,T3,P3,T4,P4,T5,P5)	virtual R F(T1 P1,T2 P2,T3 P3,T4 P4,T5 P5)	{ return T->F(P1,P2,P3,P4,P5); }


// �����
#define BEGIN_SCOPE()		for(;;)	{
#define END_SCOPE()			break; }


//////////////////////////////////////////////////////////////////////////
// ��ֵ����

// �õ���Χ�����Ƶ�����
#define RANGE_LIMIT(VAL,MIN_VAL,MAX_VAL)  \
	min_of(max_of((VAL),(MIN_VAL)),(MAX_VAL))

#define EN_RANGE_LIMIT(TYPE,VAL,MIN_VAL,MAX_VAL)  \
	VAL=(TYPE)min_of(max_of((VAL),(MIN_VAL)),(MAX_VAL))


// ��߸������ľ���
SAFEVCRT_IMP double RoundDouble(double dExpression,int nNumDecimalPlaces=2);


// ��չ��ѧ����(���Լ�����,��С���������,���ھ����㷨)
SAFEVCRT_IMP int math_gcd(int a,int b);			// �������������Լ��(ŷ������㷨)
SAFEVCRT_IMP int math_lcm(int a,int b);			// ������������С������
SAFEVCRT_IMP int math_ngcd(int* pa,int n);		// ��N���������Լ��
SAFEVCRT_IMP int math_nlcm(int* pa,int n);		// ��N��������С������



//////////////////////////////////////////////////////////////////////////
// ��ջ���ٺ���
//		��ջ����ʵ��
//		STACKFRAME StackFrame=GetPrevStackFrame(NULL);
//		while(StackFrame.m_lpNextEIP!=NULL&&StackFrame.m_lpEBP!=NULL)
//			StackFrame=GetPrevStackFrame(StackFrame.m_lpEBP);
typedef struct tagSTACKFRAME
{	LPVOID m_lpEBP;			// ���õ�ջ��ַ
	LPVOID m_lpESP;			// ���õ�ջ��
	LPVOID m_lpNextEIP;		// �������ص�ַ
} STACKFRAME,*LPSTACKFRAME;
typedef struct tagSTACKSNAP
{	LPVOID m_lpEBP;			// ���õ�ջ��ַ
	LPVOID m_lpESP;			// ���õ�ջ��
	LPVOID m_lpNextEIP;		// �������ص�ַ
	BYTE   m_szStack[1];	// ��ջ����
} STACKSNAP,*LPSTACKSNAP;
SAFEVCRT_IMP STACKFRAME GetPrevStackFrame(LPVOID lpEBP);
SAFEVCRT_IMP LPVOID GetNextEIP();





//////////////////////////////////////////////////////////////////////////
// ���Ժ���
SAFEVCRT_IMP VOID clibSetSwapDir(LPCSTR pszSwapDir);
SAFEVCRT_IMP LPCSTR clibGetSwapDir();

// ��ӡ��������
SAFEVCRT_IMP VOID clibTrace(LPCSTR pszFormat,...);
#ifdef _DEBUG
	#ifndef CLIBTRACE
		#define CLIBTRACE   clibTrace
	#endif
#else
	#ifndef CLIBTRACE
		#define CLIBTRACE   1 ? (void)0 : clibTrace
	#endif
#endif

// ��ӡ��������(�ڴ�)
SAFEVCRT_IMP VOID clibTraceMemory(LPVOID lpBuffer,LONG nSize,LPCSTR lpszFmt,...);


// ����
SAFEVCRT_IMP VOID clibReportAssert(LPCSTR lpszFile,LONG nLine,LPCSTR lpszExpr);
#ifdef _DEBUG
	#ifndef CLIBASSERT
		#define CLIBASSERT(expr) {if(!(expr)) { clibReportAssert(__FILE__, __LINE__,#expr); }}
	#endif
#else
	#ifndef CLIBASSERT
		#define CLIBASSERT(f)    ((void)0)
	#endif
#endif

// ����
SAFEVCRT_IMP VOID clibReportVerify(LPCSTR lpszFile,LONG nLine,LPCSTR lpszExpr);
#ifndef CLIBVERIFY
	#define CLIBVERIFY(expr) {if(!(expr)) { clibReportVerify(__FILE__, __LINE__,#expr); }}
#endif


// ��ַ����
SAFEVCRT_IMP BOOL clibIsValidAddress(const void* lp,UINT nBytes,BOOL bReadWrite=TRUE);


// ����
SAFEVCRT_IMP VOID clibReportFixMe(LPCSTR lpszFile,LONG nLine,LPSTR lpszInfo);
#ifndef CLIB_FIXME
	#define CLIB_FIXME(lpszInfo) { clibReportFixMe(__FILE__, __LINE__,lpszInfo); }
#endif
#define CLIB_BUGBUG	CLIB_FIXME

SAFEVCRT_IMP VOID clibDump(LPCSTR pszFormat,...);
#ifdef _DEBUG
	#ifndef CLIBDUMP
		#define CLIBDUMP   clibDump
	#endif
#else
	#ifndef CLIBDUMP
		#define CLIBDUMP   1 ? (void)0 : clibDump
	#endif
#endif

SAFEVCRT_IMP VOID clibDumpMemory(LPVOID lpBuffer,LONG nSize,LPCSTR lpszFmt,...);
SAFEVCRT_IMP VOID SetDebugBreak(BOOL bBreakOn);
SAFEVCRT_IMP VOID CatchDebugBreak();
SAFEVCRT_IMP LPCSTR GetMessageName(UINT nMsg);
#ifdef _DEBUG
	#define IS_DEBUG_VERSION	(1)
#else
	#define IS_DEBUG_VERSION	(0)
#endif

//////////////////////////////////////////////////////////////////////////
// ������������
SAFEVCRT_IMP VOID AddEnvironmentSearchPath(LPCSTR pszPath);



// �汾�����汾�Ż���ת��
SAFEVCRT_IMP WORD VersionString2Word(LPCSTR lpszVersionString,LPWORD lpwRelease);
SAFEVCRT_IMP VOID VersionWordString(WORD wVersion,WORD wRelease,LPSTR lpszVersionString,LONG nSize);
SAFEVCRT_IMP WORD FloatVersionToWord(FLOAT fVersion);
SAFEVCRT_IMP DWORD VersionString2Dword(LPCSTR lpszVersionString);
SAFEVCRT_IMP VOID VersionDwordString(DWORD dwVersion,LPSTR lpszVersionString,LONG nSize);
SAFEVCRT_IMP DWORD FloatVersionToDword(FLOAT fVersion);
SAFEVCRT_IMP LONG CompareVersion(DWORD dwVersion1,DWORD dwVersion2);


// �����ļ���ȡ
SAFEVCRT_IMP BOOL WritePrivateProfileInt(LPCSTR lpAppName,LPCSTR lpKeyName,INT nValue,LPCSTR lpFileName);
#ifndef GetPrivateProfileStrArray
	#define GetPrivateProfileStrArray(lpAppName,lpKeyName,lpDefault,lpReturnedString,lpFileName)					\
	GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,size_of(lpReturnedString),lpFileName);
#endif



//////////////////////////////////////////////////////////////////////////
// �ַ�������
#define STRING_EMPTY	_T("")																	// �մ�
#define STRING_PERCENTS _T("%s")																// "%s"
SAFEVCRT_IMP char* strtoupr(char*string);														// �ַ�����Сдת��
SAFEVCRT_IMP BOOL PatternCompare(LPCSTR lpszSource,LPCSTR lpszPattern);							// ģ��ƥ��
SAFEVCRT_IMP BOOL PatternCompareAsFileName(LPCSTR lpszSource,LPCSTR lpszPattern);				// ģ��ƥ��
SAFEVCRT_IMP const char* strnstr(const char* str1,size_t size,const char * str2);				// ���ַ����ڲ���
SAFEVCRT_IMP const char* strnistr(const char* str1,size_t size,const char * str2);				// ���ַ����ڲ���(���Դ�Сд)
SAFEVCRT_IMP const char* strnchr(const char* str1,size_t size,const char ch);					// ���ַ����ڲ���
SAFEVCRT_IMP LPSTR AllTrimStr(LPSTR lpszString);												// ȥ��ͷ��β��ȫ���ո�
SAFEVCRT_IMP LPSTR AllTrimSpace(LPSTR lpszString);												// ȥ��ͷ��β��ȫ���ո�(ͬAllTrimStr)
SAFEVCRT_IMP LPSTR AllTrimBlank(LPSTR lpszString);												// ȥ��ͷ��β��ȫ���ո��TAB��
SAFEVCRT_IMP LPSTR AllTrimBlankAndZero(LPSTR lpszString);										// ȥ��ͷ��β��ȫ���ո��TAB���Լ�ͷ����'0'�ַ�
SAFEVCRT_IMP LPSTR TrimSpace(LPSTR lpszString);													// ȥ��ȫ���ո�(�����ַ����м��)
SAFEVCRT_IMP LPSTR TrimBlank(LPSTR lpszString);													// ȥ��ȫ���ո��TAB��(�����ַ����м��)
SAFEVCRT_IMP LPSTR TrimNonAlpla(LPSTR lpszString);												// ȥ��ȫ����Alpha�ַ�(�����ַ����м��)
SAFEVCRT_IMP LPSTR TrimNonEnglish(LPSTR lpszString);											// ȥ��ȫ����Ӣ���ַ�(�����ַ����м��)
SAFEVCRT_IMP LPSTR TrimExclude(LPSTR lpszString,LPCTSTR pszExclude);							// ȥ��ָ����Χ���ַ�(�����ַ����м��)
SAFEVCRT_IMP LPSTR TrimPathStr(LPSTR lpszString,LONG nSize);									// ��ʽ��·����
SAFEVCRT_IMP LPSTR TrimComment(LPSTR lpszString,LPCSTR pszLead);								// ȥ��ע��
SAFEVCRT_IMP LPSTR StrLowerCase(LPSTR lpszString);												// ת���ַ�����Сд
SAFEVCRT_IMP LPSTR StrUpperCase(LPSTR lpszString);												// ת���ַ�������д
SAFEVCRT_IMP LPSTR Unix2Dos(LPSTR lpBuffer,LONG nSize);											// ת��UNIX��ʽ�ı���DOS��ʽ�ı�
SAFEVCRT_IMP LPSTR FormatStrToWidth(LPSTR lpOut,LPCSTR lpIn,LONG nSize);						// �ַ�������
SAFEVCRT_IMP BOOL IsTailBy(LPCSTR lpszString,CHAR cChar);										// �ж��ַ����������ַ���β
SAFEVCRT_IMP LONG FindString(LPCSTR lpszString,LPCSTR lpszFindWhat,LONG nFrom=0);				// �ַ�������
SAFEVCRT_IMP LONG ReverseFindString(LPCSTR lpszString,LPCSTR lpszFindWhat);						// �ַ�������
SAFEVCRT_IMP LONG FindChar(LPCSTR lpszString,CHAR cFindWhat,LONG nFrom=0);						// �ַ�����
SAFEVCRT_IMP LONG ReverseFindChar(LPCSTR lpszString,CHAR cFindWhat);							// �ַ�����
SAFEVCRT_IMP LONG FindCharOneOf(LPCSTR lpszString,LPCSTR lpszCharSet,LONG nFrom=0);				// �ַ�����
SAFEVCRT_IMP BOOL ReplaceString(LPSTR lpStr,DWORD dwSize,LPCSTR lpszFrom,LPCSTR lpszTo);		// �ַ����滻
SAFEVCRT_IMP LPSTR ReplaceStringEx(LPSTR lpStr,LPCSTR lpszFrom,LPCSTR lpszTo);					// �ַ����滻
SAFEVCRT_IMP VOID ReplaceString(LPSTR lpStr,DWORD dwSize,TCHAR chFrom,TCHAR chTo);				// �ַ����滻
SAFEVCRT_IMP VOID ReplaceStringEx(LPSTR lpStr,TCHAR chFrom,TCHAR chTo);							// �ַ����滻
SAFEVCRT_IMP LPSTR GetMidString(LPSTR lpszString,LONG nOffset,LONG nLength,LPSTR lpszTarget);	// ֱ�ӽ�ȡ�Ӵ�
SAFEVCRT_IMP LPSTR GetLeftString(LPSTR lpszString,LONG nLength,LPSTR lpszTarget);				// ֱ�ӽ�ȡ�Ӵ�(��)
SAFEVCRT_IMP LPSTR GetRightString(LPSTR lpszString,LONG nLength,LPSTR lpszTarget);				// ֱ�ӽ�ȡ�Ӵ�(��)
SAFEVCRT_IMP LPSTR MidString(LPSTR lpszString,LONG nOffset,LONG nLength=-1);					// ֱ�ӽ�ȡ�Ӵ�
SAFEVCRT_IMP LPSTR LeftString(LPSTR lpszString,LONG nCount);									// ȡ�����ַ���
SAFEVCRT_IMP LPSTR RightString(LPSTR lpszString,LONG nCount);									// ȡ�Ҳ���ַ���
SAFEVCRT_IMP BOOL IsNumber(LPCSTR lpszString);													// �ж��Ƿ������ִ�
SAFEVCRT_IMP VOID RandString(LPSTR pszString,LONG nStringLen);									// ��������ַ���
SAFEVCRT_IMP BOOL ShiftBuffer(LPCSTR& pszBuffer,DWORD& cbBuffer,CHAR chSplitter,LPCSTR& pszPart,DWORD& cbPart);	// �ӻ��������Ƴ�һƬ




//////////////////////////////////////////////////////////////////////////
// ����ת��

// HEX�ַ�ת��Ϊ����
#define HEXCHARTOBYTE(ch)													\
	(BYTE)(((BYTE)(ch)>='A'&&(BYTE)(ch)<='Z')?((ch)-'A'+10):				\
	(((BYTE)(ch)>='a'&&(BYTE)(ch)<='z')?(ch-'a'+10):(ch-'0')))				\

// HEX����ת��Ϊ�ַ�
#define HEXBYTETOCHAR(ch)													\
	(CHAR)(((BYTE)(ch)<=9)?((ch)+'0'):((ch)-10+'A'))						\

// MD5����������(����: ASEStdDef.h)
#ifndef MD5BIN_DEFINED
	#define MD5BIN_DEFINED
	#pragma pack(1)
	typedef struct tagMD5BIN	// MD5��ṹ
	{	BYTE m_szMD5[16];		// MD5�뻺����
	} MD5BIN,*LPMD5BIN;
	typedef const MD5BIN* LPCMD5BIN;
	const MD5BIN md5Null={{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
	#pragma pack()
	inline bool operator==(const MD5BIN s1,const MD5BIN s2)
	{	if(memcmp(&s1,&s2,size_of(MD5BIN))==0) return true;
		return false;
	}
	#if 0
		inline bool operator!=(const MD5BIN s1,const MD5BIN s2)
		{	if(memcmp(&s1,&s2,size_of(MD5BIN))==0) return false;
			return true;
		}
	#endif
#endif

// ��ĿHASH����(����: ASEStdDef.h)
#ifndef IHASH_DEFINED
#define IHASH_DEFINED
	#pragma pack(1)
	typedef struct tagIHASH		// ��ĿHASH�ṹ
	{	DWORD m_dwSize;			// ��Ŀ��С
		BYTE m_acHash[16];		// ��ĿHASH
	} IHASH,*LPIHASH;
	const IHASH ihashNull={0,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
	#pragma pack()
	inline bool operator==(const IHASH s1,const IHASH s2)
	{	if(memcmp(&s1,&s2,size_of(IHASH))==0) return true;
		return false;
	}
	#if 0
		inline bool operator!=(const IHASH s1,const IHASH s2)
		{	if(memcmp(&s1,&s2,size_of(IHASH))==0) return false;
			return true;
		}
	#endif
#endif

SAFEVCRT_IMP LPCSTR BoolToStr(BOOL i,LPSTR lpszTemp,LONG nSize);											// BOOL��ת�ַ���
SAFEVCRT_IMP LPCSTR ByteToStr(BYTE i,LPSTR lpszTemp,LONG nSize);											// BYTE��ת�ַ���
SAFEVCRT_IMP LPCSTR DwordToStr(DWORD i,LPSTR lpszTemp,LONG nSize);											// DWORD��ת�ַ���
SAFEVCRT_IMP LPCSTR UintToStr(UINT i,LPSTR lpszTemp,LONG nSize);											// UINT��ת�ַ���
SAFEVCRT_IMP LPCSTR UlongToStr(ULONG i,LPSTR lpszTemp,LONG nSize);											// ULONG��ת�ַ���
SAFEVCRT_IMP LPCSTR UshortToStr(USHORT i,LPSTR lpszTemp,LONG nSize);										// USHORT��ת�ַ���
SAFEVCRT_IMP LPCSTR CharToStr(CHAR i,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);							// CHAR��ת�ַ���
SAFEVCRT_IMP LPCSTR IntToStr(INT i,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);								// INT��ת�ַ���
SAFEVCRT_IMP LPCSTR Int64ToStr(INT64 i,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);							// INT64��ת�ַ���
SAFEVCRT_IMP LPCSTR Uint64ToStr(UINT64 i,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);						// UINT64��ת�ַ���
SAFEVCRT_IMP LPCSTR LongToStr(LONG i,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);							// LONG��ת�ַ���
SAFEVCRT_IMP LPCSTR ShortToStr(SHORT i,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);							// SHORT��ת�ַ���
SAFEVCRT_IMP LPCSTR FloatToStr(FLOAT fValue,LONG nDecimal,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);		// CY����(FLOAT)ת�ַ���
SAFEVCRT_IMP LPCSTR DoubleToStr(DOUBLE dValue,LONG nDecimal,LPSTR lpszTemp,LONG nSize,BOOL bSign=FALSE);	// CY����(DOUBLE)ת�ַ���
SAFEVCRT_IMP LONG FloatToLong(FLOAT fValue,LONG nDecimal);													// CY����(FLOAT)תLONG
SAFEVCRT_IMP LONG DoubleToLong(DOUBLE dValue,LONG nDecimal);												// CY����(DOUBLE)תLONG
SAFEVCRT_IMP FLOAT LongToFloat(LONG nValue,LONG nDecimal);													// CY����(LONG)תFLOAT
SAFEVCRT_IMP DOUBLE LongToDouble(LONG nValue,LONG nDecimal);												// CY����(LONG)תDOUBLE
SAFEVCRT_IMP DWORD StringToDword(LPCSTR lpszString);														// �����ַ���ת��(֧��16���Ʊ���)
SAFEVCRT_IMP UINT64 StringToUint64(LPCSTR lpszString);														// �����ַ���ת��(֧��16���Ʊ���)
SAFEVCRT_IMP LPVOID StringToPointer(LPCSTR lpszString);														// ָ���ַ���ת��
SAFEVCRT_IMP LPCSTR DwordToHexStr(DWORD dwVar,LPSTR lpszTemp,LONG nSize);									// ����ת��Ϊ16�����ַ���
SAFEVCRT_IMP DWORD HexStrToDword(LPCSTR pszString,DWORD cbString);											// 16�����ַ���ת��Ϊ����

// 16����ת��
SAFEVCRT_IMP DWORD HexStrToBinary(LPCSTR lpszString,LPBYTE pBuffer,DWORD cbBuffer);							// �����ַ���ת��(֧��16���Ʊ���)
SAFEVCRT_IMP DWORD HexStrToBinaryEx(LPCSTR lpszString,DWORD dwStrLen,LPBYTE pBuffer,DWORD cbBuffer);		// �����ַ���ת��(֧��16���Ʊ���)
SAFEVCRT_IMP LPCSTR BinaryToHexStr(LPCBYTE pBuffer,DWORD cbBuffer,LPSTR lpszTemp,LONG nSize);				// ����ת��Ϊ16�����ַ���

// ����ַ�������
#define STRING_COMMON		(0)			// �����ַ���
#define STRING_NUMBER		(1)			// ����
#define STRING_CHARACTER	(2)			// �ַ�
#define STRING_ALPHANUM		(3)			// ���ֺ��ַ�
#define STRING_FLOAT		(4)			// ����
#define STRING_EMAIL		(5)			// �ʼ���ַ
#define STRING_IPV4ADDR		(6)			// IPv4��ַ
SAFEVCRT_IMP BOOL IsStringType(LPCSTR pszString,LONG nType);


#define BOOLTOSTR(var,szTemp)		BoolToStr(var,szTemp,size_of(szTemp))
#define BYTETOSTR(var,szTemp)		ByteToStr(var,szTemp,size_of(szTemp))
#define UINTTOSTR(var,szTemp)		UintToStr(var,szTemp,size_of(szTemp))
#define ULONGTOSTR(var,szTemp)		UlongToStr(var,szTemp,size_of(szTemp))
#define USHORTTOSTR(var,szTemp)		UshortToStr(var,szTemp,size_of(szTemp))
#define CHARTOSTR(var,szTemp)		ByteToStr(var,szTemp,size_of(szTemp))
#define INTTOSTR(var,szTemp)		IntToStr(var,szTemp,size_of(szTemp))
#define LONGTOSTR(var,szTemp)		LongToStr(var,szTemp,size_of(szTemp))
#define SHORTTOSTR(var,szTemp)		ShortToStr(var,szTemp,size_of(szTemp))
#define PRICE_MIN					(0.0001f)
#define FLOAT_MIN					(0.000001f)

SAFEVCRT_IMP BOOL StringToBool(LPCSTR lpszString);															// BOOL��ת�ַ���
SAFEVCRT_IMP LPCSTR BoolToString(BOOL bVar,LPSTR lpszTemp,LONG nSize);										// �ַ���תBOOL��
SAFEVCRT_IMP COLORREF StringToColor(LPCSTR lpszString);														// COLORREF��ת�ַ���
SAFEVCRT_IMP LPCSTR ColorToString(COLORREF crVar,LPSTR lpszTemp,LONG nSize);								// �ַ���תCOLORREF��
#ifdef PLATFORM_WINDOWS
	SAFEVCRT_IMP DWORD StringToHotKey(LPCSTR lpszString);													// HOTKEY��ת�ַ���
	SAFEVCRT_IMP LPCSTR HotKeyToString(DWORD dwVar,LPSTR lpszTemp,LONG nSize);								// �ַ���תHOTKEY��
#endif
SAFEVCRT_IMP MD5BIN StringToMd5(LPCSTR lpszString);															// MD5��ת�ַ���
SAFEVCRT_IMP LPCSTR Md5ToString(MD5BIN md5Var,LPSTR lpszTemp,LONG nSize);									// �ַ���תMD5��
SAFEVCRT_IMP RECT StringToRect(LPCSTR lpszString);															// RECT��ת�ַ���
SAFEVCRT_IMP LPCSTR RectToString(RECT rcVar,LPSTR lpszTemp,LONG nSize);										// �ַ���תRECT��
SAFEVCRT_IMP POINT StringToPoint(LPCSTR lpszString);														// POINT��ת�ַ���
SAFEVCRT_IMP LPCSTR PointToString(POINT ptVar,LPSTR lpszTemp,LONG nSize);									// �ַ���תPOINT��
SAFEVCRT_IMP SIZE StringToSize(LPCSTR lpszString);															// SIZE��ת�ַ���
SAFEVCRT_IMP LPCSTR SizeToString(SIZE szVar,LPSTR lpszTemp,LONG nSize);										// �ַ���תSIZE��

const RECT rectNull={0,0,0,0};
const SIZE sizeNull={0,0};
const POINT pointNull={0,0};



//////////////////////////////////////////////////////////////////////////
// �ַ�����������,�����ַָ���ַ���ת��Ϊ����
// (ע��:����ת��ʧ�ܽ��ƻ�ԭ�л�����)
SAFEVCRT_IMP LONG ParseStrFields(LPSTR lpszString,CHAR cSplitter,BOOL bTransBracket,LPSTR* lppFields,LONG nMaxField);
// �����ֶ�
SAFEVCRT_IMP LONG FindStrFields(LPCSTR lpszField,LPSTR* lppFields,LONG nFieldNum);
// �ı�����������(�ֽ��ı���,��������ı��ķֽ��ͷ���ټ���,�������ָ����,�����кα���Ϊ���һ���ı�)
SAFEVCRT_IMP LONG ParseTextFields(LPSTR lpszText,LPCSTR lpszSplitter,LONG nWantField,LPSTR* lppFields);
// �ַ�����������,���ַ����ָ���ַ���ת��Ϊ����
SAFEVCRT_IMP LONG ParseStrFieldsEx(LPSTR lpszString,LPCSTR lpszSplitter,BOOL bTransBracket,LPSTR* lppFields,LONG nMaxCount);
SAFEVCRT_IMP LONG ParseNStrFieldsEx(LPSTR lpszString,LPCSTR lpszSplitter,BOOL bTransBracket,BOOL bParseAll,LPSTR* lppFields,LONG nMaxCount);
// �ֽ�A:\r\nB\r\nA:\r\nB\r\n��ʽ���ַ���Ϊ����
SAFEVCRT_IMP LONG ParseLabelFields(LPSTR lpszString,LONG nMaxCount,LPSTR* lppLabels,LPSTR* lpContent);
// �ֽ�A:B\r\nA:B\r\n��ʽ���ַ���Ϊ����
SAFEVCRT_IMP LONG ParseLabelFieldsEx(LPSTR lpszString,LONG nMaxCount,LPSTR* lppLabels,LPSTR* lpContent,LPCSTR pszLineSpliter="\r\n",LPCSTR pszFieldSplitter=":");
SAFEVCRT_IMP LONG ParseNLabelFieldsEx(LPSTR lpszString,LONG nMaxCount,BOOL bParseAll,LPSTR* lppLabels,LPSTR* lpContent,LPCSTR pszLineSpliter="\r\n",LPCSTR pszFieldSplitter=":");
// ���ַ�������ȡ�ַ���(��Ŵ�1��ʼ)
SAFEVCRT_IMP LONG GetSpliterCount(LPCSTR lpszSrc,CHAR cSpliter);
SAFEVCRT_IMP LONG GetSpliterCount(LPCSTR lpszSrc,LPCSTR lpszSpliter);
SAFEVCRT_IMP LPCSTR GetStr(LPCSTR lpszSrc,LPSTR lpszDest,LONG nSize,LONG nIndex,CHAR cSpliter);
SAFEVCRT_IMP LPCSTR GetStrEx(LPCSTR lpszSrc,LPSTR lpszDest,LONG nSize,LONG nIndex,LPCSTR lpszSpliter);
SAFEVCRT_IMP LPCSTR GetKeyStr(LPCSTR lpszSrc,LPSTR lpszDest,LONG nSize,LPCTSTR pszKey,CHAR cSpliter,CHAR cSpliter2='=');
SAFEVCRT_IMP LPCSTR GetKeyStrEx(LPCSTR lpszSrc,LPSTR lpszDest,LONG nSize,LPCTSTR pszKey,LPCSTR lpszSpliter,CHAR cSpliter2='=');






//////////////////////////////////////////////////////////////////////////
// �����ת��
// ����BASE64����ͽ���
// ����BASE64����,����ĳ���,���뻺�������Ⱥ�ԭʼ���ȵù�ϵΪ
// ����󳤶�=(����ǰ����*8+5)/6
SAFEVCRT_IMP LONG Base64DecodeString(LPCSTR lpszDecoding,LPBYTE lpszDecoded,LONG nDecodedSize);
SAFEVCRT_IMP LONG Base64DecodeBinary(LPCSTR lpszDecoding,LPBYTE lpszDecoded,LONG nDecodedSize);
SAFEVCRT_IMP LONG Base64Encode(LPBYTE lpszEncoding,LONG nInputSize,LPSTR lpszEncoded,LONG nEncodedSize);
#define Base64Decode Base64DecodeString

// ����URL����ͽ���
// ����URL����,����ĳ���,���뻺�������Ⱥ�ԭʼ���ȵù�ϵΪ
// ����URL����,����Ļ��������Ȱ���'\0',�����ַ�������(������'\0')
SAFEVCRT_IMP LONG UrlEncode(LPCSTR bufin,LPSTR bufcoded,LONG outbufmax);
SAFEVCRT_IMP LPCSTR UrlDecode(LPSTR bufinout);
SAFEVCRT_IMP LONG UrlDecode(LPCSTR bufcoded,LPSTR bufplain,LONG outbufsize);
SAFEVCRT_IMP LPCSTR JsonDecode(LPSTR pszCoded);

// ���ô���ҳ
#define CP_GB18030	54936
#define CP_GB2312	936
#define CP_BIG5		950
#define CP_ENU		1252
#define CP_UTF16LE	1200
#define CP_UTF16BE	1201
// ����ת��
SAFEVCRT_IMP LONG GB2312ToBIG5(LPCSTR pszGb2312,LONG nGb2312Len,LPSTR pszBig5,LONG cbBig5);
SAFEVCRT_IMP LONG BIG5ToGB2312(LPCSTR pszBig5,LONG nBig5Len,LPSTR pszGb2312,LONG cbGb2312);
SAFEVCRT_IMP VOID ConvertCodePage(LPTSTR pszBuf,LONG cbBuf,UINT nDataCodePage,UINT nUserCodePage,BOOL bToUser);
// MBCS��UTF8ת��
SAFEVCRT_IMP LONG UTF8ToMBCS(LPCSTR pszUtf8,UINT nCodePage,LPSTR pszMbcs,LONG cbMbcs);
SAFEVCRT_IMP LONG MBCSToUTF8(LPCSTR pszMbcs,UINT nCodePage,LPSTR pszUtf8,LONG cbUtf8);
SAFEVCRT_IMP LONG MBCSTransCodePage(LPCSTR pszSrc,UINT nSrcCodePage,LPSTR pszDest,LONG cbDest,UINT nDestCodePage);










//////////////////////////////////////////////////////////////////////////
// ʱ�������Լ�ת������
#pragma pack(1)
	struct bc_date
	{	unsigned short da_year; // ��(YYYY)
		char  da_mon;			// ��(MM)
		char  da_day;			// ��(DD)
	};
	struct bc_time
	{	char  ti_hour;			// ʱ(HH)
		char  ti_min;			// ��(MM)
		char  ti_sec;			// ��(SS)
	};
#pragma pack()
SAFEVCRT_IMP UINT64 GetMicroseconds();																	// ��ȡ��������������΢����
SAFEVCRT_IMP DWORD GetMillSeconds();																	// ��ȡϵͳ���������ĺ�����
SAFEVCRT_IMP struct tm get_localtime(const time_t* ptTime);												// ȡGMT��Ӧ�ı���ʱ��
SAFEVCRT_IMP LONG get_ydays(LONG nYear);																// ȡ��ĳ�������
SAFEVCRT_IMP LONG get_mdays(LONG nYear,LONG nMonth);													// ȡ��ĳ��ĳ�µ�����
SAFEVCRT_IMP LONG get_nextdays(LONG nDate,LONG nSpanDays);												// ȡ��ĳ���ں�N�������
SAFEVCRT_IMP LONG get_prevdays(LONG nDate,LONG nSpanDays);												// ȡ��ĳ����ǰN�������
SAFEVCRT_IMP LONG get_spandays(LONG nDate1,LONG nDate2);												// ��ȡ��������֮��ļ��
SAFEVCRT_IMP VOID gettime(struct bc_time* timep);														// �õ���ǰʱ��
SAFEVCRT_IMP LONG getltime();																			// �õ���ǰʱ��
SAFEVCRT_IMP VOID getdate(struct bc_date* datep);														// �õ���ǰ����
SAFEVCRT_IMP LONG getldate();																			// �õ���ǰ����
SAFEVCRT_IMP LONG time2ldate(time_t tTime);																// ʱ��ת��Ϊ����
SAFEVCRT_IMP BOOL IsLongTimeInRange(DWORD dwTime,DWORD dwTime1,DWORD dwTime2);							// �ж�ʱ���Ƿ��ڷ�Χ��(֧�ָ�ҹ,������ǰһ��ʱ���)
SAFEVCRT_IMP time_t MakeTime(LONG nYear,LONG nMon,LONG nDay,LONG nHour,LONG nMin,LONG nSec);			// ת��Ϊʱ��
SAFEVCRT_IMP time_t MakeTime(LONG lDate,LONG lTime);													// ת��Ϊʱ��
SAFEVCRT_IMP time_t TimeFromInetString(LPCSTR lpszTime);												// �ַ���ת����ʱ��(Fri, 02 Jun 2006 08:01:17��ʽ)
SAFEVCRT_IMP time_t TimeFromString(LPCSTR lpszTime);													// �ַ���ת����ʱ��(YYYY-MM-DD HH:MM:SS��ʽ)
SAFEVCRT_IMP LPCSTR TimeToString(time_t tTime,LPSTR lpszTime,LONG nSize);								// ʱ��ת�����ַ���(YYYY-MM-DD HH:MM:SS��ʽ)
SAFEVCRT_IMP LPCSTR TimeToShortString(time_t tTime,LPSTR lpszTime,LONG nSize);							// ʱ��ת�����ַ���(HH:MM:SS��ʽ)
SAFEVCRT_IMP DWORD LongTimeFromString(LPCSTR lpszTime);													// �ַ���ת����ʱ��(HH:MM:SS��ʽ)
SAFEVCRT_IMP LPCSTR LongTimeToString(DWORD dwTime,LPSTR lpszTime,LONG nSize);							// ʱ��ת�����ַ���(HH:MM:SS��ʽ)
SAFEVCRT_IMP DWORD LongDateFromString(LPCSTR lpszDate);													// �ַ���ת��������(YYYY-MM-DD��ʽ)
SAFEVCRT_IMP LPCSTR LongDateToString(DWORD dwDate,LPSTR lpszDate,LONG nSize);							// ����ת�����ַ���(YYYY-MM-DD��ʽ)
SAFEVCRT_IMP systm_t systm();																			// ȡ��ϵͳʱ��(���뵥λ)
SAFEVCRT_IMP LPCSTR SysTmToShortString(systm_t tTime,LPSTR lpszTime,LONG nSize);						// ʱ��ת�����ַ���(HH:MM:SS.mmm��ʽ)
SAFEVCRT_IMP LPCSTR SysTmToLongString(systm_t tTime,LPSTR lpszTime,LONG nSize);							// ʱ��ת�����ַ���(YYMMDD-HH:MM:SS.mmm��ʽ)

#define time2systm(t) ((systm_t)t*1000)


//////////////////////////////////////////////////////////////////////////
// �汾����
SAFEVCRT_IMP VOID BuildDate2Long(LPCSTR pszBuildDate,LONG* pnBuildDate);
SAFEVCRT_IMP VOID BuildTime2Long(LPCSTR pszBuildTime,LONG* pnBuildTime);
SAFEVCRT_IMP LONG GetBuildNo(LPCSTR pszBuildDate,LPCSTR pszBuildTime);
SAFEVCRT_IMP LPCSTR GetBuildNoStr(LPCSTR pszBuildDate,LPCSTR pszBuildTime,LPSTR pszTemp,DWORD cbTemp);

#ifdef USE_VERSION_EX
	// �滻��ִ���ļ���ͼ��(���ܵ���Ŀ��������GroupError)
	SAFEVCRT_IMP BOOL ReplaceIcon(LPCTSTR lpszPEFile,LPCTSTR lpszIconFile);
	// ��ȡPE�汾��Ϣ��������Ϣ
	SAFEVCRT_IMP BOOL PEGetVersionInfoAndTranslation(LPCSTR pszPEFile,VS_FIXEDFILEINFO* pVersionInfo,WORD* pwLanguage,WORD* pwCodePage);
	// ����PE�汾��Ϣ
	SAFEVCRT_IMP BOOL PESetVersionInfo(LPCSTR pszPEFile,VS_FIXEDFILEINFO* pVersionInfo);
	SAFEVCRT_IMP BOOL PESetVersion(LPCSTR pszPEFile,LPCSTR pszVersion);
	SAFEVCRT_IMP BOOL PESetVersionIncrease(LPCSTR pszPEFile);
#endif






//========================================================================================================================
// ������
SAFEVCRT_IMP VOID SortValue(DWORD* pdwValues,const DWORD* pdwKeys,DWORD dwCount,BOOL bIncrease);
SAFEVCRT_IMP VOID SortValue(DWORD* pdwValues,const INT64* pi64Keys,DWORD dwCount,BOOL bIncrease);







//========================================================================================================================
// ��ַ��������
// ����URL����վ����,����ftp://abc@abc:kaka.com.cn:23/index.html#link1
// �������ݰ���:Э����(Сд),��������,�˿ں�,Ŀ��URL,Ŀ���,�Ƿ�ʹ���û�У��,�û���,����
// ���ܽ������ص�ַ�͹�����ַ
// lpszProtocol		MAX_INET_SCHEME_LEN
// lpszHost			MAX_INET_HOSTNAME_LEN
// lpszFolder		MAX_INET_PATH_LEN
// lpszObject		MAX_INET_PATH_LEN
// lpszObjectName	MAX_INET_PATH_LEN
// lpszParagraph	MAX_INET_PATH_LEN
// lpszUsername		MAX_INET_USERNAME_LEN
// lpszPassword		MAX_INET_PASSWORD_LEN
SAFEVCRT_IMP LONG ParseURLEx(LPCSTR lpszSiteOrURL,LPSTR lpszProtocol,LPSTR lpszHost,USHORT& usPort,LPSTR lpszFolder,LPSTR lpszObject,
			LPSTR lpszObjectName,LPSTR lpszParagraph,BOOL& bAuthorization,LPSTR lpszUsername,LPSTR lpszPassword);
SAFEVCRT_IMP LONG ParseProtocol(LPCSTR lpszSiteOrURL,LPSTR lpszProtocol);
SAFEVCRT_IMP BOOL IsUrl(LPCSTR lpszSiteOrURL);






//========================================================================================================================
// ��ַת���Ͳ�������

// ��ַת������
SAFEVCRT_IMP IPV4 SockAddrToIPv4(SOCKADDR_IN* psa);													// SOCKADDRת��IPv4��ַ
SAFEVCRT_IMP WORD SockAddrToPortIPv4(SOCKADDR_IN* psa);												// SOCKADDRת��IPv4�˿�
SAFEVCRT_IMP IPV4 BytesToIPv4(LPBYTE pBytes);														// BYTESת��IPv4��ַ
SAFEVCRT_IMP IPV4 StringToIPv4(LPCSTR lpszIpv4);													// �ַ���תIPv4��ַ
SAFEVCRT_IMP VOID StringToSubNetIPv4(LPCSTR lpszIpv4,IPV4& Ip,IPV4& Mask);							// �ַ���תIPv4(����)��ַ
SAFEVCRT_IMP VOID IPv4ToSockAddr(const IPV4& Ip,USHORT nPort,SOCKADDR_IN* psa,DWORD& cbsa);			// IPv4��ַת��SOCKADDR
SAFEVCRT_IMP VOID IPv4ToBytes(const IPV4& Ip,LPBYTE pBytes);										// IPv4��ַת��BYTES
SAFEVCRT_IMP LPCSTR IPv4ToString(const IPV4& Ip,LPSTR pszTemp,LONG nSize);							// IPv4��ַת�ַ���
SAFEVCRT_IMP LPCSTR IPv4ToCommaString(const IPV4& Ip,LPSTR pszTemp,LONG nSize);						// IPv4��ַת�ַ���(���ŷָ�)
SAFEVCRT_IMP LPCSTR SubNetIPv4ToString(const IPV4& Ip,const IPV4& Mask,LPSTR pszTemp,LONG nSize);	// IPv4(����)��ַת�ַ���
SAFEVCRT_IMP IPV4 MaskIPv4(const IPV4& Ip,const IPV4& IpMask);										// IPv4������
SAFEVCRT_IMP IPV4 AddIPv4(const IPV4& Ip1,const IPV4& Ip2,LONG& nSignFlag);							// IPv4���
SAFEVCRT_IMP IPV4 SubIPv4(const IPV4& Ip1,const IPV4& Ip2,LONG& nSignFlag);							// IPv4���
SAFEVCRT_IMP IPV4 SpanToIPv4(const IPV4& Ip,const LONG nSpan);										// IPv4����
SAFEVCRT_IMP LONG SpanBetweenIPv4(const IPV4& Ip1,const IPV4& Ip2);									// IPv4��Ͳ�
SAFEVCRT_IMP int CompareIPv4(const IPV4& Ip1,const IPV4& Ip2);										// IPv4�Ƚ�
SAFEVCRT_IMP BOOL IsLoopbackIPv4(const IPV4& Ip);													// IPv4�ػ���ַ�ж�
SAFEVCRT_IMP BOOL IsBoardcastIPv4(const IPV4& Ip);													// IPv4�㲥��ַ�ж�
SAFEVCRT_IMP BOOL IsEthernetIPv4(const IPV4& Ip);													// IPv4��������ַ�ж�
SAFEVCRT_IMP BOOL IsInternetIPv4(const IPV4& Ip);													// IPv4������ַ�ж�
SAFEVCRT_IMP CLIB_INLINE IPV4 operator+(const IPV4& Ip,const LONG nSpan) { return SpanToIPv4(Ip,nSpan); }
SAFEVCRT_IMP CLIB_INLINE LONG operator-(const IPV4& Ip1,const IPV4& Ip2) { return SpanBetweenIPv4(Ip1,Ip2); }
SAFEVCRT_IMP CLIB_INLINE const bool operator<(const IPV4& Ip1,const IPV4& Ip2) { return CompareIPv4(Ip1,Ip2)<0; }
SAFEVCRT_IMP CLIB_INLINE const bool operator>(const IPV4& Ip1,const IPV4& Ip2) { return CompareIPv4(Ip1,Ip2)>0; }
SAFEVCRT_IMP CLIB_INLINE bool operator==(const IPV4& Ip1,const IPV4& Ip2) { return CompareIPv4(Ip1,Ip2)==0; }
SAFEVCRT_IMP CLIB_INLINE bool operator!=(const IPV4& Ip1,const IPV4& Ip2) { return CompareIPv4(Ip1,Ip2)!=0; }


SAFEVCRT_IMP IPV6 SockAddrToIPv6(SOCKADDR_IN6* psa);												// SOCKADDRת��IPv6��ַ
SAFEVCRT_IMP WORD SockAddrToPortIPv6(SOCKADDR_IN6* psa);											// SOCKADDRת��IPv6�˿�
SAFEVCRT_IMP IPV6 BytesToIPv6(LPBYTE pBytes);														// BYTESת��IPv6��ַ
SAFEVCRT_IMP IPV6 StringToIPv6(LPCSTR lpszIpv6);													// �ַ���תIPv6��ַ
SAFEVCRT_IMP VOID StringToSubNetIPv6(LPCSTR lpszIpv6,IPV6& Ip,IPV6& Mask);							// �ַ���תIPv6(����)��ַ
SAFEVCRT_IMP VOID IPv6ToSockAddr(const IPV6& Ip,USHORT nPort,SOCKADDR_IN6* psa,DWORD& cbsa);		// IPv6��ַת��SOCKADDR
SAFEVCRT_IMP VOID IPv6ToBytes(const IPV6& Ip,LPBYTE pBytes);										// IPv6��ַת��BYTES
SAFEVCRT_IMP LPCSTR IPv6ToString(const IPV6& Ip,LPSTR pszTemp,LONG nSize);							// IPv6��ַת�ַ���
SAFEVCRT_IMP LPCSTR IPv6ToCommaString(const IPV6& Ip,LPSTR pszTemp,LONG nSize);						// IPv6��ַת�ַ���(���ŷָ�)
SAFEVCRT_IMP LPCSTR SubNetIPv6ToString(const IPV6& Ip,const IPV6& Mask,LPSTR pszTemp,LONG nSize);	// IPv6(����)��ַת�ַ���
SAFEVCRT_IMP IPV6 MaskIPv6(const IPV6& Ip,const IPV6& IpMask);										// IPv6������
SAFEVCRT_IMP IPV6 AddIPv6(const IPV6& Ip1,const IPV6& Ip2,LONG& nSignFlag);							// IPv6���
SAFEVCRT_IMP IPV6 SubIPv6(const IPV6& Ip1,const IPV6& Ip2,LONG& nSignFlag);							// IPv6���
SAFEVCRT_IMP IPV6 SpanToIPv6(const IPV6& Ip,const LONG nSpan);										// IPv6����
SAFEVCRT_IMP LONG SpanBetweenIPv6(const IPV6& Ip1,const IPV6& Ip2);									// IPv6��Ͳ�
SAFEVCRT_IMP int CompareIPv6(const IPV6& Ip1,const IPV6& Ip2);										// IPv6�Ƚ�
SAFEVCRT_IMP BOOL IsLoopbackIPv6(const IPV6& Ip);													// IPv6�ػ���ַ�ж�
SAFEVCRT_IMP BOOL IsBoardcastIPv6(const IPV6& Ip);													// IPv6�㲥��ַ�ж�
SAFEVCRT_IMP BOOL IsEthernetIPv6(const IPV6& Ip);													// IPv6��������ַ�ж�
SAFEVCRT_IMP BOOL IsInternetIPv6(const IPV6& Ip);													// IPv6������ַ�ж�
SAFEVCRT_IMP CLIB_INLINE IPV6 operator+(const IPV6& Ip,const LONG nSpan) { return SpanToIPv6(Ip,nSpan); }
SAFEVCRT_IMP CLIB_INLINE LONG operator-(const IPV6& Ip1,const IPV6& Ip2) { return SpanBetweenIPv6(Ip1,Ip2); }
SAFEVCRT_IMP CLIB_INLINE const bool operator<(const IPV6& Ip1,const IPV6& Ip2) { return CompareIPv6(Ip1,Ip2)<0; }
SAFEVCRT_IMP CLIB_INLINE const bool operator>(const IPV6& Ip1,const IPV6& Ip2) { return CompareIPv6(Ip1,Ip2)>0; }
SAFEVCRT_IMP CLIB_INLINE bool operator==(const IPV6& Ip1,const IPV6& Ip2) { return CompareIPv6(Ip1,Ip2)==0; }
SAFEVCRT_IMP CLIB_INLINE bool operator!=(const IPV6& Ip1,const IPV6& Ip2) { return CompareIPv6(Ip1,Ip2)!=0; }

SAFEVCRT_IMP MAC StringToMac(LPCSTR lpszMac);														// �ַ���תMAC��ַ
SAFEVCRT_IMP LPCSTR MacToString(MAC Mac,LPSTR pszTemp,LONG nSize);									// MAC��ַת�ַ���
SAFEVCRT_IMP inline bool operator==(const MAC& Mac1,const MAC& Mac2) { return memcmp(&Mac1,&Mac2,size_of(Mac1))==0; }
SAFEVCRT_IMP inline bool operator!=(const MAC& Mac1,const MAC& Mac2) { return memcmp(&Mac1,&Mac2,size_of(Mac1))!=0; }


// ��ַ�ж��͵�ַ֮���໥ת��
SAFEVCRT_IMP BOOL IsIPv4(const IPV6& Ipv6);															// IPv4��ַ�ж�
SAFEVCRT_IMP IPV6 IPConvert4To6(const IPV4& Ipv4);													// IP��ַת��(IPV4->IPV6)
SAFEVCRT_IMP IPV4 IPConvert6To4(const IPV6& Ipv6);													// IP��ַת��(IPV6->IPV4)
SAFEVCRT_IMP MAC IPConvertFakeMac(const IPV4& Ipv4,BYTE cKind);										// IP��ַת��(IPV4->MAC)
SAFEVCRT_IMP MAC IPConvertRandMac();																// IP��ַת��(IPV4->MAC)

// ��ַ���͵�ת��HASH����
SAFEVCRT_IMP CLIB_INLINE BOOL CLIBAPI CollCompareElements(const UNALIGNED IPV4* pElement1,const UNALIGNED IPV4* pElement2) { return *pElement1==*pElement2; }
SAFEVCRT_IMP CLIB_INLINE UINT CLIBAPI GenHashKey(IPV4 Ip,UINT nKeyOffset) { UNREFERENCED_PARAMETER(nKeyOffset); return Ip.m_acIpData[size_of(Ip.m_acIpData)-2]*256+Ip.m_acIpData[size_of(Ip.m_acIpData)-1]; }
SAFEVCRT_IMP CLIB_INLINE BOOL CLIBAPI CollCompareElements(const UNALIGNED IPV6* pElement1,const UNALIGNED IPV6* pElement2) { return *pElement1==*pElement2; }
SAFEVCRT_IMP CLIB_INLINE UINT CLIBAPI GenHashKey(IPV6 Ip,UINT nKeyOffset) { UNREFERENCED_PARAMETER(nKeyOffset); return Ip.m_acIpData[size_of(Ip.m_acIpData)-2]*256+Ip.m_acIpData[size_of(Ip.m_acIpData)-1]; }
SAFEVCRT_IMP CLIB_INLINE BOOL CLIBAPI CollCompareElements(const UNALIGNED MAC* pElement1,const UNALIGNED MAC* pElement2) { return *pElement1==*pElement2; }
SAFEVCRT_IMP CLIB_INLINE UINT CLIBAPI GenHashKey(MAC Mac,UINT nKeyOffset) { UNREFERENCED_PARAMETER(nKeyOffset); return (UINT)(Mac.m_acMacData[1]*256+Mac.m_acMacData[0]); }



#ifndef NO_EXTEND_CRT

	// ����ģʽ
	#if defined(SAFEVCRT_FULLMODE)

		// MD5����
		#define LITEMD5_IMP SAFEVCRT_IMP						// MD5����
		#ifndef SAFEVCRT_LIB
			#include "litemd5/md5.h"							// MD5����
		#endif

		// �������ݽṹ����
		#define COLLECTION_IMP SAFEVCRT_IMP						// �������ݽṹ����
		#ifndef SAFEVCRT_LIB
			#include "collection/clibstr.h"						// �ַ�����
			#include "collection/coll.h"						// �������ݽṹ
			#include "collection/clibplex.h"					// ���ݿ���
			#include "collection/clibfixedalloc.h"				// �򵥵��ڴ�ع�����
			#include "collection/sampling.h"					// ������
			#include "collection/variantdata.h"					// ���Ͻṹ
		#endif

		// XML�ļ�����
		#define LITEXML_IMP SAFEVCRT_IMP						// XML�ļ�����
		#ifndef SAFEVCRT_LIB
			#include "litexml/xml.h"							// XML��
			#include "litexml/xmlprofile.h"						// XML���Ա���
		#endif

		// �������ݽṹ����(ģ����)
		#define COLLECTION_IMP SAFEVCRT_IMP						// �������ݽṹ����
		#ifndef SAFEVCRT_LIB
			#include "collection/clibtempl.h"					// �������ݽṹ(ģ����)
		#endif

		// ���ܴ���
		#define SECURITY_IMP SAFEVCRT_IMP						// ���ܴ���
		#ifndef SAFEVCRT_LIB
			#include "security/blowfish.h"						// Blowfish�����㷨
			#include "security/des3.h"							// DES3�����㷨
			#include "security/des3ex.h"						// DES3�����㷨(PKCS��׼)
			#include "security/talgorefer.h"					// �㷨����
			#include "security/talgobase.h"						// �㷨����
			#include "security/thashbase.h"						// HASH�㷨����
			#include "security/thashrender.h"					// HASH�㷨������
			#include "security/tsessionbase.h"					// SESSIN�㷨����
			#include "security/tsessionrender.h"				// SESSIN�㷨������
			#include "security/tpki.h"							// PKI�㷨��
			#include "security/rsakeyqueue.h"					// RSA����
		#endif

		// �ļ�IO����
		#define LITEFILEIO_IMP SAFEVCRT_IMP						// �ļ�IO����
		#ifndef SAFEVCRT_LIB
			#include "litefileio/cachedfile.h"					// ���л��������ļ�����
			#include "litefileio/anyfiles.h"					// ��ƽ̨�ļ���
			#include "litefileio/imagefile.h"					// �����ļ�������(֧��IMG�ļ�)
			#include "litefileio/miniarchive.h"					// ���л�֧��
			#include "litefileio/vfs.h"							// �����ļ�ϵͳ֧��
		#endif

		// INI����
		#define LITEINI_IMP SAFEVCRT_IMP						// INI�ļ�����
		#ifndef SAFEVCRT_LIB
			#include "liteini/iniconf.h"						// INI�ļ�����
		#endif

		// DBF����
		#define LITEDBF_IMP SAFEVCRT_IMP						// DBF�ļ�����
		#ifndef SAFEVCRT_LIB
			#include "litedbf/dbf.h"							// DBF�ļ�����
		#endif

		// GDI����
		#define LITEGDI_IMP SAFEVCRT_IMP						// GDI����
		#ifndef SAFEVCRT_LIB
			#include "litegdi/gdiplus.h"						// GDI����
		#endif

		// ѹ������
		#define COMPRESS_IMP SAFEVCRT_IMP						// ѹ������
		#ifndef SAFEVCRT_LIB
			#if defined(PLATFORM_WINDOWS)
				#include "compress/zlib.h"						// ѹ������
			#endif
			#include "compress/zmore.h"							// ѹ������
			#include "compress/zfile.h"							// ѹ������
			#include "compress/unzipfile.h"						// ѹ������
		#endif

		// �����Դ���
		#define MLANG_IMP SAFEVCRT_IMP							// �����Դ���
		#ifndef SAFEVCRT_LIB
			#include "mlang/mlang.h"							// �����Դ���
		#endif

		// �����
		#define CLIBMISC_IMP SAFEVCRT_IMP						// �����
		#ifndef SAFEVCRT_LIB
			#include "clibmisc/mt.h"							// �������
			#include "clibmisc/misc.h"							// �����
			#include "clibmisc/memory.h"						// �ڴ洦��
			#include "clibmisc/staticinit.h"					// ��̬��ʼ��
			#include "clibmisc/escapencode.h"					// ESCA����
			#include "clibmisc/uuencode.h"						// BASE64����
			#include "clibmisc/urlencode.h"						// URL����
			#include "clibmisc/perfcheck.h"						// ���ܼ��ģ��
			#include "clibmisc/wrscore.h"						// ����ģ��
			#include "clibmisc/rsmanager.h"						// ��Դ����
			#include "clibmisc/memtable.h"						// �ڴ��
			#include "clibmisc/safetyvalve.h"					// ��ȫ��
			#include "clibmisc/niostream.h"						// ������
		#endif

	// ��Сģʽ
	#else
		// MD5����
		#define LITEMD5_IMP SAFEVCRT_IMP						// MD5����
		#ifndef SAFEVCRT_LIB
			#include "litemd5/md5.h"							// MD5����
		#endif

		// �������ݽṹ����
		#define COLLECTION_IMP SAFEVCRT_IMP						// �������ݽṹ����
		#ifndef SAFEVCRT_LIB
			#include "collection/clibstr.h"						// �ַ�����
			#include "collection/coll.h"						// �������ݽṹ
			#include "collection/clibplex.h"					// ���ݿ���
			#include "collection/clibfixedalloc.h"				// �򵥵��ڴ�ع�����
		#endif

		// XML�ļ�����
		#define LITEXML_IMP SAFEVCRT_IMP						// XML�ļ�����
		#ifndef SAFEVCRT_LIB
			#include "litexml/xml.h"							// XML��
			#include "litexml/xmlprofile.h"						// XML���Ա���
		#endif

		// �������ݽṹ����(ģ����)
		#define COLLECTION_IMP SAFEVCRT_IMP						// �������ݽṹ����
		#ifndef SAFEVCRT_LIB
			#include "collection/clibtempl.h"					// �������ݽṹ(ģ����)
		#endif

		#define SECURITY_IMP SAFEVCRT_IMP						// ���ܴ���
		#ifndef SAFEVCRT_LIB
			#include "security/blowfish.h"						// Blowfish�����㷨
			#include "security/des3.h"							// Blowfish�����㷨
			#include "security/tpki.h"							// PKI�㷨��
		#endif
		
		// GDI����
		#define LITEGDI_IMP SAFEVCRT_IMP						// GDI����
		#ifndef SAFEVCRT_LIB
			#include "litegdi/gdiplus.h"						// GDI����
		#endif

		// �����Դ���
		#define MLANG_IMP SAFEVCRT_IMP							// �����Դ���
		#ifndef SAFEVCRT_LIB
			#include "mlang/mlang.h"							// �����Դ���
		#endif

		// �����
		#define CLIBMISC_IMP SAFEVCRT_IMP						// �����
		#ifndef SAFEVCRT_LIB
			#include "clibmisc/misc.h"							// �����
		#endif
	#endif // defined(SAFEVCRT_FULLMODE)
#endif


#ifndef SAFEVCRT_NOAUTOLINK
	#if defined(PLATFORM_WINDOWS) && !defined(SAFEVCRT_LIB)
		#pragma once
		#pragma comment(lib,"ws2_32.lib")

		#if defined(_ARCH_AMD64_)
			// ��̬ģʽ(��֧������ģʽ)
			#if defined(SAFEVCRT_DYNAMIC)
				#ifdef _DEBUG
					#pragma message("Automatically linking with SafeVCRTx64d.dll....")
					#pragma comment(lib,"SafeVcrtx64d.lib")
				#else
					#pragma message("Automatically linking with SafeVCRTx64.dll....")
					#pragma comment(lib,"SafeVcrtx64.lib")
				#endif
			// ����ģʽ(��̬)
			#elif defined(SAFEVCRT_FULLMODE)
				#ifdef _DEBUG
					#pragma message("Automatically linking with SafeVCRTSx64d.lib....")
					#pragma comment(lib,"SafeVcrtSx64d.lib")
				#else
					#pragma message("Automatically linking with SafeVCRTSx64.lib....")
					#pragma comment(lib,"SafeVcrtSx64.lib")
				#endif
			// ����ģʽ(��̬)
			#elif defined(SAFEVCRT_MINIMODE)
				#ifdef _DEBUG
					#pragma message("Automatically linking with SafeVCRTMx64d.lib....")
					#pragma comment(lib,"SafeVcrtMx64d.lib")
				#else
					#pragma message("Automatically linking with SafeVCRTMx64.lib....")
					#pragma comment(lib,"SafeVcrtMx64.lib")
				#endif
			#else
				#error SafeVCRT.h mode unknown
			#endif
		#else
			// ��̬ģʽ(��֧������ģʽ)
			#if defined(SAFEVCRT_DYNAMIC)
				#ifdef _DEBUG
					#pragma message("Automatically linking with SafeVCRTd.dll....")
					#pragma comment(lib,"SafeVcrtd.lib")
				#else
					#pragma message("Automatically linking with SafeVCRT.dll....")
					#pragma comment(lib,"SafeVcrt.lib")
				#endif
			// ����ģʽ(��̬)
			#elif defined(SAFEVCRT_FULLMODE)
				#ifdef _DEBUG
					#pragma message("Automatically linking with SafeVCRTSd.lib....")
					#pragma comment(lib,"SafeVcrtSd.lib")
				#else
					#pragma message("Automatically linking with SafeVCRTS.lib....")
					#pragma comment(lib,"SafeVcrtS.lib")
				#endif
			// ����ģʽ(��̬)
			#elif defined(SAFEVCRT_MINIMODE)
				#ifdef _DEBUG
					#pragma message("Automatically linking with SafeVCRTMd.lib....")
					#pragma comment(lib,"SafeVcrtMd.lib")
				#else
					#pragma message("Automatically linking with SafeVCRTM.lib....")
					#pragma comment(lib,"SafeVcrtM.lib")
				#endif
			#else
				#error SafeVCRT.h mode unknown
			#endif
		#endif
	#endif
#endif

#endif
#endif