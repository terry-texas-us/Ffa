#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "FileJob.h"
#include "Vax.h"

bool CFileJob::OpenForRead(CString strPathName)
{
	DWORD dwAccess = GENERIC_READ;
	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
	DWORD dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwDisposition = OPEN_ALWAYS;//CREATE_ALWAYS;//OPEN_ALWAYS;

	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);

	if((HANDLE)hFile == INVALID_HANDLE_VALUE)
	{
		msgWarning(IDS_MSG_TRACING_OPEN_FAILURE, strPathName); 
		return false;
	}

	return true;
}

bool CFileJob::OpenForWrite(CString strPathName)
{
	DWORD dwAccess = GENERIC_WRITE;
	DWORD dwAttr = FILE_ATTRIBUTE_NORMAL;
	DWORD dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwDisposition = OPEN_ALWAYS;//CREATE_ALWAYS;//OPEN_ALWAYS;

	hFile = (HFILE)::CreateFile(strPathName.GetBuffer(), dwAccess, dwShare, NULL, dwDisposition, dwAttr, NULL);

	if((HANDLE)hFile == INVALID_HANDLE_VALUE)
	{
		msgWarning(IDS_MSG_TRACING_OPEN_FAILURE, strPathName); 
		return false;
	}

	return true;
}

void CFileJob::Close()
{
	CloseHandle((HANDLE)hFile);
}

void CFileJob::Seek(LONG lDist, DWORD dwMove)
{
	SetFilePointer((HANDLE)hFile, lDist, NULL, dwMove);
}

DWORD CFileJob::GetPosition()
{
	return SetFilePointer((HANDLE)hFile, 0, NULL, FILE_CURRENT);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CFileJob::ReadHeader()
{
	DWORD dwRead;

//	if (Read(m_PrimBuf, 32) == 32)
	if(ReadFile((HANDLE)hFile, m_PrimBuf, 32, &dwRead, NULL))// == 32)
	{
		if (ReleaseVersion() == 1)
		{
			Seek(0, FILE_BEGIN);
		}
		else
		{
//			if(GetLength() >= 96)
			if(GetFileSize((HANDLE)hFile, NULL) >= 96)
				Seek(96, FILE_BEGIN);
		}
	}
}

void CFileJob::ReadSegs(CLayer* pLayer)
{	
	CSeg* pSeg;

	while (filejob_GetNextSeg2(hFile, m_iVersion, m_PrimBuf, pSeg))
	{
		if(pSeg != 0) 
			pLayer->AddTail(pSeg);
	}
}

int CFileJob::ReleaseVersion()
{
	switch (m_PrimBuf[5])
	{
		case 17:	// 0x11	text
		case 24:	// 0x18	bspline
		case 33:	// 0x21 conic (twopi) 64 bytes must have been ellispe?
		case 61:	// 0x3D conic
		case 67:	// 0x43	line
		case 70:	// 0x46 mark
		case 100:	// 0x64 polygon
			m_iVersion = 1;
			break;

		default:
			m_iVersion = 3;
	}
	return (m_iVersion);
}

void CFileJob::WriteHeader()
{
	::ZeroMemory(m_PrimBuf, 96);
	m_PrimBuf[4] = 'T';
	m_PrimBuf[5] = 'c';

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, m_PrimBuf, 96, &dwWrite, NULL);
}

void CFileJob::WriteSeg(CSeg* pSeg)
{
	m_PrimBuf[0] = 0; 
	*((short*) &m_PrimBuf[1]) = short(pSeg->GetCount());
	
	POSITION pos = pSeg->GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = pSeg->GetNext(pos);
		pPrim->WriteJob(hFile, m_PrimBuf);
	}
}

void CFileJob::WriteSegs(CLayer* pLayer)
{
	pLayer->BreakSegRefs();
	pLayer->BreakPolylines();
		
	POSITION pos = pLayer->GetHeadPosition();
	while (pos != 0)
	{	
		CSeg* pSeg = pLayer->GetNext(pos);
		WriteSeg(pSeg);
	}
}

