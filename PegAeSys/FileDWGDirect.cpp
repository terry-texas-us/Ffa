#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "FileDWGDirectDumper.h"
#include "FileDWGDirect.h"

#include "SmartPtr.h"
#include "DbCircle.h"
#include "DbEllipse.h"
#include "DbText.h"
#include "DbMText.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbPoint.h"
#include "DbBlockReference.h"
#include "DbFace.h"
#include "DbSolid.h"
#include "Db3dSolid.h"
#include "DbHatch.h"
#include "DbPolyline.h"
#include "Db2dPolyline.h"
#include "Db3dPolyline.h"
#include "Db3dPolylineVertex.h"
#include "DbDimension.h"
#include "DbArcAlignedText.h"
#include "DbViewport.h"
#include "DbTextStyleTableRecord.h"
#include "DbAttribute.h"
#include "DbLayout.h"
#include "Ge/GeLineSeg2d.h"
#include "Ge/GeCurve2d.h"
#include "XRefMan.h"

#include "DbTextStyleTable.h"
#include "DbTextStyleTableRecord.h"

#include "DbAttribute.h"
#include "Ge/GeScale3d.h"
#include "DbMInsertBlock.h"

#include "Gs/GsBaseVectorizer.h"

#include "GiContextForDbDatabase.h"
#include "..\\Vectorizer\\ExGsSimpleDevice.h"

#include "DbIdMapping.h"

#include "OdFileBuf.h"

#include <deque>
using std::deque;

extern CExportState estate;

////////////////////////////////////////////////////////////////////////////////////////////////
// mak - these need to be defined or include "alloc.lib"
////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {

ALLOCDLL_EXPORT void* odrxAlloc(size_t s)
{
	return ::malloc(s);
}

ALLOCDLL_EXPORT void* odrxRealloc(void* p, size_t new_size, size_t /*old_size*/)
{
	return ::realloc(p, new_size);
}

ALLOCDLL_EXPORT void odrxFree(void* p)
{
	::free(p);
}

} // extern "C"
////////////////////////////////////////////////////////////////////////////////////////////////

CFileDWGDirect::CFileDWGDirect()
{
	odInitialize(&svcs);  // Register classes needed for ODA use. Just once per process.
	pProtExt.initialize(); // Initialize protocol extension
	odgsInitialize(); // Gs implementation classes
}

CFileDWGDirect::~CFileDWGDirect()
{
	pDb.release();		// Delete the database before odUninitialize() is called.
	odgsUninitialize();
	pProtExt.uninitialize();
	odUninitialize();
}

// Acad Mode Settings
namespace AcadMode
{
	deque<CString> strLayouts;

	CString strSelectedLayout;
	bool bVectorize = false;
	deque<CString> strXREFs;
}

// Load DWG/DXF File
void CFileDWGDirect::Load(char* szPathKey, const CUPDUPDATA*)
{
	try
	{
		pDb = svcs.readFile(szPathKey, false, false);  // no conversion

        CString FileAndVersion;
		FileAndVersion.Format("Opened <%s> (Version: %d)\n", (LPCWSTR) pDb->getFilename(), pDb->originalFileVersion());
	}
	catch(OdError ex)
	{
		try
		{
			OdSmartPtr<OdRdFileBuf> pFile = OdRdFileBuf::createObject();
			pFile->open(szPathKey, Oda::kShareDenyNo, Oda::kFileRead, Oda::kOpenExisting);
			pDb = svcs.recoverFile(pFile);
		}
		catch(...)
		{
			MessageBox(0, "File has errors", "Error", 0);
			return;
		}
	}

	if(pDb.isNull())
		return;

	//* Traverse Block Table to get layout names
	OdDbBlockTablePtr pBlocks = pDb->getBlockTableId().safeOpenObject();	  
	OdDbSymbolTableIteratorPtr pBlkIter = pBlocks->newIterator();
	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
	{
		OdDbBlockTableRecordPtr pBlock = pBlkIter->getRecordId().safeOpenObject();
		if(pBlock->isLayout())
		{
			//OdDbObjectId LayoutId = pDb->findLayoutNamed(pBlock->getName());
			OdDbLayoutPtr pLayout = pBlock->getLayoutId().safeOpenObject();
			AcadMode::strLayouts.push_back((LPCTSTR)pLayout->getLayoutName());
			//AcadMode::strLayouts.push_back((LPCTSTR)pBlock->getName());
		}
	}

	/////////////////////////////
	OdDbTextStyleTablePtr pTable = pDb->getTextStyleTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pTableIter = pTable->newIterator();
	for(pTableIter->start(); !pTableIter->done(); pTableIter->step())
	{
		OdDbTextStyleTableRecordPtr txtStyle = pTableIter->getRecordId().safeOpenObject();

		if (txtStyle->getName() == "Standard" && !txtStyle->isShapeFile() && !txtStyle.isNull()) 
		{
			//txtStyle->setBigFontFileName("");
			//txtStyle->setFileName("RomanS.shx");
			//txtStyle->setFont("Verdana", false, false, 0, 0x22);
		}
	}
	/////////////////////////////////////

	//* prompt user to select properties/mode
	if(DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ACADSELECT), app.GetSafeHwnd(), DlgProcLayoutSelect) != IDOK)
		return;

	AcadMode::strLayouts.clear();

	if(AcadMode::bVectorize)
		LoadPrimVectorize();
	else
		LoadPrimData();
}

void CFileDWGDirect::LoadPrimData()
{
////DumpHeader();
//	DumpViewportTable();
////DumpViewTable();
	DumpLineTypeTable();
	DumpLayerTable();

	// explode block references
	//ExplodBlockRefs();
	// explode block table
	//ExplodeBlockTable();

	DumpBlockTable();
	DumpBlocks();

	if(istate.m_bIsImport && istate.GetViewports())
		DumpMSBlocks();

//	DumpTextStyleTable();
//	DumpRegAppTable();
//	DumpDimStyleTable();
//	DumpUCSTable();
//	DumpMLineStyles();
//	DumpObject(pDb->getNamedObjectsDictionaryId(), "Named Objects Dictionary", "  ");
}

void CFileDWGDirect::LoadPrimVectorize()
{
////DumpHeader();
//	DumpViewportTable();
////DumpViewTable();
//	DumpLineTypeTable();
	DumpLayerTable();
//	DumpBlockTable();
//	DumpBlocks();

//	DumpTextStyleTable();
//	DumpRegAppTable();
//	DumpDimStyleTable();
//	DumpUCSTable();
//	DumpMLineStyles();
//	DumpObject(pDb->getNamedObjectsDictionaryId(), "Named Objects Dictionary", "  ");

	//* Create the vectorization context.  This class defines the operations and 
	//* properties that are used in the DWGdirect vectorization of an OdDbDatabase.
	OdGiContextForDbDatabasePtr pDwgContext = OdGiContextForDbDatabase::createObject();

	//* Create the custom rendering device, and set the output stream for the device.
	//* This class implements bitmapped GUI display windows.
	OdGsDevicePtr pDevice = ExGsSimpleDevice::createObject(ExGsSimpleDevice::k3dDevice);

	//* Set the database to be vectorized.
	pDwgContext->setDatabase(pDb);

	//* find the correct layout
	OdDbBlockTablePtr pBlocks = pDb->getBlockTableId().safeOpenObject();	  
	// Get an iterator for the block table
	OdDbSymbolTableIteratorPtr pBlkIter = pBlocks->newIterator();
	// For each block in the block table
	OdDbBlockTableRecordPtr pBlock;
	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
	{
		pBlock = pBlkIter->getRecordId().safeOpenObject();
		if((pBlock->isLayout()) && (pBlock->getName() == AcadMode::strSelectedLayout.GetBuffer()))
			break;
	}

	//* Prepare the device to render the active layout in this database.
	pDevice = OdDbGsManager::setupLayoutViews(pBlock->getLayoutId(), pDevice, pDwgContext);

	//* Set the screen size for the generated (vectorized) geometry.
	OdGsDCRect screenRect(OdGsDCPoint(0, 0), OdGsDCPoint(1000, 1000));
	pDevice->onSize(screenRect);

	//* Initiate Vectorization
	pDevice->update();
}

