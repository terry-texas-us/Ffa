#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Prim.h"

#include "OdaCommon.h"
#include "FileDWGDirectDumper.h"

#include "SubProcAnnotate.h"

#include "RxObjectImpl.h"
#include "Db2dPolyline.h"
#include "Db3dPolyline.h"
#include "Db3dPolylineVertex.h"
#include "DbPolyFaceMesh.h"
#include "DbPolyFaceMeshVertex.h"
#include "DbFaceRecord.h"
#include "DbPolygonMesh.h"
#include "DbPolygonMeshVertex.h"
#include "DbBlockReference.h"
#include "DbBlockTableRecord.h"
#include "DbAttribute.h"
#include "DbMInsertBlock.h"
#include "DbSpline.h"
#include "DbEllipse.h"
#include "DbSolid.h"
#include "DbTrace.h"
#include "DbHatch.h"
#include "DbCircle.h"
#include "Db3dSolid.h"
#include "DbRegion.h"
#include "DbMText.h"
#include "DbMline.h"
#include "DbRasterImage.h"
#include "DbOle2Frame.h"
#include "Ge/GeKnotVector.h"
#include "DbProxyEntity.h"
#include "StaticRxObject.h"
#include "DbSpatialFilter.h"
#include "DbIndex.h"
#include "DbPolyline.h"
#include "DbArcAlignedText.h"
#include "Ge/GeCircArc3d.h"
#include "Ge/GeCurve2d.h"
#include "Ge/GeEllipArc2d.h"
#include "Ge/GeNurbCurve2d.h"
#include "OdFileBuf.h"
#include "GeometryFromProxy.h"
#include "DbArcAlignedText.h"
#include "DbAlignedDimension.h"
#include "DbAttribute.h"
#include "DbViewport.h"

#include "DbLine.h"
#include "DbArc.h"
#include "DbPoint.h"
#include "DbFace.h"
#include "DbSolid.h"
#include "Db3dSolid.h"
#include "DbLeader.h"

//#include "GiWorldDrawDumper.h"

int ExplodeTextFragments(OdDbMTextFragment *, void *);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ODRX_NO_CONS_DEFINE_MEMBERS(OdDbEntity_Dumper, OdRxObject)

extern PENCOLOR GetEntPenColor(OdDbEntity* pEnt);
extern PENSTYLE GetEntPenStyle(OdDbEntity* pEnt);

CSeg* BreakPolyline(OdDbEntity* pEnt, int nPolyType)
{
	// 0 - polyline
	// 1 - 2d polyline
	// 2 - 3d polyline

	CPnt p_pt;
	CPnts m_pts;
	//WORD m_wFlags = 0;
	bool bIsClosed = false;

	//PENCOLOR PenColor = 1;
	//PENSTYLE PenStyle = 1;

	CSeg *pSeg = new CSeg();

	if(nPolyType == 0)
	{
		OdDbPolylinePtr pPoly = pEnt;

		//PenColor = pPoly-

		for (unsigned int i = 0; i < pPoly->numVerts(); i++)
		{ 
			OdGePoint3d pt;
			pPoly->getPointAt(i, pt);

			p_pt(pt.x, pt.y, pt.z);
			m_pts.Add(p_pt);

			// arcs in the polyline
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
	//				
	////				os << "      " << "Arc Start: " << start.x << ", " << start.y << ", " << start.z << STD(endl);
	////				os << "      " << "Arc End: " << end.x << ", " << end.y << ", " << end.z << STD(endl);
	//			}
	//		}
		}

		//m_wFlags = (pPoly->isClosed() ? AD_PLINE_CLOSED : 0);
		bIsClosed = pPoly->isClosed();
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

		//m_wFlags = (pPoly->isClosed() ? AD_PLINE_CLOSED : 0);
		bIsClosed = pPoly->isClosed();
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

		//m_wFlags = (pPoly->isClosed() ? AD_PLINE_CLOSED : 0);
		bIsClosed = pPoly->isClosed();
	}

	////////////////////////////////////////////////////////////////////////////////////////
	// time to create the prims
	for(int j = 0; j < m_pts.GetSize() - 1; j++)
	{
		CPrimLine *pPrimLine = new CPrimLine(GetEntPenColor(pEnt), GetEntPenStyle(pEnt), m_pts[j], m_pts[j+1]);
		//((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
		pSeg->AddTail(pPrimLine);
	}

	// add final line if closed polyline
	if(bIsClosed)
	{
		CPrimLine *pPrimLine = new CPrimLine(GetEntPenColor(pEnt), GetEntPenStyle(pEnt), m_pts[m_pts.GetSize() - 1], m_pts[0]);
		//((CObList*)pSeg)->InsertAfter(curPos, pPrimLine);
		pSeg->AddTail(pPrimLine);
	}

	// add arcs if applicable
	if(nPolyType == 0)
	{

	}

	return pSeg;
}

