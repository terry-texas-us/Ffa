//#pragma once

//namespace opengl
//{
//	extern double dAmbientLight[];
//	
//	extern double dL0Position[];
//	extern double dL0Ambient[];
//	extern double dL0Diffuse[];
//	extern double dL0Specular[];
//		
//#if defined(GL_FUNCTIONALITY)
//	
//	inline void BeginLineLoop() {glBegin(GL_LINE_LOOP);}
//	inline void BeginLineStrip() {glBegin(GL_LINE_STRIP);}
//	inline void BeginPoints() {glBegin(GL_POINTS);}
//	inline void BeginPolygon() {glBegin(GL_POLYGON);}
//	inline void Clear() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);} 
//	void EnableColorMaterial();
//	inline void EnablePolygonSmooth() {glEnable(GL_POLYGON_SMOOTH);}
//	inline void End() {glEnd();}
//	inline void Flush() {glFlush();}
//	void Frustum(double dUMin, double dUMax, double dVMin, double dVMax, double dNearClipDistance, double dFarClipDistance);
//	void Initialize(CDC* pDC);
//	void LookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec vVwUp);
//	void MultModelViewMatrix(const CTMat& tm);
//	void Ortho(double dUMin, double dUMax, double dVMin, double dVMax, double dNearClipDistance, double dFarClipDistance);
//	inline void PolygonModeFill() {glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);}
//	inline void PolygonModeLine() {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);}
//	
//	void PopModelViewMatrix();
//	void PushModelViewMatrix();
//	void Release();
//	void SetAmbientLight(double*);
//	void SetCurrentColor(COLORREF cr);
//	void SetDCPixelFormat(CDC* pDC);
//	void SetLightAmbient(GLenum nLight, double*);
//	void SetLightDiffuse(GLenum nLight, double* dDiffuse);
//	void SetLightPosition(GLenum nLight, double*);
//	void SetLightSpecular(GLenum nLight, double*);
//	void SetVertex(const CPnt& pt);
//	inline void Viewport(GLint iX, GLint iY, GLsizei iWidth, GLsizei iHeight) {glViewport(iX, iY, iWidth, iHeight);}
//#else
//	inline void BeginLineLoop() {}
//	inline void BeginLineStrip() {}
//	inline void BeginPoints() {}
//	inline void BeginPolygon() {}
//	inline void Clear() {} 
//	inline void EnableColorMaterial() {}
//	inline void EnablePolygonSmooth() {}
//	inline void End() {}
//	inline void Flush() {}
//	inline void Frustum(double, double, double, double, double, double) {}
//	inline void Initialize(CDC*) {}
//	inline void LookAt(const CPnt&, const CPnt&, const CVec) {}
//	inline void MultModelViewMatrix(const CTMat&) {}
//	inline void Ortho(double, double, double, double, double, double) {}
//	inline void PolygonModeFill() {}
//	inline void PolygonModeLine() {}
//	inline void PopModelViewMatrix() {}
//	inline void PushModelViewMatrix() {}
//	inline void Release() {}
//	inline void SetAmbientLight(Vecd) {}
//	inline void SetCurrentColor(COLORREF) {}
//	inline void SetDCPixelFormat(CDC*) {}
//	inline void SetLightAmbient(UINT, Vecd) {}
//	inline void SetLightDiffuse(UINT, double*) {}
//	inline void SetLightPosition(UINT, Vecd) {}
//	inline void SetLightSpecular(UINT, Vecd) {}
//	inline void SetVertex(const CPnt&) {}
//	inline void Viewport(int, int, int, int) {}
//#endif // GL_FUNCTIONALITY
//}