void CFileDWGDirect::DumpHeader()//OdDbDatabase* pDb, OdDbHostAppServices *pHostApp,STD(ostream) &os)
{
	OdString sName = pDb->getFilename();
//	os << "Database was loaded from: " << sName.c_str() << STD(endl);

//	OdDb::DwgVersion vVer = pDb->originalFileVersion();
//	os << "File version is: " << OdDb::DwgVersionToStr(vVer) << STD(endl);

	//os << "Header Variables:" << STD(endl);
	//os << "  LTSCALE: " << pDb->getLTSCALE() << STD(endl);
	//os << "  ATTMODE: " << pDb->getATTMODE() << STD(endl);

	//#ifdef  DUMP_ALL_HEADERVARS
	//os << "  DIMASO: "        << pDb->getDIMASO()         << STD(endl);               
	//os << "  DIMSHO: "        << pDb->getDIMSHO()         << STD(endl);               
	//os << "  PLINEGEN: "      << pDb->getPLINEGEN()       << STD(endl);                
	//os << "  ORTHOMODE: "     << pDb->getORTHOMODE()      << STD(endl);               
	//os << "  REGENMODE: "     << pDb->getREGENMODE()      << STD(endl);               
	//os << "  FILLMODE: "      << pDb->getFILLMODE()       << STD(endl);                
	//os << "  QTEXTMODE: "     << pDb->getQTEXTMODE()      << STD(endl);               
	//os << "  PSLTSCALE: "     << pDb->getPSLTSCALE()      << STD(endl);               
	//os << "  LIMCHECK: "      << pDb->getLIMCHECK()       << STD(endl);                
	//os << "  USRTIMER: "      << pDb->getUSRTIMER()       << STD(endl);               
	//os << "  SKPOLY: "        << pDb->getSKPOLY()         << STD(endl);               
	//os << "  ANGDIR: "        << pDb->getANGDIR()         << STD(endl);               
	//os << "  SPLFRAME: "      << pDb->getSPLFRAME()       << STD(endl);               
	//os << "  MIRRTEXT: "      << pDb->getMIRRTEXT()       << STD(endl);                
	//os << "  WORLDVIEW: "     << pDb->getWORLDVIEW()      << STD(endl);               
	//os << "  TILEMODE: "      << pDb->getTILEMODE()       << STD(endl);                
	//os << "  PLIMCHECK: "     << pDb->getPLIMCHECK()      << STD(endl);               
	//os << "  VISRETAIN: "     << pDb->getVISRETAIN()      << STD(endl);               
	//os << "  DISPSILH: "      << pDb->getDISPSILH()       << STD(endl);               
	//os << "  PELLIPSE: "      << pDb->getPELLIPSE()       << STD(endl);               
	//os << "  PROXYGRAPHICS: " << pDb->getPROXYGRAPHICS()  << STD(endl);                
	//os << "  TREEDEPTH: "     << pDb->getTREEDEPTH()      << STD(endl);               
	//os << "  LUNITS: "        << pDb->getLUNITS()         << STD(endl);               
	//os << "  LUPREC: "        << pDb->getLUPREC()         << STD(endl);                   
	//os << "  AUNITS: "        << pDb->getAUNITS()         << STD(endl);                                   
	//os << "  AUPREC: "        << pDb->getAUPREC()         << STD(endl);                   
	//os << "  ATTMODE: "       << pDb->getATTMODE()        << STD(endl);                   
	//os << "  PDMODE: "        << pDb->getPDMODE()         << STD(endl);           
	//os << "  USERI1: "        << pDb->getUSERI1()         << STD(endl);           
	//os << "  USERI2: "        << pDb->getUSERI2()         << STD(endl);           
	//os << "  USERI3: "        << pDb->getUSERI3()         << STD(endl);           
	//os << "  USERI4: "        << pDb->getUSERI4()         << STD(endl);           
	//os << "  USERI5: "        << pDb->getUSERI5()         << STD(endl);           
	//os << "  SPLINESEGS: "    << pDb->getSPLINESEGS()     << STD(endl);           
	//os << "  SURFU: "         << pDb->getSURFU()          << STD(endl);           
	//os << "  SURFV: "         << pDb->getSURFV()          << STD(endl);           
	//os << "  SURFTYPE: "      << pDb->getSURFTYPE()       << STD(endl);           
	//os << "  SURFTAB1: "      << pDb->getSURFTAB1()       << STD(endl);           
	//os << "  SURFTAB2: "      << pDb->getSURFTAB2()       << STD(endl);           
	//os << "  SPLINETYPE: "    << pDb->getSPLINETYPE()     << STD(endl);           
	//os << "  SHADEDGE: "      << pDb->getSHADEDGE()       << STD(endl);           
	//os << "  SHADEDIF: "      << pDb->getSHADEDIF()       << STD(endl);           
	//os << "  UNITMODE: "      << pDb->getUNITMODE()       << STD(endl);           
	//os << "  MAXACTVP: "      << pDb->getMAXACTVP()       << STD(endl);           
	//os << "  ISOLINES: "      << pDb->getISOLINES()       << STD(endl);           
	//os << "  CMLJUST: "       << pDb->getCMLJUST()        << STD(endl);               
	//os << "  TEXTQLTY: "      << pDb->getTEXTQLTY()       << STD(endl);          
	//os << "  LTSCALE: "       << pDb->getLTSCALE()        << STD(endl);           
	//os << "  TEXTSIZE: "      << pDb->getTEXTSIZE()       << STD(endl);          
	//os << "  TRACEWID: "      << pDb->getTRACEWID()       << STD(endl);       
	//os << "  SKETCHINC: "     << pDb->getSKETCHINC()      << STD(endl);       
	//os << "  FILLETRAD: "     << pDb->getFILLETRAD()      << STD(endl);       
	//os << "  THICKNESS: "     << pDb->getTHICKNESS()      << STD(endl);        
	//os << "  ANGBASE: "       << pDb->getANGBASE()        << STD(endl);        
	//os << "  PDSIZE: "        << pDb->getPDSIZE()         << STD(endl);             
	//os << "  PLINEWID: "      << pDb->getPLINEWID()       << STD(endl);           
	//os << "  USERR1: "        << pDb->getUSERR1()         << STD(endl);         
	//os << "  USERR2: "        << pDb->getUSERR2()         << STD(endl);         
	//os << "  USERR3: "        << pDb->getUSERR3()         << STD(endl);         
	//os << "  USERR4: "        << pDb->getUSERR4()         << STD(endl);         
	//os << "  USERR5: "        << pDb->getUSERR5()         << STD(endl);         
	//os << "  CHAMFERA: "      << pDb->getCHAMFERA()       << STD(endl);           
	//os << "  CHAMFERB: "      << pDb->getCHAMFERB()       << STD(endl);         
	//os << "  CHAMFERC: "      << pDb->getCHAMFERC()       << STD(endl);         
	//os << "  CHAMFERD: "      << pDb->getCHAMFERD ()      << STD(endl);
	//os << "  FACETRES: "      << pDb->getFACETRES()       << STD(endl);         
	//os << "  CMLSCALE: "      << pDb->getCMLSCALE()       << STD(endl);         
	//os << "  CELTSCALE: "     << pDb->getCELTSCALE()      << STD(endl);            
	//os << "  ELEVATION: "     << pDb->getELEVATION()      << STD(endl);      

	//os << "  UCSNAME: "       << pDb->getUCSNAME()        << STD(endl);      
	//os << "  PUCSNAME: "      << pDb->getPUCSNAME()       << STD(endl);         
	//os << "  PELEVATION: "    << pDb->getPELEVATION()     << STD(endl);            
	//os << "  PINSBASE: "      << pDb->getPINSBASE().x     << "," << pDb->getPINSBASE().y <<  "," << pDb->getPINSBASE().z << STD(endl);           
	//os << "  PEXTMIN: "       << pDb->getPEXTMIN().x      << "," << pDb->getPEXTMIN().y  <<  "," << pDb->getPEXTMIN().z << STD(endl);          
	//os << "  PEXTMAX: "       << pDb->getPEXTMAX().x      << "," << pDb->getPEXTMAX().y  <<  "," << pDb->getPEXTMAX().z << STD(endl);         
	//os << "  PLIMMIN: "       << pDb->getPLIMMIN().x      << "," << pDb->getPLIMMIN().y  <<  "," << STD(endl);         
	//os << "  PLIMMAX: "       << pDb->getPLIMMAX().x      << "," << pDb->getPLIMMAX().y  <<  "," << STD(endl);         
	//os << "  PUCSORG: "       << pDb->getPUCSORG().x      << "," << pDb->getPUCSORG().y  <<  "," << pDb->getPUCSORG().z << STD(endl);          
	//os << "  PUCSXDIR: "      << pDb->getPUCSXDIR().x     << "," << pDb->getPUCSXDIR().y <<  "," << pDb->getPUCSXDIR().z << STD(endl);         
	//os << "  PUCSYDIR: "      << pDb->getPUCSYDIR().x     << "," << pDb->getPUCSYDIR().y <<  "," << pDb->getPUCSYDIR().z  << STD(endl);          
	//os << "  INSBASE: "       << pDb->getINSBASE().x      << "," << pDb->getINSBASE().y  <<  "," << pDb->getINSBASE().z << STD(endl);         
	//os << "  EXTMIN: "        << pDb->getEXTMIN().x       << "," << pDb->getEXTMIN().y   <<  "," << pDb->getEXTMIN().z << STD(endl);         
	//os << "  EXTMAX: "        << pDb->getEXTMAX().x       << "," << pDb->getEXTMAX().y   <<  "," << pDb->getEXTMAX().z << STD(endl);          
	//os << "  LIMMIN: "        << pDb->getLIMMIN().x       << "," << pDb->getLIMMIN().y   <<  "," << STD(endl);          
	//os << "  LIMMAX: "        << pDb->getLIMMAX().x       << "," << pDb->getLIMMAX().y   <<  "," << STD(endl);        
	//os << "  UCSORG: "        << pDb->getUCSORG().x       << "," << pDb->getUCSORG().y   <<  "," << pDb->getUCSORG().z  << STD(endl);       
	//os << "  UCSXDIR: "       << pDb->getUCSXDIR().x      << "," << pDb->getUCSXDIR().y  <<  "," << pDb->getUCSXDIR().z << STD(endl);      
	//os << "  UCSYDIR: "       << pDb->getUCSYDIR().x      << "," << pDb->getUCSYDIR().y  <<  "," << pDb->getUCSYDIR().z << STD(endl);      

	//os << "  Dimscale: "      << pDb->dimscale()          << STD(endl);
	//os << "  Dimasz  : "      << pDb->dimasz()            << STD(endl);
	//os << "  Dimexo  : "      << pDb->dimexo()            << STD(endl);
	//os << "  Dimdli  : "      << pDb->dimdli()            << STD(endl); 
	//os << "  Dimexe  : "      << pDb->dimexe()            << STD(endl);
	//os << "  Dimrnd  : "      << pDb->dimrnd()            << STD(endl); 
	//os << "  Dimdle  : "      << pDb->dimdle()            << STD(endl); 
	//os << "  Dimtp   : "      << pDb->dimtp()             << STD(endl); 
	//os << "  Dimtm   : "      << pDb->dimtm()             << STD(endl); 
	//os << "  Dimtxt  : "      << pDb->dimtxt()            << STD(endl); 
	//os << "  Dimcen  : "      << pDb->dimcen()            << STD(endl); 
	//os << "  Dimtsz  : "      << pDb->dimtsz()            << STD(endl); 
	//os << "  Dimaltf : "      << pDb->dimaltf()           << STD(endl); 
	//os << "  Dimlfac : "      << pDb->dimlfac()           << STD(endl); 
	//os << "  Dimtvp  : "      << pDb->dimtvp()            << STD(endl);   
	//os << "  Dimtfac : "      << pDb->dimtfac()           << STD(endl); 
	//os << "  Dimgap  : "      << pDb->dimgap()            << STD(endl); 

	//os << "  dimadec: "       << pDb->dimadec()           << STD(endl);
	//os << "  dimalt: "        << pDb->dimalt()            << STD(endl);
	//os << "  dimaltd: "       << pDb->dimaltd()           << STD(endl);
	//os << "  dimaltf: "       << pDb->dimaltf()           << STD(endl);
	//os << "  dimaltrn: "      << pDb->dimaltrnd()         << STD(endl);
	//os << "  dimalttd: "      << pDb->dimalttd()          << STD(endl);
	//os << "  dimalttz: "      << pDb->dimalttz()          << STD(endl);
	//os << "  dimaltu: "       << pDb->dimaltu()           << STD(endl);
	//os << "  dimaltz: "       << pDb->dimaltz()           << STD(endl);
	//os << "  dimapost: "      << pDb->dimapost()          << STD(endl);
	//os << "  dimasz: "        << pDb->dimasz()            << STD(endl);
	//os << "  dimatfit: "      << pDb->dimatfit()          << STD(endl);
	//os << "  dimaunit: "      << pDb->dimaunit()          << STD(endl);
	//os << "  dimazin: "       << pDb->dimazin()           << STD(endl);
	//os << "  dimblk: "        << pDb->dimblk()            << STD(endl); 
	//os << "  dimblk1: "       << pDb->dimblk1()           << STD(endl);
	//os << "  dimblk2: "       << pDb->dimblk2()           << STD(endl);
	//os << "  dimcen: "        << pDb->dimcen()            << STD(endl); 
	//os << "  dimclrd: "       << pDb->dimclrd().color()   << STD(endl);
	//os << "  dimclre: "       << pDb->dimclre().color()   << STD(endl);
	//os << "  dimclrt: "       << pDb->dimclrt().color()   << STD(endl);
	//os << "  dimdec: "        << pDb->dimdec()            << STD(endl); 
	//os << "  dimdle: "        << pDb->dimdle()            << STD(endl);
	//os << "  dimdli: "        << pDb->dimdli()            << STD(endl);
	//os << "  dimexe: "        << pDb->dimexe()            << STD(endl);
	//os << "  dimdsep: "       << pDb->dimdsep()           << STD(endl); 
	//os << "  dimexo: "        << pDb->dimexo()            << STD(endl);
	//os << "  dimfrac: "       << pDb->dimfrac()           << STD(endl);
	//os << "  dimgap: "        << pDb->dimgap()            << STD(endl); 
	//os << "  dimjust: "       << pDb->dimjust()           << STD(endl);
	//os << "  dimldrblk: "     << pDb->dimldrblk()         << STD(endl);
	//os << "  dimlfac: "       << pDb->dimlfac()           << STD(endl);
	//os << "  dimlim: "        << pDb->dimlim()            << STD(endl);
	//os << "  dimlunit: "      << pDb->dimlunit()          << STD(endl);
	//os << "  dimlwd: "        << pDb->dimlwd()            << STD(endl); 
	//os << "  dimlwe: "        << pDb->dimlwe()            << STD(endl); 
	//os << "  dimpost: "       << pDb->dimpost()           << STD(endl); 
	//os << "  dimrnd: "        << pDb->dimrnd()            << STD(endl); 
	//os << "  dimsah: "        << pDb->dimsah()            << STD(endl); 
	//os << "  dimscale: "      << pDb->dimscale()          << STD(endl); 
	//os << "  dimsd1: "        << pDb->dimsd1()            << STD(endl); 
	//os << "  dimsd2: "        << pDb->dimsd2()            << STD(endl); 
	//os << "  dimse1: "        << pDb->dimse1()            << STD(endl); 
	//os << "  dimse2: "        << pDb->dimse2()            << STD(endl); 
	//os << "  dimsoxd: "       << pDb->dimsoxd()           << STD(endl);
	//os << "  dimtad: "        << pDb->dimtad()            << STD(endl); 
	//os << "  dimtdec: "       << pDb->dimtdec()           << STD(endl); 
	//os << "  dimtfac: "       << pDb->dimtfac()           << STD(endl); 
	//os << "  dimtih: "        << pDb->dimtih()            << STD(endl);  
	//os << "  dimtix: "        << pDb->dimtix()            << STD(endl);  
	//os << "  dimtm: "         << pDb->dimtm()             << STD(endl);   
	//os << "  dimtmove: "      << pDb->dimtmove()          << STD(endl); 
	//os << "  dimtofl: "       << pDb->dimtofl()           << STD(endl); 
	//os << "  dimtoh: "        << pDb->dimtoh()            << STD(endl);  
	//os << "  dimtol: "        << pDb->dimtol()            << STD(endl); 
	//os << "  dimtolj: "       << pDb->dimtolj()           << STD(endl);
	//os << "  dimtp: "         << pDb->dimtp()             << STD(endl);  
	//os << "  dimtsz: "        << pDb->dimtsz()            << STD(endl);  
	//os << "  dimtvp: "        << pDb->dimtvp()            << STD(endl);  
	//os << "  dimtxsty: "      << pDb->dimtxsty()          << STD(endl);  
	//os << "  dimtxt: "        << pDb->dimtxt()            << STD(endl);   
	//os << "  dimtzin: "       << pDb->dimtzin()           << STD(endl); 
	//os << "  dimupt: "        << pDb->dimupt()            << STD(endl);  
	//os << "  dimzin: "        << pDb->dimzin()            << STD(endl);  

	//os << "  ATTREQ: "        << pHostApp->getATTREQ()    << STD(endl);       
	//os << "  ATTDIA: "        << pHostApp->getATTDIA()    << STD(endl);        
	//os << "  BLIPMODE: "      << pHostApp->getBLIPMODE()  << STD(endl);      
	//os << "  DELOBJS: "       << pHostApp->getDELOBJS()   << STD(endl);       
	//os << "  COORDS: "        << pHostApp->getCOORDS()    << STD(endl);        
	//os << "  DRAGMODE: "      << pHostApp->getDRAGMODE()  << STD(endl);      
	//os << "  OSMODE: "        << pHostApp->getOSMODE()    << STD(endl);        
	//os << "  PICKSTYLE: "     << pHostApp->getPICKSTYLE() << STD(endl);     
	//os << "  LWDEFAULT: "     << pHostApp->getLWDEFAULT() << STD(endl);      
	//os << "  FONTALT: "       << pHostApp->getFONTALT()   << STD(endl);       
	//os << "  PLINETYPE: "     << pHostApp->getPLINETYPE() << STD(endl);     
	//os << "  SAVEROUNDTRIP: " << pHostApp->getSAVEROUNDTRIP()   << STD(endl); 
	//os << "  LWDISPSCALE: "   << pHostApp->getLWDISPSCALE()     << STD(endl);   
	//os << "  PREVIEW_WIDTH: " << pHostApp->getPREVIEW_WIDTH()   << STD(endl);   
	//os << "  PREVIEW_HEIGHT: "<< pHostApp->getPREVIEW_HEIGHT()  << STD(endl); 
	//os << "  GRIPHOVER: "     << pHostApp->getGRIPHOVER() << STD(endl);      
	//os << "  GRIPOBJLIMIT: "  << pHostApp->getGRIPOBJLIMIT()    << STD(endl);  
	//os << "  GRIPTIPS: "      << pHostApp->getGRIPTIPS()  << STD(endl);       
	//os << "  HPASSOC: "       << pHostApp->getHPASSOC()   << STD(endl);        
	//os << "  LOGFILEMODE: "   << pHostApp->getLOGFILEMODE()     << STD(endl);    
	//os << "  LOCALROOTPREFIX: " << pHostApp->getLOCALROOTPREFIX()  << STD(endl); 
	//os << "  MAXHATCHDENSITY: " << pHostApp->getMAXHATCHDENSITY()  << STD(endl);
//	#endif
	OdDbDate d = pDb->getTDCREATE();
	short month, day, year, hour, min, sec, msec;
	d.getDate(month, day, year);
	d.getTime(hour, min, sec, msec);
//	os << "  TDCREATE: " << month << "-" << day << "-" << year << ", " << hour << ":" << min << ":" << sec << STD(endl);

	d = pDb->getTDUPDATE();
	d.getDate(month, day, year);
	d.getTime(hour, min, sec, msec);
//	os << "  TDUPDATE: " << month << "-" << day << "-" << year << ", " << hour << ":" << min << ":" << sec << STD(endl);
}

void CFileDWGDirect::DumpViewportTable()
{
	m_iVpCount = 0;

	OdDbBlockTablePtr pBlockTable = pDb->getBlockTableId().safeOpenObject();
	OdDbSymbolTableIteratorPtr pBlockIter = pBlockTable->newIterator();
	while(!pBlockIter->done())
	{
		OdDbBlockTableRecordPtr pBlockTableRecord = pBlockIter->getRecord(OdDb::kForWrite);
		if(!pBlockTableRecord->isAnonymous() && !pBlockTableRecord->isFromExternalReference())
		{
			OdDbObjectIteratorPtr pEntIter = pBlockTableRecord->newIterator();
			while(!pEntIter->done())
			{ 
				OdDbEntityPtr pEnt = pEntIter->entity();
				pEntIter->step();

				if(pEnt->isKindOf(OdDbViewport::desc()))// && (m_iVpCount < 17))
				{
					OdDbViewportPtr pView = pEnt;
					
					m_av[m_iVpCount] = CAbstractView(pView);
					m_vp[m_iVpCount] = CViewport(pView);
							
					view::PushActive();
					view::LookAt(m_av[m_iVpCount].GetEye(), m_av[m_iVpCount].GetTarget(), m_av[m_iVpCount].GetVwUp());
					view::SetNearClipDistance(m_av[m_iVpCount].GetNearClipDistance());
					view::SetFarClipDistance(m_av[m_iVpCount].GetFarClipDistance());
					view::SetWnd(m_av[m_iVpCount].GetUMin(), m_av[m_iVpCount].GetVMin(), m_av[m_iVpCount].GetUMax(), m_av[m_iVpCount].GetVMax());
					m_tmView[m_iVpCount] = view::GetMatrix();
					view::PopActive();
			
					double dXCenter;
					double dYCenter;
					m_av[m_iVpCount].GetCenter(dXCenter, dYCenter);
					m_tm[m_iVpCount].Translate(CVec(- dXCenter, - dYCenter, 0.));
							
					double dScale = m_vp[m_iVpCount].GetHeight() / m_av[m_iVpCount].GetHeight();
					m_tm[m_iVpCount].Scale(CVec(dScale, dScale, 1.));
					CPnt ptCenter = m_vp[m_iVpCount].GetCenter();
					m_tm[m_iVpCount].Translate(CVec(ptCenter[0], ptCenter[1], 0.));
					m_iVpCount++;
				}
			}
		}
		pBlockIter->step();
	}
}

void CFileDWGDirect::DumpViewTable()
{
//	OdDbObjectId tableId = pDb->getViewportTableId();
//	OdDbViewTablePtr pVTable = tableId.safeOpenObject();
////	OdDbSymbolTablePtr pTable = tableId.safeOpenObject();
//
////	os << STD(endl) << pTable->isA()->name() << STD(endl);
//	OdDbSymbolTableIteratorPtr pIter = pVTable->newIterator();
//	for (pIter->start(); !pIter->done(); pIter->step())
//	{
//		OdDbViewTableRecordPtr pVTableRec = pIter->getRecordId().safeOpenObject();
////		OdDbSymbolTableRecordPtr pTableRec = pIter->getRecordId().safeOpenObject();
//
////		pVTableRec->
////		os << "  " << pTableRec->getName().c_str() << " <" << 
////		pTableRec->isA()->name() << ">" << STD(endl);
//	}
}

void CFileDWGDirect::DumpLineTypeTable()
{
	OdDbObjectId tableId = pDb->getLinetypeTableId();
	OdDbLinetypeTablePtr pLineTable = tableId.safeOpenObject();

	OdDbLinetypeTableIteratorPtr pIter = pLineTable->newIterator();
	for (pIter->start(); !pIter->done(); pIter->step())
	{
		OdDbLinetypeTableRecordPtr pLineTableRec = pIter->getRecordId().safeOpenObject();

		//if(CPegDoc::GetDoc()->PenStylesLookup(pLineTableRec->getName().c_str()) != PENSTYLE_LookupErr)
		if(CPegDoc::GetDoc()->PenStylesLookup((LPCTSTR)pLineTableRec->getName()) != PENSTYLE_LookupErr)
			continue;

		WORD nDashLens = (WORD)pLineTableRec->numDashes();
		double* dDashLen = new double[nDashLens];

		for(short n = 0; n < nDashLens; n++)
			dDashLen[n] = pLineTableRec->dashLengthAt(n);

		CPenStyle *pPenStyle = new CPenStyle((LPCTSTR)pLineTableRec->getName(), (LPCTSTR)pLineTableRec->getName(), nDashLens, dDashLen);
		CPegDoc::GetDoc()->PenStylesAdd(pPenStyle);
			
		delete [] dDashLen;
	}
}

void CFileDWGDirect::DumpLayerTable()
{
	// Layer table smart pointer, opened for read.
	OdDbLayerTablePtr pLayers = pDb->getLayerTableId().safeOpenObject();

	// Get a new layer table iterator (as a smart pointer)
	OdDbSymbolTableIteratorPtr pIter = pLayers->newIterator();
	for(pIter->start(); !pIter->done(); pIter->step())
	{
		// Layer Record smart pointer, opened for read.
		OdDbLayerTableRecordPtr pLayer = pIter->getRecordId().safeOpenObject();

		if(CPegDoc::GetDoc()->LayersLookup((LPCTSTR)pLayer->getName()) < 0)
		{
			CLayer* pPegLayer = new CLayer((LPCTSTR)pLayer->getName());
			
			pPegLayer->SetPenColor((PENCOLOR)abs(pLayer->colorIndex()));
			pPegLayer->SetPenStyleName("Continuous");

			if(pLayer->colorIndex() < 0 || pLayer->isFrozen() || pLayer->isOff())
				pPegLayer->SetStateOff();
			
			CPegDoc::GetDoc()->LayersAdd(pPegLayer);
		}
	}
}