ExProtocolExtension::ExProtocolExtension() {}

ExProtocolExtension::~ExProtocolExtension()
{
	if(m_pDumpers)
		uninitialize();
}

// dumps common data (for all entities)
void DumpCommonData(OdDbEntity* pEnt)
{
	OdDbHandle    hTmp;
//	char          buff[20];
	OdChar		  buff[20];

	hTmp = pEnt->getDbHandle();
	hTmp.getIntoAsciiBuffer(buff);
//	os << "    " << pEnt->isA()->name() << ", " << buff << STD(endl);
}

// this method is called for all entities for which there 
// aren't individual handlers
CSeg* OdDbEntity_Dumper::Dump(OdDbEntity* pEnt) const
{
	DumpCommonData(pEnt);
	  
	//  os << "      " << "Entity graphics data:" << STD(endl);
	  
	// Dump the graphics data of "unknown" entity
	// graphics for proxy entity are retrieved by the same way
//	OdGiContextDumper ctx(pEnt->database());
//	OdGiWorldDrawDumper wd;
//	wd.setContext(&ctx);
//	pEnt->worldDraw(&wd);
	
	return ((CSeg*)0);
} // end OdDbEntity_Dumper::dump

class OdDbPolyFaceMesh_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);

			OdDbPolyFaceMeshPtr pPoly = pEnt;

			CSeg *pRetSeg = new CSeg;
			OdRxObjectPtrArray entitySet;

			pPoly->explode(entitySet);

			for(int i = 0; i < (int)entitySet.size(); i++)
			{
				OdDbFacePtr pFace = entitySet.getAt(i);
				
				OdSmartPtr<OdDbEntity_Dumper> pFaceDumper = pFace;

				CSeg* pSeg = pFaceDumper->Dump(pFace);
				POSITION pos = pSeg->GetHeadPosition();
				while(pos != 0)
				{
					CPrim *pPrim = pSeg->GetNext(pos);

					if(pPrim != 0)
						pRetSeg->AddTail(pPrim);
				}
			}

			return pRetSeg;
		}
};

class OdDbPolygonMesh_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);

			OdDbPolygonMeshPtr pPoly = pEnt;

			CSeg *pRetSeg = new CSeg;
			OdRxObjectPtrArray entitySet;

			pPoly->explode(entitySet);

			for(int i = 0; i < (int)entitySet.size(); i++)
			{
				OdDbFacePtr pFace = entitySet.getAt(i);
				
				OdSmartPtr<OdDbEntity_Dumper> pFaceDumper = pFace;

				CSeg* pSeg = pFaceDumper->Dump(pFace);
				POSITION pos = pSeg->GetHeadPosition();
				while(pos != 0)
				{
					CPrim *pPrim = pSeg->GetNext(pos);

					if(pPrim != 0)
						pRetSeg->AddTail(pPrim);
				}
			}

			return pRetSeg;
		}
};

//class OdDbTrace_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//
//			OdDbTracePtr pTrace = pEnt;
//			OdGePoint3d pt;
//			pTrace->getPointAt(0, pt);
////			os << "      " << "Point 0: " << pt.x << ", " << pt.y << ", " << pt.z << STD(endl);
//			pTrace->getPointAt(1, pt);
////			os << "      " << "Point 1: " << pt.x << ", " << pt.y << ", " << pt.z << STD(endl);
//			pTrace->getPointAt(2, pt);
////			os << "      " << "Point 2: " << pt.x << ", " << pt.y << ", " << pt.z << STD(endl);
//			pTrace->getPointAt(3, pt);
////			os << "      " << "Point 3: " << pt.x << ", " << pt.y << ", " << pt.z << STD(endl);
//		}
//};

//class OdDbProxyEntity_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			// this will dump proxy entity graphics
//			OdDbEntity_Dumper::Dump(pEnt);
//
//			// If the entity is of type AcAdPart, then dump the 
//			// associated SAT file.
//			OdDbProxyEntityPtr pProxy(pEnt);
//			OdString satString;
//
//			if (pProxy->originalClassName() == "AcAdPart" && odGetSatFromProxy(pProxy, satString))
//			{
////				os << "      " << "AD_PART SAT file: " << STD(endl) << satString.c_str();
//			}      
//		}
//}; // end OdDbProxyEntity_Dumper

