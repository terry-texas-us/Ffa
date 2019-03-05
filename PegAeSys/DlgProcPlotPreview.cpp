#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "FileJob2.h"
#include "Job2State.h"

#include "Print.h"

void DlgProcPlotPreviewInit(HWND hDlg);
void DlgProcPlotPreviewLoad(HWND hDlg);
void DlgProcPlotPreviewInclude(HWND hDlg);
void DlgProcPlotPreviewSave(HWND hDlg);
void DlgProcPlotPreviewSaveAs(HWND hDlg);

//UINT CALLBACK OFNHookProcTracingInclude(HWND, UINT, WPARAM, LPARAM);

CJob2State job2state;

LRESULT GetCurSel(HWND hDlg, int iListId, CString& strTitle);

#include "StateInfoFrm.h"
#include "DlgProcPlotPreview.h"

//////////////////////////////////////////////////////////////////////////////////////////

CPlotFrm plotframe(0, 100, 200, 300);
CPlotFrm::CPlotFrm(int x, int y, int width, int height) { SetInfo(x, y, width, height); }
void CPlotFrm::OnPaint(HWND) {}

//////////////////////////////////////////////////////////////////////////////////////////

void DlgProcPlotPreviewBorderEnable(HWND hDlg, bool bValue);

BOOL CALLBACK DlgProcPlotPreview(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
	CPegView *pView = CPegView::GetActiveView();

	UINT bState;
	bool bValue;

	static bool bUseBorder = true;

	switch(nMsg)
	{
		case WM_INITDIALOG:
		{
			DlgProcPlotPreviewInit(hDlg);
			return (TRUE);
		}
		case WM_SIZE:
		{
			RECT cRect;
			::GetClientRect(app.GetSafeHwnd(), &cRect);

			POINT point;
			point.x = cRect.right - 181;
			point.y = cRect.bottom - 318;

			SetWindowPos(hDlg, HWND_TOPMOST, point.x, point.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

			return (TRUE);
		}
		case WM_PLOT_UPDATE:
		{
			::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMIN, job2state.m_rectClip.left);
			::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMAX, job2state.m_rectClip.right);
			::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMIN, job2state.m_rectClip.bottom);
			::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMAX, job2state.m_rectClip.top);

			::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_X, job2state.m_pntOrigin[0]);
			::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_Y, job2state.m_pntOrigin[1]);

			return (TRUE);
		}
		case WM_COMMAND:
		{
			switch(LOWORD(wParam)) 
			{
				//case ID_LOAD_JB2:
				//{
				//	DlgProcPlotPreviewLoad(hDlg);
				//	return (TRUE);
				//}
				//case ID_SAVE_JB2:
				//{
				//	if(job2state.m_strFilePath != "Filename - <empty>")
				//		DlgProcPlotPreviewSave(hDlg);
				//	else
				//		DlgProcPlotPreviewSaveAs(hDlg);

				//	return (TRUE);
				//}
				//case ID_SAVEAS_JB2:
				//{
				//	DlgProcPlotPreviewSaveAs(hDlg);
				//	SetWindowText(::GetDlgItem(hDlg, IDC_JB2_FILE), job2state.m_strFilePath);
				//	return (TRUE);
				//}
				//case ID_NEWJB2:
				//{
				//	job2state.m_strFilePath = "Filename - <empty>";
				//	SetWindowText(::GetDlgItem(hDlg, IDC_JB2_FILE), job2state.m_strFilePath);

				//	pDoc->BorderLayersRemoveAll();
				//	SendMessage(::GetDlgItem(hDlg, IDC_LIST1), LB_RESETCONTENT, 0, 0);

				//	job2state.m_rectClip.left = 5;
				//	job2state.m_rectClip.right = 40;
				//	job2state.m_rectClip.bottom = 5;
				//	job2state.m_rectClip.top = 30;

				//	job2state.m_pntOrigin[0] = 0;
				//	job2state.m_pntOrigin[1] = 0;

				//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMIN, job2state.m_rectClip.left);
				//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMAX, job2state.m_rectClip.right);
				//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMIN, job2state.m_rectClip.bottom);
				//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMAX, job2state.m_rectClip.top);

				//	::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_X, job2state.m_pntOrigin[0]);
				//	::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_Y, job2state.m_pntOrigin[1]);
				//	return (TRUE);
				//}
				//case ID_ADD_BORDER:
				//{
				//	DlgProcPlotPreviewInclude(hDlg);
				//	return (TRUE);
				//}
				//case ID_REMOVE_BORDER:
				//{
				//	CString strName;
				//	LRESULT lrCurSel = GetCurSel(hDlg, IDC_LIST1, strName);
				//	if (lrCurSel != LB_ERR)
				//	{
				//		pDoc->BorderLayersRemove(strName);
				//		::SendDlgItemMessage(hDlg, IDC_LIST1, LB_DELETESTRING, (WPARAM) lrCurSel, 0L);
				//	}

				//	return (TRUE);
				//}
				//case ID_SET_EXTENT:
				//{
				//	CPegView::GetActiveView()->SetClipMode(CPegView::MODE_PLOT_CLIPSET);
				//	return (TRUE);
				//}
				//case ID_SET_ORIGIN:
				//{
				//	CPegView::GetActiveView()->SetClipMode(CPegView::MODE_PLOT_ORIGINSET);
				//	return (TRUE);
				//}
				case IDC_PP_PENWIDTH:
				{
					bState = ::IsDlgButtonChecked(hDlg, IDC_PP_PENWIDTH);
					bValue = (bState == BST_CHECKED) ? true : false;
					printStates.SetUsePenWidths(bValue);
					pView->OnViewPenWidths();
					::EnableWindow(::GetDlgItem(hDlg, IDC_WIDTH_FILE), bValue);

					// get translate filename
					LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_GETCURSEL, 0, 0);
					if(lResult != CB_ERR)
					{
						char szBuf2[MAX_PATH];
						::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
						DlgProcPlotPreviewUsePenWidths(szBuf2);

						if(HIWORD(wParam) == BN_CLICKED)
							::SetFocus(app.GetSafeHwnd());
					}

					return (TRUE);
				}
				case IDC_WIDTH_FILE:
				{
					// get translate filename
					LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_GETCURSEL, 0, 0);
					if(lResult != CB_ERR)
					{
						char szBuf2[MAX_PATH];
						::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
						printStates.SetPenWidthsFile(szBuf2);
						DlgProcPlotPreviewUsePenWidths(szBuf2);
					}

					if((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_SELENDCANCEL))
						::SetFocus(app.GetSafeHwnd());

					return (TRUE);
				}
				case IDC_DISP_LAYER_TEXT:
				{
					bState = ::IsDlgButtonChecked(hDlg, IDC_DISP_LAYER_TEXT);
					bValue = (bState == BST_CHECKED) ? true : false;
					printStates.SetDispLayer(bValue);
					pView->InvalidateRect(NULL, TRUE);

					if(HIWORD(wParam) == BN_CLICKED)
						::SetFocus(app.GetSafeHwnd());

					return (TRUE);
				}
				case IDC_BCK_IMAGE:
				{
					//bState = ::IsDlgButtonChecked(hDlg, IDC_BCK_IMAGE);
					//bValue = (bState == BST_CHECKED) ? true : false;
					//printStates.SetDispBkgImage(bValue);
					//pView->InvalidateRect(NULL, TRUE);

					//// toggle background layer
					//CLayer *pLayer = CPegDoc::GetDoc()->BitmapLayerGet();
					//if(bValue)
					//	pLayer->SetStateCold();
					//else
					//	pLayer->SetStateOff();

					//if(HIWORD(wParam) == BN_CLICKED)
					//	::SetFocus(app.GetSafeHwnd());

					//////////////

					bState = ::IsDlgButtonChecked(hDlg, IDC_BCK_IMAGE);
					bValue = (bState == BST_CHECKED) ? true : false;
					printStates.SetDispBkgImage(bValue);
					pView->InvalidateRect(NULL, TRUE);

					// toggle background layer
					CPegView::GetActiveView()->OnViewBackgroundImage();

					if(HIWORD(wParam) == BN_CLICKED)
						::SetFocus(app.GetSafeHwnd());

					return (TRUE);
				}
				case IDC_PRINT_MARKERS:
				{
					bState = ::IsDlgButtonChecked(hDlg, IDC_PRINT_MARKERS);
					bValue = (bState == BST_CHECKED) ? true : false;
					printStates.SetPrintMarkers(bValue);
					pView->InvalidateRect(NULL, TRUE);

					if(HIWORD(wParam) == BN_CLICKED)
						::SetFocus(app.GetSafeHwnd());

					return (TRUE);
				}
				case IDC_PP_TRANSLATEPENS:
				{
					bState = ::IsDlgButtonChecked(hDlg, IDC_PP_TRANSLATEPENS);
					bValue = (bState == BST_CHECKED) ? true : false;
					printStates.SetTranslatePens(bValue);
					::EnableWindow(::GetDlgItem(hDlg, IDC_PP_TRANSLATELIST), bValue);

					// get translate filename
					LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_GETCURSEL, 0, 0);
					if(lResult != CB_ERR)
					{
						char szBuf2[MAX_PATH];
						::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
						DlgProcPlotPreviewTranslatePens(szBuf2);

						if(HIWORD(wParam) == BN_CLICKED)
							::SetFocus(app.GetSafeHwnd());
					}

					return (TRUE);
				}
				case IDC_PP_TRANSLATELIST:
				{
					// get translate filename
					LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_GETCURSEL, 0, 0);
					if(lResult != CB_ERR)
					{
						char szBuf2[MAX_PATH];
						::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
						printStates.SetTranslateFile(szBuf2);

						DlgProcPlotPreviewTranslatePens(szBuf2);

						if((HIWORD(wParam) == CBN_SELENDOK) || (HIWORD(wParam) == CBN_SELENDCANCEL))
							::SetFocus(app.GetSafeHwnd());
					}
					return (TRUE);
				}
				//case IDC_USE_BORDER:
				//{
				//	bUseBorder = !bUseBorder;
				//	DlgProcPlotPreviewBorderEnable(hDlg, bUseBorder);
				//	pView->OnClippingRegion();

				//	return (TRUE);
				//}
				//case IDC_PLOT_CLIP:
				//{
				//	printStates.SetDispClipRect(!printStates.GetDispClipRect());
				//	pView->InvalidateRect(NULL, TRUE);
				//	return (TRUE);
				//}
				case IDC_SHEET_FULL:
				case IDC_SHEET_HALF:
				case IDC_SHEET_QUARTER:
				{
					switch(LOWORD(wParam))
					{
						case IDC_SHEET_FULL:	printStates.SetSheetSize(FULL_SHEET);		break;
						case IDC_SHEET_HALF:	printStates.SetSheetSize(HALF_SHEET);		break;
						case IDC_SHEET_QUARTER:	printStates.SetSheetSize(QUARTER_SHEET);	break;
					}

					if(HIWORD(wParam) == BN_CLICKED)
						::SetFocus(app.GetSafeHwnd());

					return (TRUE);
				}
				case ID_PLOTFILE:
				{
					if(printStates.GetCurrentState() < 2)		// print mode
					{
						print::FilePrintCurrentView();
					}
					else										// plot mode
					{
						if(::IsDlgButtonChecked(hDlg, IDC_SHEET_FULL))
							print::FilePlot(ID_FILE_PLOT_FULL);
						else if(::IsDlgButtonChecked(hDlg, IDC_SHEET_HALF))
							print::FilePlot(ID_FILE_PLOT_HALF);
						else // quarter
							print::FilePlot(ID_FILE_PLOT_QUARTER);
					}

					if(HIWORD(wParam) == BN_CLICKED)
						::SetFocus(app.GetSafeHwnd());

					return (TRUE);
				}
			}
			break;
		}
	}
	return (FALSE); 
}

