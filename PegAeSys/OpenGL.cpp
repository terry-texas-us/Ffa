#include "stdafx.h"

//namespace opengl
//{
//	HGLRC  hRC;
//
//	double dAmbientLight[] = {.2, .2, .2, 1.};	// ambient light intensity (range 0. to 1.)
//	
//	double dL0Position[] = {0., 0., 1000., 1.};	// point light source position in world space
//	double dL0Ambient[] = {0., 0., 0., 1.};
//	double dL0Diffuse[] = {1., 1., 1., 1.};
//	double dL0Specular[] = {1., 1., 1., 1.};	// incident intensity from point light source
//
//	float  fMatAmbient[] = {.2f, .2f, .2f, 1.f};
//	float  fMatDiffuse[] = {.8f, .8f, .8f, 1.f};
//	float  fMatSpecular[] = {0.f, 0.f, 0.f, 1.f};
//}

//#if defined(GL_FUNCTIONALITY)

/////<summary>Enables color material tracking to handle front and back faces the same.</summary>
//void opengl::EnableColorMaterial() 
//{
//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//}

//void opengl::Frustum(double dUMin, double dUMax, double dVMin, double dVMax, double dNearClipDistance, double dFarClipDistance)
//{
//	GLint nMatrixMode;
//	glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glFrustum(dUMin, dUMax, dVMin, dVMax, dNearClipDistance, dFarClipDistance);
//	glMatrixMode(nMatrixMode);
//}

//void opengl::Initialize(CDC* pDC)
//{
//	opengl::SetDCPixelFormat(pDC);
//
//	// Associate a rendering context with the device context of the application window
//	// Make it the active context (for now it is also the only context)
//	opengl::hRC = wglCreateContext(pDC->GetSafeHdc());
//	wglMakeCurrent(pDC->GetSafeHdc(), opengl::hRC);
//	
//	opengl::EnableColorMaterial();
//	opengl::EnablePolygonSmooth();
//
//	// clear buffers with a black pen (or whatever background color is)
//	
//	COLORREF cr = app.PenColorsGetHot(0);
//	glClearColor(GetRValue(cr), GetGValue(cr), GetBValue(cr), 0.);
//
//	// Global ambient light level is same for RGB values (white light only)
//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
//	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
//	opengl::SetAmbientLight(opengl::dAmbientLight);
//	
//	glEnable(GL_LIGHTING);
//	
//	// Use material properties and lighting parameters to render
//
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, opengl::fMatAmbient);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, opengl::fMatDiffuse);
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, opengl::fMatSpecular);
//		
//	// Light 0 is only point light initially
//	opengl::SetLightPosition(GL_LIGHT0, opengl::dL0Position);
//	opengl::SetLightAmbient(GL_LIGHT0, opengl::dL0Ambient);
//	opengl::SetLightDiffuse(GL_LIGHT0, opengl::dL0Diffuse);
//	opengl::SetLightSpecular(GL_LIGHT0, opengl::dL0Specular);
//	glEnable(GL_LIGHT0);
//	
//	// Enable smooth shading. This is the default. 
//	glShadeModel(GL_SMOOTH);
//	// Enable hidden surface removal
//	glEnable(GL_DEPTH_TEST); 
//	glEnable(GL_NORMALIZE);
//}

//void opengl::LookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec vVwUp)
//{
//	GLint nMatrixMode;
//	glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(ptEye[0], ptEye[1], ptEye[2], ptTarget[0], ptTarget[1], ptTarget[2], vVwUp[0], vVwUp[1], vVwUp[2]);
//	glMatrixMode(nMatrixMode);
//}

//void opengl::MultModelViewMatrix(const CTMat& tm)
//{
//	CTMat tmTranspose(tm);
//	tmTranspose = tmTranspose.Transpose();
//	
//	double dMat[16];
//	tmTranspose[0].Get(&dMat[0]);
//	tmTranspose[1].Get(&dMat[4]);
//	tmTranspose[2].Get(&dMat[8]);
//	tmTranspose[3].Get(&dMat[12]);
//
//	GLint nMatrixMode;
//	glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
//	glMatrixMode(GL_MODELVIEW);
//	glMultMatrixd(dMat);
//	glMatrixMode(nMatrixMode);
//}

