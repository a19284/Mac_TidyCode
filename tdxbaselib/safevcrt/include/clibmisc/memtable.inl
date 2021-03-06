#ifndef __MEM_TABLE__INL__
#define __MEM_TABLE__INL__

#ifndef __MEM_TABLE__H__
	#error memtable.h must be included
#endif


// 记录插入回调(构造索引)
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE BOOL TIndex<KEY,RECORD,SUBINDEXNUM>::OnInsertRecord(LPVOID pRecordVoid)
{	RECORD* pRecord=(RECORD*)pRecordVoid;
	KEY aKeys[SUBINDEXNUM];
	memset(aKeys,0,size_of(aKeys));
	DWORD dwKeyNum=RecordToKeys(*pRecord,(KEY*)aKeys,ARRAYSIZE(aKeys));
	BOOL bRollback=FALSE;
	DWORD dwKey=0;
	for(dwKey=0; dwKey<dwKeyNum; dwKey++)
	{	if(!InsertIndex(aKeys[dwKey],pRecord))
		{	bRollback=TRUE;
			break;
		}
	}
	// 如果进行数据回退,则撤销已经建立的索引并返回错误
	if(bRollback)
	{	while(dwKey>0)
		{	dwKey--;
			DeleteIndex(aKeys[dwKey],pRecord);
		}
		return FALSE;
	}
	return TRUE;
}

// 记录删除回调(撤销索引)
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE VOID TIndex<KEY,RECORD,SUBINDEXNUM>::OnDeleteRecord(LPVOID pRecordVoid)
{	RECORD* pRecord=(RECORD*)pRecordVoid;
	KEY aKeys[SUBINDEXNUM];
	memset(aKeys,0,size_of(aKeys));
	DWORD dwKeyNum=RecordToKeys(*pRecord,(KEY*)aKeys,ARRAYSIZE(aKeys));
	for(DWORD dwKey=0; dwKey<dwKeyNum; dwKey++) DeleteIndex(aKeys[dwKey],pRecord);
}

// 记录查找子过程(通过列表)
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE VOID TIndex<KEY,RECORD,SUBINDEXNUM>::OnSelectUsingIndex(LONG nTop,LPVOID pKeyVoid,LPVOID plstResultsVoid)
{
	KEY* pKey=(KEY*)pKeyVoid;
	TList<RECORD*,RECORD*&>* plstResults=(TList<RECORD*,RECORD*&>*)plstResultsVoid;
	plstResults->RemoveAll();

	// 查找当前键对应的记录指针序列,如果不存在则返回
	LPVOID pChainVoid=NULL;
	if(!m_ChainMap.Lookup(*pKey,pChainVoid)) pChainVoid=NULL;
	if(pChainVoid==NULL) return;

	// 遍历和循环查找
	POSITION posChain=(POSITION)pChainVoid;
	while(posChain!=NULL)
	{	CHAIN* pChain=&m_ChainList.GetAt(posChain);
		posChain=pChain->m_posNext;
		if(pChain->m_pRecord==NULL) continue;
		plstResults->AddTail(pChain->m_pRecord);
		if(nTop>0&&plstResults->GetCount()>=nTop) break;
	}
}

// 记录查找子过程(快速模式)
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE DWORD TIndex<KEY,RECORD,SUBINDEXNUM>::OnSelectUsingIndexEx(DWORD dwMax,LONG nTop,LPVOID pKeyVoid,LPVOID* ppResults)
{
	KEY* pKey=(KEY*)pKeyVoid;
	
	// 查找当前键对应的记录指针序列,如果不存在则返回
	LPVOID pChainVoid=NULL;
	if(!m_ChainMap.Lookup(*pKey,pChainVoid)) pChainVoid=NULL;
	if(pChainVoid==NULL) return 0;

	// 遍历和循环查找
	DWORD dwSelected=0;
	POSITION posChain=(POSITION)pChainVoid;
	while(posChain!=NULL)
	{	CHAIN* pChain=&m_ChainList.GetAt(posChain);
		posChain=pChain->m_posNext;
		if(pChain->m_pRecord==NULL) continue;
		if(dwSelected>=dwMax) break;
		ppResults[dwSelected]=pChain->m_pRecord;
		dwSelected++;
		if(nTop>0&&dwSelected>=(DWORD)nTop) break;
		if(dwSelected>=dwMax) break;
	}
	return dwSelected;
}

// 对象删除
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE VOID TIndex<KEY,RECORD,SUBINDEXNUM>::DeleteRender()
{	delete this;
}

// 索引构建
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE BOOL TIndex<KEY,RECORD,SUBINDEXNUM>::InsertIndex(const KEY& Key,RECORD* pRecord)
{
	// 查找当前键对应的记录指针序列
	LPVOID pChainVoid=NULL;
	if(!m_ChainMap.Lookup(Key,pChainVoid)) pChainVoid=NULL;

	// 检查唯一键
	if(m_bUnique&&pChainVoid!=NULL) return FALSE;

	// 构造一个新的记录指针单元并插入到链表中
	CHAIN Chain;
	memset(&Chain,0,size_of(Chain));
	POSITION posChain=m_ChainList.AddTail(Chain);
	CHAIN* pChain=&m_ChainList.GetAt(posChain);
	pChain->m_pRecord=pRecord;
	pChain->m_posNext=(POSITION)pChainVoid;
	m_ChainMap.SetAt(Key,posChain);
	return TRUE;
}

