#include "stdafx.h"
//
//#if defined(GL_FUNCTIONALITY)
//
//BOOL CALLBACK DlgProcViewLighting(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
//{
//	CPegView* pView = CPegView::GetActiveView();
//
//	switch (nMsg) 
//	{
//		case WM_INITDIALOG:
//			// General illumination
//
//			//	Ambient light
//			DlgBoxSetItemDouble(hDlg, IDC_LIGHT_AMBIENT_RED, opengl::dAmbientLight[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_LIGHT_AMBIENT_GREEN, opengl::dAmbientLight[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_LIGHT_AMBIENT_BLUE, opengl::dAmbientLight[2]);
//			
//			// Point source lights (only L0 now)
//			//	Position
//			DlgBoxSetItemDouble(hDlg, IDC_L0_POSITION_X, opengl::dL0Position[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_POSITION_Y, opengl::dL0Position[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_POSITION_Z, opengl::dL0Position[2]);
//			//	Ambient
//			DlgBoxSetItemDouble(hDlg, IDC_L0_AMBIENT_RED, opengl::dL0Ambient[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_AMBIENT_GREEN, opengl::dL0Ambient[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_AMBIENT_BLUE, opengl::dL0Ambient[2]);
//			//	Diffuse
//			DlgBoxSetItemDouble(hDlg, IDC_L0_DIFFUSE_RED, opengl::dL0Diffuse[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_DIFFUSE_GREEN, opengl::dL0Diffuse[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_DIFFUSE_BLUE, opengl::dL0Diffuse[2]);
//			// Specular
//			DlgBoxSetItemDouble(hDlg, IDC_L0_SPECULAR_RED, opengl::dL0Specular[0]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_SPECULAR_GREEN, opengl::dL0Specular[1]);
//			DlgBoxSetItemDouble(hDlg, IDC_L0_SPECULAR_BLUE, opengl::dL0Specular[2]);
//			return (TRUE);
//
//		case WM_COMMAND:
//
//			switch (LOWORD(wParam)) 
//			{
//				case IDOK:
//					opengl::dAmbientLight[0] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_LIGHT_AMBIENT_RED), 1.));
//					opengl::dAmbientLight[1] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_LIGHT_AMBIENT_GREEN), 1.));
//					opengl::dAmbientLight[2] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_LIGHT_AMBIENT_BLUE), 1.));
//
//					opengl::dL0Position[0] = DlgBoxGetItemDouble(hDlg, IDC_L0_POSITION_X);
//					opengl::dL0Position[1] = DlgBoxGetItemDouble(hDlg, IDC_L0_POSITION_Y);
//					opengl::dL0Position[2] = DlgBoxGetItemDouble(hDlg, IDC_L0_POSITION_Z);
//					
//					opengl::dL0Ambient[0] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_L0_AMBIENT_RED), 1.));
//					opengl::dL0Ambient[1] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_L0_AMBIENT_GREEN), 1.));
//					opengl::dL0Ambient[2] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_L0_AMBIENT_BLUE), 1.));
//
//					opengl::dL0Diffuse[0] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_L0_DIFFUSE_RED), 1.));
//					opengl::dL0Diffuse[1] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_L0_DIFFUSE_GREEN), 1.));
//					opengl::dL0Diffuse[2] = Max(0., Min(DlgBoxGetItemDouble(hDlg, IDC_L0_DIFFUSE_BLUE), 1.));
//
//					opengl::dL0Specular[0] = DlgBoxGetItemDouble(hDlg, IDC_L0_SPECULAR_RED);
//					opengl::dL0Specular[1] = DlgBoxGetItemDouble(hDlg, IDC_L0_SPECULAR_GREEN);
//					opengl::dL0Specular[2] = DlgBoxGetItemDouble(hDlg, IDC_L0_SPECULAR_BLUE);
//					
//					
//					// Global ambient light level
//					opengl::SetAmbientLight(opengl::dAmbientLight);
//					
//					// Light 0 settings
//					opengl::SetLightPosition(GL_LIGHT0, opengl::dL0Position);
//					opengl::SetLightAmbient(GL_LIGHT0, opengl::dL0Ambient);
//					opengl::SetLightDiffuse(GL_LIGHT0, opengl::dL0Diffuse);
//					opengl::SetLightSpecular(GL_LIGHT0, opengl::dL0Specular);
//					pView->InvalidateRect(NULL, TRUE);
//										
//				case IDCANCEL:
//					::EndDialog(hDlg, TRUE);
//					return (TRUE);
//			}
//			break;
//	}
//	return (FALSE); 		
//}
//#else
//BOOL CALLBACK DlgProcViewLighting(HWND, UINT, WPARAM, LPARAM) 
//{
//	return FALSE;
//}
//#endif // GL_FUNCTIONALITY