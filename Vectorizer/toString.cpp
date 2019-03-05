#define STL_USING_IOSTREAM
#include "OdaSTL.h"
#define  STD(a)  std:: a

#include "toString.h"


/************************************************************************/
/* Shorten a path with ellipses.                                        */
/************************************************************************/
OdString shortenPath(OdString path, int maxPath)
{
  /**********************************************************************/
  /* If the path fits, just return it                                   */
  /**********************************************************************/
  if (path.getLength()<= maxPath)
  {
    return path;
  }
  /**********************************************************************/
  /* If there's no backslash, just truncate the path                    */
  /**********************************************************************/
  int lastBackslash = path.reverseFind('\\');
  if (lastBackslash < 0)
  {
      return path.left(maxPath - 3) + DD_T("...");
  }    

  /**********************************************************************/
  /* Shorten the front of the path                                      */
  /**********************************************************************/
  int fromLeft = (lastBackslash - 3) - (path.getLength() - maxPath);
  // (12 - 3) - (19 - 10) = 9 - 9 = 0 
  if ((lastBackslash <= 3)|| (fromLeft < 1))  
  {
    path = DD_T("...") + path.mid(lastBackslash);
  }
  else
  {
    path = path.left(fromLeft) + DD_T("...") + path.mid(lastBackslash);
  }
  
  /**********************************************************************/
  /* Truncate the path                                                  */
  /**********************************************************************/
  if (path.getLength() > maxPath)
  {
    path = path.left(maxPath-3) + DD_T("...");
  }
    
  return path;
}


/************************************************************************/
/* Convert the specified value to a degree string                       */
/************************************************************************/
OdString toDegreeString(const double val)
{ 
  return toString(val * 180.0 / OdaPI) + DD_T("d");
}

/************************************************************************/
/* Convert the specified value to a hex string                          */
/************************************************************************/
OdString toHexString(const int val)
{ 
  OdString buffer;
  buffer.format(DD_T("%#x"), val); 
  return buffer;
}

