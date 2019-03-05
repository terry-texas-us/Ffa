#pragma once

typedef DWORD (WINAPI *PSLWA)(HWND, DWORD, BYTE, DWORD);
static PSLWA pSetLayeredWindowAttributes = NULL;
static BOOL initialized = FALSE;

#define WS_EX_LAYERED           0x00080000    
#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002

BOOL MakeWindowTransparent(HWND hWnd, unsigned char factor);
BOOL MakeWindowTransparent2(HWND hWnd, COLORREF color);

class CInfoFrm
{
	public:
		CInfoFrm() {m_bVisible = true;}
		
		void SetInfo(int x, int y, int width, int height);
		void SetPosition(int x, int y);

		void SetHandle(HWND hWnd) {m_hWnd = hWnd; m_hDC = ::GetDC(hWnd);}
		CDC* GetDC() const {return CDC::FromHandle(m_hDC);}
		HWND GetHWND() { return m_hWnd; }
		BOOL InvalidateRect();

		void OnMove(HWND hwnd, int x, int y);
//		void OnSize(HWND hwnd, UINT nType, int x, int y);
		virtual void OnPaint(HWND hwnd) = 0;

		bool IsVisible() { return m_bVisible; }

		void ToggleState();
		void Refresh();

	protected:
		HWND		m_hWnd;
		HDC			m_hDC;	// private hDC to be used for all drawing to this window
		CBitmap		m_bmp;
		bool		m_bVisible;

		int			m_nWidth;
		int			m_nHeight;
		int			m_nX;
		int			m_nY;
};

class CInfoStatusFrm : public CInfoFrm
{
	public:
		CInfoStatusFrm(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
		void OnSize(HWND hwnd, UINT nType, int x, int y);
};

class CInfoModeFrm : public CInfoFrm
{
	public:
		CInfoModeFrm();
		CInfoModeFrm(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
		void OnSize(HWND hwnd, UINT nType, int x, int y);
};

class CInfoOdomFrm : public CInfoFrm
{
	public:
		CInfoOdomFrm(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
		void OnSize(HWND hwnd, UINT nType, int x, int y);
};

class CBackgroundLayer : public CInfoFrm
{
	public:
		CBackgroundLayer(int x, int y, int width, int height);
		void OnPaint(HWND hwnd);
		void OnSize(HWND hwnd, UINT nType, int x, int y);
};