//void DlgProcPrintPreviewOk(HWND hDlg)
//{
	//UINT bState;
	//bool bValue;

	//bState = ::IsDlgButtonChecked(hDlg, IDC_PP_PENWIDTH);
	//bValue = (bState == BST_CHECKED) ? true : false;
	//printState.SetUsePenWidths(bValue);

	//bState = ::IsDlgButtonChecked(hDlg, IDC_DISP_LAYER_TEXT);
	//bValue = (bState == BST_CHECKED) ? true : false;
	//printState.SetDispLayer(bValue);

	//bState = ::IsDlgButtonChecked(hDlg, IDC_PP_TRANSLATEPENS);
	//bValue = (bState == BST_CHECKED) ? true : false;
	//printState.SetTranslatePens(bValue);

	//// get translate filename
	//LRESULT lResult = ::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_GETCURSEL, 0, 0);
	//if(lResult != CB_ERR)
	//{
	//	char szBuf2[MAX_PATH];

	//	::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_GETLBTEXT, lResult, (LPARAM)szBuf2);
	//	printState.SetTranslateFile(szBuf2);
	//}

	//// margin edit boxes
	//dMarginX = DlgBoxGetItemDouble(hDlg, IDC_MARGIN_X);
	//dMarginY = DlgBoxGetItemDouble(hDlg, IDC_MARGIN_Y);