// 索引删除
template<class KEY,class RECORD,int SUBINDEXNUM>
CLIB_INLINE VOID TIndex<KEY,RECORD,SUBINDEXNUM>::DeleteIndex(const KEY& Key,RECORD* pRecord)
{
	// 查找当前键对应的记录指针序列,如果不存在则返回
	LPVOID pChainVoid=NULL;
	if(!m_ChainMap.Lookup(Key,pChainVoid)) pChainVoid=NULL;
	if(pChainVoid==NULL) return;

	// 从索引序列中删除记录
	CHAIN* pChainPrev=NULL;
	POSITION posChain=(POSITION)pChainVoid;
	while(posChain!=NULL)
	{
		// 查找对应的记录
		CHAIN* pChain=&m_ChainList.GetAt(posChain);
		if(pChain->m_pRecord!=pRecord)
		{	pChainPrev=pChain;
			posChain=pChain->m_posNext;
			continue;
		}
		// 删除的是后面的记录
		if(pChainPrev!=NULL)
		{	pChainPrev->m_posNext=pChain->m_posNext;
			m_ChainList.RemoveAt(posChain);
		}
		// 删除的是头部记录
		else
		{	POSITION posNext=pChain->m_posNext;
			m_ChainList.RemoveAt(posChain);
			if(posNext==NULL)
					m_ChainMap.RemoveKey(Key);
			else	m_ChainMap.SetAt(Key,posNext);
		}
		return;
	}
}




























// 创建索引
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE VOID TMemTable<PKEY,RECORD,INDEXNUM>::BindIndex(LONG nIndex,IIndexRender* pIIndexRender)
{	CLIBVERIFY(nIndex>=0&&nIndex<INDEXNUM);
	CLIBVERIFY(m_apIndexRender[nIndex]==NULL);
	m_apIndexRender[nIndex]=pIIndexRender;
}

// 获取记录数目
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::GetRecordNum()
{	return m_RecordMap.GetCount();
}

// 列出数据
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE VOID TMemTable<PKEY,RECORD,INDEXNUM>::List(LONG nTop,VIEWER pfnView)
{	
	LONG nCount=0;
	POSITION posRecord=m_RecordList.GetHeadPosition();
	while(posRecord!=NULL&&(nTop<=0||nCount<nTop))
	{	RECORD* pRecord=&m_RecordList.GetNext(posRecord);
		if(pRecord->__Krnl.m_bDeleted||pRecord->__Krnl.m_pCopyFrom!=NULL) continue;
		if(pfnView!=NULL) (*pfnView)(pRecord);
		nCount++;
	}
}

// 插入记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::Insert(RECORD& Record)
{
	// 通过记录得到主键
	PKEY PKey;
	memset(&PKey,0,size_of(PKey));
	RecordToKeys(Record,&PKey,1);

	CAutoLock AutoLock(&m_CsTable);

	// 实际插入记录
	return InsertRecord(NULL,PKey,Record);
}






// 根据主键选取记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::Select(RECORD& Record,const PKEY& PKey)
{
	CAutoLock AutoLock(&m_CsTable);

	// 查找主键
	LPVOID pRecordVoid=NULL;
	if(!m_RecordMap.Lookup(PKey,pRecordVoid)) return FALSE;
	POSITION posRecord=(POSITION)pRecordVoid;
	RECORD* pRecord=&m_RecordList.GetAt(posRecord);
	if(pRecord->__Krnl.m_bDeleted) return FALSE;

	// 获取记录
	Record=(*pRecord);

	return TRUE;
}


// 根据数据查找
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectWhere(RECORDLIST* plstResult,LONG nTop,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(plstResult!=NULL);
	plstResult->RemoveAll();
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pfnWhere,argsWhere,lstPreliminary);
	va_end(argsWhere);

	// 返回数据
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		plstResult->AddTail(*pRecord);
	}
	return plstResult->GetCount();
}

// 根据索引查找
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectByIndex(RECORDLIST* plstResult,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(plstResult!=NULL);
	plstResult->RemoveAll();
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pIIndexRender,pKey,pfnWhere,argsWhere,lstPreliminary);
	va_end(argsWhere);

	// 返回数据
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		plstResult->AddTail(*pRecord);
	}
	return plstResult->GetCount();
}





// 按照主键查找并锁定
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE RECORD* TMemTable<PKEY,RECORD,INDEXNUM>::SelectForUpdate(const PKEY& PKey)
{
	CAutoLock AutoLock(&m_CsTable);

	// 查找主键
	LPVOID pRecordVoid=NULL;
	if(!m_RecordMap.Lookup(PKey,pRecordVoid)) return NULL;
	POSITION posRecord=(POSITION)pRecordVoid;
	RECORD* pRecord=&m_RecordList.GetAt(posRecord);

	// 生成记录副本
	return CreateCopyRecord(pRecord);
}

