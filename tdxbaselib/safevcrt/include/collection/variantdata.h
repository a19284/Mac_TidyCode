//////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2006 - All Rights Reserved
//  模块名称: VariantData
//  创建日期: 2006/08/05
//  代码编写: zhanglijun
//  功能说明: 数据聚合类
//            数据聚合类主要用于组织多组变长的结构,构造统一的缓冲区,用于增强协议兼容性/扩充性以及
//            在合理降低网络传输包大小的情况下,增强接口的灵活程度,便于多组变长数据的传送
//  引用示例: 
//  修改记录:
//////////////////////////////////////////////////////////////////////////
#ifndef __VARIANT_DATA__H__
#define __VARIANT_DATA__H__
#if _MSC_VER > 1000
#pragma once
#endif

// 使用1字节对齐
#pragma pack(1)

// 添加字段返回的结构(便于直接访问)
typedef struct tagVARIANTDINFO
{	LPDWORD	m_lpcbUsed;		// 使用中的缓冲区指针
	LPBYTE	m_lpItem;		// 当前数据项指针
	DWORD	m_cbItem;		// 当前数据项大小
	DWORD	m_dwMaxSize;	// 数据项最大大小
} VARIANTDINFO,*LPVARIANTDINFO;

class COLLECTION_IMP CVariantData
{
public:
	CVariantData();
	virtual~CVariantData();

	// 创建聚合数据区(缓冲区最小9字节)
	// 参数:	lpBuffer	外部缓冲区
	//			cbBuffer	外部缓冲区大小
	//			cbUsed		已使用的字节数,为0表明重新创建
	// 返回:	(BOOL)		是否创建成功
	BOOL Create(LPBYTE lpBuffer,DWORD cbBuffer,DWORD cbUsed);

	// 得到使用中的数据区大小
	DWORD GetUsedLength();

	// 添加数据项
	// 参数:	cKey		数据项
	//			lpData		数据缓冲区
	//			cbData		数据大小
	// 返回:	(BOOL)		是否添加成功
	BOOL SetData(BYTE cKey,const LPBYTE lpData,DWORD cbData,LPVARIANTDINFO lpVariantDInfo=NULL);

	// 添加字符串数据项
	// 参数:	cKey		数据项
	//			nMaxLength	最大允许长度
	//			lpString	数据缓冲区
	//			nLength		文件长度,如果<0则自动判断
	// 返回:	(BOOL)		是否添加成功
	BOOL SetString(BYTE cKey,LONG nMaxLength,LPCSTR lpString,LONG nLength);

	// 删除数据项
	// 参数:	cKey		数据项
	// 返回:	(BOOL)		是否删除成功
	BOOL DelData(BYTE cKey);

	// 判断数据项是否存在
	// 参数:	cKey		数据项
	// 返回:	(BOOL)		是否数据存在
	BOOL IsDataExist(BYTE cKey);
	
	// 查找数据项开始地址
	// 参数:	cKey		数据项
	// 返回:	(LPBYTE)	数据项开始位置,如果不存在,则返回NULL
	LPBYTE GetDataAddr(BYTE cKey);

	// 得到数据项长度
	// 参数:	cKey		数据项
	// 返回:	(DWORD)		数据长度,(DWORD)-1表明数据不存在
	DWORD GetDataLength(BYTE cKey);