bool filejob_GetNextSeg(CFile& f, int iVersion, char* pBuf, CSeg*& pSeg)
{
	ULONGLONG dwPosition = f.GetPosition();

	pSeg = 0;
	try
	{
		CPrim*	pPrim;
		if (!filejob_GetNextPrim(f, iVersion, pBuf, pPrim)) return false;
		pSeg = new CSeg(pPrim); 
		WORD wPrims = *((WORD*) ((iVersion == 1) ? &pBuf[2] : &pBuf[1]));
		for (WORD w = 1; w < wPrims; w++)
		{
			try
			{
				dwPosition = f.GetPosition();
				if (!filejob_GetNextPrim(f, iVersion, pBuf, pPrim))
					throw "Exception.FileJob: Unexpected end of file.";
                pSeg->AddTail(pPrim); 
			}
			catch (char* szMessage)
			{
				msgInformation(szMessage);
				f.Seek(dwPosition + 32, CFile::begin);
			}
		}
	}
	catch (char* szMessage)
	{
		if (dwPosition >= 96)
		{
			if (::MessageBox(0, szMessage, 0, MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
				return false;
//			if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INVALID_PRIM), app.GetSafeHwnd(), DlgProcInvalidPrim))
//				return false;
        }
		f.Seek(dwPosition + 32, CFile::begin);
	}
	return true;
}

bool filejob_GetNextPrim(CFile& f, int iVersion, char* pBuf, CPrim*& pPrim)
{
	short* pTyp = (short*) &pBuf[4];

	do
	{
		if (f.Read(pBuf, 32) < 32) 
			return false;
				
		int iLen = (iVersion == 1) ? pBuf[6] : pBuf[3];

		if (!filejob_IsValidPrimitive(*pTyp))
			throw "Exception.FileJob: Invalid primitive type.";

		if (iLen > 1)
		{
            UINT nCount = (iLen - 1) * 32;
		
			if (nCount >= CPrim::BUFFER_SIZE - 32)
				throw "Exception.FileJob: Primitive buffer overflow.";

			if (f.Read(&pBuf[32], nCount) < nCount) 
				throw "Exception.FileJob: Unexpected end of file.";
		}
	}	 
	while (*pTyp <= 0 || pBuf[5] == 33);

	switch(*pTyp)
	{
		case CPrim::PRIM_MARK:
			pPrim = new CPrimMark(pBuf, 3);
			break;
		case CPrim::PRIM_LINE:
			pPrim = new CPrimLine(pBuf, 3);
			break;
		case CPrim::PRIM_POLYGON:
			pPrim = new CPrimPolygon(pBuf, 3);
			break;
		case CPrim::PRIM_ARC:
			pPrim = new CPrimArc(pBuf, 3);
			break;
		case CPrim::PRIM_BSPLINE:
			pPrim = new CPrimBSpline(pBuf, 3);
			break;
		case CPrim::PRIM_CSPLINE:
			pPrim = new CPrimCSpline(pBuf);
			break;
		case CPrim::PRIM_TEXT:
			pPrim = new CPrimText(pBuf, 3);
			break;
		case CPrim::PRIM_TAG:
			pPrim = new CPrimTag(pBuf);
			break;
		case CPrim::PRIM_DIM:
			pPrim = new CPrimDim(pBuf);
			break;

		default:
			switch (pBuf[5])
			{
				case 17:
					pPrim = new CPrimText(pBuf, 1); 
					break;
				case 24:
					pPrim = new CPrimBSpline(pBuf, 1); 
					break;
				//case 33:
					//pPrim = new CPrimArc(pBuf, 1); 
					//break;
				case 61:
					pPrim = new CPrimArc(pBuf, 1); 
					break;
				case 67:
					pPrim = new CPrimLine(pBuf, 1); 
					break;
				case 70:
					pPrim = new CPrimMark(pBuf, 1); 
					break;
				case 100:
					pPrim = new CPrimPolygon(pBuf, 1); 
					break;

				default:
					throw "Exception.FileJob: Invalid primitive type.";
			}
	}
	return true;
}

bool CFileJob::filejob_GetNextSeg2(HFILE hFile, int iVersion, char* pBuf, CSeg*& pSeg)
{
	DWORD dwPosition = GetPosition();

	pSeg = 0;
	try
	{
		CPrim*	pPrim;
		if (!filejob_GetNextPrim2(hFile, iVersion, pBuf, pPrim)) return false;
		pSeg = new CSeg(pPrim); 
		WORD wPrims = *((WORD*) ((iVersion == 1) ? &pBuf[2] : &pBuf[1]));
		for (WORD w = 1; w < wPrims; w++)
		{
			try
			{
				dwPosition = GetPosition();
				if (!filejob_GetNextPrim2(hFile, iVersion, pBuf, pPrim))
					throw "Exception.FileJob: Unexpected end of file.";
                pSeg->AddTail(pPrim); 
			}
			catch (char* szMessage)
			{
				msgInformation(szMessage);
				Seek(dwPosition + 32, FILE_BEGIN);
			}
		}
	}
	catch (char* szMessage)
	{
		if (dwPosition >= 96)
		{
			if (::MessageBox(0, szMessage, 0, MB_ICONERROR | MB_RETRYCANCEL) == IDCANCEL)
				return false;
//			if(::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INVALID_PRIM), app.GetSafeHwnd(), DlgProcInvalidPrim))
//				return false;
        }
		Seek(dwPosition + 32, FILE_BEGIN);
	}
	return true;
}

bool CFileJob::filejob_GetNextPrim2(HFILE hFile, int iVersion, char* pBuf, CPrim*& pPrim)
{
	short* pTyp = (short*) &pBuf[4];

	DWORD dwRead;

	do
	{
//		if (f.Read(pBuf, 32) < 32)
		ReadFile((HANDLE)hFile, pBuf, 32, &dwRead, NULL);
		if(dwRead < 32) 
			return false;
				
		int iLen = (iVersion == 1) ? pBuf[6] : pBuf[3];

		if (!filejob_IsValidPrimitive(*pTyp))
			throw "Exception.FileJob: Invalid primitive type.";

		if (iLen > 1)
		{
            UINT nCount = (iLen - 1) * 32;
		
			if (nCount >= CPrim::BUFFER_SIZE - 32)
				throw "Exception.FileJob: Primitive buffer overflow.";

			ReadFile((HANDLE)hFile, &pBuf[32], nCount, &dwRead, NULL);
//			if (f.Read(&pBuf[32], nCount) < nCount) 
			if(dwRead < nCount) 
				throw "Exception.FileJob: Unexpected end of file.";
		}
	}	 
	while (*pTyp <= 0 || pBuf[5] == 33);

	switch(*pTyp)
	{
		case CPrim::PRIM_MARK:
			pPrim = new CPrimMark(pBuf, 3);
			break;
		case CPrim::PRIM_LINE:
			pPrim = new CPrimLine(pBuf, 3);
			break;
		case CPrim::PRIM_POLYGON:
			pPrim = new CPrimPolygon(pBuf, 3);
			break;
		case CPrim::PRIM_ARC:
			pPrim = new CPrimArc(pBuf, 3);
			break;
		case CPrim::PRIM_BSPLINE:
			pPrim = new CPrimBSpline(pBuf, 3);
			break;
		case CPrim::PRIM_CSPLINE:
			pPrim = new CPrimCSpline(pBuf);
			break;
		case CPrim::PRIM_TEXT:
			pPrim = new CPrimText(pBuf, 3);
			break;
		case CPrim::PRIM_TAG:
			pPrim = new CPrimTag(pBuf);
			break;
		case CPrim::PRIM_DIM:
			pPrim = new CPrimDim(pBuf);
			break;

		default:
			switch (pBuf[5])
			{
				case 17:
					pPrim = new CPrimText(pBuf, 1); 
					break;
				case 24:
					pPrim = new CPrimBSpline(pBuf, 1); 
					break;
				//case 33:
					//pPrim = new CPrimArc(pBuf, 1); 
					//break;
				case 61:
					pPrim = new CPrimArc(pBuf, 1); 
					break;
				case 67:
					pPrim = new CPrimLine(pBuf, 1); 
					break;
				case 70:
					pPrim = new CPrimMark(pBuf, 1); 
					break;
				case 100:
					pPrim = new CPrimPolygon(pBuf, 1); 
					break;

				default:
					throw "Exception.FileJob: Invalid primitive type.";
			}
	}
	return true;
}

bool filejob_IsValidPrimitive(const short nType)
{
	char* pType = (char*) &nType;
	
	switch (nType)
	{
		case CPrim::PRIM_MARK:				// 0x0100
		//case CPrim::PRIM_INSERT:			// 0x0101
		//case CPrim::PRIM_SEGREF			// 0x0102
		case CPrim::PRIM_LINE:				// 0x0200
		case CPrim::PRIM_POLYGON:			// 0x0400
		case CPrim::PRIM_ARC:				// 0x1003
		case CPrim::PRIM_BSPLINE:			// 0x2000
		case CPrim::PRIM_CSPLINE:			// 0x2001
		//case CPrim::PRIM_POLYLINE:		// 0x2002
		case CPrim::PRIM_TEXT:				// 0x4000
		case CPrim::PRIM_TAG:				// 0x4100
		case CPrim::PRIM_DIM:				// 0x4200
			return true;
		
		default:
			switch (pType[1])
			{
				case 17:			// 0x11	to PRIM_TEXT
				case 24:			// 0x18	to PRIM_BSPLINE
				case 33:			// 0x21 conic (twopi)
				case 61:			// 0x3d to PRIM_ARC 
				case 67:			// 0x43 to PRIM_LINE 
				case 70:			// 0x46 to PRIM_MARK
				case 100:			// 0x64 to PRIM_POLYGON
					return true;

				default:
					return false;
			}
	}
}

CPrimArc::CPrimArc(char* p, int iVer)
{
	if (iVer == 1)
	{
		m_nPenColor = PENCOLOR(p[4] & 0x000f);
		m_nPenStyle = PENSTYLE((p[4] & 0x00ff) >> 4);

		CPnt ptBeg(((CVaxFloat*) &p[8])->Convert(), ((CVaxFloat*) &p[12])->Convert(), 0.);
		ptBeg *= 1.e-3;		
		
		m_ptCenter[0] = ((CVaxFloat*) &p[20])->Convert();
		m_ptCenter[1] = ((CVaxFloat*) &p[24])->Convert();
		m_ptCenter[2] = 0.;
		m_ptCenter *= 1.e-3;
		
		m_dSwpAng = ((CVaxFloat*) &p[28])->Convert();;

		CPnt pt;
		if (m_dSwpAng < 0.)
		{
			pt[0] = (m_ptCenter[0] + ((ptBeg[0] - m_ptCenter[0]) * cos(m_dSwpAng) - (ptBeg[1] - m_ptCenter[1]) * sin(m_dSwpAng)));
            pt[1] = (m_ptCenter[1] + ((ptBeg[0] - m_ptCenter[0]) * sin(m_dSwpAng) + (ptBeg[1] - m_ptCenter[1]) * cos(m_dSwpAng)));
		}
		else
		{
			pt = ptBeg;
		}
		m_vMajAx = pt - m_ptCenter;
		m_vMinAx = ZDIR ^ m_vMajAx;
		m_dSwpAng = fabs(m_dSwpAng);
	}
	else
	{
		m_nPenColor = PENCOLOR(p[6]);
		m_nPenStyle = PENSTYLE(p[7]);

		m_ptCenter = ((CVaxPnt*) &p[8])->Convert();
		m_vMajAx = ((CVaxVec*) &p[20])->Convert();
		m_vMinAx = ((CVaxVec*) &p[32])->Convert();
		m_dSwpAng = ((CVaxFloat*) &p[44])->Convert();

		if (m_dSwpAng > TWOPI || m_dSwpAng < - TWOPI)
			m_dSwpAng = TWOPI;
	}
}

void CPrimArc::WriteJob(HFILE hFile, char* p) const
{
	p[3] = 2;
	*((short*) &p[4]) = PRIM_ARC;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	if (p[7] >= 16) p[7] = 2;

	CPnt pt = m_ptCenter;
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);

	CVec v = m_vMajAx;
	mspace.Transform(v);
	((CVaxVec*) &p[20])->Convert(v);
	
	v = m_vMinAx;
	mspace.Transform(v);
	((CVaxVec*) &p[32])->Convert(v);

	((CVaxFloat*) &p[44])->Convert(m_dSwpAng);
	
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, 64, &dwWrite, NULL);
}