// 按照条件查找并锁定
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectForUpdateWhere(RECORDPTRLIST* plstForUpdate,LONG nTop,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(plstForUpdate!=NULL);
	plstForUpdate->RemoveAll();
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pfnWhere,argsWhere,lstPreliminary);
	va_end(argsWhere);

	// 生成副本数据
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		RECORD* pRecordCopy=CreateCopyRecord(pRecord);
		plstForUpdate->AddTail(pRecordCopy);
	}
	return plstForUpdate->GetCount();
}

// 按照索引查找并锁定
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectForUpdateByIndex(RECORDPTRLIST* plstForUpdate,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(plstForUpdate!=NULL);
	plstForUpdate->RemoveAll();
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pIIndexRender,pKey,pfnWhere,argsWhere,lstPreliminary);
	va_end(argsWhere);

	// 生成副本数据
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		RECORD* pRecordCopy=CreateCopyRecord(pRecord);
		plstForUpdate->AddTail(pRecordCopy);
	}
	return plstForUpdate->GetCount();
}

// 提交或撤销修改
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::CommitUpdate(RECORD* pRecord,BOOL bCancel)
{
	// 检查是否复制出来的记录
	CLIBVERIFY(pRecord->__Krnl.m_bDeleted==FALSE);
	CLIBVERIFY(pRecord->__Krnl.m_nRecordRef==-1);
	CLIBVERIFY(pRecord->__Krnl.m_pCopyFrom!=NULL);

	// 锁定表进行提交处理
	BOOL bCommitSuccess=TRUE;
	RECORD* pCopyFrom=(RECORD*)pRecord->__Krnl.m_pCopyFrom;
	CAutoLock AutoLock(&m_CsTable);
	if(!bCancel)
	{	bCommitSuccess=(pRecord->__Krnl.m_nRecordVer==pCopyFrom->__Krnl.m_nRecordVer);
		if(bCommitSuccess)
		{	pRecord->__Krnl=pCopyFrom->__Krnl;
			(*pCopyFrom)=(*pRecord);
			pCopyFrom->__Krnl.m_nRecordVer++;
		}
	}
	// 删除临时记录
	m_RecordList.RemoveByPtr(pRecord);
	pCopyFrom->__Krnl.m_nRecordRef--;
	if(pCopyFrom->__Krnl.m_nRecordRef<=0) m_RecordList.RemoveByPtr(pCopyFrom);
	return bCommitSuccess;
}

// 批量提交或撤销修改
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::BatchCommitUpdate(RECORDPTRLIST* plstForUpdate,BOOL bCancel)
{
	CLIBVERIFY(plstForUpdate!=NULL);
	CAutoLock AutoLock(&m_CsTable);
	BOOL bOneOrMoreFail=FALSE;
	while(!plstForUpdate->IsEmpty())
	{	RECORD* pRecord=plstForUpdate->RemoveHead();
		if(!CommitUpdate(pRecord,bCancel)) bOneOrMoreFail=TRUE;
	}
	return !bOneOrMoreFail;
}


// 更新记录(主键)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::Update(RECORD* pAffect,const PKEY& PKey,UPDATE_ROUTINE pfnUpdateRoutine,...)
{
	// 检查参数
	if(pfnUpdateRoutine==NULL) return FALSE;

	CAutoLock AutoLock(&m_CsTable);

	// 查找主键
	LPVOID pRecordVoid=NULL;
	if(!m_RecordMap.Lookup(PKey,pRecordVoid)) return FALSE;
	POSITION posRecord=(POSITION)pRecordVoid;
	RECORD* pRecord=&m_RecordList.GetAt(posRecord);
	if(pRecord->__Krnl.m_bDeleted) return FALSE;

	va_list args;
	va_start(args,pfnUpdateRoutine);
	(*pfnUpdateRoutine)(pRecord,args);
	pRecord->__Krnl.m_nRecordVer++;
	if(pAffect!=NULL) (*pAffect)=(*pRecord);
	va_end(args);
	return TRUE;
}

// 更新记录(逐行匹配)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::UpdateWhere(RECORDLIST* plstAffect,LONG nTop,WHERE pfnWhere,ARGS_SHIFTER pfnWhereShifer,UPDATE_ROUTINE pfnUpdateRoutine,...)
{
	// 检查参数
	if(pfnUpdateRoutine==NULL) return 0;
	va_list argsWhere,argsUpdate;
	va_start(argsWhere,pfnUpdateRoutine);
	va_start(argsUpdate,pfnUpdateRoutine);
	if(pfnWhere!=NULL&&pfnWhereShifer!=NULL) (*pfnWhereShifer)(argsUpdate);
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pfnWhere,argsWhere,lstPreliminary);

	// 调用更新
	LONG nAffectRecords=0;
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		(*pfnUpdateRoutine)(pRecord,argsUpdate);
		pRecord->__Krnl.m_nRecordVer++;
		nAffectRecords++;
		if(plstAffect!=NULL) plstAffect->AddTail(*pRecord);
		if(nTop>0&&nAffectRecords>=nTop) break;
	}
	return nAffectRecords;
}