/************************************************************************/
/* Convert the specified value to an ArcSymbolType string               */
/************************************************************************/
OdString toArcSymbolTypeString(const int val)
{
  OdString retVal (DD_T("???"));
  switch(val)
  {
    case 0: retVal = DD_T("Precedes text"); break;
    case 1: retVal = DD_T("Above text");    break;
    case 2: retVal = DD_T("None");          break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to a LoopType string                     */
/************************************************************************/
OdString toLooptypeString(const int loopType)
{
  OdString retVal;
  if(loopType & OdDbHatch::kExternal)
    retVal += DD_T(" | kExternal");

  if(loopType & OdDbHatch::kPolyline)
    retVal += DD_T(" | kPolyline");

  if(loopType & OdDbHatch::kDerived)
    retVal += DD_T(" | kDerived");
    
  if(loopType & OdDbHatch::kTextbox)
    retVal += DD_T(" | kTextbox");
  
  if(loopType & OdDbHatch::kOutermost)
    retVal += DD_T(" | kOutermost");
  
  if(loopType & OdDbHatch::kNotClosed)
    retVal += DD_T(" | kNotClosed");
  
  if(loopType & OdDbHatch::kSelfIntersecting)
    retVal += DD_T(" | kSelfIntersecting");
  
  if(loopType & OdDbHatch::kTextIsland)
    retVal += DD_T(" | kTextIsland");
  
  if(loopType & OdDbHatch::kDuplicate)
    retVal += DD_T(" | kDuplicate");

  return (retVal.isEmpty() ? OdString(DD_T("kDefault")) : retVal.mid(3));
}

/************************************************************************/
/* Convert the specified value to an OdGiVisibility string              */
/************************************************************************/
//OdString toString(const OdAveGraniteMaterial::GraniteColor val)
//{ 
//  OdString retVal(DD_T("???"));
//  switch (val) 
//  {
//    case OdAveGraniteMaterial::kFirst     : retVal = DD_T("kFirst");     break; 
//    case OdAveGraniteMaterial::kSecond    : retVal = DD_T("kSecond");    break; 
//    case OdAveGraniteMaterial::kThird     : retVal = DD_T("kThird");     break; 
//    case OdAveGraniteMaterial::kFourth    : retVal = DD_T("kFourth");    break; 
//  }
//  return retVal;
//}
/************************************************************************/
/* Convert the specified value to an OdGiVisibility string              */
/************************************************************************/
OdString toOdGiVisibilityString(const OdUInt8 val)
{ 
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case kOdGiInvisible  : retVal = DD_T("kOdGiInvisible");   break; 
    case kOdGiVisible    : retVal = DD_T("kOdGiVisible");    break; 
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdGiFillType string                */
/************************************************************************/
OdString toString(const OdGiFillType val)
{ 
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case kOdGiFillAlways   : retVal = DD_T("kOdGiFillAlways");   break; 
    case kOdGiFillNever    : retVal = DD_T("kOdGiFillNever");    break; 
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to a bool string                         */
/************************************************************************/
OdString toString(const bool val)
{ 
  return (val  ? DD_T("true") : DD_T("false")); 
}

/************************************************************************/
/* Convert the specified value to a fixed-point double string with      */
/* trailing zeros suppressed                                            */
/************************************************************************/
OdString toString(const double val)
{ 
  OdString temp;
  if (fabs(val) < 1e6)
  {
    temp.format(DD_T("%.4f"), val);

    /***********************************************************************/
    /* Strip all but one trailing zero from buffer                         */
    /***********************************************************************/
    for (int n = temp.getLength(); ((temp[n-2] != '.') && (temp[n-1] == '0')); n--)
    {
      temp = temp.left(temp.getLength() - 1);
    }
  }
  else
  {
    temp.format(DD_T("%.4E"), val);
  }
  return temp;
}

/************************************************************************/
/* Convert the specified value to formatted double string               */
/************************************************************************/
OdString toString(const OdString& fmt, double val)
{ 
  OdString buffer;
  buffer.format(fmt, val); 
  return buffer;
}

/************************************************************************/
/* Convert the specified value to an int string                         */
/************************************************************************/
OdString toString(const int val)
{ 
  OdString buffer;
  buffer.format(DD_T("%d"), val); 
  return buffer;
}

/************************************************************************/
/* Convert the specified value to a formatted int string                */
/************************************************************************/
OdString toString(const OdString& fmt, int val)
{ 
  OdString buffer;
  buffer.format(fmt, val); 
  return buffer;
}

/************************************************************************/
/* Convert the specified value to an OdArcTextAlignment string          */
/************************************************************************/
OdString toString(const OdArcTextAlignment val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case kFit    : retVal = DD_T("kFit");    break; 
    case kLeft   : retVal = DD_T("kLeft");   break; 
    case kRight  : retVal = DD_T("kRight");  break; 
    case kCenter : retVal = DD_T("kCenter"); break; 
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbMText::AttachmentPoint string  */
/************************************************************************/
OdString toString(const OdDbMText::AttachmentPoint val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDbMText::kTopLeft      : retVal = DD_T("kTopLeft");      break;
    case OdDbMText::kTopCenter    : retVal = DD_T("kTopCenter");    break;
    case OdDbMText::kTopRight     : retVal = DD_T("kTopRight");     break;
    case OdDbMText::kMiddleLeft   : retVal = DD_T("kMiddleLeft");   break;
    case OdDbMText::kMiddleCenter : retVal = DD_T("kMiddleCenter"); break;
    case OdDbMText::kMiddleRight  : retVal = DD_T("kMiddleRight");  break;
    case OdDbMText::kBottomLeft   : retVal = DD_T("kBottomLeft");   break;
    case OdDbMText::kBottomCenter : retVal = DD_T("kBottomCenter"); break;
    case OdDbMText::kBottomRight  : retVal = DD_T("kBottomRight");  break;
    case OdDbMText::kBaseLeft     : retVal = DD_T("kBaseLeft");     break;
    case OdDbMText::kBaseCenter   : retVal = DD_T("kBaseCenter");   break;
    case OdDbMText::kBaseRight    : retVal = DD_T("kBaseRight");    break;
    case OdDbMText::kBaseAlign    : retVal = DD_T("kBaseAlign");    break;
    case OdDbMText::kBottomAlign  : retVal = DD_T("kBottomAlign");  break;
    case OdDbMText::kMiddleAlign  : retVal = DD_T("kMiddleAlign");  break;
    case OdDbMText::kTopAlign     : retVal = DD_T("kTopAlign");     break;
    case OdDbMText::kBaseFit      : retVal = DD_T("kBaseFit");      break;
    case OdDbMText::kBottomFit    : retVal = DD_T("kBottomFit");    break;
    case OdDbMText::kMiddleFit    : retVal = DD_T("kMiddleFit");    break;
    case OdDbMText::kTopFit       : retVal = DD_T("kTopFit");       break;
    case OdDbMText::kBaseMid      : retVal = DD_T("kBaseMid");      break;
    case OdDbMText::kBottomMid    : retVal = DD_T("kBottomMid");    break;
    case OdDbMText::kMiddleMid    : retVal = DD_T("kMiddleMid");    break;
    case OdDbMText::kTopMid       : retVal = DD_T("kTopMid");       break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbMText::FlowDirection string    */
/************************************************************************/
OdString toString(const OdDbMText::FlowDirection val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDbMText::kLtoR     : retVal = DD_T("kLtoR");     break;
    case OdDbMText::kRtoL     : retVal = DD_T("kRtoL");     break;
    case OdDbMText::kTtoB     : retVal = DD_T("kTtoB");     break;
    case OdDbMText::kBtoT     : retVal = DD_T("kBtoT");     break;
    case OdDbMText::kByStyle  : retVal = DD_T("kByStyle");  break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::Vertex2dType string          */
/************************************************************************/
OdString toString(const OdDb::Vertex2dType val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::k2dVertex          : retVal = DD_T("k2dVertex");          break;
    case OdDb::k2dSplineCtlVertex : retVal = DD_T("k2dSplineCtlVertex"); break;
    case OdDb::k2dSplineFitVertex : retVal = DD_T("k2dSplineFitVertex"); break;
    case OdDb::k2dCurveFitVertex  : retVal = DD_T("k2dCurveFitVertex");  break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::Vertex3dType string          */
/************************************************************************/
OdString toString(const OdDb::Vertex3dType val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::k3dSimpleVertex  : retVal = DD_T("k3dSimpleVertex");  break;
    case OdDb::k3dControlVertex : retVal = DD_T("k3dControlVertex"); break;
    case OdDb::k3dFitVertex     : retVal = DD_T("k3dFitVertex");     break;  
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbPolyline::SegType string       */
/************************************************************************/
OdString toString(const OdDbPolyline::SegType val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDbPolyline::kArc        : retVal = DD_T("kArc");        break;
    case OdDbPolyline::kCoincident : retVal = DD_T("kCoincident"); break;
    case OdDbPolyline::kEmpty      : retVal = DD_T("kEmpty");      break;  
    case OdDbPolyline::kLine       : retVal = DD_T("kLine");       break;  
    case OdDbPolyline::kPoint      : retVal = DD_T("kPoint");      break;  
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdGiPolyline::SegType string       */
/************************************************************************/
OdString toString(const OdGiPolyline::SegType val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdGiPolyline::kArc        : retVal = DD_T("kArc");        break;
    case OdGiPolyline::kCoincident : retVal = DD_T("kCoincident"); break;
    case OdGiPolyline::kEmpty      : retVal = DD_T("kEmpty");      break;  
    case OdGiPolyline::kLine       : retVal = DD_T("kLine");       break;  
    case OdGiPolyline::kPoint      : retVal = DD_T("kPoint");      break;  
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdCmColor string                   */
/*                                                                      */
/* Note: isByACI() returns true for ACI values of 0 (ByBlock),          */
/* 7 (ByForeground), 256 (ByLayer), and 257 (None).                     */
/************************************************************************/
OdString toString(const OdCmColor& val) 
{ 
  OdString retVal(DD_T("???"));
  if (val.isByLayer())
  {
    retVal =  DD_T("ByLayer");
  }
  else if (val.isByBlock())
  {
    retVal = DD_T("ByBlock");
  }
  else if (val.isForeground())
  {
   retVal = DD_T("Foreground");
  }
  else if (val.isNone())
  {
    retVal = DD_T("None");
  }
  else if (val.isByACI())
  {
    retVal = toString(DD_T("ACI %d"), val.colorIndex());
  }
  else if (val.isByColor())
  {
    retVal = OdString(DD_T("ByColor")) + DD_T(" r") + toString(val.red()) + DD_T(":g") 
        + toString(val.green()) + DD_T(":b") + toString(val.blue());
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified 24 bit color value to an RGB string            */
/************************************************************************/
OdString toRGBString(const OdUInt32& val) 
{ 
  OdString retVal = DD_T("r") + toString((int)ODGETRED(val)) + DD_T(":g") 
    + toString((int)ODGETGREEN(val)) + DD_T(":b") + toString((int)ODGETBLUE(val));
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdCmEntityColor string             */
/*                                                                      */
/* Note: isByACI() returns true for ACI values of 0 (ByBlock),          */
/* 7 (ByForeground), 256 (ByLayer), and 257 (None).                     */
/************************************************************************/
OdString toString(const OdCmEntityColor& val) 
{ 
  OdString retVal(DD_T("???"));
  if (val.isByLayer())
  {
    retVal =  DD_T("ByLayer");
  }
  else if (val.isByBlock())
  {
    retVal = DD_T("ByBlock");
  }
  else if (val.isForeground())
  {
   retVal = DD_T("Foreground");
  }
  else if (val.isNone())
  {
    retVal = DD_T("None");
  }
  else if (val.isByACI())
  {
    retVal = toString(DD_T("ACI %d"), val.colorIndex());
  }
  else if (val.isByColor())
  {
    retVal = OdString(DD_T("ByColor")) + DD_T(" r") + toString(val.red()) + DD_T(":g") 
        + toString(val.green()) + DD_T(":b") + toString(val.blue());
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::DwgVersion string            */
/************************************************************************/
OdString toString(const OdDb::DwgVersion val) 
{ 
  return OdString(OdDb::DwgVersionToStr(val));
}


/************************************************************************/
/* Convert the specified value to an OdDb::EndCaps string               */
/************************************************************************/
OdString toString(const OdDb::EndCaps val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kEndCapNone   : retVal = DD_T("kEndCapNone");   break;
    case OdDb::kEndCapRound  : retVal = DD_T("kEndCapRound");  break;
    case OdDb::kEndCapAngle  : retVal = DD_T("kEndCapAngle");  break;
    case OdDb::kEndCapSquare : retVal = DD_T("kEndCapSquare"); break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::JoinStyle string             */
/************************************************************************/
OdString toString(const OdDb::JoinStyle val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kJnStylNone  : retVal = DD_T("kJnStylNone");  break;
    case OdDb::kJnStylRound : retVal = DD_T("kJnStylRound"); break;
    case OdDb::kJnStylAngle : retVal = DD_T("kJnStylAngle"); break;  
    case OdDb::kJnStylFlat  : retVal = DD_T("kJnStylFlat");  break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdDb::LineWeight string            */
/************************************************************************/
OdString toString(const OdDb::LineWeight val)
{ 
  OdString retVal(DD_T("???"));
  switch (val)
  {
    case OdDb::kLnWt000          : retVal = DD_T("kLnWt000");         break;
    case OdDb::kLnWt005          : retVal = DD_T("kLnWt005");         break;
    case OdDb::kLnWt009          : retVal = DD_T("kLnWt009");         break;
    case OdDb::kLnWt013          : retVal = DD_T("kLnWt013");         break;
    case OdDb::kLnWt015          : retVal = DD_T("kLnWt015");         break;
    case OdDb::kLnWt018          : retVal = DD_T("kLnWt018");         break;
    case OdDb::kLnWt020          : retVal = DD_T("kLnWt020");         break;
    case OdDb::kLnWt025          : retVal = DD_T("kLnWt025");         break;
    case OdDb::kLnWt030          : retVal = DD_T("kLnWt030");         break;
    case OdDb::kLnWt035          : retVal = DD_T("kLnWt035");         break;
    case OdDb::kLnWt040          : retVal = DD_T("kLnWt040");         break;
    case OdDb::kLnWt050          : retVal = DD_T("kLnWt050");         break;
    case OdDb::kLnWt053          : retVal = DD_T("kLnWt053");         break;
    case OdDb::kLnWt060          : retVal = DD_T("kLnWt060");         break;
    case OdDb::kLnWt070          : retVal = DD_T("kLnWt070");         break;
    case OdDb::kLnWt080          : retVal = DD_T("kLnWt080");         break;
    case OdDb::kLnWt090          : retVal = DD_T("kLnWt090");         break;
    case OdDb::kLnWt100          : retVal = DD_T("kLnWt100");         break;
    case OdDb::kLnWt106          : retVal = DD_T("kLnWt106");         break;
    case OdDb::kLnWt120          : retVal = DD_T("kLnWt120");         break;
    case OdDb::kLnWt140          : retVal = DD_T("kLnWt140");         break;
    case OdDb::kLnWt158          : retVal = DD_T("kLnWt158");         break;
    case OdDb::kLnWt200          : retVal = DD_T("kLnWt200");         break;
    case OdDb::kLnWt211          : retVal = DD_T("kLnWt211");         break;
    case OdDb::kLnWtByLayer      : retVal = DD_T("kLnWtByLayer");     break;
    case OdDb::kLnWtByBlock      : retVal = DD_T("kLnWtByBlock");     break;
    case OdDb::kLnWtByLwDefault  : retVal = DD_T("kLnWtByLwDefault"); break;
  }
  return retVal; 
}

/************************************************************************/
/* Convert the specified value to an OdDb::MeasurementValue string      */
/************************************************************************/
OdString toString(const OdDb::MeasurementValue val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kEnglish : retVal = DD_T("kEnglish"); break;
    case OdDb::kMetric  : retVal = DD_T("kMetric");  break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::Planarity string             */
/************************************************************************/
OdString toString(const OdDb::Planarity val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kNonPlanar : retVal = DD_T("kNonPlanar"); break;
    case OdDb::kPlanar    : retVal = DD_T("kPlanar");    break;
    case OdDb::kLinear    : retVal = DD_T("kLinear");    break;  
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdDb::PlotStyleNameType string     */
/************************************************************************/
OdString toString(const OdDb::PlotStyleNameType val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kPlotStyleNameByLayer       : retVal = DD_T("kPlotStyleNameByLayer");       break;
    case OdDb::kPlotStyleNameByBlock       : retVal = DD_T("kPlotStyleNameByBlock");       break;
    case OdDb::kPlotStyleNameIsDictDefault : retVal = DD_T("kPlotStyleNameIsDictDefault"); break;  
    case OdDb::kPlotStyleNameById          : retVal = DD_T("kPlotStyleNameById");          break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::ProxyImage string            */
/************************************************************************/
OdString toString(const OdDb::ProxyImage val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kProxyNotShow     : retVal = DD_T("kProxyNotShow");     break;
    case OdDb::kProxyShow        : retVal = DD_T("kProxyShow");        break;
    case OdDb::kProxyBoundingBox : retVal = DD_T("kProxyBoundingBox"); break;  
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdGsView::RenderMode string        */
/************************************************************************/
OdString toString(const OdGsView::RenderMode val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdGsView::kBoundingBox                : retVal = DD_T("kBoundingBox");                break;
    case OdGsView::k2DOptimized                : retVal = DD_T("k2DOptimized");                break;
    case OdGsView::kWireframe                  : retVal = DD_T("kWireframe");                  break;
    case OdGsView::kHiddenLine                 : retVal = DD_T("kHiddenLine");                 break;
    case OdGsView::kFlatShaded                 : retVal = DD_T("kFlatShaded");                 break;
    case OdGsView::kGouraudShaded              : retVal = DD_T("kGouraudShaded");              break;
    case OdGsView::kFlatShadedWithWireframe    : retVal = DD_T("kFlatShadedWithWireframe");    break;
    case OdGsView::kGouraudShadedWithWireframe : retVal = DD_T("kGouraudShadedWithWireframe"); break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::RenderMode string            */
/************************************************************************/
OdString toString(const OdDb::RenderMode val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::k2DOptimized                : retVal = DD_T("k2DOptimized");                break;
    case OdDb::kWireframe                  : retVal = DD_T("kWireframe");                  break;
    case OdDb::kHiddenLine                 : retVal = DD_T("kHiddenLine");                 break;
    case OdDb::kFlatShaded                 : retVal = DD_T("kFlatShaded");                 break;
    case OdDb::kGouraudShaded              : retVal = DD_T("kGouraudShaded");              break;
    case OdDb::kFlatShadedWithWireframe    : retVal = DD_T("kFlatShadedWithWireframe");    break;
    case OdDb::kGouraudShadedWithWireframe : retVal = DD_T("kGouraudShadedWithWireframe"); break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::TextHorzMode string          */
/************************************************************************/
OdString toString(const OdDb::TextHorzMode val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kTextLeft        : retVal = DD_T("kTextLeft");   break;
    case OdDb::kTextCenter      : retVal = DD_T("kTextCenter"); break;
    case OdDb::kTextRight       : retVal = DD_T("kTextRight");  break;
    case OdDb::kTextAlign       : retVal = DD_T("kTextAlign");  break;
    case OdDb::kTextMid         : retVal = DD_T("kTextMid");    break;
    case OdDb::kTextFit         : retVal = DD_T("kTextFit");    break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdDb::TextVertMode string          */
/************************************************************************/
OdString toString(const OdDb::TextVertMode val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kTextBase        : retVal = DD_T("kTextBase"); break;
    case OdDb::kTextBottom      : retVal = DD_T("kTextBottom"); break;
    case OdDb::kTextVertMid     : retVal = DD_T("kTextVertMid"); break;
    case OdDb::kTextTop         : retVal = DD_T("kTextTop"); break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::UnitsValue string            */
/************************************************************************/
OdString toString(const OdDb::UnitsValue val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kUnitsUndefined    : retVal = DD_T("kUnitsUndefined");    break;
    case OdDb::kUnitsInches       : retVal = DD_T("kUnitsInches");       break;
    case OdDb::kUnitsFeet         : retVal = DD_T("kUnitsFeet");         break;
    case OdDb::kUnitsMiles        : retVal = DD_T("kUnitsMiles");        break;
    case OdDb::kUnitsMillimeters  : retVal = DD_T("kUnitsMillimeters");  break;
    case OdDb::kUnitsCentimeters  : retVal = DD_T("kUnitsCentimeters");  break;
    case OdDb::kUnitsMeters       : retVal = DD_T("kUnitsMeters");       break;
    case OdDb::kUnitsKilometers   : retVal = DD_T("kUnitsKilometers");   break;
    case OdDb::kUnitsMicroinches  : retVal = DD_T("kUnitsMicroinches");  break;
    case OdDb::kUnitsMils         : retVal = DD_T("kUnitsMils");         break;
    case OdDb::kUnitsYards        : retVal = DD_T("kUnitsYards");        break;
    case OdDb::kUnitsAngstroms    : retVal = DD_T("kUnitsAngstroms");    break;
    case OdDb::kUnitsNanometers   : retVal = DD_T("kUnitsNanometers");   break;
    case OdDb::kUnitsMicrons      : retVal = DD_T("kUnitsMicrons");      break;
    case OdDb::kUnitsDecimeters   : retVal = DD_T("kUnitsDecimeters");   break;
    case OdDb::kUnitsDekameters   : retVal = DD_T("kUnitsDekameters");   break;
    case OdDb::kUnitsHectometers  : retVal = DD_T("kUnitsHectometers");  break;
    case OdDb::kUnitsGigameters   : retVal = DD_T("kUnitsGigameters");   break;
    case OdDb::kUnitsAstronomical : retVal = DD_T("kUnitsAstronomical"); break;
    case OdDb::kUnitsLightYears   : retVal = DD_T("kUnitsLightYears");   break;
    case OdDb::kUnitsParsecs      : retVal = DD_T("kUnitsParsecs");      break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdDb::Visibility string            */
/************************************************************************/
OdString toString(const OdDb::Visibility val)
{ 
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDb::kVisible   : retVal = DD_T("kVisible");   break;
    case OdDb::kInvisible : retVal = DD_T("kInvisible"); break;
  }
  return retVal; 
}

/************************************************************************/
/* Convert the specified value to an OdDb::XrefStatus string            */
/************************************************************************/
OdString toString(const OdDb::XrefStatus val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
  case OdDb::kXrfNotAnXref    : retVal = DD_T("kXrfNotAnXref");    break;
  case OdDb::kXrfResolved     : retVal = DD_T("kXrfResolved");     break;
  case OdDb::kXrfUnloaded     : retVal = DD_T("kXrfUnloaded");     break;
  case OdDb::kXrfUnreferenced : retVal = DD_T("kXrfUnreferenced"); break;
  case OdDb::kXrfFileNotFound : retVal = DD_T("kXrfFileNotFound"); break;
  case OdDb::kXrfUnresolved   : retVal = DD_T("kXrfUnresolved");   break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::AnnoType string              */
/************************************************************************/
OdString toString(const OdDbLeader::AnnoType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbLeader::kBlockRef : retVal = DD_T("kBlockRef"); break;
    case OdDbLeader::kFcf      : retVal = DD_T("kFcf");      break;
    case OdDbLeader::kMText    : retVal = DD_T("kMText");    break;
    case OdDbLeader::kNoAnno   : retVal = DD_T("kNoAnno");   break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::HatchPatternType string      */
/************************************************************************/
OdString toString(const OdDbHatch::HatchPatternType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbHatch::kPreDefined    : retVal = DD_T("kPreDefined");    break;
    case OdDbHatch::kUserDefined   : retVal = DD_T("kUserDefined");   break;
    case OdDbHatch::kCustomDefined : retVal = DD_T("kCustomDefined"); break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::HatchObjectType string       */
/************************************************************************/
OdString toString(const OdDbHatch::HatchObjectType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbHatch::kHatchObject    : retVal = DD_T("kHatchObject");    break;
    case OdDbHatch::kGradientObject : retVal = DD_T("kGradientObject"); break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::HatchStyle string            */
/************************************************************************/
OdString toString(const OdDbHatch::HatchStyle val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbHatch::kNormal : retVal = DD_T("kNormal");    break;
    case OdDbHatch::kOuter  : retVal = DD_T("kOuter");     break;
    case OdDbHatch::kIgnore : retVal = DD_T("kIgnore");    break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbBlockTableRecord::BlockScaling */
/* string                                                               */
/************************************************************************/
OdString toString(const OdDbBlockTableRecord::BlockScaling val)
{ 
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdDbBlockTableRecord::kAny     : retVal = DD_T("kAny");     break;
    case OdDbBlockTableRecord::kUniform : retVal = DD_T("kUniform"); break;
  }
  return retVal; 
}

/************************************************************************/
/* Convert the specified value to an OdDbDate string                    */
/************************************************************************/
OdString toString(const OdDbDate& val) { 
  OdString timeString;
  val.ctime(timeString);
  return timeString; 
}

/************************************************************************/
/* Convert the specified value to an OdDbDimAssoc::RotatedType string   */
/************************************************************************/
OdString toString(const OdDbDimAssoc::RotatedType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbDimAssoc::kParallel           : retVal = DD_T("kParallel"); break;
    case OdDbDimAssoc::kPerpendicular      : retVal = DD_T("kPerpendicular");      break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbHandle string                  */
/************************************************************************/
OdString toString(const OdDbHandle& val)
{
  return DD_T("[") + val.ascii() + DD_T("]");
}

/************************************************************************/
/* Convert the specified value to an OdDbObjectId string                */
/************************************************************************/
OdString toString(const OdDbObjectId val)
{
  if (val.isNull()) 
  {
    return DD_T("Null");
  }

  /**********************************************************************/
  /* Open the object                                                    */
  /**********************************************************************/
  OdDbObjectPtr pObject = val.safeOpenObject();

  /**********************************************************************/
  /* Return the name of an OdDbSymbolTableRecord                        */
  /**********************************************************************/
  if (pObject->isKindOf(OdDbSymbolTableRecord::desc()))
  {
    OdDbSymbolTableRecordPtr pSTR = pObject;
    return pSTR->getName(); 
  }

  /**********************************************************************/
  /* Return the name of an OdDbMlineStyle                               */
  /**********************************************************************/
  if (pObject->isKindOf(OdDbMlineStyle::desc()))
  {
    OdDbMlineStylePtr pStyle = pObject;
    return pStyle->name(); 
  }

  /**********************************************************************/
  /* Return the name of a PlotStyle                                      */
  /**********************************************************************/
  if (pObject->isKindOf(OdDbPlaceHolder::desc()))
  {
    OdDbDictionaryPtr pDictionary = val.database()->getPlotStyleNameDictionaryId().safeOpenObject(); 
    OdString plotStyleName = pDictionary->nameAt(val);
    return plotStyleName; 
  }

  /**********************************************************************/
  /* Return the name of an OdDbMaterial                                 */
  /**********************************************************************/
  if (pObject->isKindOf(OdDbMaterial::desc()))
  {
    OdDbMaterialPtr pMaterial = pObject;
    return pMaterial->name(); 
  }
  
  /**********************************************************************/
  /* We don't know what it is, so return the description of the object  */
  /* object specified by the ObjectId                                   */
  /**********************************************************************/
  return toString(pObject->isA());
}


/************************************************************************/
/* Convert the specified value to an OdRxClass name string              */
/************************************************************************/
OdString toString(const OdRxClass* val)
{
  return OdString(DD_T("<")) + (val->name()) + DD_T(">");
}

/************************************************************************/
/* Convert the specified value to an OdGeScale2d string                 */
/************************************************************************/
OdString toString(const OdGeScale2d& val)
{ 
  return DD_T("{") + toString(val.sx) + DD_T(" ") + toString(val.sy) + DD_T("}");
}

/************************************************************************/
/* Convert the specified value to an OdGeScale3d string                 */
/************************************************************************/
OdString toString(const OdGeScale3d& val)
{ 
  OdString s;
  s.format(DD_T("[%.1f %.1f %.1f]"), val.sx, val.sy, val.sz);
  return s;
}
/************************************************************************/
/* Convert the specified value to an OdGePoint2d string                 */
/************************************************************************/
OdString toString(const OdGePoint2d& val)
{ 
  OdString s;
  s.format(DD_T("[%.1f %.1f]"), val.x, val.y);
  return s;
}

/************************************************************************/
/* Convert the specified value to an OdGePoint3d string                 */
/************************************************************************/
OdString toString(const OdGePoint3d& val)
{ 
  OdString s;
  s.format(DD_T("[%.1f %.1f %.1f]"), val.x, val.y, val.z);
  return s;
}

/************************************************************************/
/* Convert the specified value to an OdGeVector2d string                */
/************************************************************************/
OdString toString(const OdGeVector2d& val)
{ 
  OdString s;
  s.format(DD_T("[%.1f %.1f]"), val.x, val.y);
  return s;
}

/************************************************************************/
/* Convert the specified value to an OdGeVector3d string                */
/************************************************************************/
OdString toString(const OdGeVector3d& val)
{ 
  OdString s;
  s.format(DD_T("[%.1f %.1f %.1f]"), val.x, val.y, val.z);
  return s;
}
/************************************************************************/
/* Convert the specified value to an OdGeVector3d string                */
/************************************************************************/
OdString toString(const OdGeVector3d* pVal)
{ 
  if(pVal && !pVal->isZeroLength())
    return toString(*pVal);
  else
    return DD_T("Null");
}
/************************************************************************/
/* Convert the specified value to an OdString string                    */
/************************************************************************/
OdString toString(const OdChar* val) 
{ 
  if (!val) 
  {
    return DD_T("\"\"");
  }
  
  return val;
}
/************************************************************************/
/* Convert the specified value to an OdString string                    */
/************************************************************************/
OdString toString(const OdString& val) 
{ 
  if (val.isEmpty()) 
  {
    return DD_T("\"\"");
  }
  return val;
}

/************************************************************************/
/* Convert the specified value to an OdDbViewport::ShadePlotType string */
/************************************************************************/
OdString toString(const OdDbViewport::ShadePlotType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbViewport::kAsDisplayed : retVal = DD_T("kAsDisplayed"); break;
    case OdDbViewport::kHidden      : retVal = DD_T("kHidden");      break;
    case OdDbViewport::kRendered    : retVal = DD_T("kRendered");    break;
    case OdDbViewport::kWireframe   : retVal = DD_T("kWireframe");   break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbPlotSettings::ShadePlotType    */
/* string                                                               */
/************************************************************************/
OdString toString(const OdDbPlotSettings::ShadePlotType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbPlotSettings::kAsDisplayed : retVal = DD_T("kAsDisplayed"); break;
    case OdDbPlotSettings::kHidden      : retVal = DD_T("kHidden");      break;
    case OdDbPlotSettings::kRendered    : retVal = DD_T("kRendered");    break;
    case OdDbPlotSettings::kWireframe   : retVal = DD_T("kWireframe");   break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbOle2Frame::Type string         */
/************************************************************************/
OdString toString(const OdDbOle2Frame::Type val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbOle2Frame::kEmbedded : retVal = DD_T("kEmbedded"); break;
    case OdDbOle2Frame::kLink     : retVal = DD_T("kLink");     break;
    case OdDbOle2Frame::kStatic   : retVal = DD_T("kStatic");   break;
    case OdDbOle2Frame::kUnknown  : retVal = DD_T("kUnknown");  break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbRasterImage::ClipBoundaryType  */
/* string                                                               */
/************************************************************************/
OdString toString(const OdDbRasterImage::ClipBoundaryType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbRasterImage::kInvalid  : retVal = DD_T("kInvalid"); break;
    case OdDbRasterImage::kPoly     : retVal = DD_T("kPoly");     break;
    case OdDbRasterImage::kRect     : retVal = DD_T("kRect");   break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdGiRasterImage::Units string      */
/************************************************************************/
OdString toString(const OdGiRasterImage::Units val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdGiRasterImage::kNone         : retVal = DD_T("kNone");         break;
    case OdGiRasterImage::kMillimeter   : retVal = DD_T("kMillimeter");   break;
    case OdGiRasterImage::kCentimeter   : retVal = DD_T("kCentimeter");   break;
    case OdGiRasterImage::kMeter        : retVal = DD_T("kMeter");        break;
    case OdGiRasterImage::kKilometer    : retVal = DD_T("kKilometer");    break;
    case OdGiRasterImage::kInch         : retVal = DD_T("kInch");         break;
    case OdGiRasterImage::kFoot         : retVal = DD_T("kFoot");         break;
    case OdGiRasterImage::kYard         : retVal = DD_T("kYard");         break;
    case OdGiRasterImage::kMile         : retVal = DD_T("kMile");         break;
    case OdGiRasterImage::kMicroinches  : retVal = DD_T("kMicroinches");  break;
    case OdGiRasterImage::kMils         : retVal = DD_T("kMils");         break;
    case OdGiRasterImage::kAngstroms    : retVal = DD_T("kAngstroms");    break;
    case OdGiRasterImage::kNanometers   : retVal = DD_T("kNanometers");   break;
    case OdGiRasterImage::kMicrons      : retVal = DD_T("kMicrons");      break;
    case OdGiRasterImage::kDecimeters   : retVal = DD_T("kDecimeters");   break;
    case OdGiRasterImage::kDekameters   : retVal = DD_T("kDekameters");   break;
    case OdGiRasterImage::kHectometers  : retVal = DD_T("kHectometers");  break;
    case OdGiRasterImage::kGigameters   : retVal = DD_T("kGigameters");   break;
    case OdGiRasterImage::kAstronomical : retVal = DD_T("kAstronomical"); break;
    case OdGiRasterImage::kLightYears   : retVal = DD_T("kLightYears");   break;
    case OdGiRasterImage::kParsecs      : retVal = DD_T("kParsecs");      break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdGiArcType string                 */
/************************************************************************/
OdString toString(const OdGiArcType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case kOdGiArcSimple  : retVal = DD_T("kOdGiArcSimple");   break;
    case kOdGiArcSector  : retVal = DD_T("kOdGiArcSector");   break;
    case kOdGiArcChord   : retVal = DD_T("kOdGiArcChord");    break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdGe::EntityId string              */
/************************************************************************/
OdString toString(const OdGe::EntityId val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdGe::kAugPolyline2d         : retVal = DD_T("kAugPolyline2d");          break;
    case OdGe::kAugPolyline3d         : retVal = DD_T("kAugPolyline3d");          break;
    case OdGe::kBezierCurve3d         : retVal = DD_T("kBezierCurve3d");          break;
    case OdGe::kBoundBlock2d          : retVal = DD_T("kBoundBlock2d");           break;
    case OdGe::kBoundBlock3d          : retVal = DD_T("kBoundBlock3d");           break;
    case OdGe::kBoundedPlane          : retVal = DD_T("kBoundedPlane");           break;
    case OdGe::kCircArc2d             : retVal = DD_T("kCircArc2d");              break;
    case OdGe::kCircArc3d             : retVal = DD_T("kCircArc3d");              break;
    case OdGe::kClipBoundary2d        : retVal = DD_T("kClipBoundary2d");         break;
    case OdGe::kCompositeCrv2d        : retVal = DD_T("kCompositeCrv2d");         break;
    case OdGe::kCompositeCrv3d        : retVal = DD_T("kCompositeCrv3d");         break;
    case OdGe::kCone                  : retVal = DD_T("kCone");                   break;
    case OdGe::kConic2d               : retVal = DD_T("kConic2d");                break;
    case OdGe::kConic3d               : retVal = DD_T("kConic3d");                break;
    case OdGe::kCubicSplineCurve2d    : retVal = DD_T("kCubicSplineCurve2d");     break;
    case OdGe::kCubicSplineCurve3d    : retVal = DD_T("kCubicSplineCurve3d");     break;
    case OdGe::kCurve2d               : retVal = DD_T("kCurve2d");                break;
    case OdGe::kCurve3d               : retVal = DD_T("kCurve3d");                break;
    case OdGe::kCurveBoundedSurface   : retVal = DD_T("kCurveBoundedSurface");    break;
    case OdGe::kCurveCurveInt2d       : retVal = DD_T("kCurveCurveInt2d");        break;
    case OdGe::kCurveCurveInt3d       : retVal = DD_T("kCurveCurveInt3d");        break;
    case OdGe::kCurveSampleData       : retVal = DD_T("kCurveSampleData");        break;
    case OdGe::kCurveSurfaceInt       : retVal = DD_T("kCurveSurfaceInt");        break;
    case OdGe::kCylinder              : retVal = DD_T("kCylinder");               break;
    case OdGe::kDSpline2d             : retVal = DD_T("kDSpline2d");              break;
    case OdGe::kDSpline3d             : retVal = DD_T("kDSpline3d");              break;
    case OdGe::kEllipArc2d            : retVal = DD_T("kEllipArc2d");             break;
    case OdGe::kEllipArc3d            : retVal = DD_T("kEllipArc3d");             break;
    case OdGe::kEllipCone             : retVal = DD_T("kEllipCone");              break;
    case OdGe::kEllipCylinder         : retVal = DD_T("kEllipCylinder");          break;
    case OdGe::kEntity2d              : retVal = DD_T("kEntity2d");               break;
    case OdGe::kEntity3d              : retVal = DD_T("kEntity3d");               break;
    case OdGe::kEnvelope2d            : retVal = DD_T("kEnvelope2d");             break;
    case OdGe::kExternalBoundedSurface: retVal = DD_T("kExternalBoundedSurface"); break;
    case OdGe::kExternalCurve2d       : retVal = DD_T("kExternalCurve2d");        break;
    case OdGe::kExternalCurve3d       : retVal = DD_T("kExternalCurve3d");        break;
    case OdGe::kExternalObject        : retVal = DD_T("kExternalObject");         break;
    case OdGe::kExternalSurface       : retVal = DD_T("kExternalSurface");        break;
    case OdGe::kFitData3d             : retVal = DD_T("kFitData3d");              break;
    case OdGe::kHatch                 : retVal = DD_T("kHatch");                  break;
    case OdGe::kIntervalBoundBlock    : retVal = DD_T("kIntervalBoundBlock");     break;
    case OdGe::kLine2d                : retVal = DD_T("kLine2d");                 break;
    case OdGe::kLine3d                : retVal = DD_T("kLine3d");                 break;
    case OdGe::kLineSeg2d             : retVal = DD_T("kLineSeg2d");              break;
    case OdGe::kLineSeg3d             : retVal = DD_T("kLineSeg3d");              break;
    case OdGe::kLinearEnt2d           : retVal = DD_T("kLinearEnt2d");            break;
    case OdGe::kLinearEnt3d           : retVal = DD_T("kLinearEnt3d");            break;
    case OdGe::kNurbCurve2d           : retVal = DD_T("kNurbCurve2d");            break;
    case OdGe::kNurbCurve3d           : retVal = DD_T("kNurbCurve3d");            break;
    case OdGe::kNurbSurface           : retVal = DD_T("kNurbSurface");            break;
    case OdGe::kObject                : retVal = DD_T("kObject");                 break;
    case OdGe::kOffsetCurve2d         : retVal = DD_T("kOffsetCurve2d");          break;
    case OdGe::kOffsetCurve3d         : retVal = DD_T("kOffsetCurve3d");          break;
    case OdGe::kOffsetSurface         : retVal = DD_T("kOffsetSurface");          break;
    case OdGe::kPlanarEnt             : retVal = DD_T("kPlanarEnt");              break;
    case OdGe::kPlane                 : retVal = DD_T("kPlane");                  break;
    case OdGe::kPointEnt2d            : retVal = DD_T("kPointEnt2d");             break;
    case OdGe::kPointEnt3d            : retVal = DD_T("kPointEnt3d");             break;
    case OdGe::kPointOnCurve2d        : retVal = DD_T("kPointOnCurve2d");         break;
    case OdGe::kPointOnCurve3d        : retVal = DD_T("kPointOnCurve3d");         break;
    case OdGe::kPointOnSurface        : retVal = DD_T("kPointOnSurface");         break;
    case OdGe::kPolyline2d            : retVal = DD_T("kPolyline2d");             break;
    case OdGe::kPolyline3d            : retVal = DD_T("kPolyline3d");             break;
    case OdGe::kPolynomCurve3d        : retVal = DD_T("kPolynomCurve3d");         break;
    case OdGe::kPosition2d            : retVal = DD_T("kPosition2d");             break;
    case OdGe::kPosition3d            : retVal = DD_T("kPosition3d");             break;
    case OdGe::kRay2d                 : retVal = DD_T("kRay2d");                  break;
    case OdGe::kRay3d                 : retVal = DD_T("kRay3d");                  break;
    case OdGe::kSphere                : retVal = DD_T("kSphere");                 break;
    case OdGe::kSplineEnt2d           : retVal = DD_T("kSplineEnt2d");            break;
    case OdGe::kSplineEnt3d           : retVal = DD_T("kSplineEnt3d");            break;
    case OdGe::kSurface               : retVal = DD_T("kSurface");                break;
    case OdGe::kSurfaceCurve2dTo3d    : retVal = DD_T("kSurfaceCurve2dTo3d");     break;
    case OdGe::kSurfaceCurve3dTo2d    : retVal = DD_T("kSurfaceCurve3dTo2d");     break;
    case OdGe::kSurfaceSurfaceInt     : retVal = DD_T("kSurfaceSurfaceInt");      break;
    case OdGe::kTorus                 : retVal = DD_T("kTorus");                  break;
    case OdGe::kTrimmedCrv2d          : retVal = DD_T("kTrimmedCrv2d");           break;
    case OdGe::kTrimmedCurve2d        : retVal = DD_T("kTrimmedCurve2d");         break;
    case OdGe::kTrimmedCurve3d        : retVal = DD_T("kTrimmedCurve3d");         break;
    case OdGe::kTrimmedSurface        : retVal = DD_T("kTrimmedSurface");         break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDbDimAssoc::Associativity string */
/************************************************************************/
OdString toString(const OdDbDimAssoc::Associativity val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDbDimAssoc::kFirstPoint    : retVal = DD_T("kFirstPoint");   break;
    case OdDbDimAssoc::kSecondPoint   : retVal = DD_T("kSecondPoint");  break;
    case OdDbDimAssoc::kThirdPoint    : retVal = DD_T("kThirdPoint");   break;
    case OdDbDimAssoc::kFourthPoint   : retVal = DD_T("kFourthPoint");  break;
    case OdDbDimAssoc::kPointAmount   : retVal = DD_T("kPointAmount");  break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdDb::OsnapMode string             */
/************************************************************************/
OdString toString(const OdDb::OsnapMode val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDb::kOsModeEnd     : retVal = DD_T("kOsModeEnd");    break;
    case OdDb::kOsModeMid     : retVal = DD_T("kOsModeMid");    break;
    case OdDb::kOsModeCen     : retVal = DD_T("kOsModeCen");    break;
    case OdDb::kOsModeNode    : retVal = DD_T("kOsModeNode");   break;
    case OdDb::kOsModeQuad    : retVal = DD_T("kOsModeQuad");   break;
    case OdDb::kOsModeIns     : retVal = DD_T("kOsModeIns");    break;
    case OdDb::kOsModePerp    : retVal = DD_T("kOsModePerp");   break;
    case OdDb::kOsModeTan     : retVal = DD_T("kOsModeTan");    break;
    case OdDb::kOsModeNear    : retVal = DD_T("kOsModeNear");   break;
    case OdDb::kOsModeApint   : retVal = DD_T("kOsModeApint");  break;
    case OdDb::kOsModePar     : retVal = DD_T("kOsModePar");    break;
    case OdDb::kOsModeStart   : retVal = DD_T("kOsModeStart");  break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdDb::OsnapMode string             */
/************************************************************************/
OdString toString(const OdDb::SubentType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case OdDb::kNullSubentType     : retVal = DD_T("kNullSubentType");    break;
    case OdDb::kFaceSubentType     : retVal = DD_T("kFaceSubentType");    break;
    case OdDb::kEdgeSubentType     : retVal = DD_T("kEdgeSubentType");    break;
    case OdDb::kVertexSubentType   : retVal = DD_T("kVertexSubentType");  break;
    case OdDb::kMlineSubentCache   : retVal = DD_T("kMlineSubentCache");  break;
  }
  return retVal;
}

/************************************************************************/
/* Convert the specified value to an OdGiOrientationType string         */
/************************************************************************/
OdString toString(const OdGiOrientationType val)
{
  OdString retVal(DD_T("???"));  
  switch (val) 
  {
    case kOdGiCounterClockwise    : retVal = DD_T("kOdGiCounterClockwise");  break;
    case kOdGiNoOrientation       : retVal = DD_T("kOdGiNoOrientation");     break;
    case kOdGiClockwise           : retVal = DD_T("kOdGiClockwise");         break;
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an string                             */
/************************************************************************/
OdString toString(const OdAve::MaterialType val)
{ 
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdAve::kStandard  : retVal = DD_T("kStandard");    break; 
    case OdAve::kGranite   : retVal = DD_T("kGranite");     break; 
    case OdAve::kMarble    : retVal = DD_T("kMarble");      break; 
    case OdAve::kWood      : retVal = DD_T("kWood");        break; 
  }
  return retVal;
}
/************************************************************************/
/* Convert the specified value to an OdGsLight string                   */
/************************************************************************/
OdString toString(const OdGsLight::LightType val)
{
  OdString retVal(DD_T("???"));
  switch (val) 
  {
    case OdGsLight::kPointLight   : retVal = DD_T("kPointLight");    break; 
    case OdGsLight::kDistantLight : retVal = DD_T("kDistantLight");  break; 
    case OdGsLight::kSpotlight    : retVal = DD_T("kSpotlight");     break; 
  }  
  return retVal;
}