	// 得到数据项长度(数据缓冲区会被清零)
	// 参数:	cKey		数据项
	//			lpBuffer	数据缓冲区
	//			cbBuffer	数据缓冲区大小
	// 返回:	(DWORD)		数据长度,(DWORD)-1表明数据不存在或者数据超过长度
	DWORD GetData(BYTE cKey,LPBYTE lpBuffer,DWORD cbBuffer);

protected:
	// 索引区格式
	typedef struct tagDATIDX
	{	BYTE  m_cKey;
		DWORD m_cbData;
	} DATIDX,*LPDATIDX;
	// 得到每个索引区块容纳的数据项个数
	inline BYTE CalcIdxNumPerBlk()
	{	return (BYTE)4;
	}
	// 得到每个索引区块的大小
	inline DWORD CalcIdxBufSizePerBlk()
	{	return CalcIdxNumPerBlk()*size_of(DATIDX);
	}
	// 计算键个数为cKeyNum时候键索引缓冲区包含的索引区的数据项个数
	// 注:索引区不允许为空
	inline BYTE CalcMaxIdxNum(BYTE cKeyNum)
	{	return (BYTE)max_of((cKeyNum+CalcIdxNumPerBlk()-1)/CalcIdxNumPerBlk()*CalcIdxNumPerBlk(),CalcIdxNumPerBlk());
	}
	// 计算键个数为cKeyNum时候键索引缓冲区包含的索引区的大小
	inline DWORD CalcIdxBufSize(BYTE cKeyNum)
	{	return CalcMaxIdxNum(cKeyNum)*size_of(DATIDX);
	}
protected:
	DWORD FindKeyOffset(BYTE cKey,LPBYTE lpIdx);
protected:
	BOOL		m_bCreated;		// 是否被创建
	LPBYTE		m_lpBuffer;		// 缓冲区
	DWORD		m_cbBuffer;		// 缓冲区大小
	DWORD		m_cbUsed;		// 使用中的缓冲区
	LPBYTE		m_lpcDatsNum;	// 数据项
	LPDATIDX	m_lpDatIdxs;	// 数据项指针
	LPBYTE		m_lpDatas;		// 数据缓冲区指针
};


class COLLECTION_IMP CSequence
{
public:
	CSequence(LPBYTE lpBuffer,DWORD cbBuffer,DWORD cbUsed=0);
	CSequence();
	virtual~CSequence();

	VOID SetBuffer(LPBYTE lpBuffer,DWORD cbBuffer,DWORD cbUsed=0);
	LPBYTE GetBuffer() { return m_lpBuffer; }
	DWORD GetBufferSize() const { return m_cbBuffer; }
	DWORD GetFreeSize() const { return m_cbBuffer-m_cbUsed; }
	DWORD GetUsed() const { return m_cbUsed; }
	VOID SeekToBegin() { m_dwOffset=0; }
	VOID SeekToEnd() { m_dwOffset=m_cbUsed; }
	VOID SeekTo(DWORD dwOffset) { m_dwOffset=dwOffset; }
	VOID Truncate() { m_cbUsed=m_dwOffset=0; }
	DWORD Tell() const { return m_dwOffset; }
	LPBYTE GetRest() const { return m_lpBuffer+m_dwOffset; }
	DWORD GetRestSize() const { return max_of(m_dwOffset,GetUsed())-m_dwOffset; }
	BOOL IsEmpty() const { return m_cbUsed==0; }

	// 基本类型数据写入和读取
	#define DECLARE_WRITE_STREAM_X(FUNC,TYPE)					\
		BOOL FUNC(TYPE xValue)									\
			{	if(m_dwOffset+size_of(TYPE)>=m_cbBuffer)			\
				{	m_bOverflow=TRUE;							\
					return FALSE;								\
				}												\
				*((TYPE*)(m_lpBuffer+m_dwOffset))=xValue;		\
				m_dwOffset+=size_of(TYPE);						\
				if(m_dwOffset>m_cbUsed) m_cbUsed=m_dwOffset;	\
				return TRUE;									\
			}
	DECLARE_WRITE_STREAM_X(WriteChar,CHAR);
	DECLARE_WRITE_STREAM_X(WriteByte,BYTE);
	DECLARE_WRITE_STREAM_X(WriteShort,SHORT);
	DECLARE_WRITE_STREAM_X(WriteUShort,USHORT);
	DECLARE_WRITE_STREAM_X(WriteLong,LONG);
	DECLARE_WRITE_STREAM_X(WriteULong,ULONG);
	DECLARE_WRITE_STREAM_X(WriteLongLong,INT64);
	DECLARE_WRITE_STREAM_X(WriteULongLong,UINT64);
	DECLARE_WRITE_STREAM_X(WriteInt,INT);
	DECLARE_WRITE_STREAM_X(WriteUint,UINT);
	DECLARE_WRITE_STREAM_X(WriteWord,WORD);
	DECLARE_WRITE_STREAM_X(WriteDWord,DWORD);
	DECLARE_WRITE_STREAM_X(WriteBool,BOOL);
	DECLARE_WRITE_STREAM_X(WriteFloat,FLOAT);
	DECLARE_WRITE_STREAM_X(WriteDouble,double);
	#undef DECLARE_WRITE_STREAM_X