void CFileDWGDirect::ExplodBlockRefs()
{
	
}

void CFileDWGDirect::ExplodeBlockTable()
{
//		// Open the block table
//	OdDbBlockTablePtr pBlocks = pDb->getBlockTableId().safeOpenObject();
//	  
//	// Get an iterator for the block table
//	OdDbSymbolTableIteratorPtr pBlkIter = pBlocks->newIterator();
//	  
//	// For each block in the block table
//	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
//	{
//		// Open the block
//		OdDbBlockTableRecordPtr pBlock = pBlkIter->getRecordId().safeOpenObject();
//
//		// mak - dimension not displaying
////		if((pBlock->getName() == "*Model_Space") || (pBlock->getName() == "*MODEL_SPACE"))
//
//		OdDbObjectId id = pBlock->getLayoutId();
//		if(id.isNull()) continue;
//
//		// Get an entity iterator
//		OdDbObjectIteratorPtr pEntIter = pBlock->newIterator();
//	    
//		// For each entity in the block
//		for(; !pEntIter->done(); pEntIter->step
}

void CFileDWGDirect::DumpBlockTable()
{
	//Load all block containers, local or external. An external reference contains the
	//name and the filename of the external drawing. Local blocks containers are an unordered list
	//drawing entities. The two type of local block containers are layout and non-layout.

	// Open the block table
	OdDbBlockTablePtr pBlocks = pDb->getBlockTableId().safeOpenObject();
	  
	// Get an iterator for the block table
	OdDbSymbolTableIteratorPtr pBlkIter = pBlocks->newIterator();
	  
	// For each block in the block table
	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
	{
		// Open the block
		OdDbBlockTableRecordPtr pBlock = pBlkIter->getRecordId().safeOpenObject();

//		if(pBlock->isLayout())
//			continue;

		//if(pBlock->getName() == "bradley1 side")
		//{
		//	int isdfs = -1;
		//	isdfs += 2;
		//}

		// Get an entity iterator
		OdDbObjectIteratorPtr pEntIter = pBlock->newIterator();
	    
		// For each entity in the block
		for(; !pEntIter->done(); pEntIter->step())
		{
			CBlock* pPegBlock;
			if(!CPegDoc::GetDoc()->BlksLookup((LPCTSTR)pBlock->getName(), pPegBlock))
			{
				//		b0 set - anonymous block
				//		b1 set - block has attribute definitions
				//		b2 set - block is an external reference
				//		b3 set - not used
				//		b4 set - block is externally dependent
				//		b5 set - block is a resolved external reference
				//		b6 set - definition is referenced
				WORD wFlag = 0;
				if(pBlock->isAnonymous())				{wFlag |= 0x01;}
				if(pBlock->hasAttributeDefinitions())	{wFlag |= 0x02;}
				if(pBlock->isFromExternalReference())	{wFlag |= 0x04;}

				CPnt ptBase(pBlock->origin().x, pBlock->origin().y, pBlock->origin().z);
				pPegBlock = new CBlock(wFlag, ptBase, (LPCTSTR)pBlock->pathName());
				CPegDoc::GetDoc()->BlksSetAt((LPCTSTR)pBlock->getName(), pPegBlock);
			}

			//////////////////////////////////////////////////////////////////////////////////
			// Open the entity
			OdDbEntityPtr pEnt = pEntIter->objectId().safeOpenObject();//id.safeOpenObject();

			// Retrieve the registered protocol extension object registered for this object type.
			OdSmartPtr<OdDbEntity_Dumper> pEntDumper = pEnt;
			CSeg* pSeg = pEntDumper->Dump(pEnt);

			if(pSeg != 0)
			{
				// 0 indicates a color of BYBLOCK.
				// 256 indicates a color of BYLAYER.
				PENCOLOR nPenColor = pEnt->colorIndex();
				if(nPenColor == 0) 
					nPenColor = 7;
				else if(nPenColor == 256) 
					nPenColor = CPrim::PENCOLOR_BYLAYER;
				
				PENSTYLE nPenStyle = 0;
				if((!pEnt->isKindOf(OdDbFace::desc())) && (!pEnt->isKindOf(OdDbSolid::desc())) && (!pEnt->isKindOf(OdDbHatch::desc())))
				{
					if(Prim_IsByBlockPenStyle(pEnt->linetype())) 
						nPenStyle = 1;
					else if(Prim_IsByLayerPenStyle(pEnt->linetype())) 
						nPenStyle = CPrim::PENSTYLE_BYLAYER;
					else 
						nPenStyle = CPegDoc::GetDoc()->PenStylesLookup((LPCTSTR)pEnt->linetype());
				}

				POSITION pos = pSeg->GetHeadPosition();
				while(pos != 0)
				{
					CPrim *pPrim = pSeg->GetNext(pos);

					if((pPrim != 0) && (pPegBlock != 0))
					{
						pPrim->SetPenColor(nPenColor);
						pPrim->SetPenStyle(nPenStyle);

						CPrim *pPrimCopy;
						pPrim->Copy(pPrimCopy);
						pPegBlock->AddTail(pPrimCopy);
					}
				}
			}
		}
	}
}

void CFileDWGDirect::DumpBlocks()
{
	// Open the block table
	OdDbBlockTablePtr pBlocks = pDb->getBlockTableId().safeOpenObject();
	  
	// Get an iterator for the block table
	OdDbSymbolTableIteratorPtr pBlkIter = pBlocks->newIterator();
	  
	// For each block in the block table
	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
	{
		// Open the block
		OdDbBlockTableRecordPtr pBlock = pBlkIter->getRecordId().safeOpenObject();

		// mak - dimension not displaying
//		if((pBlock->getName() == "*Model_Space") || (pBlock->getName() == "*MODEL_SPACE"))

		OdDbObjectId id = pBlock->getLayoutId();
		if(id.isNull()) continue;

		OdDbLayoutPtr pLayout = pBlock->getLayoutId().safeOpenObject();
		//if(pBlock->getName() == AcadMode::strSelectedLayout.GetBuffer())
		if(pLayout->getLayoutName() == AcadMode::strSelectedLayout.GetBuffer())
		{

			OdDbObjectId LayoutId = pDb->findLayoutNamed(pBlock->getName());
			if(!LayoutId.isNull())
			{
//				OdDbLayoutPtr pLayout = LayoutId.safeOpenObject();

//				OdDbObjectId BlockId = pLayout->getBlockTableRecordId();
//				OdDbBlockTableRecordPtr BlockId.safeOpenObject();
			}

			// Get an entity iterator
			OdDbObjectIteratorPtr pEntIter = pBlock->newIterator();
		    
			// For each entity in the block
			for(; !pEntIter->done(); pEntIter->step())
			{
				OdDbEntityPtr pEnt3 = pEntIter->objectId().safeOpenObject();

				// explode block refs
				if( ((pEnt3->isA()->name() == "AcDbBlockReference")/* || 
					 (pEnt3->isA()->name() == "AcDbHatch") ||
					 (pEnt3->isA()->name() == "AcDbPolyline")*/) && 
					 (istate.GetExplode() && istate.m_bIsImport))
				{
					OdRxObjectPtrArray entitySet;
					pEnt3->explode(entitySet);

					for(unsigned int i = 0; i < entitySet.size(); i++)
					{
						OdDbEntityPtr pEntity = entitySet.getAt(i);
						DumpEntity(pDb->addOdDbObject(pEntity));
						//pEntity.release();
					}
				}
				else
				{
					//DumpEntity(pEntIter->objectId());
					DumpEntity(pEnt3->objectId());
				}
			}
		}
	}
}

bool isOverallVp(const OdDbEntity* pObj)
{
	OdDbViewportPtr pVp = OdDbViewport::cast(pObj);
	if(pVp.get() && pVp->number()==1)
		return true;
	return false;
}

void CFileDWGDirect::DumpMSBlocks()
{
	// Open the block table
	OdDbBlockTablePtr pBlocks = pDb->getBlockTableId().safeOpenObject();
	  
	// Get an iterator for the block table
	OdDbSymbolTableIteratorPtr pBlkIter = pBlocks->newIterator();
	  
	// For each block in the block table
	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
	{
		// Open the block
		OdDbBlockTableRecordPtr pBlock = pBlkIter->getRecordId().safeOpenObject();

		// mak - dimension not displaying
//		if((pBlock->getName() == "*Model_Space") || (pBlock->getName() == "*MODEL_SPACE"))

		OdDbObjectId id = pBlock->getLayoutId();
		if(id.isNull()) continue;

		OdDbLayoutPtr pLayout = pBlock->getLayoutId().safeOpenObject();
		//if(pBlock->getName() == AcadMode::strSelectedLayout.GetBuffer())
		if(pLayout->getLayoutName() == AcadMode::strSelectedLayout.GetBuffer())
		{

			OdDbObjectId LayoutId = pDb->findLayoutNamed(pBlock->getName());
			if(!LayoutId.isNull())
			{
//				OdDbLayoutPtr pLayout = LayoutId.safeOpenObject();

//				OdDbObjectId BlockId = pLayout->getBlockTableRecordId();
//				OdDbBlockTableRecordPtr BlockId.safeOpenObject();
			}

			// Get an entity iterator
			OdDbObjectIteratorPtr pEntIter = pBlock->newIterator();
		    
			// For each entity in the block
			for(; !pEntIter->done(); pEntIter->step())
			{
				//DumpEntity(pEntIter->objectId());

				//OdDbEntityPtr pEnt = pEntIter->entity();
				OdDbEntityPtr pEnt = pEntIter->objectId().safeOpenObject();
				OdGeMatrix3d geMatrix;

				if(pEnt->isA()->name() == "AcDbViewport")				
				{
					OdDbViewportPtr pView = pEnt;

					if(isOverallVp(pView))
						continue;
						

					TD_MS2PS(pView, geMatrix);

					// Get the drawing db pointer
					OdDbObjectId idModelSpace = pDb->getModelSpaceId();	
			
					// Get ModelSpace and iterator
					OdDbBlockTableRecordPtr pModelSpace = idModelSpace.safeOpenObject();
					OdDbObjectIteratorPtr pIt = pModelSpace->newIterator();
			
					// frozen layers
					OdDbObjectIdArray aFrozenLayers;
					pView->getFrozenLayerList(aFrozenLayers);
						
					for (pIt->start(); !pIt->done(); pIt->step())
					{
						OdDbEntityPtr pEnt1 = pIt->entity();

						//if(pIt->entity() == 0)
						//	continue;

						if(pEnt1.isNull())
							continue;

						// test if on a frozen layer
						if(aFrozenLayers.contains(pEnt1->layerId()))
						{
							//pEnt1->release();
							continue;
						}

						// clipping
						OdGeExtents3d geExtView;
						if(pView->getGeomExtents(geExtView) != /*OdResult::*/eOk)//GIG - It's handy to have this in case a human ever needs it, but VStudio complains
							continue;

						OdGeExtents3d geExtEnt;
						if(pEnt1->getGeomExtents(geExtEnt) != /*OdResult::*/eOk)
							continue;

						geExtEnt.transformBy(geMatrix);

						if((!geExtView.contains(geExtEnt)) && (geExtView.intersectWith(geExtEnt) != OdGeExtents3d::kIntersectOk))
						{
							//pEnt1->release();
							continue;
						}

					/*	IntersectionStatus iStat = geExtEnt.intersectWith(geExtView);
						if((iStat != kIntersectOpIn) ||(iStat != kIntersectOk))
							continue;*/
					
						//pIt->entity()->transformBy(geMatrix);
						OdDbEntityPtr pEnt2 = OdDbEntity::createObject();

						if(pEnt1.isNull())
						{
							//pEnt1->release();
							continue;
						}

						//try{

						//if(geMatrix.sc

						pEnt1->getTransformedCopy(geMatrix, pEnt2);
						//}
						//catch(...)
						//{
						//	continue;
						//}

						if(pEnt2.isNull())
						{
							//pEnt1->release();
							//pEnt2->release();
							continue;
						}

						// clipping
					/*	OdGeExtents3d geExtView;
						pView->getGeomExtents(geExtView);*/

						/*OdGeExtents3d geExtEnt2;
						pEnt2->getGeomExtents(geExtEnt2);
						geExtEnt2.transformBy(geMatrix);*/

						// explode block refs
						//if((pEnt2->isA()->name() == "AcDbBlockReference") && (istate.GetExplode() && istate.m_bIsImport))
						if(((pEnt->isA()->name() == "AcDbBlockReference") || 
							(pEnt->isA()->name() == "AcDbHatch") ||
							(pEnt->isA()->name() == "AcDbPolyline")) && (istate.GetExplode() && istate.m_bIsImport))
						{
							OdRxObjectPtrArray entitySet;
							pEnt2->explode(entitySet);

							for(unsigned int i = 0; i < entitySet.size(); i++)
							{
								OdDbEntityPtr pEntity = entitySet.getAt(i);

								if(geExtView.intersectWith(geExtEnt) == OdGeExtents3d::kIntersectOk)
									DumpEntity(pDb->addOdDbObject(pEntity), true, geExtView);
								else
									DumpEntity(pDb->addOdDbObject(pEntity));
							}
						}
						else
						{
							//DumpEntity(pIt->objectId());
							if(geExtView.intersectWith(geExtEnt) == OdGeExtents3d::kIntersectOk)
								DumpEntity(pDb->addOdDbObject(pEnt2), true, geExtView);
							else
								DumpEntity(pDb->addOdDbObject(pEnt2));
							//DumpEntity(pEnt2->objectId());
						}

						//pEnt1->release();
						//pEnt2->release();
					}
					//ids.push_back(pIt->objectId());
					
			
					//pModelSpace.release();
				}

		//		/////////////////////////////////////////////////////////////////////////
		//		// Open the entity
		//		OdDbEntityPtr pEnt = pEntIter->objectId().safeOpenObject();

		//		OdRxObjectPtrArray entitySet;
		////		pEnt->explode(entitySet);
		//		pEnt->explodeGeometry(entitySet);

		//		for(int i = 0; i < entitySet.size(); i++)
		//		{
		//			OdDbEntityPtr pEnt2 = entitySet.at(i);
		//			DumpEntity(pEnt2->objectId());
		//		}
			}

		}
	}
}

void CFileDWGDirect::TD_MS2PS(OdDbViewportPtr pVp, OdGeMatrix3d &resultMat) 
{
	// first get all the data
	OdGeVector3d viewDirection = pVp->viewDirection();
	OdGePoint2d centre2d = pVp->viewCenter();
	OdGePoint3d viewCenter = OdGePoint3d( centre2d.x, centre2d.y, 0);

	OdGePoint3d viewTarget = pVp->viewTarget ();
	double twistAngle = -pVp->twistAngle();
	OdGePoint3d centerPoint = pVp->centerPoint();
	double viewHeight = pVp->viewHeight();
	double height = pVp->height();
	double width = pVp->width();
	double scaling = viewHeight / height;
	double lensLength = pVp->lensLength();

	// prepare the transformation
	OdGeVector3d xAxis, yAxis, zAxis;
	zAxis = viewDirection.normal();
	xAxis = OdGeVector3d::kZAxis.crossProduct( viewDirection );

	if( !xAxis.isZeroLength() ) 
	{
		xAxis.normalize();
		yAxis = zAxis.crossProduct( xAxis );
	} 
	else if( zAxis.z < 0 ) 
	{
		xAxis = -OdGeVector3d::kXAxis;
		yAxis = OdGeVector3d::kYAxis;
		zAxis = -OdGeVector3d::kZAxis;
	} 
	else 
	{
		xAxis = OdGeVector3d::kXAxis;
		yAxis = OdGeVector3d::kYAxis;
		zAxis = OdGeVector3d::kZAxis;
	}

	OdGeMatrix3d dcs2wcs; // display coordinate system (DCS) to world coordinate system (WCS)
	OdGeMatrix3d ps2Dcs; // paperspace to DCS


	// First initialise with a transformation to centerPoint
	ps2Dcs = OdGeMatrix3d::translation( OdGePoint3d::kOrigin - centerPoint );

	// then scale for the view
	//CH_101102 viewCenter was placed by me; previous was centerPoint. and didn't work at all
	ps2Dcs = ps2Dcs * OdGeMatrix3d::scaling( scaling, centerPoint/*viewCenter*/ );

	// then adjust to the viewCenter
	dcs2wcs = OdGeMatrix3d::translation( viewCenter - OdGePoint3d::kOrigin );

	// Then transform for the view direction
	OdGeMatrix3d matCoords;
	matCoords.setCoordSystem( OdGePoint3d::kOrigin, xAxis, yAxis, zAxis);

	dcs2wcs = matCoords * dcs2wcs;

	// Then adjust for the viewTarget
	dcs2wcs = OdGeMatrix3d::translation( viewTarget - OdGePoint3d::kOrigin ) * dcs2wcs;

	// Then the twist angle
	dcs2wcs = OdGeMatrix3d::rotation(twistAngle, zAxis, viewTarget ) * dcs2wcs;

	OdGeMatrix3d perspMat;
	if( pVp->isPerspectiveOn()) 
	{
		// we do special perspective handling
		double viewSize = viewHeight;
		double aspectRatio = width / height;

		double adjustFactor = 1.0 / 42.0;
		double adjustedLensLength = viewSize * lensLength * sqrt ( 1.0 +
		aspectRatio * aspectRatio ) * adjustFactor;

		double eyeDistance = viewDirection.length();
		double lensDistance = eyeDistance - adjustedLensLength;


		double ed = eyeDistance;
		double ll = adjustedLensLength;
		double l = lensDistance;


		perspMat.entry[2][2] = (ll - l ) / ll;
		perspMat.entry[2][3] = l * ( ed - ll ) / ll;
		perspMat.entry[3][2] = -1.0 / ll;
		perspMat.entry[3][3] = ed / ll;
	}

	resultMat = ps2Dcs.inverse() * perspMat * dcs2wcs.inverse();
}