// 更新记录(索引匹配)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::UpdateByIndex(RECORDLIST* plstAffect,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,ARGS_SHIFTER pfnWhereShifer,UPDATE_ROUTINE pfnUpdateRoutine,...)
{
	// 检查参数
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	if(pfnUpdateRoutine==NULL) return 0;
	va_list argsWhere,argsUpdate;
	va_start(argsWhere,pfnUpdateRoutine);
	va_start(argsUpdate,pfnUpdateRoutine);
	if(pfnWhere!=NULL&&pfnWhereShifer!=NULL) (*pfnWhereShifer)(argsUpdate);
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pIIndexRender,pKey,pfnWhere,argsWhere,lstPreliminary);

	// 调用更新
	LONG nAffectRecords=0;
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		(*pfnUpdateRoutine)(pRecord,argsUpdate);
		pRecord->__Krnl.m_nRecordVer++;
		nAffectRecords++;
		if(plstAffect!=NULL) plstAffect->AddTail(*pRecord);
		if(nTop>0&&nAffectRecords>=nTop) break;
	}
	return nAffectRecords;
}




// 插入或更新
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::InsertOrUpdate(RECORD* pAffect,const PKEY& PKey,INSERTUPDATE_ROUTINE pfnInsertUpdateRoutine,...)
{
	// 检查参数
	if(pfnInsertUpdateRoutine==NULL) return FALSE;

	CAutoLock AutoLock(&m_CsTable);

	// 查找记录是否存在,如果存在则更新数据
	LPVOID pRecordVoid=NULL;
	if(m_RecordMap.Lookup(PKey,pRecordVoid))
	{	POSITION posRecord=(POSITION)pRecordVoid;
		RECORD* pRecord=&m_RecordList.GetAt(posRecord);
		va_list args;
		va_start(args,pfnInsertUpdateRoutine);
		(*pfnInsertUpdateRoutine)(pRecord,&PKey,FALSE,args);
		va_end(args);
		pRecord->__Krnl.m_nRecordVer++;
		if(pAffect!=NULL) (*pAffect)=(*pRecord);
		return TRUE;
	}

	// 尝试插入记录
	RECORD Record;
	memset(&Record,0,size_of(Record));
	va_list args;
	va_start(args,pfnInsertUpdateRoutine);
	(*pfnInsertUpdateRoutine)(&Record,&PKey,TRUE,args);
	va_end(args);
	return InsertRecord(pAffect,PKey,Record);
}

// 插入或更新
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::InsertOrUpdateWhere(RECORD* pAffect,WHERE pfnWhere,ARGS_SHIFTER pfnWhereShifer,INSERTUPDATE_ROUTINE pfnInsertUpdateRoutine,...)
{
	// 检查参数
	if(pfnInsertUpdateRoutine==NULL) return FALSE;
	va_list argsWhere,argsInsertUpdate;
	va_start(argsWhere,pfnInsertUpdateRoutine);
	va_start(argsInsertUpdate,pfnInsertUpdateRoutine);
	if(pfnWhere!=NULL&&pfnWhereShifer!=NULL) (*pfnWhereShifer)(argsInsertUpdate);
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	RECORD* apPreliminary[2]={0,0};
	DWORD dwSelected=SelectRecordEx(ARRAYSIZE(apPreliminary),2,pfnWhere,argsWhere,apPreliminary);
	
	// 进行插入或更新
	BOOL bSuccess=TRUE;
	do 
	{	// 记录超过一条(不知道更新哪一条)
		if(dwSelected>1)
		{	bSuccess=FALSE;
			break;
		}
		// 存在且仅存一条记录(进行更新)
		else if(dwSelected>0)
		{	RECORD* pRecord=apPreliminary[0];
			(*pfnInsertUpdateRoutine)(pRecord,NULL,FALSE,argsInsertUpdate);
			pRecord->__Krnl.m_nRecordVer++;
			if(pAffect!=NULL) (*pAffect)=(*pRecord);
			break;
		}
		// 记录不存在,尝试插入记录
		RECORD Record;
		memset(&Record,0,size_of(Record));
		(*pfnInsertUpdateRoutine)(&Record,NULL,TRUE,argsInsertUpdate);
		PKEY PKey;
		memset(&PKey,0,size_of(PKey));
		RecordToKeys(Record,&PKey,1);
		bSuccess=InsertRecord(pAffect,PKey,Record);
		break;
	} while(FALSE);
	va_end(argsWhere);
	va_end(argsInsertUpdate);
	return bSuccess;
}

