#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

//#include "dde.h"
//#include "ddeGItms.h"
#include "Hatch.h"

void DlgProcTabSixHatchInit(HWND);
void DlgProcHatchPaint(HWND hDlg, RECT dimen);

static short index = 1;
static double dAngle = 0.0;
static double dxOffset = 0.1;
static double dyOffset = 0.1;

BOOL CALLBACK DlgProcOptionsTabHatch(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	HWND hHatch;

	RECT dimen;
	dimen.top = 26;
	dimen.bottom = dimen.top + 70;
	dimen.left = 14;
	dimen.right = dimen.left + 140;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			SetDlgItemInt(hDlg, IDC_FIL_AREA_HAT_ID, pstate.PolygonIntStyleId(), FALSE);
			DlgBoxSetItemDouble(hDlg, IDC_FIL_AREA_HAT_X_SCAL, hatch::dXAxRefVecScal);
			DlgBoxSetItemDouble(hDlg, IDC_FIL_AREA_HAT_Y_SCAL, hatch::dYAxRefVecScal);
			DlgBoxSetItemDouble(hDlg, IDC_FIL_AREA_HAT_ROT_ANG, hatch::dOffAng / RADIAN);

			index = pstate.PolygonIntStyleId();
			dAngle = hatch::dOffAng;
			dxOffset = hatch::dXAxRefVecScal;
			dyOffset = hatch::dYAxRefVecScal;
			
			if(pstate.GetPolygonIntStyle() == POLYGON_HOLLOW)
				hHatch = GetDlgItem(hDlg, IDC_FILL_HOLLOW1);
			else if(pstate.GetPolygonIntStyle() == POLYGON_SOLID)
				hHatch = GetDlgItem(hDlg, IDC_FILL_SOLID1);
			//else if(pstate.GetPolygonIntStyle() == POLYGON_PATTERN)
			//	hHatch = GetDlgItem(hDlg, IDC_FILL_PATTERN1);
			else// if(pstate.GetPolygonIntStyle() == POLYGON_HATCH)
				hHatch = GetDlgItem(hDlg, IDC_FILL_HATCH1);

			SendNotifyMessage(hHatch, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);

			DlgProcTabSixHatchInit(hDlg);

			InvalidateRect(hDlg, &dimen, TRUE);
			SendMessage(hDlg, WM_PAINT, 0, 0);

			return TRUE;
		}
		case WM_PAINT:
		{
			DlgProcHatchPaint(hDlg, dimen);
			return (TRUE);
		}
		case WM_DIALOG_FOCUS:
		{
			switch(pDoc->GetOptionsFocus())
			{
				case IDC_LIST2:
					SetFocus(::GetDlgItem(hDlg, IDC_LIST2));
					break;
				default:
					SetFocus(GetWindow(hDlg,GW_CHILD));
					break;
			}
			return TRUE;
		}	
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
				case IDC_LIST2:
				{
					if(HIWORD(wParam) == CBN_SELCHANGE)
					{
						index = (short)::SendDlgItemMessage(hDlg, IDC_LIST2, CB_GETCURSEL, 0, 0) + 1;
						CheckRadioButton(hDlg, IDC_FILL_HOLLOW1, IDC_FILL_HATCH1, IDC_FILL_HATCH1);

						InvalidateRect(hDlg, &dimen, TRUE);
						DlgProcHatchPaint(hDlg, dimen);
					}
					else if(HIWORD(wParam) == CBN_SETFOCUS)
					{
//						CheckRadioButton(hDlg, IDC_FILL_HOLLOW1, IDC_FILL_HATCH1, IDC_FILL_HATCH1);
					}
					return (TRUE);
				}
				case IDC_FIL_AREA_HAT_X_SCAL:
				{
					if(HIWORD(wParam) == EN_CHANGE)
					{
						dxOffset = Max(.01, DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_X_SCAL));
						InvalidateRect(hDlg, &dimen, TRUE);
						SendMessage(hDlg, WM_PAINT, 0, 0);
					}
					return (TRUE);
				}
				case IDC_FIL_AREA_HAT_Y_SCAL:
				{
					if(HIWORD(wParam) == EN_CHANGE)
					{
						dyOffset = Max(.01, DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_Y_SCAL));
						InvalidateRect(hDlg, &dimen, TRUE);
						SendMessage(hDlg, WM_PAINT, 0, 0);
					}
					return (TRUE);
				}
				case IDC_FIL_AREA_HAT_ROT_ANG:
				{
					if(HIWORD(wParam) == EN_CHANGE)
					{
						dAngle = Arc(DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_ROT_ANG));
						InvalidateRect(hDlg, &dimen, TRUE);
						SendMessage(hDlg, WM_PAINT, 0, 0);
					}
					return (TRUE);
				}
			}

			break;
		}
		case WM_DIALOG_OK:
		{
			if(index > 0)
				pstate.SetPolygonIntStyleId(index);

			BOOL bSuccess = false;
			UINT nIndex2 = ::GetDlgItemInt(hDlg, IDC_LIST2, &bSuccess, FALSE);
			if(bSuccess)
			{
				// set hatch index and turn it on
				pstate.SetPolygonIntStyleId((short)nIndex2);
				CheckRadioButton(hDlg, IDC_FILL_HOLLOW1, IDC_FILL_HATCH1, IDC_FILL_HATCH1);
			}

			hatch::dXAxRefVecScal = Max(.01, DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_X_SCAL));
			hatch::dYAxRefVecScal = Max(.01, DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_Y_SCAL));
			hatch::dOffAng = Arc(DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_ROT_ANG));

			if(::IsDlgButtonChecked(hDlg, IDC_FILL_HOLLOW1))
				pstate.SetPolygonIntStyle(POLYGON_HOLLOW);

			if(::IsDlgButtonChecked(hDlg, IDC_FILL_SOLID1))
				pstate.SetPolygonIntStyle(POLYGON_SOLID);

			//if(::IsDlgButtonChecked(hDlg, IDC_FILL_PATTERN1))
			//	pstate.SetPolygonIntStyle(POLYGON_PATTERN);

			if(::IsDlgButtonChecked(hDlg, IDC_FILL_HATCH1))
				pstate.SetPolygonIntStyle(POLYGON_HATCH);

			return TRUE;
			break;
		}
	}
	return FALSE; 
}