//class OdDbRegion_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//			OdDbRegionPtr pRegion = pEnt;
//
//			OdWrFileBuf ow("RegionAcisOut.acis");
//			pRegion->acisOut( &ow , kAfTypeASCII );
//
//		}
//}; // end class OdDbCircle_Dumper

//class OdDbMline_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//
//			OdDbMlinePtr pMline = pEnt; // OdDbMline
//		  
//			int n = pMline->numVertices();
//			for (int i = 0; i < n; i++)
//			{
//				OdMLSegmentArray p;
//				pMline->getParametersAt(i, p);
////				os << "    Segment: " << i << ", " << p.size() << " parameters:" << STD(endl);
//				// pmk  '<' : signed/unsigned mismatch
//				for (int j = 0; j < (int) p.size(); j++)
//				{
//					int k;
////					os << "      41: ";
//					// pmk  '<' : signed/unsigned mismatch
//					for (k = 0; k < (int) p[j].m_SegParams.size(); k++)
//					{
////						os << p[j].m_SegParams[k] << ", ";
//					}
////					os << STD(endl);
////					os << "      42: ";
//					// pmk  '<' : signed/unsigned mismatch
//					for (k = 0; k < (int) p[j].m_AreaFillParams.size(); k++)
//					{
////						os << p[j].m_AreaFillParams[k] << ", ";
//					}
////					os << STD(endl);
//				}
//			}
//		}
//}; 

//class OdDbRasterImage_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//
//			OdDbRasterImagePtr pImage = pEnt;  
//			OdGePoint3d origin;
//			OdGeVector3d u,v,u2,v2;
//			pImage->getOrientation (origin,u,v);
//			u.normalize();
//			v.normalize();
//			int i = 4;//break;
//		}
//}; 

//class OdDbArcAlignedText_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//			OdDbArcAlignedTextPtr pAAT = pEnt;  
//		}
//}; 

//class OdDbOle2Frame_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//			OdDbOle2FramePtr pOle = pEnt;
//		    
//			int type = pOle->getType();
//		    
//			OdString str = pOle->getUserType();
//		    
////			os << "\n\t ===== OLE2FRAME entity data =====\n";
////			os << "\n\t type = "<<type << "\n\t user type string\'"<<str.c_str()<<"\'";
////			os << "\n\t =================================\n";
//		}
//}; // end class OdDbOle2Frame_Dumper

//class OdDb3dSolid_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CPrim* Dump(OdDbEntity* pEnt) const
//		{
//			DumpCommonData(pEnt);
//
////			CPrimPolygon *pPrimPolygon = new CPrimPolygon(pEnt, 2);
//
////			CPegDoc *pDoc = CPegDoc::GetDoc();
////			CSeg *pSeg = new CSeg(pPrimMark);
////			pDoc->LayersGet(pEnt->layer().c_str())->AddTail(pSeg);
//
//
////			OdDb3dSolidPtr   pSolid;
////
////			DumpCommonData(pEnt);
////			pSolid = pEnt;
////
//////			os << "      " << " it is 3D Solid object " << STD(endl);
//		}
//};

class OdDb2dPolyline_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			return BreakPolyline(pEnt, 1);
			//CPrimPolyline* pPolyline = new CPrimPolyline(pEnt, 1);
			//return (new CSeg(pPolyline));
		}
}; // end class OdDb2dPolyline_Dumper

class OdDb3dPolyline_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			return BreakPolyline(pEnt, 2);
			//CPrimPolyline* pPolyline = new CPrimPolyline(pEnt, 2);
			//return (new CSeg(pPolyline));
		}
};

class OdDbBlockReference_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimSegRef *pPrimSegRef = new CPrimSegRef(pEnt, 0);
			return (new CSeg(pPrimSegRef));
		}
};

class OdDbMInsertBlock_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimSegRef *pPrimSegRef = new CPrimSegRef(pEnt, 1);
			return (new CSeg(pPrimSegRef));
		}
};