//void opengl::PopModelViewMatrix()
//{
//	GLint nMatrixMode;
//	glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
//	glMatrixMode(nMatrixMode);
//}

//void opengl::PushModelViewMatrix()
//{
//	GLint nMatrixMode;
//	glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glMatrixMode(nMatrixMode);
//}

//void opengl::Ortho(double dUMin, double dUMax, double dVMin, double dVMax, double dNearClipDistance, double dFarClipDistance)
//{
//	GLint nMatrixMode;
//	glGetIntegerv(GL_MATRIX_MODE, &nMatrixMode);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(dUMin, dUMax, dVMin, dVMax, dNearClipDistance, dFarClipDistance);
//	glMatrixMode(nMatrixMode);
//}

//void opengl::Release()
//{
//	// Before deleting the rendering context, delete any display lists, 
//	// texture objects, or other OpenGL allocated memory
//	
//	// Deselect the current rendering context and delete it
//	wglMakeCurrent(0, 0);
//	wglDeleteContext(opengl::hRC);
//}

//void opengl::SetAmbientLight(Vecd dAmbientLight)
//{
//	float fAmbientLight[] = {float(dAmbientLight[0]), float(dAmbientLight[1]), float(dAmbientLight[2]), 1.};
//	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fAmbientLight);
//}

//void opengl::SetCurrentColor(COLORREF cr)
//{
//	BYTE nRed = (BYTE) (cr & 0x000000ff);
//	BYTE nGreen = (BYTE) ((cr >> 8) & 0x000000ff);
//	BYTE nBlue = (BYTE) ((cr >> 16) & 0x000000ff);
//
//	glColor3ub(nRed, nGreen, nBlue);
//}

//void opengl::SetDCPixelFormat(CDC* pDC)
//{	
//	// Setup pixel format.
//	// For now we mix gdi calls so no double buffering enabled
//
//	PIXELFORMATDESCRIPTOR pfd; 
// 
//	::ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
//    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
//	pfd.nVersion = 1;
//    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI; 
//	pfd.iPixelType = PFD_TYPE_RGBA;
//    pfd.cColorBits = 24; 
//    pfd.cAlphaBits = 0;
//    pfd.cAccumBits = 0; 
//    pfd.cDepthBits = 32;					// 32 bit z-buffer 
//    pfd.cStencilBits = 0;					// no stencil buffer
//	pfd.cAuxBuffers = 0;					// no auxiliary buffer
//	pfd.iLayerType = PFD_MAIN_PLANE;
// 
//	int ipixelformat = ChoosePixelFormat(pDC->GetSafeHdc(), &pfd); 
// 
//	SetPixelFormat(pDC->GetSafeHdc(), ipixelformat, &pfd);
//}

//void opengl::SetLightAmbient(GLenum nLight, Vecd dAmbient)
//{
//	float fAmbient[] = {float(dAmbient[0]), float(dAmbient[1]), float(dAmbient[2]), 1.f};
//	glLightfv(nLight, GL_AMBIENT, fAmbient);
//}

//void opengl::SetLightDiffuse(GLenum nLight, Vecd dDiffuse)
//{
//	float fDiffuse[] = {float(dDiffuse[0]), float(dDiffuse[1]), float(dDiffuse[2]), 1.f};
//	glLightfv(nLight, GL_DIFFUSE, fDiffuse);
//}

//void opengl::SetLightPosition(GLenum nLight, Vecd dPosition)
//{
//	float fPosition[] = {float(dPosition[0]), float(dPosition[1]), float(dPosition[2]), 1.f};
//	glLightfv(nLight, GL_POSITION, fPosition);
//}

//void opengl::SetLightSpecular(GLenum nLight, Vecd dSpecular)
//{
//	float fSpecular[] = {float(dSpecular[0]), float(dSpecular[1]), float(dSpecular[2]), 1.f};
//	glLightfv(nLight, GL_SPECULAR, fSpecular);
//}

//void opengl::SetVertex(const CPnt& pt)
//{
//	static double dPt[3];
//	
//	dPt[0] = pt[0];
//	dPt[1] = pt[1];
//	dPt[2] = pt[2];
//
//	glVertex3dv(dPt);
//}
//#endif // GL_FUNCTIONALITY