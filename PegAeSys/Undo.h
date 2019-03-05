#pragma once

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include <deque>
using std::deque;

const int MAX_UNDO_LEVELS = 5;

//class CUndoStruct
//{
//	public:
//		CUndoStruct() 
//		{
//			m_nUndoState = 0;
//			m_SegsDet    = 0;
//			m_PrimState  = 0;
//			m_PegDoc	 = 0;
//		}
//
//		CUndoStruct(const CUndoStruct &b)
//		{
//			m_nUndoState = b.m_nUndoState;
//			m_SegsDet    = b.m_SegsDet;
//			m_PrimState	 = b.m_PrimState;
//			m_PegDoc	 = b.m_PegDoc;
//		}
//
//		CUndoStruct& CUndoStruct::operator= (const CUndoStruct& param)
//		{
//			m_nUndoState = param.m_nUndoState;
//			m_SegsDet    = param.m_SegsDet;
//			m_PrimState  = param.m_PrimState;
//			m_PegDoc	 = param.m_PegDoc;
//			return *this;
//		}
//
//		void SetSegsDet(CSegsDet *param)			{ m_SegsDet = param; }
//		void SetPrimState(CPrimState *param)		{ m_PrimState = param; }
//		void SetPegDoc(CPegDoc *param)				{ m_PegDoc = param; }
//		
//		CSegsDet *GetSegsDet()						{ return m_SegsDet; }
//		CPrimState *GetPrimState()					{ return m_PrimState; }
//		CPegDoc *GetPegDoc()						{ return m_PegDoc; }
//
//		void UndoModeSegsDet()						{ m_nUndoState = m_nUndoState | 0x08; }
//		void UndoModePrimState()					{ m_nUndoState = m_nUndoState | 0x01; }
//
//		bool IsUndoModeSegsDet()					{ return ((m_nUndoState & 0x08) == 0x08) ? true : false; }
//		bool IsUndoModePrimState()					{ return ((m_nUndoState & 0x01) == 0x01) ? true : false; }
//
//	private:
//		int m_nUndoState;
//
//		CSegsDet *m_SegsDet;
//		CPrimState *m_PrimState;
//
//		CPegDoc * m_PegDoc;
//};

class CUndoStruct
{
	public:
		CUndoStruct() 
		{
			//m_nUndoState = 0;
			//m_SegsDet    = 0;
			//m_PrimState  = 0;
			//m_PegDoc	 = 0;
		}

		CUndoStruct(const CUndoStruct &b)
		{
			//m_nUndoState = b.m_nUndoState;
			m_SegsDet    = b.m_SegsDet;
			m_PrimState	 = b.m_PrimState;
			m_PegDoc	 = b.m_PegDoc;
		}

		~CUndoStruct()
		{

		}

		//CUndoStruct& CUndoStruct::operator= (const CUndoStruct& param)
		//{
		//	//m_nUndoState = param.m_nUndoState;
		//	m_SegsDet    = param.m_SegsDet;
		//	m_PrimState  = param.m_PrimState;
		//	m_PegDoc	 = param.m_PegDoc;
		//	return *this;
		//}

		CUndoStruct(CSegsDet segsDet, CPrimState primState, CPegDoc pegDoc)
		{
			m_SegsDet    = segsDet;
			m_PrimState	 = primState;
			m_PegDoc	 = pegDoc;
		}

		void SetSegsDet(CSegsDet param)			{ m_SegsDet = param; }
		void SetPrimState(CPrimState param)		{ m_PrimState = param; }
		void SetPegDoc(CPegDoc param)			{ m_PegDoc = param; }
		
		CSegsDet GetSegsDet()						{ return m_SegsDet; }
		CPrimState GetPrimState()					{ return m_PrimState; }
		CPegDoc GetPegDoc()							{ return m_PegDoc; }

		//void UndoModeSegsDet()						{ m_nUndoState = m_nUndoState | 0x08; }
		//void UndoModePrimState()					{ m_nUndoState = m_nUndoState | 0x01; }

		//bool IsUndoModeSegsDet()					{ return ((m_nUndoState & 0x08) == 0x08) ? true : false; }
		//bool IsUndoModePrimState()					{ return ((m_nUndoState & 0x01) == 0x01) ? true : false; }

	//private:
		//int m_nUndoState;

		CSegsDet m_SegsDet;
		CPrimState m_PrimState;
		CPegDoc m_PegDoc;
};

class CUndo
{
	//public:
		//static const WORD UNDO_SEGS_DET			= 0;
		//static const WORD UNDO_PRIM_STATE		= 1;

	public:
		CUndo();

		//void SaveState(const WORD state);
		void SaveState();
		bool RestoreLastState();

		void SaveDoc();
		void SavePrim();
		void SaveSegs();

		void RestoreDoc();
		void RestorePrim();
		void RestoreSegs();

	private:
		//deque<CUndoStruct> m_UndoDeque;
		//CPrimState* psSav[4];// = {0, 0, 0, 0};
		//CPegDoc* pDocSav[4];// = {0, 0, 0, 0};
		//CSegsDet pSegsDetSav[4];// = {0, 0, 0, 0};
		int nDoc;
		int nPrim;
		int nSegs;
};

extern CUndo undo;