class OdDbSpline_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			OdDbSplinePtr pSpline = pEnt;

			

			// fit points
			if(pSpline->hasFitData())
			{
				return ((CSeg*)0);
			}
			// nurbs
			else
			{
				int degree;
				bool rational, closed, periodic;
				OdGePoint3dArray ctrlPts;
				OdGeDoubleArray weights;
				OdGeKnotVector  knots;
				double tol;
			
				pSpline->getNurbsData(degree, rational, closed, periodic, ctrlPts, knots, weights, tol);
				unsigned i;

//				CPnts p_pts;
//				int nSize = ctrlPts.size();		
				//CSeg *pSegReturn = new CSeg();

				//do
				//{
					CPrimBSpline *pPrimSpline = 0;

					CPnt p_pt(0, 0, 0);
					CPnt p_pts[512] = {p_pt};

					// TODO: add to report about missing segs
					if(ctrlPts.size() > 500)
						ctrlPts.removeSubArray(500, ctrlPts.size() - 1);

					for(i = 0; i < ctrlPts.size(); i++)
					{
						p_pt(ctrlPts[i].x, ctrlPts[i].y, ctrlPts[i].z);
	//					p_pts.Add(p_pt);
						p_pts[i] = p_pt;
					}

					//if((closed) && (ctrlPts.size() > 2))
					//{
					//	// copy the first three elements into the end
					//	p_pt(ctrlPts[0].x, ctrlPts[0].y, ctrlPts[0].z);
					//	p_pts[i++] = p_pt;
					//	p_pt(ctrlPts[1].x, ctrlPts[1].y, ctrlPts[1].z);
					//	p_pts[i++] = p_pt;
					//	p_pt(ctrlPts[2].x, ctrlPts[2].y, ctrlPts[2].z);
					//	p_pts[i++] = p_pt;
					//}

					if(ctrlPts.size() == 2)
					{
						CPrimLine *pLine = new CPrimLine(p_pts[0], p_pts[1]);
						return (new CSeg(pLine));
					}
					else if(ctrlPts.size() < 2)
						return ((CSeg*)0);

					pPrimSpline = new CPrimBSpline((WORD)ctrlPts.size(), p_pts);
					//pSegReturn->AddTail(pPrimSpline);

			/*		if(ctrlPts.size() > 60)
						ctrlPts.removeSubArray(0, 59);
				}
				while(ctrlPts.size() > 60);*/
	
				//os << "      " << "Knots: " << STD(endl);
//				for (i = 0; i < (unsigned)knots.length(); i++)
//				{
	//				os << "        " << knots[i] << STD(endl);
//				}
	
//				if (rational)
//				{
	//				os << "      " << "Weights: " << STD(endl);
//					for (i = 0; i < weights.size(); i++)
//					{
	//					os << "        " << weights[i] << STD(endl);
//					}
//				}
				return (new CSeg(pPrimSpline));
				//return pSegReturn;
				//return ((CSeg*)0);
			}
		}
};

class OdDbEllipse_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimArc *pPrimArc = new CPrimArc(pEnt, 2);
			return (new CSeg(pPrimArc));
		}
};

class OdDbPolyline_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			return BreakPolyline(pEnt, 0);
			//CPrimPolyline* pPolyline = new CPrimPolyline(pEnt, 0);
			//return (new CSeg(pPolyline));
		}
}; // end class OdDbPolyline_Dumper

class OdDbHatch_Dumper : public OdDbEntity_Dumper
{
	private:
		static void DumpPolylineType(int loopIndex, OdDbHatchPtr &pHatch)
		{
			OdGePoint2dArray vertices;
			OdGeDoubleArray bulges;
		    
			pHatch->getLoopAt (loopIndex, 
			vertices, 
			bulges);
		    
			bool hasbulges = vertices.size () == bulges.size () ? true: false ;
			OdGeDoubleArray::const_iterator blgs = bulges.begin ();
			double bulge;
		    
			for (OdGePoint2dArray::const_iterator verts = vertices.begin(); vertices.end () != verts; ++verts) {
		      
			if (hasbulges)
			{
				bulge = *blgs;
				blgs++;
			}
			else
				bulge = 0.0;
//				os << "      " << (*verts).x << "," << (*verts).y << "," <<  0.0 <<  4.0 * atan (bulge) << STD(endl);
			}
		}