// 插入或更新
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::InsertOrUpdateByIndex(RECORD* pAffect,LONG nIndex,LPVOID pKey,WHERE pfnWhere,ARGS_SHIFTER pfnWhereShifer,INSERTUPDATE_ROUTINE pfnInsertUpdateRoutine,...)
{
	// 检查参数
	if(pfnInsertUpdateRoutine==NULL) return FALSE;
	if(nIndex<0||nIndex>=INDEXNUM) return FALSE;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	va_list argsWhere,argsInsertUpdate;
	va_start(argsWhere,pfnInsertUpdateRoutine);
	va_start(argsInsertUpdate,pfnInsertUpdateRoutine);
	if(pfnWhere!=NULL&&pfnWhereShifer!=NULL) (*pfnWhereShifer)(argsInsertUpdate);
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	RECORD* apPreliminary[2]={0,0};
	DWORD dwSelected=SelectRecordEx(ARRAYSIZE(apPreliminary),2,pIIndexRender,pKey,pfnWhere,argsWhere,apPreliminary);

	// 进行插入或更新
	BOOL bSuccess=TRUE;
	do 
	{	// 记录超过一条(不知道更新哪一条)
		if(dwSelected>1)
		{	bSuccess=FALSE;
			break;
		}
		// 存在且仅存一条记录(进行更新)
		else if(dwSelected>0)
		{	RECORD* pRecord=apPreliminary[0];
			(*pfnInsertUpdateRoutine)(pRecord,NULL,FALSE,argsInsertUpdate);
			pRecord->m_nRecordVer++;
			if(pAffect!=NULL) (*pAffect)=(*pRecord);
			break;
		}
		// 记录不存在,尝试插入记录
		RECORD Record;
		memset(&Record,0,size_of(Record));
		(*pfnInsertUpdateRoutine)(&Record,NULL,TRUE,argsInsertUpdate);
		PKEY PKey;
		memset(&PKey,0,size_of(PKey));
		RecordToKeys(Record,&PKey,1);
		bSuccess=InsertRecord(pAffect,PKey,Record);
		break;
	} while(FALSE);
	va_end(argsWhere);
	va_end(argsInsertUpdate);
	return bSuccess;
}



// 按照主键删除记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::Delete(RECORD* pAffect,const PKEY& PKey)
{
	CAutoLock AutoLock(&m_CsTable);

	// 查找主键
	LPVOID pRecordVoid=NULL;
	if(!m_RecordMap.Lookup(PKey,pRecordVoid)) return FALSE;
	POSITION posRecord=(POSITION)pRecordVoid;
	RECORD* pRecord=&m_RecordList.GetAt(posRecord);

	// 删除主键,索引和记录
	DeleteRecord(pAffect,pRecord);

	return TRUE;
}

// 按照条件删除记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::DeleteWhere(RECORDLIST* plstAffect,LONG nTop,WHERE pfnWhere,...)
{
	// 检查参数
	CAutoLock AutoLock(&m_CsTable);
	
	// 遍历记录
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pfnWhere,argsWhere,lstPreliminary);
	va_end(argsWhere);

	// 删除记录
	LONG nAffectRecords=0;
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		nAffectRecords++;
		RECORD RecordAffect;
		memset(&RecordAffect,0,size_of(RecordAffect));
		DeleteRecord(&RecordAffect,pRecord);
		if(plstAffect!=NULL) plstAffect->AddTail(RecordAffect);
		if(nTop>0&&nAffectRecords>=nTop) break;
	}
	return nAffectRecords;
}

// 按照索引删除记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::DeleteByIndex(RECORDLIST* plstAffect,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,...)
{
	// 检查参数
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORDPTRLIST lstPreliminary;
	SelectRecord(nTop,pIIndexRender,pKey,pfnWhere,argsWhere,lstPreliminary);
	va_end(argsWhere);

	// 删除记录
	LONG nAffectRecords=0;
	while(!lstPreliminary.IsEmpty())
	{	RECORD* pRecord=lstPreliminary.RemoveHead();
		nAffectRecords++;
		RECORD RecordAffect;
		memset(&RecordAffect,0,size_of(RecordAffect));
		DeleteRecord(&RecordAffect,pRecord);
		if(plstAffect!=NULL) plstAffect->AddTail(RecordAffect);
		if(nTop>0&&nAffectRecords>=nTop) break;
	}
	return nAffectRecords;
}













// 根据数据查找(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectWhereEx(RECORD* pResult,DWORD dwMax,LONG nTop,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(pResult!=NULL);
	dwMax=max_of(dwMax,1);
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pfnWhere,argsWhere,ppPreliminary);
	va_end(argsWhere);

	// 返回数据
	for(DWORD dwItem=0; dwItem<dwSelected; dwItem++)
	{	RECORD* pRecord=ppPreliminary[dwItem];
		pResult[dwItem]=(*pRecord);
	}
	return dwSelected;
}

// 根据索引查找(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectByIndexEx(RECORD* pResult,DWORD dwMax,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(pResult!=NULL);
	dwMax=max_of(dwMax,1);
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pIIndexRender,pKey,pfnWhere,argsWhere,ppPreliminary);
	va_end(argsWhere);

	// 返回数据
	for(DWORD dwItem=0; dwItem<dwSelected; dwItem++)
	{	RECORD* pRecord=ppPreliminary[dwItem];
		pResult[dwItem]=(*pRecord);
	}
	return dwSelected;
}