void CFileDWGDirect::DumpMLineStyles()
{
	OdDbDictionaryPtr pDic = pDb->getMLStyleDictionaryId().safeOpenObject();
	OdDbDictionaryIteratorPtr pIter = pDic->newIterator(); /* OdDbDictionaryIterator */
	  
	for(; !pIter->done(); pIter->next())
	{
		OdDbObjectId MLinestyleId = pIter->objectId();
		OdDbMlineStylePtr mp = MLinestyleId.safeOpenObject();/*OdDbMlineStyle*/

//		if(!mp.isNull())
//			os << "MLineStyle: " << mp->name().c_str() << STD(endl);
	}
}

//void CFileDWGDirect::DumpObject(OdDbObjectId id, const OdString& itemName, const OdString& padding)
void CFileDWGDirect::DumpObject(OdDbObjectId, const OdString&, const OdString&)
{
//	OdString pad(padding);
//	// Open the object
//	OdDbObjectPtr pObject = id.safeOpenObject();
//
////	os << pad.c_str();
//	if(!itemName.isEmpty())
//	{
////		os << itemName.c_str() << ", ";
//	}
//
//	// Print the object's class name.
////	os << "<" << pObject->isA()->name() << ">" << STD(endl);
//
//	// Check for specific object types.
//	if(pObject->isKindOf(OdDbDictionary::desc()))
//	{
//		OdDbDictionaryPtr pDic = pObject;
//
//		// Get a dictionary iterator.
//		OdDbDictionaryIteratorPtr pIter = pDic->newIterator();
//		pad += "  ";
//		for (; !pIter->done(); pIter->next())
//		{
//			// Dump each item in the dictionary.
//			DumpObject(pIter->objectId(), pIter->name(), pad);
//		}
//	}
//	else if(pObject->isKindOf(OdDbXrecord::desc()))
//	{
////		OdDbXrecordPtr pXRec = pObject;
//		// Following is broken for objectID's
////		DumpGroupCodes(pXRec->rbChain(), pad + "  ");
//	}
//	else if(pObject->isKindOf(OdDbDimAssoc::desc()))
//	{
////		dumpDimAssoc(pObject, pad);
//	}
}

void CFileDWGDirect::DumpEntity(OdDbObjectId id, bool bIntersectView, OdGeExtents3d geExtView)//, CBlock *pPegBlock)
{
	// Open the entity
	OdDbEntityPtr pEnt = id.safeOpenObject();

	//OdRxObjectPtrArray entitySet;
	//pEnt->explode(entitySet);
	

	// Retrieve the registered protocol extension object registered for this object type.
	OdSmartPtr<OdDbEntity_Dumper> pEntDumper = pEnt;
	CSeg* pSeg = pEntDumper->Dump(pEnt);

	if(pSeg == 0)
		return;

	// clip the segment if it is on the border of a viewport
	if(bIntersectView)
	{
		CSeg pTempSeg = *pSeg;
		pTempSeg.Vectorize();
		pSeg->RemoveAll();

		CPnt ptCut[2];
		ptCut[0][0] = geExtView.maxPoint().x;
		ptCut[0][1] = geExtView.maxPoint().y;
		ptCut[1][0] = geExtView.minPoint().x;
		ptCut[1][1] = geExtView.minPoint().y;
		CSeg *pSegIn = ClipViewport(&pTempSeg, ptCut[1], ptCut[0]);

		// test extents of viewport
		//CPegDoc::GetDoc()->WorkLayerAddTail(new CSeg(new CPrimLine(ptCut[0], ptCut[1])));

		pSeg->AddTail(pSegIn);
	}

	// 0 indicates a color of BYBLOCK.
	// 256 indicates a color of BYLAYER.
	PENCOLOR nPenColor = pEnt->colorIndex();
	if(nPenColor == 0) 
		nPenColor = 7;
	else if(nPenColor == 256) 
		nPenColor = CPrim::PENCOLOR_BYLAYER;
	
	PENSTYLE nPenStyle = 0;
	if((!pEnt->isKindOf(OdDbFace::desc())) && (!pEnt->isKindOf(OdDbSolid::desc())) && (!pEnt->isKindOf(OdDbHatch::desc())))
	{
		if(Prim_IsByBlockPenStyle(pEnt->linetype())) 
			nPenStyle = 1;
		else if(Prim_IsByLayerPenStyle(pEnt->linetype())) 
			nPenStyle = CPrim::PENSTYLE_BYLAYER;
		else 
			nPenStyle = CPegDoc::GetDoc()->PenStylesLookup((LPCTSTR)pEnt->linetype());
	}

	POSITION pos = pSeg->GetHeadPosition();
	while(pos != 0)
	{
		CPrim *pPrim = pSeg->GetNext(pos);

		if(pPrim != 0)
		{
			pPrim->SetPenColor(nPenColor);
			pPrim->SetPenStyle(nPenStyle);
		}

//		if((pPrim != 0) && (pPegBlock != 0))
//		{
//			CPrim *pPrimCopy;
//			pPrim->Copy(pPrimCopy);
//			pPegBlock->AddTail(pPrimCopy);
//		}
	}

	CLayer *pLayer = CPegDoc::GetDoc()->LayersGet((LPCTSTR)pEnt->layer());
	if(pLayer != 0)
	{
		pLayer->AddTail(pSeg);
//		if(pDb->getTILEMODE() == 0)// && entset == 1)
//		{
//			for (int i = 1; i < m_iVpCount; i++)
//			{
//				view::PushActive();
//				view::SetMatrix(m_tmView[i]);
//				if(pSeg->IsInView())
//				{
//					CSeg *pSegCopy = new CSeg(*pSeg);
//					pSegCopy->Transform(m_tm[i]);
//					pLayer->AddTail(pSegCopy);
//				}
//				view::PopActive();
//			}
////			delete pSeg;
//		}
//		else
//		{
//			pLayer->AddTail(pSeg);
//		}
	}

//	DumpGroupCodes(pEnt->xData(), "      ");

//	if(!pEnt->extensionDictionary().isNull())
//		DumpObject(pEnt->extensionDictionary(), "ACAD_XDICTIONARY", "      ");
}

CSeg* CFileDWGDirect::ClipViewport(CSeg *pSeg, CPnt ptLL, CPnt ptUR)
{
	//CPegDoc* pDoc = CPegDoc::GetDoc();

	//CSeg*		pSeg;
	CPrim*		pPrim;

	int 	iInts;
	CPnt	ptInt[10];

	//PENCOLOR nPenColor = pstate.PenColor();
	//PENSTYLE nPenStyle = pstate.PenStyle();

	CSegs* pSegsOut = new CSegs;
	CSegs* pSegsIn = new CSegs;

	//POSITION posSeg, posSegPrv;
	//for(posSeg = detsegs.GetHeadPosition(); (posSegPrv = posSeg) != 0;)
	//{
	//	pSeg = detsegs.GetNext(posSeg);
	//	
	//	// ignore if already trapped
	//	if (trapsegs.Find(pSeg) != 0)
	//		continue;
		
		POSITION posPrim, posPrimPrv;
		for (posPrim = pSeg->GetHeadPosition(); (posPrimPrv = posPrim) != 0;)
		{
			pPrim = pSeg->GetNext(posPrim);

			if ((iInts = pPrim->IsWithinArea(ptLL, ptUR, ptInt)) == 0)
			{
//				MessageBox(0, "zero", 0, 0);
				continue;
			}

//			CString buffer;
//			itoa(iInts, buffer.GetBuffer(), 10);
//			MessageBox(0, buffer, 0, 0);

			pSeg->RemoveAt(posPrimPrv);
			
			for(int i = 0; i < iInts; i += 2)
			{
				if(i != 0)
					pSegsOut->RemoveTail();
				pPrim->CutAt2Pts(&ptInt[i], pSegsOut, pSegsIn);
			}			 
		}
		if (pSeg->IsEmpty())
		{	// seg was emptied remove from lists
//			pDoc->AnyLayerRemove(pSeg);
//			detsegs.RemoveAt(posSegPrv);
//			pSeg->RemovePrims();
//			delete pSeg;
		}
	//}
				
	/*if (pSegsOut->GetCount() > 0)
	{
		pDoc->WorkLayerAddTail(pSegsOut);
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS, pSegsOut);
	}*/
	//if (pSegsIn->GetCount() > 0)
	//{
	//	pDoc->WorkLayerAddTail(pSegsIn);
	//	trapsegs.AddTail(pSegsIn);
	//	pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEGS_SAFE_TRAP, pSegsIn);
	//}

	CSeg *pSegReturn = new CSeg;

	if(pSegsIn != 0)
	{
		POSITION pos3 = pSegsIn->GetHeadPosition();
		while(pos3 != 0)
		{
			CSeg *pSeg3 = pSegsIn->GetNext(pos3);

			if(pSeg3 != 0)
				pSegReturn->AddTail(pSeg3);
		}
	}

	delete pSegsIn;
	delete pSegsOut;

	return pSegReturn;
	
//	pstate.SetPen(pDC, nPenColor, nPenStyle);
//	app.StatusLineDisplay(TrapCnt);
}

bool CFileDWGDirect::Create(char* szPathKey, WORD wType)
{
	// adNewFile adds the normal entries for a default "empty file" in AutoCAD, including layer 0, 
	// linetypes CONTINUOUS, BYLAYER and BYBLOCK, dimstyle STANDARD, shapefile STANDARD, regapp ACAD, 
	// and the standard objects.
	
//	if (!m_bInitialized)
//		return false;

	CPegDoc *pDoc = CPegDoc::GetDoc();
	//OdWrFileBufPtr fb;
	//OdWrFileBuf fb(szPathKey);

	//try
	//{
	//	//OdWrFileBuf fb(szPathKey);
	//	fb = OdWrFileBuf::createObject();
	//	fb->open(szPathKey);
	//}
	//catch(OdError ex)
	//{
	//	MessageBox(0, ex.description(), "OdError", 0);
	//	return false;
	//}

	// initialize a default database.
    pDb = svcs.createDatabase();

	for(int i = 0; i < pDoc->LayersGetSize(); i++)
	{
		CLayer *pLayer = pDoc->LayersGetAt(i);

		POSITION pos = pLayer->GetHeadPosition();
		while(pos != 0)
		{
			CSeg *pSeg = pLayer->GetNext(pos);

			POSITION pos2 = pSeg->GetHeadPosition();
			while(pos2 != 0)
			{
				CPrim *pPrim = pSeg->GetNext(pos2);

				if(estate.bVectorizeText && pPrim->Is(CPrim::PRIM_TEXT))
				{
					pSeg->Vectorize();
					break;
				}
			}
		}
	}

//	WriteStart();
//	WriteLineTypeTable();
	WriteLayerTable();
	WriteBlockTable();
	WriteEntity();

	OdDb::SaveType type = (wType == CPegDoc::FILE_TYPE_DWG ? OdDb::kDwg : OdDb::kDxf);
	pDb->writeFile(szPathKey, type, OdDb::vAC15); //vAC15

	/*try
	{
		fb->close();
		fb.release();
	}
	catch(...)
	{
		MessageBox(0, "Unknown Error", "??", 0);
	}*/
	
	return true;
}

void CFileDWGDirect::WriteStart()
{
	// Add a layout

	// Set the drawing extents
	pDb->setEXTMIN(OdGePoint3d(0.0, 0.0, 0.0));
	pDb->setEXTMAX(OdGePoint3d(25.0, 35.0, 0.0));

	// Set Creation and last update times
	OdDbDate date;
	date.setDate(1, 20, 2001);
	date.setTime(13, 0, 0, 0);
	odDbSetTDUCREATE(*pDb, date);

	date.setTime(18, 30, 0, 0);
	odDbSetTDUUPDATE(*pDb, date);


	pDb->setTILEMODE(1);  // 0 for paperspace, 1 for modelspace

	////////////////////////////////////////////////////////////////////

//	// Add a new regapp
//	addRegApp(pDb, "ODA");

	// Add some layers
//	OdDbObjectId newLayerId = addLayer(pDb, "Oda Layer 1");

	//Add some text styles
//	OdDbObjectId newStyleId = addStyle(pDb, "OdtStyle", 0.2, 1.0, 0.2, 0.0, "txt");
	//Truetype font style
//	OdDbObjectId ttStyleId = addStyle(pDb, "OdtTtStyle", 0.2, 1.0, 0.2, 0.0, "VERDANA.TTF", false, "Verdana", false, false, 0, 34);
	//Style for linetype with embedded shapes.
//	OdDbObjectId ltStyleId = addStyle(pDb, "", 0.0, 1.0, 0.2, 0.0, "ltypeshp.shx", true);

	//OdDbBlockTableRecordPtr pPs = pDb->getPaperSpaceId().safeOpenObject(OdDb::kForWrite);
	//OdDbViewportPtr pVp = OdDbViewport::createObject();
	//  
	//pVp->setCenterPoint(OdGePoint3d(5.375, 4.125, 0));
	//pVp->setWidth(14.63);
	//pVp->setHeight(9.0);
	//pVp->setViewTarget(OdGePoint3d(0, 0, 0));
	//pVp->setViewDirection(OdGeVector3d(0, 0, 1));
	//pVp->setViewHeight(9.0);
	//pVp->setLensLength(50.0);
	//pVp->setViewCenter(OdGePoint2d(5.375, 4.125));
	//pVp->setSnapIncrement(OdGeVector2d(0.5, 0.5));
	//pVp->setGridIncrement(OdGeVector2d(0.5, 0.5));
	//pVp->setCircleSides(OdUInt16(100));

	//// TODO: Test this
	////pVp->freezeLayersInViewport(OdDbObjectIdArray&);

	//pPs->appendOdDbEntity(pVp);
	//// pVp->setOn();

	//OdDbCirclePtr pCircle = OdDbCircle::createObject();
	//pCircle->setRadius(1.0);
	//pCircle->setLayer(newLayerId, false);
	//pPs->appendOdDbEntity(pCircle);

	  
	//OdDbViewportTablePtr pVpTable = pDb->getViewportTableId().openObject(OdDb::kForWrite);
	//OdDbObjectId vpID = pVpTable->getActiveViewportId();
	//OdDbViewportTableRecordPtr vPortRec = vpID.openObject(OdDb::kForWrite);
	//vPortRec->setCenterPoint(OdGePoint2d(11.26, 4.5));
	//vPortRec->setWidth(22.53);
	//vPortRec->setHeight(9.);
	//  
	//// Add some linetypes
	//addLinetypes(pDb, ltStyleId ,newStyleId);


	//// Add some new blocks
	//OdDbObjectId newBlockId1 = addBlock(pDb, "OdtBlock1");
	//OdDbObjectId newBlockId2 = addBlock(pDb, "OdtBlock2");

//	createEntityBoxes(pDb, pDb->getModelSpaceId(), newLayerId);
	  
//	addDimAssoc(pDb, pDb->getModelSpaceId(), ttStyleId);

	// Add some entities to a regular block
//	addEntities2(pDb, newBlockId1, newLayerId, ttStyleId, true);

//	OdGePoint3d point = m_EntityBoxes.getBox(1,2);
//	double h = m_EntityBoxes.getHeight();
//	double w = m_EntityBoxes.getWidth(1,2);

	//OdDbBlockTableRecordPtr pBlock = pDb->getModelSpaceId().openObject(OdDb::kForWrite);
	//addTextEnt( pDb, pBlock, OdGePoint3d(point.x+w/4.0, point.y-h/4.0, 0), OdGePoint3d(point.x+w/4.0, point.y-h/4.0, 0),
	//	"BLOCK", 0.2, OdDb::kTextLeft, OdDb::kTextTop, newLayerId, ttStyleId );


	////Add some entities to Model Space
	//addEntities1(pDb, pDb->getModelSpaceId(), newBlockId1, newBlockId2, newLayerId, ttStyleId);


	//// Add examples of text with different alignments
	//addTextEnts(pDb, pDb->getModelSpaceId(), newLayerId, ttStyleId);

	//// Add some additional modelspace entities.
	//addEntities2(pDb, pDb->getModelSpaceId(), newLayerId, ttStyleId );

	//// Add an xref.
	//addXRef(pDb);

	//// Add some custom objects
	//addCustomObjects(pDb);

	//// Add table
	//addTable(pDb, pDb->getModelSpaceId(), newBlockId1);

	//if (pDb->getTILEMODE()) //model space
	//{
	//	OdDbViewportTablePtr pVPTbl = pDb->getViewportTableId().safeOpenObject();
	//	OdDbObjectId id = pVPTbl->getActiveViewportId();
	//	if (id)
	//	{
		//	OdDbViewportTableRecordPtr pRec = id.safeOpenObject(OdDb::kForWrite);
		//	pRec->zoomExtents();
	//	}

	//	// Create a new layout
	//	OdDbObjectId newLayoutId = pDb->createLayout("DDT Layout");
	//	OdDbLayoutPtr pLayout = newLayoutId.safeOpenObject();
	//	pDb->setCurrentLayout(newLayoutId); // also creates overall (PaperSpase) viewport 
	//	OdDbViewportPtr pOverallVp = pLayout->overallVportId().safeOpenObject();
	//	OdGePoint3d overallCenter = pOverallVp->centerPoint();

		// Create viewport with all ModelSpace entities
//		OdDbViewportPtr pNewVp = OdDbViewport::createObject();
//		OdDbBlockTableRecordPtr pNewPs = pLayout->getBlockTableRecordId().safeOpenObject(OdDb::kForWrite);
//		pNewPs->appendOdDbEntity(pNewVp);
	//	pNewVp->setWidth(pOverallVp->width());
	//	pNewVp->setHeight(pOverallVp->height()*0.5);
	//	pNewVp->setCenterPoint(OdGePoint3d(overallCenter.x, overallCenter.y*1.5, 0.));
	//	pNewVp->setViewCenter(pOverallVp->viewCenter());
	//	pNewVp->zoomExtents();

	//	// Create viewports with separate entities
	//	//
	//	if (m_layoutEntities.size())
	//	{
		//	double factor = 1.0/m_layoutEntities.size();
		//	for (unsigned i=0; i<m_layoutEntities.size(); ++i)
		//	{
		//		OdDbEntityPtr pEnt = m_layoutEntities[i].safeOpenObject();
		//		OdGeExtents3d extents;
		//		if (pEnt->getGeomExtents(extents) == eOk)
		//		{
			//		// Create viewport
			//		pNewVp = OdDbViewport::createObject();
			//		pNewPs->appendOdDbEntity(pNewVp);

			//		// Set viewport on paper
			//		pNewVp->setWidth(pOverallVp->width()*factor);
			//		pNewVp->setHeight(pOverallVp->height()*0.5);
			//		pNewVp->setCenterPoint(OdGePoint3d(overallCenter.x*(2*i*factor+factor), overallCenter.y*0.5, 0.));

			//		// Set view of viewport
			//		const OdGePoint3d& minPt = extents.minPoint();
			//		const OdGePoint3d& maxPt = extents.maxPoint();
			//		OdGePoint3d centerPt;
			//		centerPt.x =(minPt.x + maxPt.x) / 2.0;
			//		centerPt.y =(minPt.y + maxPt.y) / 2.0;
			//		centerPt.z =(minPt.z + maxPt.z) / 2.0;

			//		pNewVp->setViewHeight(fabs(maxPt.y - minPt.y));
			//		pNewVp->setViewTarget(centerPt);
			//		pNewVp->setViewCenter(OdGePoint2d(0.,0.));      // in DCS coordinates
			//		pNewVp->setViewDirection(OdGeVector3d::kZAxis); // the vector from the view's target to the view's camera
		//		}
		//	}
	//	}
	// 
	//	pDb->setTILEMODE(1);  // modelspace
	//}
	//else
	//{
	//	OdDbBlockTableRecordPtr pBlclTblRec  = pDb->getActiveLayoutBTRId().safeOpenObject();
	//	OdDbLayoutPtr pLayout = pBlclTblRec->getLayoutId().safeOpenObject();

	//	OdDbObjectId actId = pLayout->lastActiveVportId();
	//	if (actId)
	//	{
		//	OdDbViewportPtr pVp = actId.safeOpenObject(OdDb::kForWrite);
		//	pVp->zoomExtents();
	//	}
	//}


	//// If preview bitmap is already available it can be specified to avoid wasting
	//// time on generating it by DD
	//const OdChar* pBmpFileName = "preview.bmp";
	//if(::odSystemServices()->accessFile(pBmpFileName, Oda::kFileRead))
	//{
	//	OdRdFileBuf bmpFile(pBmpFileName);

	//	OdArray<OdUInt8> buf;
	//	buf.resize(bmpFile.length());
	//	OdUInt8 * pData = buf.asArrayPtr(); 
	//	bmpFile.getBytes(pData, buf.length());
	//	// Get length taking care about big-endian
	//	OdUInt32 length = pData[2] + (pData[3] << 8);
	//	pData += 14;  // Skip BITMAPFILEHEADER
	//	pDb->setThumbnailBitmap(pData, length);
	//}
}