void CPrimArc::Write(CFile& f, char* p) const
{
	p[3] = 2;
	*((short*) &p[4]) = PRIM_ARC;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	if (p[7] >= 16) p[7] = 2;

	CPnt pt = m_ptCenter;
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);

	CVec v = m_vMajAx;
	mspace.Transform(v);
	((CVaxVec*) &p[20])->Convert(v);
	
	v = m_vMinAx;
	mspace.Transform(v);
	((CVaxVec*) &p[32])->Convert(v);

	((CVaxFloat*) &p[44])->Convert(m_dSwpAng);
	
	f.Write(p, 64);
}

void CPrimBitmap::WriteJob(HFILE, char*) const
{

}

void CPrimBitmap::Write(CFile&, char*) const
{

}

CPrimBSpline::CPrimBSpline(char* p, int iVer)
{
	if (iVer == 1)
	{
		m_nPenColor = PENCOLOR(p[4] & 0x000f);
		m_nPenStyle = PENSTYLE((p[4] & 0x00ff) >> 4);
		
		WORD wPts = WORD(((CVaxFloat*) &p[8])->Convert());

		int i = 12;
	
		for (WORD w = 0; w < wPts; w++)
		{
			m_pts.Add(((CVaxPnt*) &p[i])->Convert() * 1.e-3);
			i += sizeof(CVaxPnt);
		}
	}
	else
	{
		m_nPenColor = PENCOLOR(p[6]);
		m_nPenStyle = PENSTYLE(p[7]);
	
		WORD wPts = *((short*) &p[8]);

		int i = 10;
	
		for (WORD w = 0; w < wPts; w++)
		{
			m_pts.Add(((CVaxPnt*) &p[i])->Convert());
			i += sizeof(CVaxPnt);
		}
	}
}