// 按照条件查找并锁定(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectForUpdateWhereEx(RECORD** ppForUpdate,DWORD dwMax,LONG nTop,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(ppForUpdate!=NULL);
	dwMax=max_of(dwMax,1);
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pfnWhere,argsWhere,ppPreliminary);
	va_end(argsWhere);

	// 生成副本数据
	for(DWORD dwSelect=0; dwSelect<dwSelected; dwSelect++)
	{	RECORD* pRecord=ppPreliminary[dwSelect];
		RECORD* pRecordCopy=CreateCopyRecord(pRecord);
		ppForUpdate[dwSelect]=pRecordCopy;
	}
	return dwSelected;
}

// 按照索引查找并锁定(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::SelectForUpdateByIndexEx(RECORD** ppForUpdate,DWORD dwMax,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,...)
{
	// 检查参数
	CLIBVERIFY(ppForUpdate!=NULL);
	dwMax=max_of(dwMax,1);
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pIIndexRender,pKey,pfnWhere,argsWhere,ppPreliminary);
	va_end(argsWhere);

	// 生成副本数据
	for(DWORD dwSelect=0; dwSelect<dwSelected; dwSelect++)
	{	RECORD* pRecord=ppPreliminary[dwSelect];
		RECORD* pRecordCopy=CreateCopyRecord(pRecord);
		ppForUpdate[dwSelect]=pRecordCopy;
	}
	return dwSelected;
}

// 批量提交或撤销修改(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::BatchCommitUpdateEx(RECORD** ppForUpdate,DWORD dwCount,BOOL bCancel)
{
	CLIBVERIFY(ppForUpdate!=NULL);
	CAutoLock AutoLock(&m_CsTable);
	BOOL bOneOrMoreFail=FALSE;
	for(DWORD dwRecord=0; dwRecord<dwCount; dwRecord++)
	{	RECORD* pRecord=ppForUpdate[dwRecord];
		if(!CommitUpdate(pRecord,bCancel)) bOneOrMoreFail=TRUE;
	}
	return !bOneOrMoreFail;
}

// 更新记录(逐行匹配,快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::UpdateWhereEx(RECORD* pAffect,DWORD dwMax,LONG nTop,WHERE pfnWhere,ARGS_SHIFTER pfnWhereShifer,UPDATE_ROUTINE pfnUpdateRoutine,...)
{
	// 检查参数
	dwMax=max_of(dwMax,1);
	if(pfnUpdateRoutine==NULL) return 0;
	va_list argsWhere,argsUpdate;
	va_start(argsWhere,pfnUpdateRoutine);
	va_start(argsUpdate,pfnUpdateRoutine);
	if(pfnWhere!=NULL&&pfnWhereShifer!=NULL) (*pfnWhereShifer)(argsUpdate);
	CAutoLock AutoLock(&m_CsTable);

	// 遍历记录
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pfnWhere,argsWhere,ppPreliminary);

	// 调用更新
	for(DWORD dwSelect=0; dwSelect<dwSelected; dwSelect++)
	{	RECORD* pRecord=ppPreliminary[dwSelect];
		(*pfnUpdateRoutine)(pRecord,argsUpdate);
		pRecord->__Krnl.m_nRecordVer++;
		if(pAffect!=NULL) pAffect[dwSelect]=(*pRecord);
	}
	return dwSelected;
}

// 更新记录(索引匹配,快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::UpdateByIndexEx(RECORD* pAffect,DWORD dwMax,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,ARGS_SHIFTER pfnWhereShifer,UPDATE_ROUTINE pfnUpdateRoutine,...)
{
	// 检查参数
	dwMax=max_of(dwMax,1);
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	if(pfnUpdateRoutine==NULL) return 0;
	va_list argsWhere,argsUpdate;
	va_start(argsWhere,pfnUpdateRoutine);
	va_start(argsUpdate,pfnUpdateRoutine);
	if(pfnWhere!=NULL&&pfnWhereShifer!=NULL) (*pfnWhereShifer)(argsUpdate);
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pIIndexRender,pKey,pfnWhere,argsWhere,ppPreliminary);

	// 调用更新
	for(DWORD dwSelect=0; dwSelect<dwSelected; dwSelect++)
	{	RECORD* pRecord=ppPreliminary[dwSelect];
		(*pfnUpdateRoutine)(pRecord,argsUpdate);
		pRecord->__Krnl.m_nRecordVer++;
		if(pAffect!=NULL) pAffect[dwSelect]=(*pRecord);
	}
	return dwSelected;
}