		static void DumpEllipticedge(OdGeEllipArc2d* pEllipArc) 
		{		    
			if (pEllipArc == NULL)
				return;

//			os << "      Center: " <<  pEllipArc->center().x << "," << pEllipArc->center().y << STD(endl);
//			os << "      MinorRadius: " <<  pEllipArc->minorRadius() << STD(endl);
//			os << "      MajorRadius: " <<  pEllipArc->majorRadius() << STD(endl);
//			os << "      MinorAxis: " <<  pEllipArc->minorAxis().x << "," << pEllipArc->minorAxis().y << STD(endl);
//			os << "      MajorAxis: " <<  pEllipArc->majorAxis().x << "," << pEllipArc->majorAxis().y << STD(endl);
//			os << "      StartAng: " <<  pEllipArc->startAng() << STD(endl);
//			os << "      EndAng: " <<  pEllipArc->endAng()     << STD(endl);
//			os << "      Startpoint: " <<  pEllipArc->startPoint().x << "," <<  pEllipArc->startPoint().y << STD(endl);
//			os << "      Endpoint: " <<  pEllipArc->endPoint().x << "," << pEllipArc->endPoint().y << STD(endl);
//			os << "      Direction: " <<  (( pEllipArc->isClockWise() ) ? "ClockWise" : "CounterClockWise") << STD(endl);
		    
			OdGePoint2dArray    pts;
			pEllipArc->getSamplePoints (pEllipArc->startAng (), pEllipArc->endAng (), OdaPI / 30 , pts);
//			os << "      SamplePoint Data: " << STD(endl);

			for(unsigned int i = 0; i < pts.length(); i++)
			{
//				os << "POINT " << pts[i].x << "," << pts[i].y << STD(endl);
			}
		} 

		static void DumpEdgesType(int loopIndex, OdDbHatchPtr &pHatch)
		{    
			EdgeArray edges;
		    
			pHatch->getLoopAt(loopIndex, edges);

			CPnts p_pts;
		    
			for(EdgeArray::const_iterator edge = edges.begin(); edge != edges.end(); ++edge) 
			{
				OdGeCurve2d* pEdge = *edge;

				switch(pEdge->type()) 
				{
					case OdGe::kLineSeg2d:
					{
//						OdGeLineSeg2d* pLineSeg = (OdGeLineSeg2d*)pEdge;

//						OdGePoint2d OdPoint = pLineSegstartPoint();
//						p_pts.Add(CPnt(OdPoint.x, OdPoint.y, 0.));
//						break;
					}	        
//					case OdGe::kCircArc2d:
//					{
//						OdGeCircArc2d* pCircArc = (OdGeCircArc2d*)pEdge;
////						os << " CircArc2d";
//						//dumpCircArc( pCircArc , os );
//						break;
//					}				        
//					case OdGe::kEllipArc2d:
//					{
//						OdGeEllipArc2d* pEllipArc = (OdGeEllipArc2d*)pEdge;
////						os << " EllipseArc2d";
//						DumpEllipticedge(pEllipArc);
//						break;
//					}   
//					case OdGe::kNurbCurve2d:
//					{
//						OdGeNurbCurve2d* pNurbCurve = (OdGeNurbCurve2d*)pEdge;
////						os << " NurbCurve2d";
//						//dumpNurbCurve2d( pNurbCurve , os );
//						break;
//					}					
				}
			}
		}

	public:
		  
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
//			OdDbHatchPtr pHatch = pEnt;
//			
//			if(!pHatch->numLoops())
//				return (CSeg*)0;
//
//			for(int i = 0; i < 1/*pHatch->numLoops()*/; i++)
//			{
//				OdInt32 loopType = pHatch->loopTypeAt(i);
//
////				if( loopType & OdDbHatch::kExternal )
////					os << " External";
////				if( loopType & OdDbHatch::kDerived )
////					os << " Derived";
////				if( loopType & OdDbHatch::kTextbox )
/////					os << " Textbox";
////				if( loopType & OdDbHatch::kOutermost )
////					os << " Outermost";
////				if( loopType & OdDbHatch::kNotClosed )
////					os << " NotClosed";
////				if( loopType & OdDbHatch::kSelfIntersecting )
////					os << " SelfIntersecting";
////				if( loopType & OdDbHatch::kTextIsland )
////					os << " TextIsland";
//				
//				if(loopType & OdDbHatch::kPolyline)
//				{
////					DumpPolylineType(i, pHatch);
//				}
//				else
//				{
//					DumpEdgesType(i, pHatch);
//				}
//			}

			return ((CSeg*)0);

//			CPrimPolygon *pPolygon = new CPrimPolygon(pEnt, 3);
//			return (new CSeg(pPolygon));
		}
};

class OdDbCircle_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimArc *pPrimArc = new CPrimArc(pEnt, 1);
			return (new CSeg(pPrimArc));
		}
}; // end class OdDbCircle_Dumper