	#define DECLARE_READ_STREAM_X(FUNC,TYPE)						\
		BOOL FUNC(TYPE& xValue)										\
			{	if(m_dwOffset+size_of(TYPE)>m_cbUsed) return FALSE;	\
				xValue=*((TYPE*)(m_lpBuffer+m_dwOffset));			\
				m_dwOffset+=size_of(TYPE);							\
				return TRUE;										\
			}
	DECLARE_READ_STREAM_X(ReadChar,CHAR);
	DECLARE_READ_STREAM_X(ReadByte,BYTE);
	DECLARE_READ_STREAM_X(ReadShort,SHORT);
	DECLARE_READ_STREAM_X(ReadUShort,USHORT);
	DECLARE_READ_STREAM_X(ReadLong,LONG);
	DECLARE_READ_STREAM_X(ReadULong,ULONG);
	DECLARE_READ_STREAM_X(ReadLongLong,INT64);
	DECLARE_READ_STREAM_X(ReadULongLong,UINT64);
	DECLARE_READ_STREAM_X(ReadInt,INT);
	DECLARE_READ_STREAM_X(ReadUint,UINT);
	DECLARE_READ_STREAM_X(ReadWord,WORD);
	DECLARE_READ_STREAM_X(ReadDWord,DWORD);
	DECLARE_READ_STREAM_X(ReadBool,BOOL);
	DECLARE_READ_STREAM_X(ReadFloat,FLOAT);
	DECLARE_READ_STREAM_X(ReadDouble,double);
	#undef DECLARE_READ_STREAM_X

	// 空块写入
	BOOL WriteBlank(DWORD cbBlank);

	// 数据块写入和读取
	BOOL WriteBinary(LPCVOID lpBuffer,DWORD cbBuffer);
	BOOL ReadBinary(LPVOID lpBuffer,DWORD cbBuffer);

	// 字符串写入和读取
	BOOL WriteString(LPCSTR lpString);
	BOOL ReadString(LPSTR lpBuffer,DWORD cbBuffer);
	BOOL ReadString(LPCSTR& lpBuffer);

	// 特殊类型的读取何写入
	BOOL WriteIpv4(IPV4 Ip) { return WriteBinary(&Ip,size_of(Ip)); }
	BOOL WriteIpv6(IPV6 Ip) { return WriteBinary(&Ip,size_of(Ip)); }
	BOOL WriteMac(MAC Mac) { return WriteBinary(&Mac,size_of(Mac)); }
	BOOL ReadIpv4(IPV4& Ip) { return ReadBinary(&Ip,size_of(Ip)); }
	BOOL ReadIpv6(IPV6& Ip) { return ReadBinary(&Ip,size_of(Ip)); }
	BOOL ReadMac(MAC& Mac) { return ReadBinary(&Mac,size_of(Mac)); }

	// 检溢出
	BOOL IsOverflow() { return m_bOverflow; }

	// 节处理
	DWORD BeginSection();
	BOOL EndSection(DWORD dwSectionOffset);
	BOOL ReadSection(CSequence& Sequence);

	// 缓冲区处理
	BOOL WriteBuffer(LPCBYTE pBuffer,DWORD cbData);
	BOOL WriteLiteBuffer(LPCBYTE pBuffer,DWORD cbData);
	BOOL ReadBuffer(LPBYTE pBuffer,DWORD cbBuffer,DWORD& cbData);
	BOOL ReadBuffer(LPBYTE& pBuffer,DWORD& cbData);
	BOOL ReadLiteBuffer(LPBYTE pBuffer,DWORD cbBuffer,DWORD& cbData);
	BOOL ReadLiteBuffer(LPBYTE& pBuffer,DWORD& cbData);

protected:
	LPBYTE m_lpBuffer;
	DWORD m_cbBuffer;
	DWORD m_cbUsed;
	DWORD m_dwOffset;
	BOOL m_bOverflow;
};

#pragma pack()


#endif