//}

void DlgProcPlotPreviewInit(HWND hDlg)
{
	// set correct print/plot settings
	if(printStates.GetCurrentState() < 2)		// print mode
	{
		::SetWindowText(::GetDlgItem(hDlg, ID_PLOTFILE), "Print Current View");
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET), false);
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET_FULL), false);
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET_HALF), false);
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET_QUARTER), false);
	}
	else	// plot mode
	{
		::SetWindowText(::GetDlgItem(hDlg, ID_PLOTFILE), "Plot");
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET), true);
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET_FULL), true);
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET_HALF), true);
		EnableWindow(::GetDlgItem(hDlg, IDC_SHEET_QUARTER), true);
	}

//	DlgProcPlotPreviewBorderEnable(hDlg, false);

	// get check button states and set accordingly
	UINT bCheck = (printStates.GetDispLayer() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_DISP_LAYER_TEXT, bCheck);

	bCheck = (printStates.GetDispBkgImage() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_BCK_IMAGE, bCheck);

	bCheck = (printStates.GetPrintMarkers() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_PRINT_MARKERS, bCheck);

	bCheck = (printStates.GetUsePenWidths() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_PP_PENWIDTH, bCheck);

	bCheck = (printStates.GetTranslatePens() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_PP_TRANSLATEPENS, bCheck);

	bCheck = (printStates.GetDispClipRect() == true) ? BST_CHECKED : BST_UNCHECKED;
	::CheckDlgButton(hDlg, IDC_PLOT_CLIP, bCheck);	

	// enable translate list if necessary
	if(::IsDlgButtonChecked(hDlg, IDC_PP_TRANSLATEPENS))
		::EnableWindow(::GetDlgItem(hDlg, IDC_PP_TRANSLATELIST), true);
	else
		::EnableWindow(::GetDlgItem(hDlg, IDC_PP_TRANSLATELIST), false);

	CString s3 = app.GetAppPath();
	s3 += "Pens\\Colors\\*.txt";

	char strSearch[12] = "_readme.txt";

	int iResult = DlgDirListComboBox(hDlg, s3.GetBuffer(), IDC_PP_TRANSLATELIST, 0, DDL_READWRITE);
	if(iResult == 0)
		MessageBox(hDlg, "Pen Color files must be located in \"(ApplicationDirectory)\\Pens\\Colors\\\"", 0, 0);

	int lResult = ::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_FINDSTRING, (WPARAM) -1, (LPARAM)&strSearch); 
	if(lResult != CB_ERR)
		::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_DELETESTRING, (WPARAM)lResult, 0);

	// select pen color translation file
	CString strSearch2 = printStates.GetTranslateFile();
	lResult = ::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch2.GetBuffer()); 
	if(lResult == CB_ERR)
		::SendDlgItemMessage(hDlg, IDC_PP_TRANSLATELIST, CB_SETCURSEL, 0, 0);

