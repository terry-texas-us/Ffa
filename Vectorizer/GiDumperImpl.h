#pragma once

#include "RxObjectImpl.h"
#include "GiConveyorGeometryDumper.h"

#define STL_USING_IOSTREAM
#include "OdaSTL.h"
#include "../PegAeSys/TCharDefs.h"

class OdGiDumperImpl : public OdGiDumper
{
  int   m_indentLevel;
  std::_tostream* m_target;
public:
  static OdGiDumperPtr createObject(std::_tostream* target);

  OdGiDumperImpl(); // it isn't possible to create object using this constructor, use 
                    // above createObject method for such purpose
  virtual ~OdGiDumperImpl();

  virtual void output(const OdString& label, const OdGeMatrix3d* xfm);
  virtual void output(const OdString& str);
  virtual void output(const OdString& str1, const OdString& str2);
  virtual void output(OdInt32 nPoints, const OdGePoint3d* points);
  virtual void output(const OdGiTextStyle& textStyle);
  virtual void outputEdgeData(const OdGiEdgeData* pEdgeData, OdInt32 numEdges);
  virtual void outputFaceData(const OdGiFaceData* pFaceData, OdInt32 numFaces);
  virtual void outputVertexData(const OdGiVertexData* pVertexData, OdInt32 numVerts);
  virtual void outputColors(const OdUInt16* colors, OdInt32 numColors, const OdString& name);
  virtual void outputTrueColors(const OdCmEntityColor* trueColors, OdInt32 numColors, const OdString& name);
  virtual void outputIds(OdDbStub** ids, OdInt32 numIds, const OdString& name, const OdString& table);
  virtual void outputSelectionMarkers(const OdGsMarker* selectionMarkers, OdInt32 numMarkers, const OdString& name);
  virtual void outputVisibility(const OdUInt8* visibility, OdInt32 count, const OdString& name);
  virtual void outputMappers(const OdGiMapper* mappers, OdInt32 count, const OdString& name);

  virtual void pushIndent() { m_indentLevel += 2; }
  virtual void popIndent() { m_indentLevel -= 2; }
  virtual int  currIndent() {return m_indentLevel;}
};