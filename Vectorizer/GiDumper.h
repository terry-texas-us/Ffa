
#ifndef __OD_GI_DUMPER__
#define __OD_GI_DUMPER__

class OdGiDumper : public OdRxObject
{
public:
  virtual void output(const OdString& label, const OdGeMatrix3d* xfm) = 0;
  virtual void output(const OdString& str) = 0;
  virtual void output(const OdString& str1, const OdString& str2) = 0;
  virtual void output(OdInt32 nPoints, const OdGePoint3d* points) = 0;
  virtual void output(const OdGiTextStyle& textStyle) = 0;
  virtual void outputEdgeData(const OdGiEdgeData* pEdgeData, OdInt32 count) = 0;
  virtual void outputFaceData(const OdGiFaceData* pFaceData, OdInt32 numFaces) = 0;
  virtual void outputVertexData(const OdGiVertexData* pVertexData, OdInt32 count) = 0;
  virtual void outputColors(const OdUInt16* colors, OdInt32 count, const OdString& name) = 0;
  virtual void outputTrueColors(const OdCmEntityColor* trueColors, OdInt32 count, const OdString& name) = 0;
  virtual void outputIds(OdDbStub** ids, OdInt32 count, const OdString& name, const OdString& table) = 0;
  virtual void outputSelectionMarkers(const OdInt32* selectionMarkers, OdInt32 count, const OdString& name) = 0;
  virtual void outputVisibility(const OdUInt8* visibility, OdInt32 count, const OdString& name) = 0;

  virtual void pushIndent() = 0;
  virtual void popIndent() = 0;
};

typedef OdSmartPtr<OdGiDumper> OdGiDumperPtr;

#endif // __OD_GI_DUMPER__