class OdDbMText_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			OdDbMTextPtr pMText = pEnt;

			CSeg *pSeg = new CSeg;
			pMText->explodeFragments(ExplodeTextFragments, pSeg);
			return pSeg;
		}
}; // end class OdDbMtext_Dumper

class OdDbText_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimText *pPrimText = new CPrimText(pEnt, 0);
			return (new CSeg(pPrimText));
		}
}; // end class OdDbMtext_Dumper

class OdDbArcAlignedText_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* ) const //pEnt) const
		{
//			DumpCommonData(pEnt);
//			CPrimText *pPrimText = new CPrimText(pEnt, 1);
//			return (new CSeg(pPrimText));
			return (CSeg*)0;
		}
};

class OdDbLine_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimLine *pPrimLine = new CPrimLine(pEnt);
			return (new CSeg(pPrimLine));
		}
}; 

class OdDbArc_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimArc *pPrimArc = new CPrimArc(pEnt, 0);
			return (new CSeg(pPrimArc));
		}
}; 

class OdDbPoint_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimMark *pPrimMark = new CPrimMark(pEnt);
			return (new CSeg(pPrimMark));
		}
}; 

class OdDbFace_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimPolygon *pPrimPolygon = new CPrimPolygon(pEnt, 0);
			return (new CSeg(pPrimPolygon));
		}
}; 

class OdDbSolid_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			CPrimPolygon *pPrimPolygon = new CPrimPolygon(pEnt, 1);
			return (new CSeg(pPrimPolygon));
		}
};

class OdDbLeader_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);

			OdDbLeaderPtr pLeader = pEnt;

			CSeg *pSeg = new CSeg;

			CPnts p_pts;
			CPnt p_pt;

			int nVert = pLeader->numVertices();
			for(int i = 0; i < nVert; i++)
			{
				OdGePoint3d OdPoint = pLeader->vertexAt(i);
				p_pt(OdPoint.x, OdPoint.y, OdPoint.z);
				p_pts.Add(p_pt);
			}

			for(int j = 0; j < p_pts.GetCount() - 1; j++)
				pSeg->AddTail(new CPrimLine(p_pts.GetAt(j), p_pts.GetAt(j+1)));

			if(pLeader->hasArrowHead())// || pLeader->hasHookLine())
			{
				CSeg *pArrowSeg = new CSeg;
				double dRatio = viewport::GetWidthInInches() / view::GetUExt();
				double dNewRatio = ((.1) * (dRatio)) / (.188);
				annotate::GenArrowHead(1, dNewRatio, p_pts.GetAt(1), p_pts.GetAt(0), pArrowSeg);
				pSeg->AddTail(pArrowSeg);
			}

			return pSeg;
		}
};

//class OdDbAlignedDimension_Dumper : public OdDbEntity_Dumper
//{
//	public:
//		CSeg* Dump(OdDbEntity* pEnt) const
//		{
////			DumpCommonData(pEnt);
////			CPrimPolygon *pPrimPolygon = new CPrimPolygon(pEnt, 1);
////			return (new CSeg(pPrimPolygon));
//
//			return (CSeg*)0;
//		}
//};

class OdDbAttribute_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);

//			CPrimPolygon *pPrimPolygon = new CPrimPolygon(pEnt, 1);
//			return (new CSeg(pPrimPolygon));
			return (CSeg*)0;
			
			//OdDbTextPtr pText = pEnt;
			//CPrimText *pPrimText = new CPrimText(pText, 0);
			//return (new CSeg(pPrimText));
		}
};

class OdDbViewport_Dumper : public OdDbEntity_Dumper
{
	public:
		CSeg* Dump(OdDbEntity* pEnt) const
		{
			DumpCommonData(pEnt);
			OdDbViewportPtr pVPort = pEnt;

			CSeg *pSeg = new CSeg;

			// text
			///////////////////////////////////////////////////////////////////////////////////////
			//CFontDef fd;
			//CCharCellDef ccd;
			//CRefSys rs(CPnt(pVPort->centerPoint().x, pVPort->centerPoint().y, pVPort->centerPoint().z), ccd);
			//CPrimText *pText = new CPrimText(fd, rs, "Viewport");

			//pSeg->AddTail(pText);
			///////////////////////////////////////////////////////////////////////////////////////

			CPnt center(pVPort->centerPoint().x, pVPort->centerPoint().y, pVPort->centerPoint().z);
			double height = pVPort->height();
			double width = pVPort->width();
			CPnt TL(center[0] - (width/2), center[1] - (height/2), center[2]);
			CPnt TR(center[0] + (width/2), center[1] - (height/2), center[2]);
			CPnt BL(center[0] - (width/2), center[1] + (height/2), center[2]);
			CPnt BR(center[0] + (width/2), center[1] + (height/2), center[2]);

			pSeg->AddTail(new CPrimLine(TL, TR));
			pSeg->AddTail(new CPrimLine(TR, BR));
			pSeg->AddTail(new CPrimLine(BR, BL));
			pSeg->AddTail(new CPrimLine(BL, TL));

			if(istate.GetViewBoxes() || !istate.m_bIsImport)
				return (pSeg);
			else
				return ((CSeg*)0);
		}
};