void CPrimBSpline::WriteJob(HFILE hFile, char* p) const
{
	p[3] = char((2 + m_pts.GetSize() * 3) / 8 + 1);
	*((short*) &p[4]) = PRIM_BSPLINE;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	
	*((short*) &p[8]) = (short) m_pts.GetSize();
	
	int i = 10;
	
	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		((CVaxPnt*) &p[i])->Convert(m_pts[w]);
		i += sizeof(CVaxPnt);
	}

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, p[3] * 32, &dwWrite, NULL);
}

void CPrimBSpline::Write(CFile& f, char* p) const
{
	p[3] = char((2 + m_pts.GetSize() * 3) / 8 + 1);
	*((short*) &p[4]) = PRIM_BSPLINE;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	
	*((short*) &p[8]) = (short) m_pts.GetSize();
	
	int i = 10;
	
	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		((CVaxPnt*) &p[i])->Convert(m_pts[w]);
		i += sizeof(CVaxPnt);
	}
	f.Write(p, p[3] * 32);
}

CPrimCSpline::CPrimCSpline(char* p)
{
	m_nPenColor = PENCOLOR(p[6]);
	m_nPenStyle = PENSTYLE(p[7]);
	
	m_wPtsS = *((short*) &p[8]);
	WORD wPts = *((short*) &p[10]);
	m_wEndCndId = *((short*) &p[12]);
	m_TanVec[0] = ((CVaxVec*) &p[14])->Convert();
	m_TanVec[1] = ((CVaxVec*) &p[26])->Convert();

	int i = 38;
	
	for (WORD w = 0; w < wPts; w++)
	{
		m_pts.Add(((CVaxPnt*) &p[i])->Convert());
		i += sizeof(CVaxPnt);
	}
}

void CPrimCSpline::WriteJob(HFILE hFile, char* p) const
{
	p[3] = char((69 + m_pts.GetSize() * 12) / 32);
	*((short*) &p[4]) = PRIM_CSPLINE;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	
	*((short*) &p[8]) = m_wPtsS;
	*((short*) &p[10]) = (short) m_pts.GetSize();
	*((short*) &p[12]) = m_wEndCndId;
	((CVaxVec*) &p[14])->Convert(m_TanVec[0]);
	((CVaxVec*) &p[26])->Convert(m_TanVec[1]);

	int i = 38;

	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		((CVaxPnt*) &p[i])->Convert(m_pts[w]);
		i += sizeof(CVaxPnt);
	}	

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, p[3] * 32, &dwWrite, NULL);
}

void CPrimCSpline::Write(CFile& f, char* p) const
{
	p[3] = char((69 + m_pts.GetSize() * 12) / 32);
	*((short*) &p[4]) = PRIM_CSPLINE;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	
	*((short*) &p[8]) = m_wPtsS;
	*((short*) &p[10]) = (short) m_pts.GetSize();
	*((short*) &p[12]) = m_wEndCndId;
	((CVaxVec*) &p[14])->Convert(m_TanVec[0]);
	((CVaxVec*) &p[26])->Convert(m_TanVec[1]);

	int i = 38;

	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		((CVaxPnt*) &p[i])->Convert(m_pts[w]);
		i += sizeof(CVaxPnt);
	}	
	f.Write(p, p[3] * 32);
}

CPrimDim::CPrimDim(char* p)
{
	m_nPenColor = PENCOLOR(p[6]);
	m_nPenStyle = PENSTYLE(p[7]);

	m_ln[0] = ((CVaxPnt*) &p[8])->Convert();
	m_ln[1] = ((CVaxPnt*) &p[20])->Convert();

	m_nPenColor = PENCOLOR(p[32]);
	
	m_fd.TextFontSet("Simplex.psf");
	m_fd.TextPrecSet(CFontDef::PREC_PEGSTROKEFONT);
	
	m_fd.ChrSpacSet(((CVaxFloat*) &p[36])->Convert());
	m_fd.TextPathSet(WORD(p[40]));
	m_fd.TextHorAlignSet(WORD(p[41]));
	m_fd.TextVerAlignSet(WORD(p[42]));
		
	m_rs.SetOrigin(((CVaxPnt*) &p[43])->Convert());
	m_rs.SetDirX(((CVaxVec*) &p[55])->Convert());
	m_rs.SetDirY(((CVaxVec*) &p[67])->Convert());

	short* pChrs = (short*) &p[79];
	char* pChr = &p[81];

	pChr[*pChrs] = '\0';
	m_strText = pChr;
}