//	DlgProcPlotPreviewTranslatePens(strSearch2);

	// enable pen widths list if necessary
	if(::IsDlgButtonChecked(hDlg, IDC_PP_PENWIDTH))
		::EnableWindow(::GetDlgItem(hDlg, IDC_WIDTH_FILE), true);
	else
		::EnableWindow(::GetDlgItem(hDlg, IDC_WIDTH_FILE), false);

	// fill pen widths list
	{
		CString s3 = app.GetAppPath();
		s3 += "Pens\\Widths\\*.txt";

		char strSearch[12] = "_readme.txt";

		int iResult = DlgDirListComboBox(hDlg, s3.GetBuffer(), IDC_WIDTH_FILE, 0, DDL_READWRITE);
		if(iResult == 0)
			MessageBox(hDlg, "Pen Color files must be located in \"(ApplicationDirectory)\\Pens\\Widths\\\"", 0, 0);

		int lResult = ::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_FINDSTRING, (WPARAM) -1, (LPARAM)&strSearch); 
		if(lResult != CB_ERR)
			::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_DELETESTRING, (WPARAM)lResult, 0);
	}

	// select pen widths file
	CString strSearch3 = printStates.GetPenWidthsFile();
	lResult = ::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_SELECTSTRING, (WPARAM) -1, (LPARAM)(LPSTR)strSearch3.GetBuffer()); 
	if(lResult == CB_ERR)
		::SendDlgItemMessage(hDlg, IDC_WIDTH_FILE, CB_SETCURSEL, 0, 0);

	// fill margin box
	DlgBoxSetItemDouble(hDlg, IDC_MARGIN_X, printStates.GetMarginX());
	DlgBoxSetItemDouble(hDlg, IDC_MARGIN_Y, printStates.GetMarginY());

	// select full sheet size
	switch(printStates.GetSheetSize())
	{
		case FULL_SHEET:	
			::CheckRadioButton(hDlg, IDC_SHEET_FULL, IDC_SHEET_QUARTER, IDC_SHEET_FULL);
			break;
		case HALF_SHEET:
			::CheckRadioButton(hDlg, IDC_SHEET_FULL, IDC_SHEET_QUARTER, IDC_SHEET_HALF);
			break;
		case QUARTER_SHEET:
			::CheckRadioButton(hDlg, IDC_SHEET_FULL, IDC_SHEET_QUARTER, IDC_SHEET_QUARTER);
			break;
	}

	//// Display background image
	//UINT bState = ::IsDlgButtonChecked(hDlg, IDC_BCK_IMAGE);
	//bool bValue = (bState == BST_CHECKED) ? true : false;
	//printStates.SetDispBkgImage(bValue);
	//CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);

	//// toggle background layer
	//CLayer *pLayer = CPegDoc::GetDoc()->BitmapLayerGet();
	//if(bValue)
	//	pLayer->SetStateCold();
	//else
	//	pLayer->SetStateOff();

