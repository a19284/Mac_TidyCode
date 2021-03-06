#ifndef __IMAGEFILE__H__
#define __IMAGEFILE__H__
#include "../safevcrt.h"
#include "miniarchive.h"

#define MAX_DOSPATH	256			// 最大DOS路径

// 镜像文件格式
enum enumIMGFMT
{	IMGFMT_IMG,					// .IMG格式
	IMGFMT_VFD,					// .VFD格式(暂不支持)
	IMGFMT_DDI,					// .DDI格式(暂不支持)
};


// 物理位置的定义
typedef struct tagCHS
{	BYTE	m_cHead;			// 磁头
	BYTE	m_cSector:6;		// 扇区
	WORD	m_wCylinder:10;		// 柱面
} _PACKED CHS;


//分区类型
enum enumDPTTYPE
{	DP_UNUSED      = 0X00,		// 未使用分区
    DP_FAT12       = 0X01,		// FAT12分区
    DP_XENIX       = 0X02,		// XENIX分区
    DP_FAT16       = 0X04,		// FAT16分区
    DP_EXTEND      = 0X05,		// 扩展分区
    DP_BIGDOS      = 0X06,
    DP_HPFS        = 0X07,
    DP_SPLIT       = 0X08,
    DP_FAT32       = 0X0B,
    DP_DM          = 0X50,
    DP_GB          = 0X56,
    DP_SPEED       = 0X61,
    DP_IX386       = 0X63,
    DP_NOVELL286   = 0X64,
    DP_NOVELL386   = 0X65,
    DP_PCIX        = 0X75,
    DP_CPM         = 0XDB,
    DP_BBT         = 0XFF,
};


// 定义分区表项的结构,只对硬盘镜像存在,软盘镜像不存在分区,因而不存在MBR
// 也不存在分区记录
// (Disk PartitionTable)
typedef struct tagDPT
{   BYTE	m_cBootFlag;			// 启动标志
    CHS     m_StartCHS;				// 分区开始的柱面、磁头、扇区
    BYTE    m_cSystemID;			// 分区类型
    CHS     m_EndCHS;				// 分区结束的柱面、磁头、扇区
    DWORD   m_dwRelativeSectors;	// 分区相对扇区数，指分区相对于记录该分区的分区表的扇区位置之差
    DWORD   m_dwTotalSectors;		// 分区总扇区数
} _PACKED DPT;


// 定义主引导扇区结构(MBR)
// 软盘不存在
typedef struct tagMBR
{	BYTE	m_aBootCode[0x1be];		// 启动代码
    DPT     m_DPT[4];               // 分区表
    WORD	m_wEndingFlag;			// 结束标识(55AA)
} _PACKED MBR;


// 定义DBR(DOS Boot Record)结构,该结构处于每个分区的第一个扇区
// 也就是软盘开始的分区,这个结构和分区类型相关
// 16位分区格式和12位分区格式相同
// DBR中包含重要的BPB(BIOS Paramenter Block-磁盘参数表)等重要数据
//磁盘介质类型
enum enumMEDIATYPE
{	DM_HARDDISK = 0xF8,				// 硬盘
	DM_FLOPPY   = 0xF0,				// 软盘
    DM_RAMDisk  = 0xFA				// RAM盘
};

typedef struct tagBPBFAT
{	WORD   m_wSectorBytes;			// 每扇区字节数
    BYTE   m_cSectorsPerCluster;	// 每簇扇区数
    WORD   m_wReservedSectors;		// 保留扇区数
    BYTE   m_cNbrFat;				// FAT的个数
    WORD   m_wRootEntry;			// 根目录项数
    WORD   m_wTotalSectors;			// 分区总扇区数(分区小于32M时)
    BYTE   m_cMedia;				// 分区介质标识
    WORD   m_wSectorsPerFAT;		// 每个FAT占的扇区数
    WORD   m_wSectorsPerTrack;		// 每道扇区数
    WORD   m_wHeads;				// 磁头数
    DWORD  m_dwHiddenSectors;       // 隐含扇区数
    DWORD  m_dwBigTotalSectors;     // 分区总扇区数(分区大于32M时)
    BYTE   m_cDrive;				// 驱动器号
    BYTE   m_cHead;					// 磁头号
    BYTE   m_cboot_rec_sig;			// 含义未知
    DWORD  m_dwVolSerialNo;			// 磁盘序列号
    CHAR   m_szLabel[11];           // 磁盘卷标号
    CHAR   m_szFileSys_ID[8];       // 文件系统标识号
} _PACKED BPBFAT;
typedef BPBFAT BPBFAT12,BPBFAT16;

