#pragma once

#include "RxObject.h"
#include "DbEntity.h"

//#include "ExGsSimpleDevice.h"

#include "GiGeometrySimplifier.h"
#include "GiBaseVectorizer.h"

class OdDbEntity_Dumper : public OdRxObject
{
	public:
		ODRX_DECLARE_MEMBERS(OdDbEntity_Dumper);

		virtual CSeg* Dump(OdDbEntity* pEnt) const;
}; // end class OdDbEntity_Dumper

class Dumpers;
  
class ExProtocolExtension  
{
	private:
		Dumpers* m_pDumpers;

	public:
		ExProtocolExtension();
		virtual ~ExProtocolExtension();
		void initialize();
		void uninitialize();
};

class CVectorizer : public OdGiGeometrySimplifier, public OdGiBaseVectorizer
{
	//void polygonOut(OdInt32 nbPoints, const OdGePoint3d *pVertexList, const OdGeVector3d *pNormal = 0)
	//{
	//	MessageBox(0, "polygon", 0, 0);	
	//}

	//void polylineOut(OdInt32 nbPoints, const OdGePoint3d *pVertexList)
	//{
	//	MessageBox(0, "polyline", 0, 0);
	//}

	//void circleProc(const OdGePoint3d &start, const OdGePoint3d &point, const OdGePoint3d &end, const OdGeVector3d *pExtrusion = 0)
	//{
	//	MessageBox(0, "circle", 0, 0);
	//}

	//void circle(const OdGePoint3d &, const OdGePoint3d &, const OdGePoint3d &)
	//{
	//	MessageBox(0, "circle", 0, 0);
	//}

	//void beginViewVectorization()
	//{
	//	MessageBox(0, "view", 0, 0);
	//}

	//void generateShellFaces(OdInt32 a, const OdInt32 *b, const OdGiEdgeData *c = 0, const OdGiFaceData *d = 0)
	//{
	//	MessageBox(0, "view2", 0, 0);
	//}

	//void polylineDc(OdUInt32 nVertices, const OdGePoint3d *vertexList)
	//{
	//	MessageBox(0, "polyline1", 0, 0);
	//}
	////void polylineDc(OdUInt32 nbPoints, const OdGePoint3d *pPoints)
	////{
	////	MessageBox(0, "polyline2", 0, 0);
	////}
	//void polygonDc(OdUInt32 nVertices, const OdGePoint3d *vertexList)
	//{
	//	MessageBox(0, "polygon1", 0, 0);
	//}
	////void polygonDc(OdUInt32 nbPoints, const OdGePoint3d *pPoints)
	////{
	////	MessageBox(0, "polygon2", 0, 0);
	////}

	void addRef() {}
	void release() {}
};