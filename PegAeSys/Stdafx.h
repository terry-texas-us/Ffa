// Stdafx.h :

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h> // MFC core and standard components
#include <afxext.h> // MFC extensions

#include <afxdisp.h> // MFC Automation classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <cfloat>
#include <cmath>

////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
////////////////////////////////////////////////////////////////////////////////////////
#define SHOW_CLIPPING					// viewport (clipping) toggle
//#define ENABLE_DRAGNDROP				// drag 'n' drop functionality
//#define ENABLE_UNDO					// undo functionality
//#define ENABLE_CURSOR					// cursor functionality
//#define ENABLE_BITMAP_PRIM			// bitmap primitive
//#define ENABLE_DWG_EXPORT				// dwg export
////////////////////////////////////////////////////////////////////////////////////////

#include <afxadv.h>
#include <afxtempl.h>

#include <windowsx.h>

#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif // _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include <ddeml.h>
#include <direct.h>

#include <shlobj.h>
#include <shlwapi.h>
#include <commdlg.h>

/////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
using std::ofstream;
////////////////////////////////////////////////////

//#if defined(GL_FUNCTIONALITY)
//#include <gl\gl.h>
//#include <gl\glu.h>
//#endif // GL_FUNCTIONALITY

#define tasSTR2(x) #x
#define tasSTR(x) tasSTR2(x)
#define tasMSG(desc) message(__FILE__ "(" tasSTR(__LINE__) ") : " #desc)

/////////////////////////////////////////////////////////////////////////////

#ifdef ID_APP_ABOUT
#undef ID_APP_ABOUT
#endif

#ifdef ID_VIEW_TOOLBAR
#undef ID_VIEW_TOOLBAR
#endif

#ifdef IDC_STATIC
#undef IDC_STATIC
#endif

/////////////////////////////////////////////////////////////////////////////
#include "OdaCommon.h"

#include "Ge/GePoint3d.h"

#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "DbDatabase.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbLinetypeTable.h"
#include "DbLinetypeTableRecord.h"
#include "DbViewport.h"
#include "DbViewportTable.h"
#include "DbViewportTableRecord.h"

#include "DbEntity.h"
#include "DbObjectIterator.h"
#include "DbGsManager.h"
#include "DbDictionary.h"
#include "DbMlineStyle.h"

#include "..\\CBitmapEx\\BitmapEx.h"

#include "resource.h"
#include "Messages.h"

#include "ExpProcs.h"
UINT AFXAPI HashKey(CString& str);

#include "SafeMath.h"

#define WM_SOLO_BLOCK					WM_USER+1
#define WM_SOLO_LAYER					WM_USER+2
#define WM_SOLO_TRACING					WM_USER+3
#define WM_SOLO_RETURN_BLOCK			WM_USER+4
#define WM_SOLO_RETURN_LAYER			WM_USER+5
#define WM_SOLO_RETURN_TRACING			WM_USER+6
#define WM_SOLO_RETURN_NONE				WM_USER+9

#define WM_DIALOG_OK					WM_USER+7
#define WM_DIALOG_FOCUS					WM_USER+8

//#define WM_PROGRESS_DONE				WM_USER+9

#define WM_PLOT_UPDATE					WM_USER+10

#define IDC_TRAMAP_RETURN				WM_USER+11

#include "Pnt.h"
#include "Pnt4.h"
#include "Vec.h"
#include "Vec4.h"
#include "Mat4.h"
#include "TMat.h"
#include "RefSys.h"
#include "Viewport.h"
#include "AbstractView.h"
#include "ModelView.h"
#include "ModelTransform.h"
#include "PenStyle.h"
#include "Line.h"
#include "Polyline.h"
#include "CharCellDef.h"
#include "FontDef.h"
#include "Prim.h"
#include "PrimArc.h"
#include "PrimBitmap.h"
#include "PrimBSpline.h"
#include "PrimCSpline.h"
#include "PrimInsert.h"
#include "PrimLine.h"
#include "PrimMark.h"
#include "PrimPolygon.h"
#include "PrimPolyline.h"
#include "PrimSegRef.h"
#include "PrimTag.h"
#include "PrimText.h"
#include "PrimDim.h"
#include "PrimBitmap.h"
#include "Seg.h"
#include "Block.h"
#include "Segs.h"
#include "Layer.h"
#include "SegsDet.h"
#include "SegsTrap.h"
#include "PrimState.h"
#include "ImportState.h"
#include "ExportState.h"
#include "PrintState.h"
#include "PlotState.h"
#include "FilePeg.h"
#include "FileTracing.h"
#include "PegReg.h"
#include "QuadTree.h"

#ifdef ENABLE_UNDO
	#include "Undo.h"
#endif

//#include "OpenGL.h"