//#pragma once
//
//class CFilePeg2000 : public CFile
//{
//// old filepeg defines
//private:
//	static const WORD SECTION_HEADER	= 0x0101;
//	static const WORD SECTION_TABLES	= 0x0102;
//	static const WORD SECTION_BLOCKS	= 0x0103;
//	static const WORD SECTION_SEGMENTS 	= 0x0104;
//	static const WORD SECTION_ENTITIES 	= 0x0104;
//	static const WORD SECTION_END		= 0x01ff;
//
//	static const WORD TABLE				= 0x0200;
//	static const WORD TABLE_VPORT		= 0x0201;
//	static const WORD TABLE_LTYPE		= 0x0202;
//	static const WORD TABLE_LAYER		= 0x0203;
//	static const WORD TABLE_END			= 0x02ff;
//
//// old layer defines
//private:
//	static const WORD STATE_FLG_RESIDENT	= 0x0001;
//	static const WORD STATE_FLG_INTERNAL	= 0x0002;
//	static const WORD STATE_FLG_HOT			= 0x0004;
//	static const WORD STATE_FLG_WARM		= 0x0008;
//	static const WORD STATE_FLG_COLD		= 0x0010;
//	static const WORD STATE_FLG_OFF			= 0x0020;
//
//	static const WORD TRACING_FLG_OPENED	= 0x0004;
//	static const WORD TRACING_FLG_MAPPED	= 0x0008;
//	static const WORD TRACING_FLG_VIEWED	= 0x0010;
//	static const WORD TRACING_FLG_CLOAKED	= 0x0020;
//
//// old prim defines
//public:
//	static const WORD MARK_PRIMITIVE		= 256;		// 0x0100
//	static const WORD INSERT_PRIMITIVE		= 257;		// 0x0101
//	static const WORD LINE_PRIMITIVE		= 512;		// 0x0200
//	static const WORD POLYGON_PRIMITIVE		= 1024;		// 0x0400
//	static const WORD ARC_PRIMITIVE			= 4099;		// 0x1003
//	static const WORD BSPLINE_PRIMITIVE		= 8192;		// 0x2000
//	static const WORD CSPLINE_PRIMITIVE		= 8193;		// 0x2001
//	static const WORD POLYLINE_PRIMITIVE	= 8194;		// 0x2002
//	static const WORD NOTE_PRIMITIVE		= 16384;	// 0x4000
//	static const WORD TAG_PRIMITIVE			= 16640;	// 0x4100
//	static const WORD DIM_PRIMITIVE			= 16896;	// 0x4200
//
//public:
//	CFilePeg2000() {};
//	virtual ~CFilePeg2000() {}
//
//	void	Load();
//	void	ReadBlocks();
//	void	ReadBlocksSection();
//	void	ReadEntities();
//	void	ReadEntitiesSection();
//	void	ReadHeaderSection();
//	void	ReadLayerTable();
//	void	ReadLnTypsTable();
//	void	ReadTablesSection();
//	void	ReadVPortTable();
//	void	Unload();
//	void	WriteBlocksSection();
//	void	WriteEntitiesSection();
//	void	WriteHeaderSection();
//	void	WriteLayerTable();
//	void	WritePenStyle(CPenStyle*);
//	void	WritePenStyleTable();
//	void	WriteTablesSection();
//	void	WriteVPortTable();
//
////	void	Load();
////	void	ReadBlocks();
////	bool	ReadBlocksSection();
//	void	ReadDelimitedString(char*, UINT);
////	void	ReadEntities();
////	bool	ReadEntitiesSection();
////	void	ReadHeaderSection();
////	void	ReadLayerTable();
////	void	ReadLnTypsTable();
////	bool	ReadTablesSection();
////	void	ReadVPortTable();
//	WORD	ReadWord();
////	void	Unload();
////	bool	WriteBlocksSection();
////	bool	WriteEntities();
////	bool	WriteEntitiesSection();
////	bool	WriteHeaderSection();
////	void	WriteLayer(const CString&, PENCOLOR/*, AD_OBJHANDLE*/);
////	bool	WriteLayerTable();
//////	void	WriteLineType(CLineType*);
////	bool	WriteLineTypeTable();
////	bool	WriteShadowFile();
////	void	WriteTablesSection();
////	void	WriteVPortTable();
//	void	WriteWord(WORD w) {CFile::Write(&w, sizeof(WORD));}
//	void	WriteString(const CString& s) {CFile::Write(s + "\t", s.GetLength() + 1);}
//};
//
//WORD dwgReadWord(CFile& fl);
//bool primRead(CFile& fl, CPrim*& pPrim);
//void dwgWriteWord(CFile& fl, int iGroupValue);
//void dwgWriteWord(CFile& fl, WORD iGroupValue);
//
////void FilePeg_ReadString(CFile& fl, CString& str);
////bool FilePeg_ReadPrim(CFile& fl, CPrim*&);
////inline void FilePeg_ReadDouble(CFile& fl, double& d) {fl.Read(&d, sizeof(double));}
////inline void FilePeg_ReadShort(CFile& fl, short& i) {fl.Read(&i, sizeof(short));}
////inline void FilePeg_ReadWord(CFile& fl, WORD& w) {fl.Read(&w, sizeof(WORD));}
////inline WORD FilePeg_ReadWord(CFile& fl) {WORD w; fl.Read(&w, sizeof(WORD)); return w;}
////inline void FilePeg_WriteDouble(CFile& fl, double d) {fl.Write(&d, sizeof(double));}
////inline void FilePeg_WriteWord(CFile& fl, WORD w) {fl.Write(&w, sizeof(WORD));}
////void FilePeg_WriteString(CFile& fl, const CString& str);