class Dumpers
{
	private:
		OdStaticRxObject<OdDbEntity_Dumper>				m_entityDumper;
////		OdStaticRxObject<OdDbRegion_Dumper>				m_regionDumper;
		OdStaticRxObject<OdDbPolyFaceMesh_Dumper>		m_polyFaceMeshDumper;
		OdStaticRxObject<OdDbPolygonMesh_Dumper>		m_polygonMesh;
////		OdStaticRxObject<OdDbTrace_Dumper>          m_traceDumper;
////		OdStaticRxObject<OdDb3dSolid_Dumper>        m_3DSolidDumper;
////		OdStaticRxObject<OdDbProxyEntity_Dumper>    m_proxyEntityDumper;
////		OdStaticRxObject<OdDbMline_Dumper>          m_mlineDumper;
////		OdStaticRxObject<OdDbRasterImage_Dumper>    m_imageDumper;
////		OdStaticRxObject<OdDbArcAlignedText_Dumper> m_arcAlignedTextDumper;
////		OdStaticRxObject<OdDbOle2Frame_Dumper>      m_ole2FrameDumper;
		OdStaticRxObject<OdDbPolyline_Dumper>			m_polylineDumper;
		OdStaticRxObject<OdDb2dPolyline_Dumper>			m_2dPolylineDumper;
		OdStaticRxObject<OdDb3dPolyline_Dumper>			m_3dPolylineDumper;
		OdStaticRxObject<OdDbBlockReference_Dumper>		m_blockReference;
		OdStaticRxObject<OdDbMInsertBlock_Dumper>		m_mInsertBlock;
		OdStaticRxObject<OdDbSpline_Dumper>				m_splineDumper;
		OdStaticRxObject<OdDbEllipse_Dumper>			m_ellipseDumper;
		OdStaticRxObject<OdDbSolid_Dumper>				m_solidDumper;
////		OdStaticRxObject<OdDbHatch_Dumper>				m_hatchDumper;
		OdStaticRxObject<OdDbCircle_Dumper>				m_circleDumper;
		OdStaticRxObject<OdDbMText_Dumper>				m_mTextDumper;
		OdStaticRxObject<OdDbText_Dumper>				m_textDumper;
////		OdStaticRxObject<OdDbArcAlignedText_Dumper>		m_arcAlignedTextDumper;
		OdStaticRxObject<OdDbLine_Dumper>				m_lineDumper;
		OdStaticRxObject<OdDbArc_Dumper>				m_arcDumper;
		OdStaticRxObject<OdDbPoint_Dumper>				m_pointDumper;
		OdStaticRxObject<OdDbFace_Dumper>				m_faceDumper;
		OdStaticRxObject<OdDbLeader_Dumper>				m_leaderDumper;
////		OdStaticRxObject<OdDbAlignedDimension_Dumper>	m_alignedDimDumper;
////		OdStaticRxObject<OdDbAttribute_Dumper>			m_attributeDumper;
		OdStaticRxObject<OdDbViewport_Dumper>				m_viewportDumper;