void CPrimDim::WriteJob(HFILE hFile, char* p) const
{
	p[3] = char((118 + m_strText.GetLength()) / 32);
	*((short*) &p[4]) = PRIM_DIM;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	if (p[7] >= 16) p[7] = 2;

	CPnt pt = m_ln[0];
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);

	pt = m_ln[1];
	mspace.Transform(pt);
	((CVaxPnt*) &p[20])->Convert(pt);
	
	p[32] = char(m_nPenColor);
	p[33] = char(m_fd.TextPrec());
	*((short*) &p[34]) = 0;
	((CVaxFloat*) &p[36])->Convert(m_fd.ChrSpac());
	p[40] = char(m_fd.TextPath());
	p[41] = char(m_fd.TextHorAlign());
	p[42] = char(m_fd.TextVerAlign());

	pt = m_rs.Origin();
	mspace.Transform(pt);
	((CVaxPnt*) &p[43])->Convert(pt);
	
	CVec v = m_rs.DirX();
	mspace.Transform(v);
	((CVaxVec*) &p[55])->Convert(v);
	
	v = m_rs.DirY();
	mspace.Transform(v);
	((CVaxVec*) &p[67])->Convert(v);

	short* pChrs = (short*) &p[79];
	*pChrs = short(m_strText.GetLength());

	char* pChr = &p[81];
	
	for (int i = 0; i < *pChrs; i++)
		pChr[i] = m_strText[i];

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, p[3] * 32, &dwWrite, NULL);
}

void CPrimDim::Write(CFile& f, char* p) const
{
	p[3] = char((118 + m_strText.GetLength()) / 32);
	*((short*) &p[4]) = PRIM_DIM;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	if (p[7] >= 16) p[7] = 2;

	CPnt pt = m_ln[0];
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);

	pt = m_ln[1];
	mspace.Transform(pt);
	((CVaxPnt*) &p[20])->Convert(pt);
	
	p[32] = char(m_nPenColor);
	p[33] = char(m_fd.TextPrec());
	*((short*) &p[34]) = 0;
	((CVaxFloat*) &p[36])->Convert(m_fd.ChrSpac());
	p[40] = char(m_fd.TextPath());
	p[41] = char(m_fd.TextHorAlign());
	p[42] = char(m_fd.TextVerAlign());

	pt = m_rs.Origin();
	mspace.Transform(pt);
	((CVaxPnt*) &p[43])->Convert(pt);
	
	CVec v = m_rs.DirX();
	mspace.Transform(v);
	((CVaxVec*) &p[55])->Convert(v);
	
	v = m_rs.DirY();
	mspace.Transform(v);
	((CVaxVec*) &p[67])->Convert(v);

	short* pChrs = (short*) &p[79];
	*pChrs = short(m_strText.GetLength());

	char* pChr = &p[81];
	
	for (int i = 0; i < *pChrs; i++)
		pChr[i] = m_strText[i];

	f.Write(p, p[3] * 32);
}

void CPrimInsert::WriteJob(HFILE hFile, char* p) const
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm(m_pt, m_vX, m_vY);
	tm.Translate(ptBase);
	tm.Inverse();

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	pBlock->WriteJob(hFile, p);

	mspace.Return();
}

void CPrimInsert::Write(CFile& f, char* p) const
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm(m_pt, m_vX, m_vY);
	tm.Translate(ptBase);
	tm.Inverse();

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	pBlock->Write(f, p);

	mspace.Return();
}

CPrimLine::CPrimLine(char* p, int iVer)
{
	if (iVer == 1)
	{
		m_nPenColor = PENCOLOR(p[4] & 0x000f);
		m_nPenStyle = PENSTYLE((p[4] & 0x00ff) >> 4);
		
		CPnt pt = ((CVaxPnt*) &p[8])->Convert();
		pt *= 1.e-3;

		SetPt0(pt);
		pt = ((CVaxPnt*) &p[20])->Convert();
		pt *= 1.e-3;
		SetPt1(pt);
	}
	else
	{	
		m_nPenColor = PENCOLOR(p[6]);
		m_nPenStyle = PENSTYLE(p[7]);
		SetPt0(((CVaxPnt*) &p[8])->Convert());
		SetPt1(((CVaxPnt*) &p[20])->Convert());
	}
}

void CPrimLine::WriteJob(HFILE hFile, char* p) const
{
	p[3] = 1;	
	*((short*) &p[4]) = PRIM_LINE;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	if (p[7] >= 16) p[7] = 2;

	CPnt pt = Pt0();
	mspace.Transform(pt);

	((CVaxPnt*) &p[8])->Convert(pt);
	
	pt = Pt1();
	mspace.Transform(pt);
	
	((CVaxPnt*) &p[20])->Convert(pt);

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, 32, &dwWrite, NULL);
}

void CPrimLine::Write(CFile& f, char* p) const
{
	p[3] = 1;	
	*((short*) &p[4]) = PRIM_LINE;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle == PENSTYLE_BYLAYER ? mS_nLayerPenStyle : m_nPenStyle);
	if (p[7] >= 16) p[7] = 2;

	CPnt pt = Pt0();
	mspace.Transform(pt);

	((CVaxPnt*) &p[8])->Convert(pt);
	
	pt = Pt1();
	mspace.Transform(pt);
	
	((CVaxPnt*) &p[20])->Convert(pt);

	f.Write(p, 32);
}