void CFileDWGDirect::WriteLineTypeTable()
{
	// linetype Continuous, ByBlock & ByLayer table entry created by toolkit
	CPegDoc* pDoc = CPegDoc::GetDoc();
						
	for(int i = 2; i < pDoc->PenStylesGetSize(); i++)
	{
		CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);
		WriteLineType(pPenStyle);
	}
}

void CFileDWGDirect::WriteLineType(CPenStyle* pPenStyle)
{
//	CPegDoc* pDoc = CPegDoc::GetDoc();

	OdDbLinetypeTablePtr pLineTypeTable = pDb->getLinetypeTableId().safeOpenObject(OdDb::kForWrite);
	OdDbLinetypeTableRecordPtr pLineType = OdDbLinetypeTableRecord::createObject();

	WORD wDefLen = pPenStyle->GetDefLen();
	
	pLineType->setName(pPenStyle->GetName().GetBuffer());
	pLineType->setComments(pPenStyle->GetDescription().GetBuffer());

	if (wDefLen > 0)
	{
		double* dDash = new double[wDefLen];
		pPenStyle->GetDashLen(dDash);

		pLineType->setNumDashes(wDefLen);

		for (WORD w = 0; w < wDefLen; w++)
		{
			pLineType->setDashLengthAt(w, dDash[w]);

//			pLineType->setShapeOffsetAt(w, Od

//			m_htb->ltype.segdata[w].val = dDash[w];
//			adHanclear(m_htb->ltype.segdata[w].shapeentryobjhandle);
//			m_htb->ltype.segdata[w].complexshapecode = 0;
//			m_htb->ltype.segdata[w].xoffset = 0.;
//			m_htb->ltype.segdata[w].yoffset = 0.;
//			m_htb->ltype.segdata[w].rotang = 0.;
//			m_htb->ltype.segdata[w].scale = 0.;
//			m_htb->ltype.segdata[w].shapeflag = 0;
//			m_htb->ltype.segdata[w].stroffset = 0;
		}
		delete [] dDash;
	}

	pLineTypeTable->add(pLineType);

	///////////////////////////////////////////////////////////////////////////////////

//	adSetDefaultLinetype(&m_htb->ltype);
//  
//	strcpy(m_htb->ltype.name, pPenStyle->GetName());
//	strcpy(m_htb->ltype.text, pPenStyle->GetDescription());
//	
//	WORD wDefLen = pPenStyle->GetDefLen();
//	
//	m_htb->ltype.numlinetypesegs = wDefLen;
//	m_htb->ltype.patternlength = pPenStyle->GetPatternLen();
//	
//	if (wDefLen > 0)
//	{
//		double* dDash = new double[wDefLen];
//		pPenStyle->GetDashLen(dDash);
//
//		for (WORD w = 0; w < wDefLen; w++)
//		{
//			m_htb->ltype.segdata[w].val = dDash[w];
//			adHanclear(m_htb->ltype.segdata[w].shapeentryobjhandle);
//			m_htb->ltype.segdata[w].complexshapecode = 0;
//			m_htb->ltype.segdata[w].xoffset = 0.;
//			m_htb->ltype.segdata[w].yoffset = 0.;
//			m_htb->ltype.segdata[w].rotang = 0.;
//			m_htb->ltype.segdata[w].scale = 0.;
//			m_htb->ltype.segdata[w].shapeflag = 0;
//			m_htb->ltype.segdata[w].stroffset = 0;
//		}
//		delete [] dDash;
//	}
//	adGenerateObjhandle(m_hdb, m_htb->ltype.objhandle);
//
//	if (!adAddLinetype(m_hdb, &m_htb->ltype)) 
//	{
//	
//	}
}

void CFileDWGDirect::WriteLayerTable()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
						
	// layer "0" table entry created by toolkit .. not added here .. modify some defaults
	CLayer *pPegLayer = pDoc->LayersGetAt(0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add a new layer to the drawing
	OdDbLayerTablePtr pLayers;
	OdDbLayerTableRecordPtr pLayer;
//	OdDbObjectId id;

	pLayers = pDb->getLayerTableId().safeOpenObject(OdDb::kForWrite);

	for(int i = 1; i < pDoc->LayersGetSize(); i++)
	{
		pPegLayer = pDoc->LayersGetAt(i);
		pLayer = OdDbLayerTableRecord::createObject();

		// Name must be set before a table object is added to a table.
		pLayer->setName(OdString(pPegLayer->GetName().GetBuffer()));//layerName);
//		pLayer->setColor(pPegLayer->PenColor());

		if(pPegLayer->IsOff())
			pLayer->setIsOff(true);

		// Add the object to the table.
		pLayers->add(pLayer);
	}

//	return id;
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
//	adStartLayerGet(m_hdb);
//	adGetLayer(m_hdb, &m_htb->lay);
//	
//	m_htb->lay.color = pLayer->PenColor();
//
//	////////////////////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////////////////////
//
//	// mak - removed and replaced with code below
//	for (int i = 1; i < pDoc->LayersGetSize(); i++)
//	{
//		pLayer = pDoc->LayersGetAt(i);
//		WriteLayer(pLayer->GetName(), pLayer->PenColor());
//	}
//
//	////////////////////////////////////////////////////////////////////////////////////////////
//	////////////////////////////////////////////////////////////////////////////////////////////
//
//	//// MAK - added code to fix the layer name when a tracing file is saved as ACAD file
//	//// Peg tracing layer names are fine... Tracings were including the full path
//	//for (int i = 1; i < pDoc->LayersGetSize(); i++)
//	//{
//	//	pLayer = pDoc->LayersGetAt(i);
//
//	//	// MAK - added the code below to set the tracing layer name to just the filename
//	//	// not the entire pathname
//	//	CString strFilePath = pLayer->GetName();
//	//	WORD wFileType = app.GetFileTypeFromPath(strFilePath);
//
//	//	if(pDoc->FileTypeIsTracing(wFileType))
//	//	{
//	//		CString strName = strFilePath;
//	//		strName = strName.Right(strName.GetLength() - strName.ReverseFind('\\'));
//	//		strName.TrimLeft('\\');
//	//		pLayer->SetName(strName);
//	//	}
//
//	//	WriteLayer(strFilePath, pLayer->PenColor());
//	//}
}

void CFileDWGDirect::WriteBlockTable()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	OdDbBlockTablePtr pBlockTable = pDb->getBlockTableId().safeOpenObject(OdDb::kForWrite);

	CString strKey;
	CBlock* pPegBlock;

	POSITION pos = pDoc->BlksGetStartPosition();
	while(pos != 0)
	{
        pDoc->BlksGetNextAssoc(pos, strKey, pPegBlock);

		OdDbBlockTableRecordPtr pBlock = OdDbBlockTableRecord::createObject();
		pBlock->setName(strKey.GetBuffer());

		POSITION pos2 = pPegBlock->GetHeadPosition();
		while(pos2 != 0)
		{
			CPrim *pPrim = pPegBlock->GetNext(pos2);

			OdDbEntityPtr pEnt;
			pPrim->Write(pEnt, pDb);

			if(!pEnt.isNull())
				pBlock->appendOdDbEntity(pEnt);	
		}

		if(!pBlock.isNull())
			pBlockTable->add(pBlock);
	}

		//////////////////////////////////////////////////////////////////////////////////////

		//OdDbBlockReferencePtr pBlkRef = OdDbBlockReference::createObject();

		//// Add the entity to the parent block.
		//pBlock->appendOdDbEntity(pBlkRef);

		//// Set the remaining entity properties.
		//pBlkRef->setBlockTableRecord(pBlock->objectId());//blockId);
		//
		//CPnt pntBase = pPegBlock->GetBasePt();
		//pBlkRef->set
		//pBlkRef->setPosition(OdGePoint3d(pntBase[0], pntBase[1], pntBase[2]);
//		pBlkRef->setScaleFactors(OdGeScale3d(xscale, yscale, 1.0));

//		return brefId;

//		adSetDefaultBlockheader(&m_htb->blkh);
//		strcpy(m_htb->blkh.name, strKey);
//		adGenerateObjhandle(m_hdb, m_htb->blkh.objhandle);
//		adAddBlockheader(m_hdb, &m_htb->blkh); 
//	}

//	for(int i = 1; i < pDoc->LayersGetSize(); i++)
//	{
//		pPegLayer = pDoc->LayersGetAt(i);
//		pLayer = OdDbLayerTableRecord::createObject();

		// Name must be set before a table object is added to a table.
//		pLayer->setName(OdString(pPegLayer->GetName().GetBuffer()));//layerName);
//		pLayer->setColor(pPegLayer->PenColor());

		// Add the object to the table.
//		pLayers->add(pLayer);
//	}

	/////////////////////////////////////////////////////////////////////////

//	CPegDoc* pDoc = CPegDoc::GetDoc();
//						
//	CString strKey;
//	CBlock* pBlock;
	
//	POSITION pos = pDoc->BlksGetStartPosition();
//	while (pos != 0)
//	{
//        pDoc->BlksGetNextAssoc(pos, strKey, pBlock);

//		adSetDefaultBlockheader(&m_htb->blkh);
//		strcpy(m_htb->blkh.name, strKey);
//		adGenerateObjhandle(m_hdb, m_htb->blkh.objhandle);
//		adAddBlockheader(m_hdb, &m_htb->blkh); 
//	}

	//POSITION pos = pDoc->BlksGetStartPosition();
	//while(pos != 0)
	//{
 //       pDoc->BlksGetNextAssoc(pos, strKey, pBlock);

	//	char szName[64];
	//	strcpy(szName, strKey);

	//	//////////////////////////////////////////////////////////////////////////////
	//	OdDbObjectId brefId;
  
		// Create the new insert entity.
//		OdDbBlockReferencePtr pBlkRef = OdDbBlockReference::createObject();
		// Add the entity to the parent block.
//		brefId = pBlock->appendOdDbEntity(pBlkRef);

		// Set the remaining entity properties.
//		pBlkRef->setBlockTableRecord(blockId);
		
//		CPnt pntBase = pBlock->GetBasePt();
//		pBlkRef->setPosition(OdGePoint3d(pntBase[0], pntBase[1], pntBase[2]);

//		pBlkRef->setScaleFactors(OdGeScale3d(xscale, yscale, 1.0));
//		pBlkRef->setPosition(OdGePoint3d(6, 6, 0));
//		return brefId;
		//////////////////////////////////////////////////////////////////////////////

//		AD_OBJHANDLE blkobjhandle;
//		if (adFindBlockheaderByName(m_hdb, szName, blkobjhandle) == 0)
//			continue;
//
//		AD_VMADDR entlist = adEntityList(m_hdb, blkobjhandle);
//		if (entlist != 0)
//		{
//			m_henhd->enttype = AD_ENT_BLOCK;
//			adSetEntityDefaults(m_hdb, m_henhd, m_hen);
//			adGenerateObjhandle(m_hdb, m_henhd->enthandle);
//	
//			CPnt pt = pBlock->GetBasePt();
//			pt.Get(m_hen->block.base);
//			strcpy(m_hen->block.name2, strKey);
//			
//			adAddEntityToList(m_hdb, entlist, m_henhd, m_hen);
//		}
//		pBlock->Write(m_hdb, entlist, m_henhd, m_hen);
//		
//		m_henhd->enttype = AD_ENT_ENDBLK;
//		adSetEntityDefaults(m_hdb, m_henhd, m_hen);
//		adGenerateObjhandle(m_hdb, m_henhd->enthandle);
//		adAddEntityToList(m_hdb, entlist, m_henhd, m_hen);
//	}
}

void CFileDWGDirect::WriteEntity()
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	// Layer table smart pointer, opened for read.
	OdDbLayerTablePtr pLayerTable = pDb->getLayerTableId().safeOpenObject();