//	// check use border manager
//	::CheckDlgButton(hDlg, IDC_USE_BORDER, BST_CHECKED);

	////////////////////////////////////////////////////////////////////////////////////////////

	// jb2 stuff
//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMIN, job2state.m_rectClip.left);
//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMAX, job2state.m_rectClip.right);
//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMIN, job2state.m_rectClip.bottom);
//	::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMAX, job2state.m_rectClip.top);

//	::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_X, job2state.m_pntOrigin[0]);
//	::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_Y, job2state.m_pntOrigin[1]);
}

void DlgProcPlotPreviewUsePenWidths(CString strFile)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CString s3 = app.GetAppPath();
	s3 += "Pens\\Widths\\";
	s3 += strFile;

	app.PenWidthsLoad(s3);
	pDoc->UpdateAllViews(NULL, 0L, NULL);
}

void DlgProcPlotPreviewTranslatePens(CString strFile)
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	CString s3 = app.GetAppPath();
	s3 += "Pens\\Colors\\";

	if(printStates.GetTranslatePens())
	{
		s3 += strFile;
		app.PenColorsLoad(s3);
		app.SetBackGround(crHotCols[0]);
	}
	else
	{
		s3 += printStates.GetOldTranslate();
		app.PenColorsLoad(s3);
		app.SetBackGround(crHotCols[app.BackgroundColorGet()]);
	}

	pDoc->UpdateAllViews(NULL, 0L, NULL);
}

void DlgProcPlotPreviewEnable()
{
//	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	PENCOLOR penColor = pstate.PenColor();
	PENSTYLE penStyle = pstate.PenStyle();

	// pen widths
	printStates.SetOldPenWidths(pView->IsPenWidthsOn());

	if(printStates.GetUsePenWidths())
	{
		app.PenWidthsLoad(app.GetAppPath() + "\\Pens\\Widths\\" + printStates.GetPenWidthsFile());
		pView->PenWidthsEnable(true);
	}
	else
		pView->PenWidthsEnable(false);

	// translate pens
	if(printStates.GetTranslatePens())
		DlgProcPlotPreviewTranslatePens(printStates.GetTranslateFile());

	////////////////////////////////////////////

	// Display background image
	bool bValue = printStates.GetDispBkgImage();
	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);

	// toggle background layer
	if(bValue)
		pView->ShowBackgroundImage(true);
	else
		pView->ShowBackgroundImage(false);

	//pstate.SetPen(pView->GetDC(), penColor, penStyle);
	//pstate.SetPenColor(penColor);
	//pstate.SetPenStyle(penStyle);
}