CPrimMark::CPrimMark(char* p, int iVer)
{
	if (iVer == 1)
	{
		m_nPenColor = PENCOLOR(p[4] & 0x000f);
		m_nMarkStyle = PENSTYLE((p[4] & 0x00ff) >> 4);
		
		m_pt = ((CVaxPnt*) &p[8])->Convert();
		m_pt *= 1.e-3;
		
		m_Dats = 3;
		m_dDat = new double[3];
	
		m_dDat[0] = ((CVaxFloat*) &p[20])->Convert(); 
		m_dDat[1] = ((CVaxFloat*) &p[24])->Convert(); 
		m_dDat[2] = ((CVaxFloat*) &p[28])->Convert(); 
	}
	else
	{
		m_nPenColor = PENCOLOR(p[6]);
		m_nMarkStyle = PENSTYLE(p[7]);

		m_pt = ((CVaxPnt*) &p[8])->Convert();

		m_Dats = 3;
		m_dDat = new double[3];
	
		m_dDat[0] = ((CVaxFloat*) &p[20])->Convert(); 
		m_dDat[1] = ((CVaxFloat*) &p[24])->Convert(); 
		m_dDat[2] = ((CVaxFloat*) &p[28])->Convert(); 
	}
}

void CPrimMark::WriteJob(HFILE hFile, char* p) const
{
	p[3] = 1;
	*((short*) &p[4]) = PRIM_MARK;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nMarkStyle);

	CPnt pt = m_pt;
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);

	::ZeroMemory(&p[20], 12);
	
	int i = 20;

	for (WORD w = 0; w < m_Dats; w++)
	{
		((CVaxFloat*) &p[i])->Convert(m_dDat[w]);
		i += sizeof(CVaxFloat);
	}
		
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, 32, &dwWrite, NULL);
}

void CPrimMark::Write(CFile& f, char* p) const
{
	p[3] = 1;
	*((short*) &p[4]) = PRIM_MARK;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nMarkStyle);

	CPnt pt = m_pt;
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);

	::ZeroMemory(&p[20], 12);
	
	int i = 20;

	for (WORD w = 0; w < m_Dats; w++)
	{
		((CVaxFloat*) &p[i])->Convert(m_dDat[w]);
		i += sizeof(CVaxFloat);
	}
		
	f.Write(p, 32);
}

CPrimPolygon::CPrimPolygon(char* p, int iVer)
{
	if (iVer == 1)
	{
		m_nPenColor = PENCOLOR(p[4] & 0x000f);
		m_nIntStyleId = 0;
		
		double d = ((CVaxFloat*) &p[12])->Convert();
		m_nIntStyle = short(int(d) % 16);

		switch (m_nIntStyle)
		{
			case POLYGON_HATCH:
			{
				double dXScal = ((CVaxFloat*) &p[16])->Convert();
				double dYScal = ((CVaxFloat*) &p[20])->Convert();
				double dAng = ((CVaxFloat*) &p[24])->Convert();

				m_vPosXAx[2] = 0.;
				m_vPosYAx[2] = 0.;
				
				if (fabs(dXScal) > FLT_EPSILON && fabs(dYScal) > FLT_EPSILON)
				{ // Have 2 hatch lines
					m_nIntStyleId = 2;
					m_vPosXAx[0] = cos(dAng);
					m_vPosXAx[1] = sin(dAng);
					m_vPosYAx[0] = - m_vPosXAx[1];
					m_vPosYAx[1] = m_vPosXAx[0];
					m_vPosXAx *= dXScal * 1.e-3;
					m_vPosYAx *= dYScal * 1.e-3;
				}
				else if (fabs(dXScal) > FLT_EPSILON)
				{ // Vertical hatch lines
					m_nIntStyleId = 1;
					m_vPosXAx[0] = cos(dAng + HALF_PI);
					m_vPosXAx[1] = sin(dAng + HALF_PI);
					m_vPosYAx[0] = - m_vPosXAx[1];
					m_vPosYAx[1] = m_vPosXAx[0];
					m_vPosYAx *= dXScal * 1.e-3;
				}
				else
				{ // Horizontal hatch lines
					m_nIntStyleId = 1;
					m_vPosXAx[0] = cos(dAng);
					m_vPosXAx[1] = sin(dAng);
					m_vPosYAx[0] = - m_vPosXAx[1];
					m_vPosYAx[1] = m_vPosXAx[0];
					m_vPosYAx *= dYScal * 1.e-3;
				}
				break;
			}
			case POLYGON_HOLLOW:
			case POLYGON_SOLID:
//			case POLYGON_PATTERN:
//				m_vPosXAx = XDIR;
//				m_vPosYAx = YDIR;
//				break;

			default:
				m_wPts = 3;
				m_Pt = new CPnt[m_wPts];
				m_Pt[0] = ORIGIN;
				m_Pt[1] = ORIGIN + XDIR;
				m_Pt[2] = ORIGIN + YDIR;
				m_ptOrig = m_Pt[0];
				return;
		}
		m_wPts = WORD(((CVaxFloat*) &p[8])->Convert());
		
		m_Pt = new CPnt[m_wPts];
		
		int i = 36;
	
		for (WORD w = 0; w < m_wPts; w++)
		{
			m_Pt[w] = ((CVaxPnt*) &p[i])->Convert() * 1.e-3;
			i += sizeof(CVaxPnt);
		}
		m_ptOrig = m_Pt[0];
	}
	else
	{
		m_nPenColor = PENCOLOR(p[6]);
		m_nIntStyle = char(p[7]);
		m_nIntStyleId = *((short*) &p[8]);
		m_wPts = *((short*) &p[10]);
		m_ptOrig = ((CVaxPnt*) &p[12])->Convert();
		m_vPosXAx = ((CVaxVec*) &p[24])->Convert();
		m_vPosYAx = ((CVaxVec*) &p[36])->Convert();
		m_Pt = new CPnt[m_wPts];
	
		int i = 48;
	
		for (WORD w = 0; w < m_wPts; w++)
		{
			m_Pt[w] = ((CVaxPnt*) &p[i])->Convert();
			i += sizeof(CVaxPnt);
		}
	}
}

