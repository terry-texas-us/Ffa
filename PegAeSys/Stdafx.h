// Stdafx.h :

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from windows headers
#endif

#ifdef WINVER
#undef WINVER
#endif
#define WINVER 0x0501
//#ifndef WINVER				// Allow use of features specific to Windows 95 and WIndows NT 4 or later.
//#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//#endif

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501
//#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
//#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
//#endif

//#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
//#define _WIN32_WINDOWS 0x0410	// Change this to the appropriate value to target Windows Me or later.
//#endif

#ifdef _WIN32_IE
#undef _WIN32_IE
#endif
#define _WIN32_IE 0x0501
//#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later
//#define _WIN32_IE 0x0400	// Change to the appropriate value to target IE 5.0 or later
//#endif

////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
////////////////////////////////////////////////////////////////////////////////////////
#define SHOW_CLIPPING					// viewport (clipping) toggle
//#define ENABLE_DRAGNDROP				// drag 'n' drop functionality
//#define ENABLE_UNDO					// undo functionality
//#define ENABLE_CURSOR					// cursor functionality
//#define ENABLE_BITMAP_PRIM			// bitmap primitive
//#define ENABLE_DWG_EXPORT				// dwg export
#define ENABLE_NODAL_VISUAL			// nodal visual
////////////////////////////////////////////////////////////////////////////////////////

#define _AFX_ALL_WARNINGS

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxadv.h>
#include <afxtempl.h>

#include <windowsx.h>

#include <ddeml.h>
#include <direct.h>

#include <shlobj.h>
#include <shlwapi.h>
#include <commdlg.h>

#include <cfloat>
#include <cmath>

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
#include "OdToolKit.h"
#include "OdaDefs.h"
#include "OdAlloc.h"
#include "OdString.h"

#include "DbDatabase.h"
#include "DbEntity.h"
#include "DbSystemServices.h"
#include "DbHostAppServices.h"
#include "DbObject.h"
#include "DbObjectId.h"
#include "DbObjectIterator.h"
#include "DbGsManager.h"
#include "DbDictionary.h"
#include "DbXrecord.h"
#include "DbDimAssoc.h"
#include "DbSymbolTable.h"
#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "Db2dPolyline.h"
#include "DbMlineStyle.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbSymbolTable.h"
#include "DbViewportTableRecord.h"
#include "DbViewportTable.h"
#include "DbViewTable.h"
#include "DbViewTableRecord.h"
#include "DbLinetypeTable.h"
#include "DbLinetypeTableRecord.h"
#include "DbUCSTableRecord.h"
#include "DbViewport.h"

#include "RxObjectImpl.h"
#include "Gs/Gs.h"

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
#include "FilePeg2000.h"
#include "FileTracing.h"
#include "PegReg.h"
#include "QuadTree.h"

#ifdef ENABLE_UNDO
	#include "Undo.h"
#endif

//#include "OpenGL.h"