void DlgProcPlotPreviewDisable()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();
	CPegView *pView = CPegView::GetActiveView();

	// pen widths
	if(printStates.GetOldPenWidths())
	{
		app.PenWidthsLoad(app.GetAppPath() + "\\Pens\\Widths\\" + pView->PenWdithsGetFile());
		pView->PenWidthsEnable(true);
	}
	else
		pView->PenWidthsEnable(false);

	// translate pens
	if(printStates.GetTranslatePens())
	{
		CString s3 = app.GetAppPath();
		s3 += "Pens\\Colors\\";
		s3 += printStates.GetOldTranslate();

		app.PenColorsLoad(s3);
		app.SetBackGround(crHotCols[app.BackgroundColorGet()]);
		pDoc->UpdateAllViews(NULL, 0L, NULL);
	}

	// Display background image
//	bool bDisplayBitmapLayer = pView->IsBackgroundImageOn();
//	CPegView::GetActiveView()->InvalidateRect(NULL, TRUE);
//
//	// toggle background layer
////	CLayer *pLayer = CPegDoc::GetDoc()->BitmapLayerGet();
//	if(!bDisplayBitmapLayer)
//		pView->ShowBackgroundImage(false);
//		//pLayer->SetStateCold();
//	else
		pView->ShowBackgroundImage(true);
		//pLayer->SetStateOff();

}

void DlgProcPlotPreviewLoad(HWND )//hDlg)
{
//	CPegDoc* pDoc = CPegDoc::GetDoc();
//
//	static DWORD nFilterIndex = 1;

////	of.lpstrInitialDir = strWDirectory;
//
////	char szFilter[55];
////	SecureZeroMemory(szFilter, 55);
////	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));
//	char *szFilter = "Job Border Files (.jb2)\000*.jb2";
//
//	OPENFILENAME of;
//	::ZeroMemory(&of, sizeof(OPENFILENAME));
//	of.lStructSize = sizeof(OPENFILENAME);
//	of.hwndOwner = 0;
//	of.hInstance = app.GetInstance();
//	of.lpstrFilter = szFilter;
//	of.nFilterIndex = nFilterIndex;
//	of.lpstrFile = new char[MAX_PATH]; 
//	of.lpstrFile[0] = 0;
//	of.nMaxFile = MAX_PATH;
//	of.lpstrTitle = "Include Border File";
////	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
//	of.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
////	of.lpstrDefExt = "tra";
////	of.lpfnHook = OFNHookProcTracingInclude;
////	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_INCLUDE);
//	
//	if(GetOpenFileName(&of))
//	{
//		CString strName = of.lpstrFile;//job2state.m_strFilePath = of.lpstrFile;
//		CString strPath = strName.Left(of.nFileOffset);
//
//		job2state.m_strFilePath = strName = strName.Mid(of.nFileOffset);
//
//		CFileJob2 fj2;
//		bool bFileOpen = fj2.OpenForRead(strName);//"temp.jb2");
//		if(bFileOpen)
//		{
//			fj2.ReadHeader();
//			fj2.ReadInfo();
//			fj2.Close();
//		}
//
//		// update everything
//		SetWindowText(::GetDlgItem(hDlg, IDC_JB2_FILE), strName);
//
//		::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMIN, job2state.m_rectClip.left);
//		::DlgBoxSetItemDouble(hDlg, IDC_CLIP_XMAX, job2state.m_rectClip.right);
//		::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMIN, job2state.m_rectClip.bottom);
//		::DlgBoxSetItemDouble(hDlg, IDC_CLIP_YMAX, job2state.m_rectClip.top);
//
//		::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_X, job2state.m_pntOrigin[0]);
//		::DlgBoxSetItemDouble(hDlg, IDC_ORIGIN_Y, job2state.m_pntOrigin[1]);
//
//		for(int i = 0; i < pDoc->BorderLayersGetSize(); i++)
//		{
//			CLayer *pLayer = pDoc->BorderLayersGetAt(i);
//			SendMessage(GetDlgItem(hDlg, $IDC_LIST1), (UINT)LB_ADDSTRING, 0, (LPARAM)pLayer->GetName().GetBuffer());
//		}
//
//	}
//	delete [] of.lpstrFile;
}