void CPrimPolygon::WriteJob(HFILE hFile, char* p) const
{
	p[3] = char((79 + m_wPts * 12) / 32);
	*((short*) &p[4]) = PRIM_POLYGON;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nIntStyle);
	*((short*) &p[8]) = short(m_nIntStyleId);
	*((short*) &p[10]) = m_wPts;
	
	((CVaxPnt*) &p[12])->Convert(m_ptOrig);
	((CVaxVec*) &p[24])->Convert(m_vPosXAx);
	((CVaxVec*) &p[36])->Convert(m_vPosYAx);

	int i = 48;
	
	for (WORD w = 0; w < m_wPts; w++)
	{
		((CVaxPnt*) &p[i])->Convert(m_Pt[w]);
		i += sizeof(CVaxPnt);
	}

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, p[3] * 32, &dwWrite, NULL);
}

void CPrimPolygon::Write(CFile& f, char* p) const
{
	p[3] = char((79 + m_wPts * 12) / 32);
	*((short*) &p[4]) = PRIM_POLYGON;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nIntStyle);
	*((short*) &p[8]) = short(m_nIntStyleId);
	*((short*) &p[10]) = m_wPts;
	
	((CVaxPnt*) &p[12])->Convert(m_ptOrig);
	((CVaxVec*) &p[24])->Convert(m_vPosXAx);
	((CVaxVec*) &p[36])->Convert(m_vPosYAx);

	int i = 48;
	
	for (WORD w = 0; w < m_wPts; w++)
	{
		((CVaxPnt*) &p[i])->Convert(m_Pt[w]);
		i += sizeof(CVaxPnt);
	}
	f.Write(p, p[3] * 32);
}

void CPrimPolyline::WriteJob(HFILE hFile, char* p) const
{
	CPrimLine line;

	for(WORD w = 0; w < m_pts.GetSize() - 1; w++)
	{
		line = CPrimLine(m_nPenColor, m_nPenStyle, m_pts[w], m_pts[w + 1]);
		line.WriteJob(hFile, p);
	}
	
	if(IsLooped())
	{
		line = CPrimLine(m_nPenColor, m_nPenStyle, m_pts[m_pts.GetSize() - 1], m_pts[0]);
		line.WriteJob(hFile, p);
	}
}

void CPrimPolyline::Write(CFile& f, char* p) const
{
	CPrimLine line;

	for(WORD w = 0; w < m_pts.GetSize() - 1; w++)
	{
		line = CPrimLine(m_nPenColor, m_nPenStyle, m_pts[w], m_pts[w + 1]);
		line.Write(f, p);
	}
	
	if(IsLooped())
	{
		line = CPrimLine(m_nPenColor, m_nPenStyle, m_pts[m_pts.GetSize() - 1], m_pts[0]);
		line.Write(f, p);
	}
}

void CPrimSegRef::WriteJob(HFILE hFile, char* p) const
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	pBlock->WriteJob(hFile, p);

	mspace.Return();
}

void CPrimSegRef::Write(CFile& f, char* p) const
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	pBlock->Write(f, p);

	mspace.Return();
}

CPrimTag::CPrimTag(char* p)
{
	m_nPenColor = PENCOLOR(p[6]);
	m_nPenStyle = PENSTYLE(p[7]);
	
	m_Pt = ((CVaxPnt*) &p[8])->Convert();
}

void CPrimTag::WriteJob(HFILE hFile, char* p) const
{
	p[3] = 1;
	*((short*) &p[4]) = PRIM_TAG;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle);

	CPnt pt = m_Pt;
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);
	
	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, 32, &dwWrite, NULL);
}

void CPrimTag::Write(CFile& f, char* p) const
{
	p[3] = 1;
	*((short*) &p[4]) = PRIM_TAG;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_nPenStyle);

	CPnt pt = m_Pt;
	mspace.Transform(pt);
	((CVaxPnt*) &p[8])->Convert(pt);
	
	f.Write(p, 32);
}