// 按照条件删除记录(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::DeleteWhereEx(RECORD* pAffect,DWORD dwMax,LONG nTop,WHERE pfnWhere,...)
{
	// 检查参数
	dwMax=max_of(dwMax,1);
	CAutoLock AutoLock(&m_CsTable);
	
	// 遍历记录
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pfnWhere,argsWhere,ppPreliminary);
	va_end(argsWhere);

	// 删除记录
	for(DWORD dwSelect=0; dwSelect<dwSelected; dwSelect++)
	{	RECORD* pRecord=ppPreliminary[dwSelect];
		RECORD RecordAffect;
		memset(&RecordAffect,0,size_of(RecordAffect));
		DeleteRecord(&RecordAffect,pRecord);
		if(pAffect!=NULL) pAffect[dwSelect]=RecordAffect;
	}
	return dwSelected;
}

// 按照索引删除记录(快速模式)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE LONG TMemTable<PKEY,RECORD,INDEXNUM>::DeleteByIndexEx(RECORD* pAffect,DWORD dwMax,LONG nTop,LONG nIndex,LPVOID pKey,WHERE pfnWhere,...)
{
	// 检查参数
	dwMax=max_of(dwMax,1);
	if(nIndex<0||nIndex>=INDEXNUM) return 0;
	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
	CLIBVERIFY(pIIndexRender!=NULL);
	CAutoLock AutoLock(&m_CsTable);

	// 查找索引
	va_list argsWhere;
	va_start(argsWhere,pfnWhere);
	RECORD** ppPreliminary=(RECORD**)alloca(dwMax*size_of(RECORD*));
	DWORD dwSelected=SelectRecordEx(dwMax,nTop,pIIndexRender,pKey,pfnWhere,argsWhere,ppPreliminary);
	va_end(argsWhere);

	// 删除记录
	for(DWORD dwSelect=0; dwSelect<dwSelected; dwSelect++)
	{	RECORD* pRecord=ppPreliminary[dwSelect];
		RECORD RecordAffect;
		memset(&RecordAffect,0,size_of(RecordAffect));
		DeleteRecord(&RecordAffect,pRecord);
		if(pAffect!=NULL) pAffect[dwSelect]=RecordAffect;
	}
	return dwSelected;
}













// 查找记录(通过遍历)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE VOID TMemTable<PKEY,RECORD,INDEXNUM>::SelectRecord(LONG nTop,WHERE pfnWhere,va_list argsWhere,RECORDPTRLIST& lstPreliminary)
{	POSITION posRecord=m_RecordList.GetHeadPosition();
	while(posRecord!=NULL)
	{	RECORD* pRecord=&m_RecordList.GetNext(posRecord);
		if(pRecord->__Krnl.m_bDeleted||pRecord->__Krnl.m_pCopyFrom!=NULL) continue;
		if(pfnWhere!=NULL&&!(*pfnWhere)(pRecord,argsWhere)) continue;
		lstPreliminary.AddTail(pRecord);
		if(nTop>0&&lstPreliminary.GetCount()>=nTop) break;
	}
}

// 查找记录(通过索引)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE VOID TMemTable<PKEY,RECORD,INDEXNUM>::SelectRecord(LONG nTop,IIndexRender* pIIndexRender,LPVOID pKey,WHERE pfnWhere,va_list argsWhere,RECORDPTRLIST& lstPreliminary)
{	RECORDPTRLIST lstPreliminaryIX;
	pIIndexRender->OnSelectUsingIndex((pfnWhere==NULL)?nTop:(-1),pKey,&lstPreliminaryIX);
	if(lstPreliminaryIX.IsEmpty())
	{	lstPreliminary.RemoveAll();
		return;
	}
	while(!lstPreliminaryIX.IsEmpty())
	{	RECORD* pRecord=lstPreliminaryIX.RemoveHead();
		if(pfnWhere!=NULL&&!(*pfnWhere)(pRecord,argsWhere)) continue;
		lstPreliminary.AddTail(pRecord);
		if(nTop>0&&lstPreliminary.GetCount()>=nTop) break;
	}
}

// 查找记录(通过遍历)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE DWORD TMemTable<PKEY,RECORD,INDEXNUM>::SelectRecordEx(DWORD dwMax,LONG nTop,WHERE pfnWhere,va_list argsWhere,RECORD** ppPreliminarys)
{	DWORD dwSelectedNum=0;
	POSITION posRecord=m_RecordList.GetHeadPosition();
	while(posRecord!=NULL)
	{	RECORD* pRecord=&m_RecordList.GetNext(posRecord);
		if(pRecord->__Krnl.m_bDeleted||pRecord->__Krnl.m_pCopyFrom!=NULL) continue;
		if(pfnWhere!=NULL&&!(*pfnWhere)(pRecord,argsWhere)) continue;
		if(dwSelectedNum>=dwMax) break;
		ppPreliminarys[dwSelectedNum]=pRecord;
		dwSelectedNum++;
		if(nTop>0&&dwSelectedNum>=(DWORD)nTop) break;
		if(dwSelectedNum>=dwMax) break;
	}
	return dwSelectedNum;
}