#define FAT_ENDFLAG	0XAA55			// 正常的FAT结束字

typedef struct tagDBRFAT
{	BYTE	m_aJMPCode[3];			// 引导跳转代码
    CHAR	m_szSystemID[8];		// 厂商标志和版本号
	BPBFAT	m_BPB;					// BPB记录
	BYTE	m_aBPBEx[26];			// 扩展BPB
	BYTE	m_aBootCode[422];		// 启动代码
	WORD	m_wEndingFlag;			// 结束标识(55AA)
} _PACKED DBRFAT;
typedef DBRFAT DBRFAT12,DBRFAT16;


// DBR后紧接的就是系统的文件分配表,FAT1和FAT2,
// FAT各系统记录项的取值含义(16进制) 
// FAT12记录项的取值	FAT16记录项的取值	FAT32记录项的取值	对应簇的表现情况 
// 000					0000				00000000			未分配的簇 
// 002~FFF				0002~FFEF			00000002~FFFFFFEF	已分配的簇 
// FF0~FF6				FFF0~FFF6			FFFFFFF0~FFFFFFF6	系统保留 
// FF7					FFF7				FFFFFFF7			坏簇 
// FF8~FFF				FFF8~FFFF			FFFFFFF8~FFFFFFFF	文件结束簇
// 系统需要读取全部的FAT来判断磁盘空间的分配情况
// FAT12,FAT16,FAT32主要是这里存在区别,每条记录分别是12位,16位,32位
// 前两个簇的FAT表被系统保留,分别是保留3字节,4字节,8字节
// 保留的字节中的首字节为媒质类型
// FAT16 FAT表项的含义
enum enumFAT16TYPE
{	FAT_UNUSED			= 0,			// 尚未分配
	FAT12_RESERVED0		= 0x0FF0,		// 保留
    FAT12_RESERVED6		= 0x0FF6,		// 保留
    FAT12_BADCLUSTER    = 0x0FF7,		// 表示坏簇,即该簇中磁盘有损坏；
    FAT12_FILEEND1		= 0x0FF8,		// 文件结束簇
    FAT12_FILEEND8		= 0x0FFF,		// 文件结束簇
	FAT12_FILEEND		= 0x0FFF,		// 文件结束簇
	FAT16_RESERVED0		= 0xFFF0,		// 保留
    FAT16_RESERVED6		= 0xFFF6,		// 保留
    FAT16_BADCLUSTER    = 0xFFF7,		// 表示坏簇,即该簇中磁盘有损坏；
    FAT16_FILEEND1		= 0xFFF8,		// 文件结束簇
    FAT16_FILEEND8		= 0xFFFF,		// 文件结束簇
	FAT16_FILEEND		= 0xFFFF,		// 文件结束簇
	FAT32_RESERVED0		= 0xFFFFFFF0,	// 保留
    FAT32_RESERVED6		= 0xFFFFFFF6,	// 保留
    FAT32_BADCLUSTER    = 0xFFFFFFF7,	// 表示坏簇,即该簇中磁盘有损坏；
    FAT32_FILEEND1		= 0xFFFFFFF8,	// 文件结束簇
    FAT32_FILEEND8		= 0xFFFFFFFF,	// 文件结束簇
	FAT32_FILEEND		= 0xFFFFFFFF,	// 文件结束簇
};


// FAT表后接着的是根目录表,表大小由BPBFAT决定(注意FAT12和FAT16采用现同的BPB结构)
// 定义根目录表结构单元


// 目录项中的时间定义
typedef struct tagTIMEFAT
{	WORD	m_wSec:5;		// 秒
    WORD	m_wMin:6;		// 分
    WORD	m_wHour:5;		// 时
} _PACKED TIMEFAT;


// 目录项中的日期定义
typedef struct tagDATEFAT
{	WORD    m_wDay:5;		// 日
    WORD    m_wMon:4;		// 月
    WORD    m_wYear:7;		// 年
} _PACKED DATEFAT;


// 目录项首字节含义
enum enumFDTLEADCHAR
{	FDT_UNUSED           = 0x00,	// 本表项没有使用
    FDT_ISSUBDIR		 = 0x2e,	// 是一个子目录 .,..为父目录
    FDT_FILEISDELETED    = 0xe5,	// 文件已删除
};