	public:
		void addXs()
		{
			OdDbEntity::desc()->addX(OdDbEntity_Dumper::desc(), &m_entityDumper);
////			OdDbRegion::desc()->addX(OdDbEntity_Dumper::desc(), &m_regionDumper);
			OdDbPolyFaceMesh::desc()->addX(OdDbEntity_Dumper::desc(), &m_polyFaceMeshDumper);
			OdDbPolygonMesh::desc()->addX(OdDbEntity_Dumper::desc(), &m_polygonMesh);
////			OdDbTrace::desc()->addX(OdDbEntity_Dumper::desc(), &m_traceDumper);
////			OdDb3dSolid::desc()->addX(OdDbEntity_Dumper::desc(), &m_3DSolidDumper);
////			OdDbProxyEntity::desc()->addX(OdDbEntity_Dumper::desc(), &m_proxyEntityDumper);
////			OdDbMline::desc()->addX(OdDbEntity_Dumper::desc(), &m_mlineDumper);
////			OdDbRasterImage::desc()->addX(OdDbEntity_Dumper::desc(), &m_imageDumper);
////			OdDbArcAlignedText::desc()->addX(OdDbEntity_Dumper::desc(), &m_arcAlignedTextDumper);
////			OdDbOle2Frame::desc()->addX(OdDbEntity_Dumper::desc(), &m_ole2FrameDumper);
			OdDbPolyline::desc()->addX(OdDbEntity_Dumper::desc(), &m_polylineDumper);
			OdDb2dPolyline::desc()->addX(OdDbEntity_Dumper::desc(), &m_2dPolylineDumper);
			OdDb3dPolyline::desc()->addX(OdDbEntity_Dumper::desc(), &m_3dPolylineDumper);
			OdDbBlockReference::desc()->addX(OdDbEntity_Dumper::desc(), &m_blockReference);
			OdDbMInsertBlock::desc()->addX(OdDbEntity_Dumper::desc(), &m_mInsertBlock);
			OdDbSpline::desc()->addX(OdDbEntity_Dumper::desc(), &m_splineDumper);
			OdDbEllipse::desc()->addX(OdDbEntity_Dumper::desc(), &m_ellipseDumper);
			OdDbSolid::desc()->addX(OdDbEntity_Dumper::desc(), &m_solidDumper);
////			OdDbHatch::desc()->addX(OdDbEntity_Dumper::desc(), &m_hatchDumper);
			OdDbCircle::desc()->addX(OdDbEntity_Dumper::desc(), &m_circleDumper);
			OdDbMText::desc()->addX(OdDbEntity_Dumper::desc(), &m_mTextDumper);
			OdDbText::desc()->addX(OdDbEntity_Dumper::desc(), &m_textDumper);
////			OdDbArcAlignedText::desc()->addX(OdDbEntity_Dumper::desc(), &m_arcAlignedTextDumper);
			OdDbLine::desc()->addX(OdDbEntity_Dumper::desc(), &m_lineDumper);
			OdDbArc::desc()->addX(OdDbEntity_Dumper::desc(), &m_arcDumper);
			OdDbPoint::desc()->addX(OdDbEntity_Dumper::desc(), &m_pointDumper);
			OdDbFace::desc()->addX(OdDbEntity_Dumper::desc(), &m_faceDumper);
			OdDbLeader::desc()->addX(OdDbEntity_Dumper::desc(), &m_leaderDumper);
////			OdDbAlignedDimension::desc()->addX(OdDbEntity_Dumper::desc(), &m_alignedDimDumper);
////			OdDbAttribute::desc()->addX(OdDbEntity_Dumper::desc(), &m_attributeDumper);
			OdDbViewport::desc()->addX(OdDbEntity_Dumper::desc(), &m_viewportDumper);
		} // end addXs

		void delXs()
		{
			OdDbEntity::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbRegion::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbPolyFaceMesh::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbPolygonMesh::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbTrace::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDb3dSolid::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbProxyEntity::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbMline::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbRasterImage::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbArcAlignedText::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbOle2Frame::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbPolyline::desc()->delX(OdDbEntity_Dumper::desc());
			OdDb2dPolyline::desc()->delX(OdDbEntity_Dumper::desc());
			OdDb3dPolyline::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbBlockReference::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbMInsertBlock::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbSpline::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbEllipse::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbSolid::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbHatch::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbCircle::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbMText::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbText::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbArcAlignedText::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbLine::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbArc::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbPoint::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbFace::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbLeader::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbAlignedDimension::desc()->delX(OdDbEntity_Dumper::desc());
////			OdDbAttribute::desc()->delX(OdDbEntity_Dumper::desc());
			OdDbViewport::desc()->delX(OdDbEntity_Dumper::desc());
		}
};

void ExProtocolExtension::initialize()
{
	// Register OdDbEntity_Dumper with DWGdirect
	OdDbEntity_Dumper::rxInit();
	m_pDumpers = new Dumpers;
	m_pDumpers->addXs();
}//  end ExProtocolExtension::initialize()

void ExProtocolExtension::uninitialize()
{
	m_pDumpers->delXs();
	OdDbEntity_Dumper::rxUninit();
	delete m_pDumpers;
	m_pDumpers = 0;
}