// Constructs a text primative using preclass primitive buffer.
// All text primitives from the Vax are handled as "Simplex.psf".
CPrimText::CPrimText(char* p, int iVer)
{
	m_fd.TextPrecSet(CFontDef::PREC_PEGSTROKEFONT);
	m_fd.TextFontSet("Simplex.psf");
 	
	if (iVer == 1)
	{
		m_nPenColor = PENCOLOR(p[4] & 0x000f);
		m_fd.ChrSpacSet(((CVaxFloat*) &p[36])->Convert());
		m_fd.ChrSpacSet(min(max(m_fd.ChrSpac(), 0.), 4.));

		double d = ((CVaxFloat*) &p[40])->Convert();
		
		m_fd.TextPathSet((WORD) fmod(d, 10.));
		if (m_fd.TextPath() < 0 || m_fd.TextPath() > 4) 
			m_fd.TextPathSet(CFontDef::PATH_RIGHT);
		m_fd.TextHorAlignSet((WORD) fmod(d / 10., 10.));
		if (m_fd.TextHorAlign() < 1 || m_fd.TextHorAlign() > 3) 
			m_fd.TextHorAlignSet(CFontDef::HOR_ALIGN_CENTER);
		m_fd.TextVerAlignSet((WORD) (d / 100.));
		if (m_fd.TextVerAlign() < 2 || m_fd.TextVerAlign() > 4) 
			m_fd.TextVerAlignSet(CFontDef::VER_ALIGN_MIDDLE);

		m_rs.SetOrigin(((CVaxPnt*) &p[8])->Convert() * 1.e-3);

		double dChrHgt = ((CVaxFloat*) &p[20])->Convert();
		dChrHgt = min(max(dChrHgt, .01e3), 100.e3);

		double dChrExpFac = ((CVaxFloat*) &p[24])->Convert();
		dChrExpFac = min(max(dChrExpFac, 0.), 10.);

		double dAng = ((CVaxFloat*) &p[28])->Convert();
		dAng = min(max(dAng, - TWOPI), TWOPI);

		m_rs.SetDirX(CVec(.6 * dChrHgt * dChrExpFac, 0., 0.) * 1.e-3);
		
		m_rs.SetDirY(CVec(0., dChrHgt, 0.) * 1.e-3);
		
		if (fabs(dAng) > FLT_EPSILON)
		{
			CVec vDirX(m_rs.DirX());
			vDirX.RotAboutZAx(sin(dAng), cos(dAng));
			m_rs.SetDirX(vDirX);
			CVec vDirY(m_rs.DirY());
			vDirY.RotAboutZAx(sin(dAng), cos(dAng));
			m_rs.SetDirY(vDirY);
		}
		char* pChr = strtok(&p[44], "\\");
		
		if (pChr == 0)
			m_strText = "CFileJob.PrimText error: Missing string terminator.";
		else if (strlen(pChr) > 132)
			m_strText = "CFileJob.PrimText error: Text too long.";
		else
		{
			while (*pChr != 0)
			{
				if (!isprint(*pChr)) *pChr = '.';
				pChr++;
			}
			m_strText = &p[44];
		}
	}
	else
	{
		m_nPenColor = PENSTYLE(p[6]);
		m_fd.ChrSpacSet(((CVaxFloat*) &p[10])->Convert());
		m_fd.TextPathSet(WORD(p[14]));
		m_fd.TextHorAlignSet(WORD(p[15]));
		m_fd.TextVerAlignSet(WORD(p[16]));
		m_rs.SetOrigin(((CVaxPnt*) &p[17])->Convert());
		m_rs.SetDirX(((CVaxVec*) &p[29])->Convert());
		m_rs.SetDirY(((CVaxVec*) &p[41])->Convert());

		short* pChrs = (short*) &p[53];
		char* pChr = &p[55];
	
		pChr[*pChrs] = '\0';
		m_strText = pChr;
	}
}

void CPrimText::WriteJob(HFILE hFile, char* p) const
{
	p[3] = char((86 + m_strText.GetLength()) / 32);
	*((short*) &p[4]) = PRIM_TEXT;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_fd.TextPrec());
	*((short*) &p[8]) = 0;
	((CVaxFloat*) &p[10])->Convert(m_fd.ChrSpac());
	p[14] = char(m_fd.TextPath());
	p[15] = char(m_fd.TextHorAlign());
	p[16] = char(m_fd.TextVerAlign());

	CPnt pt = m_rs.Origin();
	mspace.Transform(pt);
	((CVaxPnt*) &p[17])->Convert(pt);
	
	CVec v = m_rs.DirX();
	mspace.Transform(v);
	((CVaxVec*) &p[29])->Convert(v);
	
	v = m_rs.DirY();
	mspace.Transform(v);
	((CVaxVec*) &p[41])->Convert(v);

	short* pChrs = (short*) &p[53];
	*pChrs = short(m_strText.GetLength());

	char* pChr = &p[55];

	for (int n = 0; n < m_strText.GetLength(); n++)
		pChr[n] = m_strText[n];

	DWORD dwWrite;
	WriteFile((HANDLE)hFile, p, p[3] * 32, &dwWrite, NULL);
}

void CPrimText::Write(CFile& f, char* p) const
{
	p[3] = char((86 + m_strText.GetLength()) / 32);
	*((short*) &p[4]) = PRIM_TEXT;
	p[6] = char(m_nPenColor == PENCOLOR_BYLAYER ? mS_nLayerPenColor : m_nPenColor);
	p[7] = char(m_fd.TextPrec());
	*((short*) &p[8]) = 0;
	((CVaxFloat*) &p[10])->Convert(m_fd.ChrSpac());
	p[14] = char(m_fd.TextPath());
	p[15] = char(m_fd.TextHorAlign());
	p[16] = char(m_fd.TextVerAlign());

	CPnt pt = m_rs.Origin();
	mspace.Transform(pt);
	((CVaxPnt*) &p[17])->Convert(pt);
	
	CVec v = m_rs.DirX();
	mspace.Transform(v);
	((CVaxVec*) &p[29])->Convert(v);
	
	v = m_rs.DirY();
	mspace.Transform(v);
	((CVaxVec*) &p[41])->Convert(v);

	short* pChrs = (short*) &p[53];
	*pChrs = short(m_strText.GetLength());

	char* pChr = &p[55];

	for (int n = 0; n < m_strText.GetLength(); n++)
		pChr[n] = m_strText[n];

	f.Write(p, p[3] * 32);
}