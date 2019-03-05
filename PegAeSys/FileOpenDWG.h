//#pragma once
//
////#define AD_PROTOTYPES
////
////class CAbstractView;
////class CPenStyle;
////class CPrim;
//
//class CFileOpenDWG
//{
//public:
//	CFileOpenDWG();
//	~CFileOpenDWG();
//	
//	//bool	Create(char*, WORD);
//	//bool	Initialize(short* iError);
//	//void	Load(char*);
//	//void	ReadBlock(AD_DB_HANDLE, PAD_BLKH, PAD_ENT_HDR);
//	//void	ReadBlocks(AD_DB_HANDLE);
//	//void	ReadBlockTable(AD_DB_HANDLE);
//	//void	ReadEntities();
//	//CPrim*	ReadEntity(AD_VMADDR);
//	//CPrim*	ReadEntityPolyline(AD_VMADDR);
//	//void	ReadHeaderSection();
//	//void	ReadLayerTable();
//	//void	ReadLnTypsTable();
//	//void	ReadVPortTable();
//	//void	WriteBlocksSection();
//	//void	WriteEntities();
//	//void	WriteLayer(const CString& strName, PENCOLOR);
//	//void	WriteLayerTable();
//	//void	WritePenStyle(CPenStyle*);
//	//void	WritePenStyleTable();
//
//	//// HACK: temp to allow layer object handle to be passed to primitive without changing arg lists
//	//static AD_OBJHANDLE	ms_objecthandle;
//
//private:
//
//	//PAD_DWGHDR		m_dhd;
//	//AD_DB_HANDLE	m_hdb;
//	//PAD_ENT_HDR		m_henhd;
//	//PAD_ENT			m_hen;
//	//PAD_TB			m_htb;
//	//PAD_XD			m_hxd;
//
//	//bool			m_bInitialized;
//	//int				m_iVpCount;
//	//CViewport		m_vp[17];
//	//CAbstractView	m_av[17];
//	//CTMat			m_tm[17];
//	//CTMat			m_tmView[17];
//};