// 目录表项的含义
typedef struct tagFDTFAT
{   BYTE		m_szFileName[8];		// 主文件名
    BYTE        m_szExtName[3];			// 扩展文件名
    BYTE        m_bReadOnly:1;			// 文件属性:只读位
    BYTE        m_bHidden:1;			// 文件属性:隐含
    BYTE        m_bSystem:1;			// 文件属性:系统
    BYTE        m_bVolume:1;			// 文件属性:卷标
	BYTE        m_bDirectory:1;			// 文件属性:目录
    BYTE        m_bArchives:1;			// 文件属性:档案
	BYTE        m_cReserived1:2;		// 保留
	BYTE        m_cReserived2;			// 保留
	BYTE		m_cCreateMilliSec;		// 文件创建时间的毫秒位(FAT32)
	TIMEFAT		m_tCreateTime;			// 文件创建时间(FAT32)
    DATEFAT		m_tCreateDate;			// 文件创建日期(FAT32)
	DATEFAT		m_tAccessDate;			// 文件最后访问日期(FAT32)
	WORD        m_wStartClusterHigh;	// 文件起始簇高位(FAT32)
    TIMEFAT		m_tModifyTime;			// 文件最后修改时间
    DATEFAT		m_tModifyDate;			// 文件最后修改日期
    WORD        m_wStartClusterLow;		// 文件起始簇低位
    DWORD       m_dwFileLength;			// 文件长度
} _PACKED FDTFAT;
typedef FDTFAT FDTFAT12,FDTFAT16;

// 打开的目录表项
typedef struct tagFDTOBJ
{	DWORD		m_dwFDTCluster;			// 目录簇(如果为0表明位于根目录表下)
	DWORD		m_dwFDTIndex;			// 文件|目录索引项,标记处于目录簇的位置
	FDTFAT		m_FDT;					// 文件|目录FDT项,用于指向数据
} _PACKED FDTOBJ;

// 文件句柄(不支持共享)
typedef struct tagIMGFDOBJ
{	FDTOBJ		m_FDTObj;
	DWORD		m_dwPtr;
	BOOL		m_bWritable;
} _PACKED IMGFDOBJ;
typedef IMGFDOBJ* HIMGFDOBJ;

// 镜像文件查找结果
typedef struct tagIMGFINDDATA
{	CHAR	m_szName[MAX_DOSPATH];			// 对象路径
	LONG	m_nLength;						// 对象长度
	BOOL	m_bDirectory;					// 对象是否为目录
	time_t	m_tLastModify;					// 对象最后修改时间(精确到秒)
} _PACKED IMGFINDDATA;