// 查找记录(通过索引)
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE DWORD TMemTable<PKEY,RECORD,INDEXNUM>::SelectRecordEx(DWORD dwMax,LONG nTop,IIndexRender* pIIndexRender,LPVOID pKey,WHERE pfnWhere,va_list argsWhere,RECORD** ppPreliminarys)
{
	DWORD dwMaxPreliminaryIX=min_of(2*dwMax,1024);
	RECORD** ppPreliminaryIX=(RECORD**)alloca(dwMaxPreliminaryIX*size_of(LPVOID));
	DWORD dwSelectedIX=pIIndexRender->OnSelectUsingIndexEx(dwMaxPreliminaryIX,(pfnWhere==NULL)?nTop:(-1),pKey,(LPVOID*)ppPreliminaryIX);
	if(dwSelectedIX<=0) return 0;
	DWORD dwSelected=0;
	for(DWORD dwSelectIX=0; dwSelectIX<dwSelectedIX; dwSelectIX++)
	{	RECORD* pRecord=ppPreliminaryIX[dwSelectIX];
		if(pfnWhere!=NULL&&!(*pfnWhere)(pRecord,argsWhere)) continue;
		if(dwSelected>=dwMax) break;
		ppPreliminarys[dwSelected]=pRecord;
		dwSelected++;
		if(nTop>0&&dwSelected>=(DWORD)nTop) break;
		if(dwSelected>=dwMax) break;
	}
	return dwSelected;
}



// 插入记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE BOOL TMemTable<PKEY,RECORD,INDEXNUM>::InsertRecord(RECORD* pAffect,const PKEY& PKey,RECORD& Record)
{
	// 如果主键冲突,返回失败
	LPVOID pRecordVoid=NULL;
	if(m_RecordMap.Lookup(PKey,pRecordVoid)) return FALSE;

	// 添加记录
	POSITION posRecord=m_RecordList.AddTail(Record);
	CLIBVERIFY(posRecord!=NULL);

	// 创建全部索引
	RECORD* pRecord=&m_RecordList.GetAt(posRecord);
	pRecord->__Krnl.m_bDeleted=FALSE;
	pRecord->__Krnl.m_nRecordVer=0;
	pRecord->__Krnl.m_nRecordRef=1;
	pRecord->__Krnl.m_pCopyFrom=NULL;
	BOOL bRollback=FALSE;
	LONG nIndex=0;
	for(nIndex=0; nIndex<INDEXNUM; nIndex++)
	{	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
		CLIBVERIFY(pIIndexRender!=NULL);
		if(!pIIndexRender->OnInsertRecord(pRecord))
		{	bRollback=TRUE;
			break;
		}
	}
	// 如果进行数据回退,则撤销已经建立的索引并删除临时的数据
	if(bRollback)
	{	while(nIndex>0)
		{	nIndex--;
			IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
			CLIBVERIFY(pIIndexRender!=NULL);
			pIIndexRender->OnDeleteRecord(pRecord);
		}
		m_RecordList.RemoveAt(posRecord);
		return FALSE;
	}

	// 建立主键
	m_RecordMap.SetAt(PKey,posRecord);

	// 返回受影响的字段
	if(pAffect!=NULL) (*pAffect)=(*pRecord);

	return TRUE;
}

// 删除记录
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE VOID TMemTable<PKEY,RECORD,INDEXNUM>::DeleteRecord(RECORD* pAffect,RECORD* pRecord)
{
	// 删除主键,索引和记录
	if(!pRecord->__Krnl.m_bDeleted)
	{	
		// 通过记录得到主键
		PKEY PKey;
		memset(&PKey,0,size_of(PKey));
		RecordToKeys(*pRecord,&PKey,1);
		// 删除主键
		m_RecordMap.RemoveKey(PKey);
		// 撤销全部索引
		for(LONG nIndex=0; nIndex<INDEXNUM; nIndex++)
		{	IIndexRender* pIIndexRender=m_apIndexRender[nIndex];
			CLIBVERIFY(pIIndexRender!=NULL);
			pIIndexRender->OnDeleteRecord(pRecord);
		}
		// 删除记录
		pRecord->__Krnl.m_bDeleted=TRUE;
		pRecord->__Krnl.m_nRecordVer++;
		pRecord->__Krnl.m_nRecordRef--;
		if(pAffect!=NULL) (*pAffect)=(*pRecord);
		if(pRecord->__Krnl.m_nRecordRef<=0) m_RecordList.RemoveByPtr(pRecord);
	}
}

// 创建记录副本
template<class PKEY,class RECORD,INT INDEXNUM>
CLIB_INLINE RECORD* TMemTable<PKEY,RECORD,INDEXNUM>::CreateCopyRecord(RECORD* pRecord)
{	POSITION posCopy=m_RecordList.AddTail(*pRecord);
	RECORD* pCopy=&m_RecordList.GetAt(posCopy);
	pCopy->__Krnl.m_bDeleted=FALSE;
	pCopy->__Krnl.m_nRecordVer=pRecord->__Krnl.m_nRecordVer;
	pCopy->__Krnl.m_nRecordRef=-1;
	pCopy->__Krnl.m_pCopyFrom=pRecord;
	pRecord->__Krnl.m_nRecordRef++;
	return pCopy;
}




#endif