//	OdDbBlockTableRecordPtr pPs = pDb->getModelSpaceId().safeOpenObject(OdDb::kForWrite);

	// Get a new layer table iterator (as a smart pointer)
	OdDbSymbolTableIteratorPtr pIter = pLayerTable->newIterator();
	for(pIter->start(); !pIter->done(); pIter->step())
	{
		// Layer Record smart pointer, opened for read.
		OdDbLayerTableRecordPtr pLayer = pIter->getRecordId().safeOpenObject();

		int nLayerIndex = pDoc->LayersLookup((LPCTSTR)pLayer->getName());
		if(nLayerIndex >= 0)
		{
			CLayer* pPegLayer = pDoc->LayersGetAt(nLayerIndex);

//			MessageBox(0, pPegLayer->GetName(), 0, 0);

			// causing crash
//			pLayer->setColorIndex(pPegLayer->PenColor());

			POSITION pos = pPegLayer->GetHeadPosition();
			while(pos != 0)
			{
				CSeg* pSeg = pPegLayer->GetNext(pos);
				pSeg->Write(pLayer->objectId(), pDb);
			}
		}
	}
						
//	AD_OBJHANDLE blkobjhandle;
//	adFindBlockheaderByName(m_hdb, "*MODEL_SPACE", blkobjhandle);
//	AD_VMADDR mspaceentlist = adEntityList(m_hdb, blkobjhandle);

//	OdDbLayerTablePtr pLayers;
//	OdDbLayerTableRecordPtr pLayer;
//	OdDbObjectId Layerid;
//
//	CLayer *pPegLayer = pDoc->LayersGetAt(0);
//
//	pLayers = pDb->getLayerTableId().safeOpenObject(OdDb::kForWrite);
//	OdDbBlockTableRecordPtr pPs = pDb->getPaperSpaceId().safeOpenObject(OdDb::kForWrite);
//
//	for(int i = 1; i < pDoc->LayersGetSize(); i++)
//	{
//		pPegLayer = pDoc->LayersGetAt(i);
//		pLayer = OdDbLayerTableRecord::createObject();
//
//		// Name must be set before a table object is added to a table.
//		pLayer->setName(OdString(pPegLayer->GetName().GetBuffer()));
////		pLayer->setColor(pPegLayer->PenColor());
//
//		// Add the object to the table.
//		Layerid = pLayers->add(pLayer);
//		
////		char szName[64];
////		strcpy(szName, pLayer->GetName());
////		adFindLayerByName(m_hdb, szName, CFileOpenDWG::ms_objecthandle);
//	
//		POSITION pos = pPegLayer->GetHeadPosition();
//		while(pos != 0)
//		{
//			CSeg* pSeg = pPegLayer->GetNext(pos);
//
//			OdDbEntityPtr pEnt;
//			pSeg->Write(pEnt);
//
//			if(!pEnt.isNull())
//			{
//				pEnt->setLayer(Layerid);
//				pPs->appendOdDbEntity(pEnt);
//			}
//		}
//	}
}



PENCOLOR GetEntPenColor(OdDbEntity* pEnt)
{
	// 0 indicates a color of BYBLOCK.
	// 256 indicates a color of BYLAYER.
	PENCOLOR nPenColor = pEnt->colorIndex();
	if(nPenColor == 0) 
		nPenColor = 7;
	else if(nPenColor == 256) 
		nPenColor = CPrim::PENCOLOR_BYLAYER;

	return nPenColor;
}