// 镜像文件访问
class LITEFILEIO_IMP CImageFS
{
public:
	CImageFS();
	virtual ~CImageFS();
	BOOL		__attachimage(IN CAnyFile* pAnyFile);
	int			__getfiletime(IN const char* path,OUT time_t* modifytime);
	int			__setfiletime(IN const char* path,IN time_t modifytime);
	int			__mkdir(IN const char* dirname);
	int			__rmdir(IN const char* dirname);
	int			__access(IN const char* path,IN int mode);
	int			__chmod(IN const char* filename,IN int pmode);
	int			__remove(IN const char* path);
	int			__stat(IN const char* path,OUT struct _stat* buffer);
	int			__unlink(IN const char* filename);
	int			__chsize(IN int handle,IN long size);
	long		__filelength(IN int handle);
	int			__fstat(IN int handle,OUT struct _stat* buffer);
	HIMGFDOBJ	__fopen(IN const char* filename,const char* mode);
	int			__chsize(IN HIMGFDOBJ hImgFDObj,IN long size);
	long		__filelength(IN HIMGFDOBJ hImgFDObj);
	int			__fstat(IN HIMGFDOBJ hImgFDObj,OUT struct _stat* buffer);
	INT_PTR		__fileno(IN HIMGFDOBJ hImgFDObj);
	void		__rewind(IN HIMGFDOBJ hImgFDObj);
	long		__ftell(IN HIMGFDOBJ hImgFDObj);
	int			__feof(IN HIMGFDOBJ hImgFDObj);
	int			__fseek(IN HIMGFDOBJ hImgFDObj,IN long offset,IN int origin);
	size_t		__fread(IN void* buffer,IN size_t size,IN size_t count,IN HIMGFDOBJ hImgFDObj);
	size_t		__fwrite(IN const void* buffer,IN size_t size,IN size_t count,IN HIMGFDOBJ hImgFDObj);
	int			__fprintf(IN HIMGFDOBJ hImgFDObj,IN char* lpszFmt,IN ...);
	int			__fclose(IN HIMGFDOBJ hImgFDObj);
	int			__fflush(IN HIMGFDOBJ hImgFDObj);
	int			__scandir(IN const char* dirname,IMGFINDDATA** lppImgFindData);
	int			__format();
protected:
	BOOL SearchFDT(IN LPCTSTR lpszPath,OUT FDTOBJ* pFDTObj);
	BOOL CreateFDT(IN LPCTSTR lpszPath,IN BOOL bDirectory,OUT FDTOBJ* pFDTObj);
	BOOL RemoveFDT(IN FDTOBJ* pFDTObj);
	BOOL UpdateFDT(IN FDTOBJ* pFDTObj);
	VOID DOSNameFromFDT(IN FDTFAT* pFDT,OUT char* lpszFileName);
	VOID DOSNameToFDT(IN FDTFAT* pFDT,OUT char* lpszFileName);
	VOID ToFATDateTime(IN time_t tTime,INOUT DATEFAT* pDateFAT,INOUT TIMEFAT* pTimeFAT);
	VOID FromFATDateTime(IN DATEFAT* pDateFAT,IN TIMEFAT* pTimeFAT,OUT time_t* ptTime);
	VOID GetNowFATDate(INOUT DATEFAT* pDateFAT);
	VOID GetNowFATTime(INOUT TIMEFAT* pTimeFAT);
	DWORD StartClusterFromFDT(IN FDTFAT* pFDT);
	DWORD StartClusterToFDT(IN FDTFAT* pFDT,IN DWORD dwStartCluster);
	DWORD ReadClusterLink(IN DWORD dwCluster,IN DWORD dwOffset,IN DWORD dwSize,IN LPBYTE lpBuffer);
	DWORD WriteClusterLink(IN DWORD dwCluster,IN DWORD dwOffset,IN DWORD dwSize,IN LPBYTE lpBuffer);
	DWORD TailOfClusterLink(IN DWORD dwCluster);
	DWORD ClusterOfClusterLink(IN DWORD dwCluster,IN DWORD dwNo);
	DWORD CountOfClusterLink(IN DWORD dwCluster);
	DWORD AllocClusterLink(DWORD dwCount);
	VOID FreeClusterLink(IN DWORD dwCluster);
	BOOL AdjuestClusterLink(IN DWORD dwOldCluster,IN DWORD dwNewCount,IN BOOL bAutoTruncate,OUT DWORD* lpdwNewCluster);
	BOOL IsValidCluster(IN DWORD dwCluster);
	BOOL IsBadCluster(IN DWORD dwCluster);
	BOOL IsTailCluster(IN DWORD dwCluster);
	BOOL IsEmptyCluster(IN DWORD dwCluster);
	BOOL GetNextCluster(IN DWORD dwCluster,OUT LPDWORD pdwNextCluster);
	BOOL SetNextCluster(IN DWORD dwCluster,IN DWORD dwNextCluster);
	BOOL SetNextClusterToEnd(IN DWORD dwCluster);
	VOID FlushFAT(IN DWORD dwCluster);
protected:
	CAnyFile*	m_pAnyFile;			// 磁盘文件
	DBRFAT		m_DBR;				// DBR记录
	DWORD		m_dwTotalSectors;	// 总扇区数
	DWORD		m_dwBytesPerCluster;// 每个簇的字节数
	DWORD		m_dwMinClusters;	// 最小簇序号
	DWORD		m_dwMaxClusters;	// 最大簇序号
	DWORD		m_dwTotalClusters;	// 总簇数
	CHAR		m_szLabel[12];      // 磁盘卷标号
    CHAR		m_szFileSysID[9];	// 文件系统标识号
	BYTE		m_cDPTType;			// 分区格式
	DWORD		m_cbFATSize;		// FAT表大小
	LPBYTE		m_lpFAT;			// FAT表
	FDTFAT*		m_lpFDT;			// FDT(DIR)区内容(紧接着FAT表2)
	DWORD		m_dwFATOffset;		// FAT表偏移
	DWORD		m_dwFDTOffset;		// FDT表偏移
	DWORD		m_dwDATOffset;		// 数据区偏移
};



#endif
