#pragma once

// MainFrm.h : interface of the CMainFrame class

class CMainFrame : public CObject
{
	public:
		CMainFrame();// {}
		virtual ~CMainFrame() {}

	//#ifdef _DEBUG
	//	virtual void AssertValid() const;
	//	virtual void Dump(CDumpContext& dc) const;
	//#endif

		void	OnClose(HWND hwnd);
		void	OnCommand(HWND hwnd, int id, HWND hWndCtrl, UINT uCodeNotify);
		BOOL	OnCreate(HWND hwnd,  LPCREATESTRUCT lpCreateStruct);
		void	OnDestroy(HWND hwnd);
		void	OnKey(HWND hwnd, UINT vk, BOOL Down, int RepeatCOunt, UINT flags);
		void 	OnPaint(HWND hwnd);

	#ifdef ENABLE_DRAGNDROP
		void	OnDropfiles(HWND hwnd, HDROP hDrop);
	#endif

		void	OnMove(HWND hwnd, int x, int y);

		// MAK - solo button
		static const WORD SOLO_TYPE_NONE		= 1;
		static const WORD SOLO_TYPE_LAYER		= 2;
		static const WORD SOLO_TYPE_TRACING		= 3;
		static const WORD SOLO_TYPE_BLOCK		= 4;

		WORD	m_wSoloType;

		void	OnSoloTracing(HWND);
		void	OnSoloBlock(HWND);
		void	OnSoloLayer(HWND);

		void	OnSoloReturnBlock(HWND);
		void	OnSoloReturnTracing(HWND);
		void	OnSoloReturnLayer(HWND);

		void	OnSoloLoadPrevHwnd();
		void	OnSoloDeleteKey();
		void	OnSoloAddHwnd();
		void	OnSoloSendClose();
};