PENSTYLE GetEntPenStyle(OdDbEntity* pEnt)
{
	PENSTYLE nPenStyle = 0;
	if((!pEnt->isKindOf(OdDbFace::desc())) && (!pEnt->isKindOf(OdDbSolid::desc())) && (!pEnt->isKindOf(OdDbHatch::desc())))
	{
		if(Prim_IsByBlockPenStyle(pEnt->linetype())) 
			nPenStyle = 1;
		else if(Prim_IsByLayerPenStyle(pEnt->linetype())) 
			nPenStyle = CPrim::PENSTYLE_BYLAYER;
		else 
			nPenStyle = CPegDoc::GetDoc()->PenStylesLookup((LPCTSTR)pEnt->linetype());
	}

	return nPenStyle;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
CPrimArc::CPrimArc(OdDbEntity* pEnt, int nArcType)
{
	// 0 - arc
	// 1 - circle
	// 2 - ellipse
	
	// circle
	if(nArcType == 1)
	{ 
		OdDbCirclePtr pCircle = pEnt;

		OdGeVector3d vOdNorm = pCircle->normal();
		CVec vNorm(vOdNorm.x, vOdNorm.y, vOdNorm.z);

		CVec vXAx = Prim_ComputeArbitraryAxis(vNorm);
		CVec vYAx = vNorm ^ vXAx;

		OdGePoint3d vOdCent = pCircle->center();
		m_ptCenter(vOdCent.x, vOdCent.y, vOdCent.z);

		vXAx *= pCircle->radius();
		vYAx *= pCircle->radius();
		
		m_vMajAx = vXAx;
		m_vMinAx = vYAx;
		m_dSwpAng = TWOPI;
	}
	// ellipse
	else if(nArcType == 2)
	{
		// local z-axis is perpendicular to the plane on which the ellipse is drawn.
		// local x-axis is parallel to the major axis.
		// local origin the center of ellipse
		// within the lcs the representation of an ellipse is:
		//      E = (a * cos(u), b * sin(u), 0), u ranges form 0 to 2pi
		//			where a is half the major axis length and b is half the minor axis length
		// this entity does not use arbitrary axis algorithim
		// Conversion of parameter value to angle only matters if minor axis ratio is not 1. (non radial ellipses)
				
		// Can only properly represent ellipse which is radial or non radials which have a start parameter of 0.
		
		OdDbEllipsePtr pEll = pEnt;

		OdGeVector3d vOdNorm = pEll->normal();
		CVec vNorm(vOdNorm.x, vOdNorm.y, vOdNorm.z);

		CVec vXAx = Prim_ComputeArbitraryAxis(vNorm);
		CVec vYAx = vNorm ^ vXAx;

		OdGePoint3d vOdCent = pEll->center();
		m_ptCenter(vOdCent.x, vOdCent.y, vOdCent.z);

		OdGeVector3d vOdMajAx = pEll->majorAxis();
		m_vMajAx(vOdMajAx.x, vOdMajAx.y, vOdMajAx.z);

		m_vMinAx = vNorm ^ m_vMajAx;
		m_vMinAx *= pEll->radiusRatio();
	
		// Conversion of parameter 0 - 2pi to angle 0 - 2pi
		double dMajLen = m_vMajAx.Length();
		double dMinLen = m_vMinAx.Length();

		CVec vStart(dMajLen / 2. * cos(pEll->startAngle()), dMinLen / 2. * sin(pEll->startAngle()), 0.);
		double dStAng = Vec_Angle_xy(vStart);

		CVec vEnd(dMajLen / 2. * cos(pEll->endAngle()), dMinLen / 2. * sin(pEll->endAngle()), 0.);
		double dEndAng = Vec_Angle_xy(vEnd);

		if (dStAng >= TWOPI)
		{	// need to rationalize angs to first period angles in range on (0 to twopi)
			dStAng -= TWOPI;
			dEndAng -= TWOPI;
		}

		m_dSwpAng = dEndAng - dStAng;
		if (m_dSwpAng <= FLT_EPSILON) 
			m_dSwpAng += TWOPI;

		if (dStAng != 0.)
		{
			m_vMajAx.RotAboutArbAx(vNorm, dStAng);
			m_vMinAx.RotAboutArbAx(vNorm, dStAng);
		}
	}
	else if(nArcType == 0)
	{
		OdDbArcPtr pArc = pEnt;

		CPnt ptCP(pArc->center().x, pArc->center().y, pArc->center().z);

		////////////////////////////////////////////////////////////////////////
		OdGeVector3d vOdNorm = pArc->normal();
		CVec vNorm(vOdNorm.x, vOdNorm.y, vOdNorm.z);

		CVec vXAx = Prim_ComputeArbitraryAxis(vNorm);
		CVec vYAx = vNorm ^ vXAx;

		OdGePoint3d vOdCent = pArc->center();
		m_ptCenter(vOdCent.x, vOdCent.y, vOdCent.z);
		/////////////////////////////////////////////////////////////////////////////////////

		vXAx *= pArc->radius();
		vYAx *= pArc->radius();

		// Rectify the start and end angles .. Arcs are always drawn counterclockwise
		// from the start angle to the end angle
		double dStAng = pArc->startAngle();
		double dEndAng = pArc->endAngle();

		if (dStAng >= TWOPI)
		{	// need to rationalize angs to first period angles in range on (0 to twopi)
			dStAng -= TWOPI;
			dEndAng -= TWOPI;
		}

		double dAng = dEndAng - dStAng;
	
		if (dAng <= FLT_EPSILON)
			dAng += TWOPI;
		
		CPnt ptStart = Pnt_RotAboutArbPtAndAx(ptCP + vXAx, ptCP, vNorm, dStAng);	
	
		CVec vMajAx(ptCP, ptStart);
		CVec vMinAx = vNorm ^ vMajAx;
	
		m_ptCenter = ptCP;
		m_vMajAx = vMajAx;
		m_vMinAx = vMinAx;
		m_dSwpAng = dAng; 
	}

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

bool CPrimArc::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
	OdDbEllipsePtr pArc = OdDbEllipse::createObject();

	pArc->setColorIndex(m_nPenColor);
//	pLine->setLinetype("Continuous");

	CVec vNorm(m_vMajAx ^ m_vMinAx);
	vNorm.Normalize();

	// fixed crash that was not within the dwg set parameters
	if((min(m_vMinAx.Length() / m_vMajAx.Length(), 1.) < .000001) ||
	   (min(m_vMinAx.Length() / m_vMajAx.Length(), 1.) > 1.0) ||
	   (m_dSwpAng < .000001))
	   return false;

	pArc->set(OdGePoint3d(m_ptCenter[0], m_ptCenter[1], m_ptCenter[2]),
			  OdGeVector3d(vNorm[0], vNorm[1], vNorm[2]),
			  OdGeVector3d(m_vMajAx[0], m_vMajAx[1], m_vMajAx[2]),
			  min(m_vMinAx.Length() / m_vMajAx.Length(), 1.),
			  0., m_dSwpAng);

	pEnt = pArc;

	return true;
}

//bool CPrimBitmap::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
bool CPrimBitmap::Write(OdDbEntityPtr&, OdDbDatabasePtr)
{
	return false;
}

//bool CPrimBSpline::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
bool CPrimBSpline::Write(OdDbEntityPtr&, OdDbDatabasePtr )
{
	return false;
}

//bool CPrimCSpline::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
bool CPrimCSpline::Write(OdDbEntityPtr&, OdDbDatabasePtr)
{
	return false;
}

//bool CPrimDim::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
bool CPrimDim::Write(OdDbEntityPtr&, OdDbDatabasePtr)
{
	return false;
}

//CPrimInsert::CPrimInsert(AD_DB_HANDLE hdb, PAD_ENT_HDR henhd, PAD_ENT hen)
//{
//	AD_BLKH blkh;
//	adSeekBlockheader(hdb, hen->insert.blockheaderobjhandle, &blkh);
//
//	m_strName = blkh.name;
//	m_pt(hen->insert.pt0[0], hen->insert.pt0[1], hen->insert.pt0[2]);
//	m_vZ(henhd->extrusion[0], henhd->extrusion[1], henhd->extrusion[2]);
//	m_vScale(hen->insert.xscale, hen->insert.yscale, hen->insert.zscale);
//	m_dRotation = hen->insert.rotang;
//
//	if (henhd->extrusion[2] < 0.) {TRACE("Insert (block - %s) Entity Norm[2] = %f\n", blkh.name, henhd->extrusion[2]);}
//
//	m_vX = Prim_ComputeArbitraryAxis(m_vZ);
//	m_vY = m_vZ ^ m_vX;
//
//	m_vX *= hen->insert.xscale;
//	m_vY *= hen->insert.yscale;
//	m_vZ *= hen->insert.zscale;
//
//	//double dSinAng = sin(hen->insert.rotang);
//	//double dCosAng = cos(hen->insert.rotang);
//	//m_vX.RotAboutZAx(dSinAng, dCosAng);
//	//m_vY.RotAboutZAx(dSinAng, dCosAng);
//
//	CTMat tm(ORIGIN, m_vZ, hen->insert.rotang);
//	m_vX = tm * m_vX;
//	m_vY = tm * m_vY;
//}

//bool CPrimInsert::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
bool CPrimInsert::Write(OdDbEntityPtr&, OdDbDatabasePtr)
{
//	henhd->enttype = AD_ENT_INSERT;
//	adSetEntityDefaults(hdb, henhd, hen);
//	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
//	adGenerateObjhandle(hdb, henhd->enthandle);
//	
//	henhd->entcolor = m_nPenColor;
//	
//	char szName[64];
//	if (m_nPenStyle == PENSTYLE_BYLAYER)
//	{
//		strcpy(szName, "ByLayer");	
//	}
//	else 
//	{
//		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
//		strcpy(szName, pPenStyle->GetName());
//	}
//	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
//	
//	strcpy(szName, m_strName);
//	
//	adFindBlockheaderByName(hdb, szName, hen->insert.blockheaderobjhandle);
//	m_pt.Get(hen->insert.pt0);
//
//	hen->insert.xscale = m_vX.Length();
//	hen->insert.yscale = m_vY.Length();
//	hen->insert.zscale = m_vZ.Length();
//
//	hen->insert.rotang = Vec_Angle_xy(m_vX);
//	
//	hen->insert.numcols = m_wColCnt;
//	hen->insert.numrows = m_wRowCnt;
//	hen->insert.coldist = m_dColSpac;
//	hen->insert.rowdist = m_dRowSpac;
//
//	adAddEntityToList(hdb, entlist, henhd, hen);
//
//	return true;

	return false;
}

CPrimLine::CPrimLine(OdDbEntity *pEnt)
{
	OdDbLinePtr pLine = pEnt;

	m_ln[0](pLine->startPoint().x, pLine->startPoint().y, pLine->startPoint().z);
	m_ln[1](pLine->endPoint().x, pLine->endPoint().y, pLine->endPoint().z);

	/*SetPenColor(1);
	SetPenStyle(1);*/

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

bool CPrimLine::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
	OdDbLinePtr pLine = OdDbLine::createObject();

	if((m_nPenColor < 1) || (m_nPenColor > 255))
		m_nPenColor = 1;
	pLine->setColorIndex(m_nPenColor);
//	pLine->setLinetype("Continuous");

	pLine->setStartPoint(OdGePoint3d(m_ln[0][0], m_ln[0][1], m_ln[0][2]));
	pLine->setEndPoint(OdGePoint3d(m_ln[1][0], m_ln[1][1], m_ln[1][2]));

	pEnt = pLine;

	///////////////////////////////////////////////////////////////////////////////////

//	henhd->enttype = AD_ENT_LINE;
//	adSetEntityDefaults(hdb, henhd, hen);
//	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
//	adGenerateObjhandle(hdb, henhd->enthandle);
//	
//	henhd->entcolor = m_nPenColor;
//	
//	char szName[64];
//	if (m_nPenStyle == PENSTYLE_BYLAYER)
//	{
//		strcpy(szName, "ByLayer");	
//	}
//	else 
//	{
//		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
//		strcpy(szName, pPenStyle->GetName());
//	}
//	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
//	
//	hen->line.pt0[0] = m_ln[0][0];
//	hen->line.pt0[1] = m_ln[0][1];
//	hen->line.pt0[2] = m_ln[0][2];
//
//	hen->line.pt1[0] = m_ln[1][0];
//	hen->line.pt1[1] = m_ln[1][1];
//	hen->line.pt1[2] = m_ln[1][2];
//
//	adAddEntityToList(hdb, entlist, henhd, hen);

	return true;
}

CPrimMark::CPrimMark(OdDbEntity *pEnt)
{
	OdDbPointPtr pPoint = pEnt;

	m_nMarkStyle = m_nPenStyle;

	m_pt(pPoint->position().x, pPoint->position().y, pPoint->position().z);
	m_Dats = 0;
	m_dDat = 0;

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

bool CPrimMark::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
	OdDbPointPtr pPoint = OdDbPoint::createObject();

	pPoint->setColorIndex(m_nPenColor);
//	pLine->setLinetype("Continuous");

	pPoint->setPosition(OdGePoint3d(m_pt[0], m_pt[1], m_pt[2]));

	pEnt = pPoint;

	return true;
}

CPrimPolygon::CPrimPolygon(OdDbEntity *pEnt, int nPolyType)
{
	// 0 - Face
	// 1 - Solid
	// 2 - 3D Solid - not used
	// 3 - Hatch

	if(nPolyType == 0)
	{
		// Four sided, not necessarily planar, surface. It hides other objects and fills with solid color.
		// No support for individual edge visibilty
		OdDbFacePtr pFace = pEnt;

//		m_nIntStyle = POLYGON_SOLID;
		m_nIntStyle = POLYGON_HOLLOW;
		m_nIntStyleId = 0;

		CPnts pta;
		OdGePoint3d OdPoint;

		pFace->getVertexAt(0, OdPoint);
		CPnt pt(OdPoint.x, OdPoint.y, OdPoint.z);
		pta.Add(pt);

		pFace->getVertexAt(1, OdPoint);
		pt(OdPoint.x, OdPoint.y, OdPoint.z);
		if(pta[0] != pt)
			pta.Add(pt);

		pFace->getVertexAt(2, OdPoint);
		pt(OdPoint.x, OdPoint.y, OdPoint.z);
		if(pta[pta.GetUpperBound()] != pt)
			pta.Add(pt);

		pFace->getVertexAt(3, OdPoint);
		pt(OdPoint.x, OdPoint.y, OdPoint.z);
		if(pta[pta.GetUpperBound()] != pt && pta[0] != pt)
			pta.Add(pt);

		m_wPts = (WORD) pta.GetSize();
		m_Pt = new CPnt[m_wPts];
		
		for (WORD w = 0; w < m_wPts; w++) {m_Pt[w] = pta[w];}

		m_ptOrig = m_Pt[0];
		m_vPosXAx = XDIR;
		m_vPosYAx = YDIR;
	}
	else if(nPolyType == 1)
	{
		// The first two points define one edge of the polygon. 
		// The third point is diagonally opposite the second 
		// If the fourth point coincides with third result is a filled triangle.
		// else fourth point creates a quadrilateral area. 
		OdDbSolidPtr pSolid = pEnt;

		m_nIntStyle = POLYGON_SOLID;
		m_nIntStyleId = 0;

		CPnt pt0, pt1, pt2, pt3;
		OdGePoint3d OdPoint;
		pSolid->getPointAt(0, OdPoint);
		pt0(OdPoint.x, OdPoint.y, OdPoint.z);
		pSolid->getPointAt(1, OdPoint);
		pt1(OdPoint.x, OdPoint.y, OdPoint.z);
		pSolid->getPointAt(2, OdPoint);
		pt2(OdPoint.x, OdPoint.y, OdPoint.z);
		pSolid->getPointAt(3, OdPoint);
		pt3(OdPoint.x, OdPoint.y, OdPoint.z);

		m_wPts = WORD((pt2[0] == pt3[0] && pt2[1] == pt3[1] &&
					   pt2[2] == pt3[2]) ? 3: 4);

		m_Pt = new CPnt[m_wPts];

		m_Pt[0] = pt0;
		m_Pt[1] = pt1;
		m_Pt[2] = pt3;
		
		if (m_wPts == 4)
			m_Pt[3] = pt2;

		m_ptOrig = m_Pt[0];
		m_vPosXAx = XDIR;
		m_vPosYAx = YDIR;
	}
	else if(nPolyType == 2)
	{
		OdDb3dSolidPtr p3dSolid = pEnt;
	}
	else if(nPolyType == 3)
	{
		OdDbHatchPtr pHatch = pEnt;

		// hatch has no loops
		if(!pHatch->numLoops())
			return;

		CPnts pta;
		CPnt pt;

//		m_ptOrig = CPnt(hen->hatch.pt0[0], hen->hatch.pt0[1], hen->hatch.pt0[2]);
		m_ptOrig = CPnt(0., 0., 0.);
		
		CVec vNorm(pHatch->normal().x, pHatch->normal().y, pHatch->normal().z);
		m_vPosXAx = Prim_ComputeArbitraryAxis(vNorm);
		m_vPosYAx = vNorm ^ m_vPosXAx;
	
		m_nIntStyle = pHatch->isSolidFill() == 1 ? POLYGON_SOLID : POLYGON_HOLLOW;
		m_nIntStyleId = 0;

		// Hatched region is bounded by a number of paths (or loops). Loops must
		// be closed and may not intersect themselves or other loops. 
		// There is always at least one loop. Only processing one loop now.
		for (int iPath = 0; iPath < 1 /* hen->hatch.numpaths */; iPath++) 
		{
			long lLoopType = pHatch->loopTypeAt(iPath);
			 
			if(lLoopType & OdDbHatch::kPolyline)
			{  
				// Polyline-like loops - similar to a polyline entity. Consists of an arbitrary
				// number of vertices and option bulge factors. 
				// All vertices are 2d points within the lcs of the hatch.

				OdGePoint2dArray vertices;
				OdGeDoubleArray bulges;
			    
				pHatch->getLoopAt(iPath, vertices, bulges);

				m_wPts = (WORD)vertices.size();
				m_Pt = new CPnt[m_wPts];
			    
				OdGePoint2dArray::const_iterator verts;
				int iPathSeg = 0;
				for(verts = vertices.begin(); vertices.end() != verts; ++verts, iPathSeg++)
				{
					m_Pt[iPathSeg]((*verts).x, (*verts).y, 0.);
				}
			}
		    else
			{
				// Hatch contour composed of a series of edges.
				EdgeArray edges;
				pHatch->getLoopAt(iPath, edges);

				EdgeArray::const_iterator edge;
				for(edge = edges.begin(); edge != edges.end(); ++edge) 
				{
					OdGeCurve2d* pEdge = *edge;

					switch(pEdge->type()) 
					{
						case OdGe::kLineSeg2d:
						{
							OdGeLineSeg2d* pLineSeg = (OdGeLineSeg2d*)pEdge;
	
							pt(pLineSeg->startPoint().x, pLineSeg->startPoint().y, 0.);
							
							if (pta.GetSize() > 1) 
								if (pta[pta.GetUpperBound()] != pt) {pta.Add(pt);}
	
							pt(pLineSeg->endPoint().x, pLineSeg->endPoint().y, 0.);
							pta.Add(pt);
							break;
						}	        
						case OdGe::kCircArc2d:
						case OdGe::kEllipArc2d:
						case OdGe::kNurbCurve2d:
							break;				
					}
				}

				m_wPts = WORD(pta.GetSize());
				m_Pt = new CPnt[m_wPts];
						
				for (WORD w = 0; w < m_wPts; w++) {m_Pt[w] = pta[w];}
			}
		}
	}

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

bool CPrimPolygon::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
	//
//	if(m_nIntStyle == POLYGON_HOLLOW)
//	{
		OdDb3dPolylinePtr pPoly = OdDb3dPolyline::createObject();

		pPoly->setColorIndex(m_nPenColor);
	//	pLine->setLinetype("Continuous");
		
		for(int i = 0; i < m_wPts; i++)
		{
			OdDb3dPolylineVertexPtr pPolyVert = OdDb3dPolylineVertex::createObject();
			pPolyVert->setPosition(OdGePoint3d(m_Pt[i][0], m_Pt[i][1], m_Pt[i][2]));
			pPoly->appendVertex(pPolyVert);
		}

		pPoly->makeClosed();

		pEnt = pPoly;
//	}
//	else
//	{
//		OdDbHatchPtr pHatch = OdDbHatch::createObject();
//
//		pHatch->setColorIndex(m_nPenColor);
////		pHatch->setLinetype("Continuous");
//
//		pHatch->setElevation(0.);
//		pHatch->setHatchObjectType(OdDbHatch::kHatchObject);
//		pHatch->setHatchStyle(OdDbHatch::kNormal);
////		pHatch->setNormal(OdGeVector3d());
//
//		// set pattern
//		if(m_nIntStyle == POLYGON_SOLID)
//		{
//			
//		}
//		else if(m_nIntStyle == POLYGON_HATCH)
//		{
////			pHatch->setPattern(1, 
//		}
//
//		OdGePoint2dArray vertices;
//		OdGeDoubleArray bulges;
//		for(int i = 0; i < m_wPts; i++)
//		{
////			OdDb3dPolylineVertexPtr pPolyVert = OdDb3dPolylineVertex::createObject();
////			pPolyVert->setPosition(OdGePoint3d(m_Pt[i][0], m_Pt[i][1], m_Pt[i][2]));
////			pPoly->appendVertex(pPolyVert);
//			vertices.append(OdGePoint2d(m_Pt[i][0], m_Pt[i][1]));
//
//		}
//
////		pPoly->makeClosed();
//		pHatch->appendLoop(0, vertices, bulges);
//
//		pEnt = pHatch;
//
//	}

	return true;
}

CPrimPolyline::CPrimPolyline(OdDbEntity *pEnt, int nPolyType)
{
	// 0 - polyline
	// 1 - 2d polyline
	// 2 - 3d polyline

	CPnt p_pt;

	if(nPolyType == 0)
	{
		OdDbPolylinePtr pPoly = pEnt;
		for (unsigned int i = 0; i < pPoly->numVerts(); i++)
		{ 
			OdGePoint3d pt;
			pPoly->getPointAt(i, pt);

			p_pt(pt.x, pt.y, pt.z);
			m_pts.Add(p_pt);

	//		double bulge = pPoly->getBulgeAt(i);
	//		if (pPoly->segType(i) == OdDbPolyline::kArc)
	//		{
	////			os << "      " << "Bulge: " << bulge << STD(endl);
	//			if (i < (pPoly->numVerts() - 1) || pPoly->isClosed())
	//			{
	//				OdGeCircArc3d arc;
	//				pPoly->getArcSegAt(i, arc);
	//				OdGePoint3d start, end;
	//				start = arc.startPoint();
	//				end = arc.endPoint();
	////				os << "      " << "Arc Start: " << start.x << ", " << start.y << ", " << start.z << STD(endl);
	////				os << "      " << "Arc End: " << end.x << ", " << end.y << ", " << end.z << STD(endl);
	//			}
	//		}
		}

		m_wFlags = (pPoly->isClosed() ? AD_PLINE_CLOSED : 0);
	}
	else if(nPolyType == 1)
	{
		OdDb2dPolylinePtr pPoly = pEnt;
		OdDbObjectIteratorPtr pIter = pPoly->vertexIterator();
		for(; !pIter->done(); pIter->step())
		{
			OdDb2dVertexPtr pVertex = pIter->entity();

			p_pt(pVertex->position().x, pVertex->position().y, pVertex->position().z);
			m_pts.Add(p_pt);
		}

		m_wFlags = (pPoly->isClosed() ? AD_PLINE_CLOSED : 0);
	}
	else if(nPolyType == 2)
	{
		OdDb3dPolylinePtr pPoly = pEnt;
		OdDbObjectIteratorPtr pIter = pPoly->vertexIterator();
		for(; !pIter->done(); pIter->step())
		{
			OdDb3dPolylineVertexPtr pVertex = pIter->entity();

			p_pt(pVertex->position().x, pVertex->position().y, pVertex->position().z);
			m_pts.Add(p_pt);
		}

		m_wFlags = (pPoly->isClosed() ? AD_PLINE_CLOSED : 0);
	}

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

bool CPrimPolyline::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
	OdDb3dPolylinePtr pPoly = OdDb3dPolyline::createObject();

	pPoly->setColorIndex(m_nPenColor);
//	pLine->setLinetype("Continuous");

//	pPoint->setPosition(OdGePoint3d(m_pt[0], m_pt[1], m_pt[2]));
	
	for(int i = 0; i < m_pts.GetSize(); i++)
	{
//		double dPt = m_Pt[w];
		OdDb3dPolylineVertexPtr pPolyVert = OdDb3dPolylineVertex::createObject();
		pPolyVert->setPosition(OdGePoint3d(m_pts[i][0], m_pts[i][1], m_pts[i][2]));
		pPoly->appendVertex(pPolyVert);
	}

	if((m_wFlags & AD_PLINE_CLOSED) == AD_PLINE_CLOSED)
		pPoly->makeClosed();
	else
		pPoly->makeOpen();


	pEnt = pPoly;

	////////////////////////////////////////////////////////////////////////////////

//	henhd->enttype = adLwplineEnttype(hdb);
//	adSetEntityDefaults(hdb, henhd, hen);
//	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
//	adGenerateObjhandle(hdb, henhd->enthandle);
//	
//	hen->lwpline.numpoints = m_pts.GetSize();
//	hen->lwpline.flag = m_wFlags;
//	
//	hen->lwpline.ldblob = adCreateBlob();
//
//	PAD_BLOB_CTRL bcptr = adStartBlobWrite(hen->lwpline.ldblob);
//	
//	double dPt[3];
//	double dBulge = 0.;
//	double dWidth[2] = {0., 0.};
//
//	for (WORD w = 0; w < m_pts.GetSize(); w++) 
//	{
//		m_pts[w].Get(dPt);
//		adWriteBlob2Double(bcptr, dPt);
//		
//		if (m_wFlags & AD_LWPLINE_HAS_BULGES)
//			adWriteBlobDouble(bcptr, &dBulge);
//		if (m_wFlags & AD_LWPLINE_HAS_WIDTHS)
//			adWriteBlob2Double(bcptr, dWidth);
//	}
//	adEndBlobWrite(bcptr);
//	adAddEntityToList(hdb, entlist, henhd, hen);

	return true;
}

CPrimSegRef::CPrimSegRef(OdDbEntity *pEnt, int nBlockType)
{
	// 0 - block reference
	// 1 - mInsert

	// this type includes XREFs
	if(nBlockType == 0)
	{
		OdDbBlockReferencePtr pBlock = pEnt;

		OdDbObjectId BlockId = pBlock->blockTableRecord();
		OdDbBlockTableRecordPtr pRec = BlockId.openObject();

		if(pRec->isFromExternalReference() || pRec->isFromOverlayReference())
		{
			OdDbXRefMan::load(pRec);
			
			if(pRec->xrefStatus() == OdDb::kXrfUnreferenced ||
			   pRec->xrefStatus() == OdDb::kXrfFileNotFound ||
			   pRec->xrefStatus() == OdDb::kXrfUnresolved)
			   AcadMode::strXREFs.push_back((const TCHAR*)pRec->pathName());
			   //msgWarning((UINT)IDS_MSG_XREF_NOTFOUND, (const TCHAR*)pRec->pathName());
		}

		m_strName = (LPCTSTR)pRec->getName();
		m_pt(pBlock->position().x, pBlock->position().y, pBlock->position().z);
		m_vZ(pBlock->normal().x, pBlock->normal().y, pBlock->normal().z);

		OdGeScale3d OdScale = pBlock->scaleFactors();
		m_vScale(OdScale[0], OdScale[1], OdScale[2]);
		m_dRotation = pBlock->rotation();

		m_wColCnt = 1;
		m_wRowCnt = 1;
		m_dColSpac = 0.;
		m_dRowSpac = 0.;

		//CBlock* pPegBlock;
		//if(!CPegDoc::GetDoc()->BlksLookup(pRec->getName().c_str(), pPegBlock))
		//{
		//	//		b0 set - anonymous block
		//	//		b1 set - block has attribute definitions
		//	//		b2 set - block is an external reference
		//	//		b3 set - not used
		//	//		b4 set - block is externally dependent
		//	//		b5 set - block is a resolved external reference
		//	//		b6 set - definition is referenced
		//	WORD wFlag = 0;
		//	if(pRec->isAnonymous())				{wFlag |= 0x01;}
		//	if(pRec->hasAttributeDefinitions())	{wFlag |= 0x02;}
		//	if(pRec->isFromExternalReference())	{wFlag |= 0x04;}

		//	CPnt ptBase(pRec->origin().x, pRec->origin().y, pRec->origin().z);
		//	pPegBlock = new CBlock(wFlag, ptBase, pRec->pathName().c_str());
		//	CPegDoc::GetDoc()->BlksSetAt(pRec->getName().c_str(), pPegBlock);
		//}
	}
	else if(nBlockType == 1)
	{
		OdDbMInsertBlockPtr pBlock = pEnt;

		OdDbObjectId BlockId = pBlock->blockTableRecord();
		OdDbBlockTableRecordPtr pRec = BlockId.openObject();

//		MessageBox(0, pRec->getName().c_str(), 0, 0);

		m_strName = (LPCTSTR)pRec->getName();
		m_pt(pBlock->position().x, pBlock->position().y, pBlock->position().z);
		m_vZ(pBlock->normal().x, pBlock->normal().y, pBlock->normal().z);

		OdGeScale3d OdScale = pBlock->scaleFactors();
		m_vScale(OdScale[0], OdScale[1], OdScale[2]);
		m_dRotation = pBlock->rotation();

		m_wColCnt = pBlock->columns();
		m_wRowCnt = pBlock->rows();
		m_dColSpac = pBlock->columnSpacing();
		m_dRowSpac = pBlock->rowSpacing();

		//CBlock* pPegBlock;
		//if(!CPegDoc::GetDoc()->BlksLookup(pRec->getName().c_str(), pPegBlock))
		//{
		//	//		b0 set - anonymous block
		//	//		b1 set - block has attribute definitions
		//	//		b2 set - block is an external reference
		//	//		b3 set - not used
		//	//		b4 set - block is externally dependent
		//	//		b5 set - block is a resolved external reference
		//	//		b6 set - definition is referenced
		//	WORD wFlag = 0;
		//	if(pRec->isAnonymous())				{wFlag |= 0x01;}
		//	if(pRec->hasAttributeDefinitions())	{wFlag |= 0x02;}
		//	if(pRec->isFromExternalReference())	{wFlag |= 0x04;}

		//	CPnt ptBase(pRec->origin().x, pRec->origin().y, pRec->origin().z);
		//	pPegBlock = new CBlock(wFlag, ptBase, pRec->pathName().c_str());
		//	CPegDoc::GetDoc()->BlksSetAt(pRec->getName().c_str(), pPegBlock);
		//}
	}

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

bool CPrimSegRef::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
//	CPegDoc* pDoc = CPegDoc::GetDoc();

//	OdDbDatabasePtr pDb = pEnt->database();
	OdDbBlockTableRecordPtr pBlock;

	OdDbBlockTablePtr pBlockTable = pDb->getBlockTableId().safeOpenObject(OdDb::kForWrite);

	// Get an iterator for the block table
	OdDbSymbolTableIteratorPtr pBlkIter = pBlockTable->newIterator();
	  
	// For each block in the block table
	for(pBlkIter->start(); !pBlkIter->done(); pBlkIter->step())
	{
		// Open the block
		pBlock = pBlkIter->getRecordId().safeOpenObject();

		CString strOdName = (LPCTSTR)pBlock->getName();
		if(strOdName == m_strName)
			break;
	}

	if(pBlock.isNull())
		return false;

	CString strKey;
//	CBlock* pPegBlock;

	OdDbBlockReferencePtr pBlkRef = OdDbBlockReference::createObject();

	pBlkRef->setColorIndex(m_nPenColor);
//	pLine->setLinetype("Continuous");

	pBlkRef->setPosition(OdGePoint3d(m_pt[0], m_pt[1], m_pt[2]));
	pBlkRef->setBlockTableRecord(pBlock->objectId());
	pBlkRef->setNormal(OdGeVector3d(m_vZ[0], m_vZ[1], m_vZ[2]));
	pBlkRef->setRotation(m_dRotation);
	pBlkRef->setScaleFactors(OdGeScale3d(m_vScale[0], m_vScale[1], m_vScale[2]));

	pEnt = pBlkRef;

//	CString	m_strName;
//	CPnt		m_pt;
//	CVec		m_vZ;
//
//	CVec		m_vScale;
//	double	m_dRotation;
//
//	WORD		m_wColCnt;
//	WORD		m_wRowCnt;
//	double	m_dColSpac;
//	double	m_dRowSpac;
//
//	POSITION pos = pDoc->BlksGetStartPosition();
//	while(pos != 0)
//	{
//        pDoc->BlksGetNextAssoc(pos, strKey, pPegBlock);
//
//		OdDbBlockReferencePtr pBlkRef = OdDbBlockReference::createObject();
//
//		// Add the entity to the parent block.
//		pBlock->appendOdDbEntity(pBlkRef);
//
//		// Set the remaining entity properties.
//		pBlkRef->setBlockTableRecord(pBlock->objectId());//blockId);
//		
//		CPnt pntBase = pPegBlock->GetBasePt();
////		pBlkRef->set
//		pBlkRef->setPosition(OdGePoint3d(pntBase[0], pntBase[1], pntBase[2]);
//		pBlkRef->setScaleFactors(OdGeScale3d(xscale, yscale, 1.0));
//	}

	///////////////////////////////////////////////////////////////////////////////////////

//	OdDbBlockReferencePtr pBlock = OdDbBlockReference::createObject();

//	pBlock->setColorIndex(m_nPenColor);
//	pLine->setLinetype("Continuous");

//	pBlock->setNormal(OdGeVector3d(m_vZ[0], m_vZ[1], m_vZ[2]));
	

//	pEnt = pBlock;

	////////////////////////////////////////////////////////////////////////////////////

//	henhd->enttype = AD_ENT_INSERT;
//	adSetEntityDefaults(hdb, henhd, hen);
//	adHancpy(henhd->entlayerobjhandle, CFileOpenDWG::ms_objecthandle);
//	adGenerateObjhandle(hdb, henhd->enthandle);
//	
//	henhd->entcolor = m_nPenColor;
//	
//	char szName[64];
//	if (m_nPenStyle == PENSTYLE_BYLAYER) {strcpy(szName, "ByLayer");}
//	else 
//	{
//		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAt(m_nPenStyle);
//		strcpy(szName, pPenStyle->GetName());
//	}
//	adFindLinetypeByName(hdb, szName, henhd->entltypeobjhandle);
//	m_vZ.Get(henhd->extrusion);
//	
//	strcpy(szName, m_strName);
//	
//	adFindBlockheaderByName(hdb, szName, hen->insert.blockheaderobjhandle);
//	m_pt.Get(hen->insert.pt0);
//	
//	hen->insert.xscale = m_vScale[0];
//	hen->insert.yscale = m_vScale[1];
//	hen->insert.zscale = m_vScale[2];
//
//	hen->insert.rotang = m_dRotation;
//	
//	hen->insert.numcols = m_wColCnt;
//	hen->insert.numrows = m_wRowCnt;
//	hen->insert.coldist = m_dColSpac;
//	hen->insert.rowdist = m_dRowSpac;
//
//	adAddEntityToList(hdb, entlist, henhd, hen);

	return true;
}

//bool CPrimTag::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
bool CPrimTag::Write(OdDbEntityPtr&, OdDbDatabasePtr)
{
	return false;
}

CPrimText::CPrimText(OdDbEntity *pEnt, int nTextType)
{
	// 0 - text
	// 1 - arc aligned text

	if(nTextType == 0)
	{
		OdDbTextPtr pText = pEnt;

		OdDbObjectId OdId = pText->textStyle();
		OdDbTextStyleTableRecordPtr pTextStyle = OdId.safeOpenObject();

		OdString strFace;
		bool bb, bi;  //We don't care about these, but font() requires them
		int set, fam; //same
		pTextStyle->font(strFace, bb, bi, set, fam);
		////
		//GIG - 05/11
		//This doesn't really seem to work, although it probably should
		//Use font() method instead
		//CString strFont = (LPCTSTR)pTextStyle->fileName();
		CString strFont = (LPCTSTR)strFace;
		//MessageBox(0, strFace, "Import: font()", 0);
		int nExt = strFont.ReverseFind('.');
		if (nExt != - 1)
		{
			if (strFont.Mid(nExt).CompareNoCase(".shx") == 0)
			{		
				strFont.Truncate(nExt);
				for (int n = nExt; n < 8; n++)
					strFont += '_';
				
				strFont += ".ttf";
			}
		}

		m_fd.Set(CFontDef::PREC_TRUETYPEFONT, strFont, CFontDef::PATH_RIGHT, CFontDef::HOR_ALIGN_LEFT, CFontDef::VER_ALIGN_BOTTOM, 0.);

		CCharCellDef ccd(pText->rotation(), -pText->oblique(), pText->widthFactor(), pText->height());
		
		CVec vDirX;
		CVec vDirY;
		CharCellDef_EncdRefSys(ccd, vDirX, vDirY);
		m_rs.SetDirX(vDirX);
		m_rs.SetDirY(vDirY);

		OdGePoint3d pOdOrigin = pText->position();
		m_rs.SetOrigin(CPnt(pOdOrigin.x, pOdOrigin.y, pOdOrigin.z));
		m_strText = (LPCTSTR)pText->textString();
	}
	else if(nTextType == 1)
	{
		OdDbArcAlignedTextPtr pAAText = pEnt;
		
		OdDbObjectId OdId = pAAText->textStyle();
		OdDbTextStyleTableRecordPtr pTextStyle = OdId.safeOpenObject();
		CString strFont = (LPCTSTR)pTextStyle->fileName();
		int nExt = strFont.ReverseFind('.');
		if (nExt != - 1)
		{
			if (strFont.Mid(nExt).CompareNoCase(".shx") == 0)
			{		
				strFont.Truncate(nExt);
				for (int n = nExt; n < 8; n++)
					strFont += '_';
				
				strFont += ".ttf";
			}
		}

		m_fd.Set(CFontDef::PREC_TRUETYPEFONT, strFont, CFontDef::PATH_RIGHT, CFontDef::HOR_ALIGN_LEFT, CFontDef::VER_ALIGN_BOTTOM, 0.);

		CCharCellDef ccd(0, 0, 1, .1);
//		CCharCellDef ccd(pText->rotation(), pText->oblique(), pText->widthFactor(), pText->height());
		
		CVec vDirX;
		CVec vDirY;
		CharCellDef_EncdRefSys(ccd, vDirX, vDirY);
		m_rs.SetDirX(vDirX);
		m_rs.SetDirY(vDirY);

		OdGePoint3d pOdOrigin = pAAText->center();
		m_rs.SetOrigin(CPnt(pOdOrigin.x, pOdOrigin.y, pOdOrigin.z));
		m_strText = (LPCTSTR)pAAText->textString();
	}

	SetPenColor(GetEntPenColor(pEnt));
	SetPenStyle(GetEntPenStyle(pEnt));
}

//#include "StringExtra.h"
//#include "text.h"
//CSeg *VectorizeCharacter(const CFontDef& fd, const CTMat& tm, int nOff, int nLen, const CString& strText);

bool CPrimText::Write(OdDbEntityPtr &pEnt, OdDbDatabasePtr pDb)
{
	OdDbTextPtr pText = OdDbText::createObject();

	//// GIG - 06/11
	//The name of the font in m_fd may or may not include the extension.
	//This doesn't bother peg, but Autocad really needs it to be there.
	CString font = m_fd.TextFont();
	if (font.Right(font.ReverseFind('.')) == "shx")
	{}
	else if (font.Right(font.ReverseFind('.')) == "psf")
	{}
	else if (font.Right(font.ReverseFind('.')) == "ttf")
	{}
	else if (font.ReverseFind('.') == -1)
	{
		font += ".ttf";
	}
	else
	{
#ifdef _PEG_RC_DEBUG
		//TODO: Add conversion logging
		//font is some unknown type, need to add logging of font, text, and filename
#endif
	}

	OdDbTextStyleTablePtr pTable = pDb->getTextStyleTableId().safeOpenObject(OdDb::kForWrite);
	OdDbTextStyleTableRecordPtr pStyle = pTable->getAt(font.GetBuffer(), OdDb::kForWrite);
	if(pStyle.isNull())
	{
		pStyle = OdDbTextStyleTableRecord::createObject();
		pStyle->setFileName(font.GetBuffer());
		pStyle->setFont(m_fd.TextFont().GetBuffer(), false, false, DEFAULT_CHARSET, DEFAULT_PITCH);
	}
	
	try
	{
		pStyle->setName(font.GetBuffer());
		pTable->add(pStyle);
	}
	catch(OdError ex)
	{
#ifdef _PEG_RC_DEBUG
		//TODO: Add conversion logging
		//Teigha has thrown some kind of error.  These are usually not fatal, but still
		//a bit of a problem.  Add logging of text, filename, and error description.
#endif
	}

	pText->setTextStyle(pStyle->objectId());
	pText->setColorIndex(m_nPenColor);


	////
	//GIG
	//Quick fix for text position when converting to DWG
	CPnt pntOrig = m_rs.Origin() - (m_rs.GetYDir() / 2);
	//pntOrig[0] += estate.dXShift;	//Without these, the offset fields in the export dialog won't actually do anything
	//pntOrig[1] += estate.dYShift; //which is fine, just good to be aware of.

	pText->setHeight(m_rs.DirY().Length());
	pText->setRotation(Vec_Angle_xy(m_rs.DirX()));
	pText->setTextString(m_strText.GetBuffer());
	
	pText->setWidthFactor(estate.dWidthFactor);

	switch(m_fd.TextHorAlign())
	{
		case CFontDef::HOR_ALIGN_LEFT:
			pText->setHorizontalMode(OdDb::kTextLeft);
			break;
		case CFontDef::HOR_ALIGN_CENTER:
			pText->setHorizontalMode(OdDb::kTextCenter);
			break;
		case CFontDef::HOR_ALIGN_RIGHT:
			pText->setHorizontalMode(OdDb::kTextRight);
			break;
	}

	switch(m_fd.TextVerAlign())
	{
		case CFontDef::VER_ALIGN_TOP:
			pText->setVerticalMode(OdDb::kTextTop);
			break;
		case CFontDef::VER_ALIGN_MIDDLE:
			pText->setVerticalMode(OdDb::kTextVertMid);
			break;
		case CFontDef::VER_ALIGN_BOTTOM:
			pText->setVerticalMode(OdDb::kTextBottom);
			break;
	}

	//GIG - This seems like it could be/have been causing the text shifting problems
	pText->setAlignmentPoint(OdGePoint3d(pntOrig[0] + estate.dXShift, pntOrig[1] + estate.dYShift, pntOrig[2]));

	pEnt = pText;
	
	return true;
}

int ExplodeTextFragments(OdDbMTextFragment *pFragment, void *pStringSeg)
{
	CSeg *pSeg = (CSeg*)pStringSeg;

	WORD wPath = CFontDef::PATH_RIGHT;
	WORD wHAlign = CFontDef::HOR_ALIGN_LEFT;
	WORD wVAlign = CFontDef::VER_ALIGN_BOTTOM;

	if(pFragment->vertical)
		wPath = CFontDef::PATH_UP;

	CFontDef fd;
	fd.Set(CFontDef::PREC_TRUETYPEFONT, (LPCTSTR)pFragment->fontname, wPath, wHAlign, wVAlign, 0.);

	CString strText = (LPCTSTR)pFragment->text;

//	double dWidth = 1 - ((strText.GetLength() - 1) * .01363636);
	CCharCellDef ccd(0, -pFragment->obliqueAngle, pFragment->widthFactor, pFragment->capsHeight);

	OdGePoint3d OdPoint(pFragment->location);
	CRefSys rs(CPnt(OdPoint.x, OdPoint.y, OdPoint.z), ccd);

//	strText.Replace("\\U+00B0", "alf");

	//%%c to Ø (diameter)
	//%%d to ° (Degree)
	//%%p to ± ( Plus/minus)
	//\U+2205 to Ø Diameter
	//\U+00b0 to ° (Degree)
	//\U+00b1 ± ( Plus/minus)

	pSeg->AddTail(new CPrimText(fd, rs, strText));
	
//	//	CString strFont = shptb.file;
//	//	int nExt = strFont.ReverseFind('.');
//	//	if (nExt != - 1)
//	//	{
//	//		if (strFont.Mid(nExt).CompareNoCase(".shx") == 0)
//	//		{		
//	//			strFont.Truncate(nExt);
//	//			for (int n = nExt; n < 8; n++)
//	//				strFont += '_';
//	//			
//	//			strFont += ".ttf";
//	//		}
//	//	}

//		switch(pMText->attachment())
//		{
//			case OdDbMText::kTopLeft: 
//				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
//				wVerAlign = CFontDef::VER_ALIGN_TOP; 
//				break;
//			case OdDbMText::kTopCenter: 
//				wHorAlign = CFontDef::HOR_ALIGN_CENTER; 
//				wVerAlign = CFontDef::VER_ALIGN_TOP; 
//				break;
//			case OdDbMText::kTopRight: 
//				wHorAlign = CFontDef::HOR_ALIGN_RIGHT; 
//				wVerAlign = CFontDef::VER_ALIGN_TOP; 
//				break;
//			case OdDbMText::kMiddleLeft: 
//				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
//				wVerAlign = CFontDef::VER_ALIGN_MIDDLE; 
//				break;
//			case OdDbMText::kMiddleCenter: 
//				wHorAlign = CFontDef::HOR_ALIGN_CENTER; 
//				wVerAlign = CFontDef::VER_ALIGN_MIDDLE; 
//				break;
//			case OdDbMText::kMiddleRight: 
//				wHorAlign = CFontDef::HOR_ALIGN_RIGHT; 
//				wVerAlign = CFontDef::VER_ALIGN_MIDDLE; 
//				break;
//			case OdDbMText::kBottomLeft: 
//				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
//				wVerAlign = CFontDef::VER_ALIGN_BOTTOM; 
//				break;
//			case OdDbMText::kBottomCenter: 
//				wHorAlign = CFontDef::HOR_ALIGN_CENTER; 
//				wVerAlign = CFontDef::VER_ALIGN_BOTTOM; 
//				break;
//			case OdDbMText::kBottomRight: 
//				wHorAlign = CFontDef::HOR_ALIGN_RIGHT; 
//				wVerAlign = CFontDef::VER_ALIGN_BOTTOM; 
//				break;
//			default:
//				wHorAlign = CFontDef::HOR_ALIGN_LEFT; 
//				wVerAlign = CFontDef::VER_ALIGN_BOTTOM;
//		}
//
//		m_fd.Set(CFontDef::PREC_TRUETYPEFONT, strFont, CFontDef::PATH_RIGHT, wHorAlign, wVerAlign, 0.);
//	
//		OdGeVector3d vOdxDir = pMText->direction();
//		CVec vXDir(vOdxDir.x, vOdxDir.y, vOdxDir.z);
//		
//		CCharCellDef ccd(Vec_Angle_xy(vXDir), 0., 1., pMText->textHeight());
//				
//		CVec vDirX;
//		CVec vDirY;
//		CharCellDef_EncdRefSys(ccd, vDirX, vDirY);
//		m_rs.SetDirX(vDirX);
//		m_rs.SetDirY(vDirY);
//
//		m_rs.SetOrigin(CPnt(pMText->location().x, pMText->location().y, pMText->location().z));
//
//		m_strText = pMText->contents().c_str();

	return 1;
}