void DlgProcPlotPreviewInclude(HWND )//hDlg)
{
//	CPegDoc* pDoc = CPegDoc::GetDoc();
//
//	static DWORD nFilterIndex = 1;

////	of.lpstrInitialDir = strWDirectory;
//
////	char szFilter[55];
////	SecureZeroMemory(szFilter, 55);
////	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));
//	char *szFilter = "Job Files (.jb1)\000*.jb1";
//
//	OPENFILENAME of;
//	::ZeroMemory(&of, sizeof(OPENFILENAME));
//	of.lStructSize = sizeof(OPENFILENAME);
//	of.hwndOwner = 0;
//	of.hInstance = app.GetInstance();
//	of.lpstrFilter = szFilter;
//	of.nFilterIndex = nFilterIndex;
//	of.lpstrFile = new char[MAX_PATH]; 
//	of.lpstrFile[0] = 0;
//	of.nMaxFile = MAX_PATH;
//	of.lpstrTitle = "Include Border File";
////	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
//	of.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
////	of.lpstrDefExt = "tra";
////	of.lpfnHook = OFNHookProcTracingInclude;
////	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_INCLUDE);
//	
//	if(GetOpenFileName(&of))
//	{
//		nFilterIndex = of.nFilterIndex;
//		
//		CString strName = of.lpstrFile;
//		CString strPath = strName.Left(of.nFileOffset);
//
//		strName = strName.Mid(of.nFileOffset);
//
//		pDoc->BorderMap(strName);
//		SendMessage(GetDlgItem(hDlg, IDC_LIST1), (UINT)LB_ADDSTRING, 0, (LPARAM)strName.GetBuffer());
//	}
//	delete [] of.lpstrFile;
}

void DlgProcPlotPreviewSaveAs(HWND )//hDlg)
{
////	of.lpstrInitialDir = strWDirectory;
	
	////	char szFilter[55];
////	SecureZeroMemory(szFilter, 55);
////	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));
//	char *szFilter = "Job Border Files (.jb2)\000*.jb2";
//
//	CString strName;
//	if(job2state.m_strFilePath == "Filename - <empty>")
//		strName = "BorderFile";
//	else
//		strName = job2state.m_strFilePath;
//
//	OPENFILENAME of;
//		
//	::ZeroMemory(&of, sizeof(OPENFILENAME));
//	of.lStructSize = sizeof(OPENFILENAME);
//	of.hwndOwner = 0;
//	of.hInstance = app.GetInstance();
//	of.lpstrFilter = szFilter;
//	of.lpstrFile = new char[MAX_PATH];
//	strcpy(of.lpstrFile, strName);
//	of.nMaxFile = MAX_PATH;
//	of.lpstrTitle = "Create New File";
//	of.Flags = OFN_OVERWRITEPROMPT;
////	of.lpstrDefExt = "jb2";
//
//	if(GetSaveFileName(&of))
//	{
//		strName = of.lpstrFile;
//		job2state.m_strFilePath = strName = strName.Mid(of.nFileOffset);
//		DlgProcPlotPreviewSave(hDlg);
//	}
//	delete [] of.lpstrFile;
}

void DlgProcPlotPreviewSave(HWND)
{
	//CFileJob2 fj2;
	//bool bFileOpen = fj2.OpenForWrite(job2state.m_strFilePath);
	//if(bFileOpen)
	//{
	//	fj2.WriteHeader();
	//	fj2.WriteInfo();
	//	fj2.Close();
	//}
}

void DlgProcPlotPreviewBorderEnable(HWND hDlg, bool bValue)
{
	::EnableWindow(::GetDlgItem(hDlg, IDC_JB2_FILE), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_LOAD_JB2), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_NEWJB2), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_SAVE_JB2), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_SAVEAS_JB2), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_LIST1), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_ADD_BORDER), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_REMOVE_BORDER), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_SET_ORIGIN), bValue);
	::EnableWindow(::GetDlgItem(hDlg, ID_SET_EXTENT), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_X), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_ORIGIN_Y), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_CLIP_XMAX), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_CLIP_XMIN), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_CLIP_YMAX), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_CLIP_YMIN), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_LAY), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_X), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_Y), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_XMAX), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_XMIN), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_YMAX), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_YMIN), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_ORIGIN), bValue);
	::EnableWindow(::GetDlgItem(hDlg, IDC_STAT_CLIP), bValue);
}