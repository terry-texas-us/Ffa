#include "stdafx.h"

//#include "PegAEsysDoc.h"
//
//#include "FileJob2.h"
//
//#include "Job2State.h"
//
//extern CJob2State job2state;
//
//void CFileJob2::Close()
//{
//	CloseHandle((HANDLE)hFile);
//}
//
////bool CFileJob2::OpenForRead(const CString& strPathName)
////{
////	if(CFile::Open(strPathName, CFile::modeRead | CFile::shareDenyNone) == 0)
////	{
////		//msgWarning(IDS_MSG_TRACING_OPEN_FAILURE, strPathName); 
////		return false;
////	}
////	return true;
////}
//
//bool CFileJob2::OpenForRead(const CString& strPathName)
//{
//	DWORD dwAccess = GENERIC_READ;
//	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
//	DWORD dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
//	DWORD dwDisposition = OPEN_ALWAYS;
//
//	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);
//
//	if((HANDLE)hFile == INVALID_HANDLE_VALUE)
//	{
//		msgWarning(IDS_MSG_TRACING_OPEN_FAILURE, strPathName); 
//		return false;
//	}
//
//	return true;
//}
//
////bool CFileJob2::OpenForWrite(const CString& strPathName)
////{
////	if(CFile::Open(strPathName, CFile::modeWrite | CFile::modeCreate) == 0)
////	{
////		//msgWarning(IDS_MSG_TRACING_WRITE_FAILURE, strPathName);
////		return false;
////	}
////	return true;
////}
//
//bool CFileJob2::OpenForWrite(const CString& strPathName)
//{
//	DWORD dwAccess = GENERIC_WRITE;
//	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
//	DWORD dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
//	DWORD dwDisposition = OPEN_ALWAYS;
//
//	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);
//
//	if((HANDLE)hFile == INVALID_HANDLE_VALUE)
//	{
//		msgWarning(IDS_MSG_TRACING_OPEN_FAILURE, strPathName); 
//		return false;
//	}
//
//	return true;
//}
//
//void CFileJob2::ReadHeader()
//{
//	Read(&m_dVersion, sizeof(double));
//}
//
//void CFileJob2::ReadInfo()
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	// extents
//	Read(&job2state.m_rectClip.left, sizeof(LONG));
//	Read(&job2state.m_rectClip.right, sizeof(LONG));
//	Read(&job2state.m_rectClip.bottom, sizeof(LONG));
//	Read(&job2state.m_rectClip.top, sizeof(LONG));
//
//	// origin
//	Read(&job2state.m_pntOrigin[0], sizeof(double));
//	Read(&job2state.m_pntOrigin[1], sizeof(double));
//
//	// border files
//	int nBorderCount;
//	CString str;
//	Read(&nBorderCount, sizeof(int));
//	for(int i = 0; i < nBorderCount; i++)
//	{
//		str.Empty();
//		char c;
//		while(Read(&c, 1) == 1)
//		{
//			if (c == '\t') break;
//			str += c;
//		}
//		
//		pDoc->BorderMap(str);
//	}
//}
//
//void CFileJob2::WriteHeader()
//{
//	Write(&m_dVersion, sizeof(double));
//}
//
//void CFileJob2::WriteInfo()
//{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
//
//	// extents
//	Write(&job2state.m_rectClip.left, sizeof(LONG));
//	Write(&job2state.m_rectClip.right, sizeof(LONG));
//	Write(&job2state.m_rectClip.bottom, sizeof(LONG));
//	Write(&job2state.m_rectClip.top, sizeof(LONG));
//
//	// origin
//	Write(&job2state.m_pntOrigin[0], sizeof(double));
//	Write(&job2state.m_pntOrigin[1], sizeof(double));
//
//	// border files
//	int nSize = pDoc->BorderLayersGetSize();
//	Write(&nSize, sizeof(int));
//	for(int i = 0; i < pDoc->BorderLayersGetSize(); i++)
//	{
//		CLayer *pLayer = pDoc->BorderLayersGetAt(i);
//		Write(pLayer->GetName(), (UINT)pLayer->GetName().GetLength());
//		Write("\t", 1);
//	}
//
//	//int ad = 1;
//	//Write(&ad, sizeof(int));
//	//CString strin = "border.jb1";
//	//Write(strin, (UINT)strin.GetLength());
//	//Write("\t", 1);
//}