void DlgProcTabSixHatchInit(HWND hDlg)
{
	HWND hCombo = ::GetDlgItem(hDlg, IDC_LIST2);

	char szBuf[16];

	for(int i = 0; i < hatch::iTableSize; i++)
	{
		CString curText = _itoa(i, szBuf, 10);
		curText += ". " + hatch::HatchText[i];
		::SendMessage(hCombo, CB_ADDSTRING, 0,
			(LPARAM) curText.GetBuffer());
	}

	::SendDlgItemMessage(hDlg, IDC_LIST2, CB_DELETESTRING, 0, 0);
	::SendDlgItemMessage(hDlg, IDC_LIST2, CB_SETCURSEL, pstate.GetPolygonIntStyleId() - 1, 0L);
}

void DlgProcHatchPaint(HWND hDlg, RECT dimen)
{
	COLORREF old;
	PAINTSTRUCT ps;
	CDC dc;
	dc.Attach(::BeginPaint(hDlg, &ps));

	CDC m_dcMem;
	m_dcMem.CreateCompatibleDC(NULL);

	dc.SetTextColor(RGB(0, 0, 0));
	old = dc.SetBkColor(RGB(255, 255, 255));		
	dc.FillSolidRect(&dimen, crHotCols[app.BackgroundColorGet()]);

	int OldPState = pstate.Save();

	/////////////////////////////////////////////////////////////////////////////////////
	pstate.SetPolygonIntStyle(3); // hollow, solid, pattern, hatch 
	pstate.SetPolygonIntStyleId(index);
	/////////////////////////////////////////////////////////////////////////////////////

	pstate.SetPen(&dc, pstate.PenColor(), 1);
				
	viewport::PushActive();
	viewport::Set(dimen.right + dimen.left, dimen.bottom + dimen.top);

	double dUExt = double(dimen.right + dimen.left) / double(dc.GetDeviceCaps(LOGPIXELSX)); 
	double dVExt = double(dimen.bottom + dimen.top) / double(dc.GetDeviceCaps(LOGPIXELSY)); 
	
	view::PushActive();
	view::Initialize();
	view::SetWnd(0., 0., dUExt, dVExt);

	view::SetTarget(ORIGIN);
	view::SetEye(ZDIR);

	//////////////////////////////////////////////////////////////////////////
	double dTop = 1.00;		// origin is bottom left
	double dBot = .27;		// 0	1
	double dLef = .15;		// 3	2
	double dRig = 1.61;

	CPnt pt[4];
	pt[0] = CPnt(dLef, dTop, 0);
	pt[1] = CPnt(dRig, dTop, 0);
	pt[2] = CPnt(dRig, dBot, 0);
	pt[3] = CPnt(dLef, dBot, 0);
	WORD wPts = 4;

	// Determine plane in which fill area lies
	CVec vXAx(pt[0], pt[1]);
	CVec vYAx(pt[0], pt[2]);
	CVec vPlnNorm(vXAx ^ vYAx);
	vPlnNorm.Normalize();

	if (vPlnNorm[2] < 0)
		vPlnNorm = -vPlnNorm;

	vXAx.Normalize();
	vXAx.RotAboutArbAx(vPlnNorm, dAngle);
	vYAx = vXAx;
	vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
	vXAx *= dxOffset;
	vYAx *= dyOffset;

	// Project reference origin to plane
	CSeg* pSeg = new CSeg(new CPrimPolygon(wPts, pt[wPts], vXAx, vYAx, pt));
	pSeg->Display(&dc, false);
	//////////////////////////////////////////////////////////////////////////
	
	view::PopActive();
	viewport::PopActive();

	pstate.Restore(&dc, OldPState);
	///////////////////////////////////////////////////////////////////////////////////////////
	
	dc.SetBkColor(old);
	dc.Detach();

	::EndPaint(hDlg, &ps);
}