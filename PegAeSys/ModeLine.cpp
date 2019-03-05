#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "StateInfoFrm.h"
extern CInfoModeFrm modeframe;

// this is the test one for layered windows
void CPegApp::ModeLineDisplay()
{
	if(m_iModeId == 0)
		return;
	
	m_wOpHighlighted = 0;

//	CPegView* pView = CPegView::GetActiveView();

	if(modeframe.GetDC() == 0) return;
	CDC* pDC = modeframe.GetDC();

	if (m_bViewModeInfo)	
	{
		CString strMode;
		strMode.LoadString(m_hInstance, (UINT) m_iModeId);
		
		CString strModeOp;

		CFont* pFont = pDC->SelectObject(m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(GetTextCol()); 
		COLORREF crBk = pDC->SetBkColor(RGB(128, 128, 128));
		pDC->SetBkMode(TRANSPARENT);

		TEXTMETRIC tm;		 
		pDC->GetTextMetrics(&tm);
	
		CRect rcClient;
		::GetClientRect(modeframe.GetHWND(), &rcClient);

//		// create and select a thick, black pen
//		CPen penBlack;
//		penBlack.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
//		CPen* pOldPen = pDC->SelectObject(&penBlack);
//		::Rectangle((HDC)pDC, 0, 0, 20, 20);//rcClient.right, 5);
////		::Rectangle((HDC)pDC, rcClient.left, rcClient.bottom - 16, rcClient.right, rcClient.bottom - 13);
//		pDC->SelectObject(pOldPen);

		////////////////////////////////////////////////////////////////////////
		// bitmap
		////////////////////////////////////////////////////////////////////////
		HBITMAP hBitmap = ::LoadBitmap(app.GetInstance(), MAKEINTRESOURCE(IDB_BITMAP4));
		if(hBitmap != NULL) 
		{
			CDC dcMemory;
			dcMemory.CreateCompatibleDC(0);

			HBITMAP hOldBitmap = (HBITMAP)SelectObject(dcMemory, hBitmap);
	//		pDC->BitBlt(0, 0, rcClient.right, 16, &dcMemory, 0, 0, SRCCOPY);
			pDC->StretchBlt(0, 0, rcClient.right, 2, &dcMemory, 0, 0, 2, 2, SRCCOPY);

			SelectObject(dcMemory, hOldBitmap);

			DeleteObject(hBitmap);
			DeleteObject(hOldBitmap);
		}

		int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;
		
		for (int i = 0; i < 10; i++)
		{
			AfxExtractSubString(strModeOp, strMode, i + 1, '\n');
			UINT nLen = strModeOp.GetLength();
			CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, nLen, 0);
		}

		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}
}

WORD CPegApp::ModeLineHighlightOp(WORD wOp)
{
	ModeLineUnhighlightOp(m_wOpHighlighted);
		
	m_wOpHighlighted = wOp;

	if(wOp == 0)
		return (WORD)(-1);

	if(m_bViewModeInfo)	
	{
		CPegView* pView = CPegView::GetActiveView();

		if(modeframe.GetDC() == 0) return (WORD)(-1);
		CDC *pDC = modeframe.GetDC();

		CFont* pFont = pDC->SelectObject(m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(RGB(255, 0, 0));
		COLORREF crBk = pDC->SetBkColor(RGB(128, 128, 128));
		pDC->SetBkMode(TRANSPARENT);
		
		CString strMode;
		strMode.LoadString(m_hInstance, (UINT) m_iModeId);
	
		CString strModeOp;
		
		int i = wOp - ID_OP0;
		AfxExtractSubString(strModeOp, strMode, i + 1, '\n');
		UINT nLen = strModeOp.GetLength();

		TEXTMETRIC tm;		 
		pDC->GetTextMetrics(&tm);
	
		CRect rcClient;
		pView->GetClientRect(&rcClient);
		
		int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;

		CRect rc(i * iMaxChrs * tm.tmAveCharWidth, 2, (i + 1) * iMaxChrs * tm.tmAveCharWidth, 15);
		pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, nLen, 0);

		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}

	return (wOp);  
}

void CPegApp::ModeLineUnhighlightOp(WORD& wOp)
{
	m_wOpHighlighted = 0;

	if(wOp == 0)
		return;

	if(m_bViewModeInfo)	
	{
//		CPegView* pView = CPegView::GetActiveView();

		if(modeframe.GetDC() == 0) return;
		CDC *pDC = modeframe.GetDC();
		
		CFont* pFont = pDC->SelectObject(m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(GetTextCol()); 
		COLORREF crBk = pDC->SetBkColor(RGB(128, 128, 128));
		pDC->SetBkMode(TRANSPARENT);

		TEXTMETRIC tm;		 
		pDC->GetTextMetrics(&tm);
	
		CRect rcClient;
		::GetClientRect(modeframe.GetHWND(), &rcClient);
		
		CString strMode;
		strMode.LoadString(m_hInstance, (UINT) m_iModeId);
	
		CString strModeOp;
		
		int i = wOp - ID_OP0;
		AfxExtractSubString(strModeOp, strMode, i + 1, '\n');
		UINT nLen = strModeOp.GetLength();

		int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;

		CRect rc(i * iMaxChrs * tm.tmAveCharWidth, 2, (i + 1) * iMaxChrs * tm.tmAveCharWidth, 15);
		pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, nLen, 0);
	
		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}

	wOp = 0;
}