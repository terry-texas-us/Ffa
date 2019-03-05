#pragma once

#include "OdFileBuf.h"

#include "ExSystemServices.h"
#include "ExHostAppServices.h"

#include "FileDWGDirectDumper.h"

class MyServices : public ExSystemServices, public ExHostAppServices
{
	protected:
		ODRX_USING_HEAP_OPERATORS(ExSystemServices);
};

class CFileDWGDirect
{
	public:
		CFileDWGDirect();
		~CFileDWGDirect();
		
		void	Load(char*, const CUPDUPDATA*);
		void	LoadPrimData();
		void	LoadPrimVectorize();
		void	DumpHeader();
		void	DumpViewportTable();
		void	DumpViewTable();
		void	DumpLineTypeTable();
		void	DumpLayerTable();
		void	DumpBlockTable();
		void	DumpBlocks();
		void	DumpMSBlocks();
		void	DumpObject(OdDbObjectId id, const OdString& itemName, const OdString& padding);
		void	DumpEntity(OdDbObjectId id, bool bIntersectView = false, OdGeExtents3d geExtView = OdGeExtents3d());
		void	DumpMLineStyles();

		void	ExplodBlockRefs();
		void	ExplodeBlockTable();

		CSeg*	ClipViewport(CSeg *pSeg, CPnt ptLL, CPnt ptUR);

		bool	Create(char*, WORD);
		void	WriteStart();
		void	WriteLineTypeTable();
		void	WriteLineType(CPenStyle*);
		void	WriteLayerTable();
		void	WriteBlockTable();
		void	WriteEntity();

		void	TD_MS2PS(OdDbViewportPtr pVp, OdGeMatrix3d &resultMat);

	private:
		OdRxObjectImpl<MyServices> svcs;
		OdDbDatabasePtr pDb;  // A smart pointer to ODA database

		ExProtocolExtension pProtExt;

		int				m_iVpCount;
		CViewport		m_vp[17];
		CAbstractView	m_av[17];
		CTMat			m_tm[17];
		CTMat			m_tmView[17];
};