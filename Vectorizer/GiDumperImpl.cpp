#include "..\\PegAeSys\\stdafx.h"

#include "OdaCommon.h"
#include "OdString.h"
#include "CmColor.h"
#include "DbObjectId.h"
#include "GiDumperImpl.h"
#include "Gi/GiTextStyle.h"
#include "toString.h"
#define STL_USING_STRING
#define STL_USING_IOSTREAM
#include "OdaSTL.h"

OdGiDumperImpl::OdGiDumperImpl()
: m_indentLevel(0)
, m_target(0)
{
}

OdGiDumperImpl::~OdGiDumperImpl()
{
}

OdGiDumperPtr OdGiDumperImpl::createObject(std::_tostream* target)
{
  OdGiDumperPtr pRes = OdRxObjectImpl<OdGiDumperImpl, OdGiDumper>::createObject();
  (static_cast<OdGiDumperImpl*>(pRes.get()))->m_target = target;
  return pRes;
}

/************************************************************************/
/* Output a string in the form                                          */
/*   str1:. . . . . . . . . . . .str2                                   */
/************************************************************************/
void OdGiDumperImpl::output(const OdString& str1, 
                            const OdString& str2)
{
  const OdString spaces(OD_T("                                                            "));
  const OdString leader(OD_T(". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . "));

  OdString buffer;

  const int colWidth = 38;

  buffer = str1;
  /**********************************************************************/
  /* If rightString is not specified, just output the indented          */
  /* leftString. Otherwise, fill the space between leftString and       */
  /* rightString with leader characters.                                */
  /**********************************************************************/  
  if (str2 != OD_T(""))
  {
    int leaders = colWidth-(str1.getLength() + m_indentLevel);
    if (leaders > 0){
      buffer = str1 + leader.mid(str1.getLength() + m_indentLevel, leaders) + str2;  
    }
    else
    {
      buffer = OD_T(">") + str1 + OD_T(" ") + str2;  
    }
  }
  output(buffer);
}
/************************************************************************/
/* Output a 3d transformation matrix                                    */
/************************************************************************/
void OdGiDumperImpl::output(const OdString& label, 
                            const OdGeMatrix3d* xfm)
{
  if (xfm)
  {
    for (int i = 0; i < 4; i++)
    {
      OdString leftString = (i) ? OD_T("") : label.c_str();
      OdString rightString;
      rightString = OD_T("[");
      for (int j = 0; j < 4 ; j++)
      {
        if (j)
        {
          rightString += OD_T(" ");
        }
        rightString += toString(*xfm[i][j]);
      }
      rightString += OD_T("]");
      output(leftString, rightString);
    }
  }
  else
  {
    output(label, OD_T("[]"));
  }
}
/************************************************************************/
/* Output and indent a character string                                 */
/************************************************************************/
void OdGiDumperImpl::output(const OdString& str)
{
  OdString indent;
  for (int i = 0; i < m_indentLevel; i++)
    indent += ' ';
#ifdef _UNICODE
  *m_target << (const wchar_t*)indent << (const wchar_t*)str << L"\n";
#else
  *m_target << (const char*)indent << (const char*)str << "\n";
#endif
}
/************************************************************************/
/* Output vertices                                                      */
/************************************************************************/
void OdGiDumperImpl::output(OdInt32 nPoints, 
                            const OdGePoint3d* points)
{
  pushIndent();
  for (OdInt32 i = 0; i < nPoints; ++i)
  {
    output(toString(OD_T("Vertex[%d]"), (int)i), toString(points[i]));
  }
  popIndent();
}
/************************************************************************/
/* Output a text style                                                  */
/************************************************************************/
void OdGiDumperImpl::output(const OdGiTextStyle& textStyle)
{
  output(OD_T("Text Style"));
  pushIndent();
  if (textStyle.isTtfFont())
  {
    OdString typeface;
    bool bold;
    bool italic;
    int charset;
    int pitchAndFamily;
    textStyle.font(typeface, bold, italic, charset, pitchAndFamily);
    output(OD_T("Typeface"),                toString(typeface));
    output(OD_T("Character Set"),           toString(charset));
    output(OD_T("Bold"),                    toString(bold));
    output(OD_T("Italic"),                  toString(italic));
    output(OD_T("Font Pitch and Family"),   toHexString(pitchAndFamily));
  }
  else
  {
    output(OD_T("Filename"),                shortenPath(textStyle.ttfdecriptor().fileName()));
    output(OD_T("BigFont Filename"),        shortenPath(textStyle.bigFontFileName()));
  }
  output(OD_T("Shape File"),                toString(textStyle.isShape()));
  output(OD_T("Text Height"),               toString(textStyle.textSize()));
  output(OD_T("Width Factor"),              toString(textStyle.xScale()));
  output(OD_T("Obliquing Angle"),           toDegreeString(textStyle.obliquingAngle()));
  output(OD_T("Tracking Percentage"),       toDegreeString(textStyle.trackingPercent()));
  output(OD_T("Backwards"),                 toString(textStyle.isBackward()));
  output(OD_T("Vertical"),                  toString(textStyle.isVertical()));
  output(OD_T("Upside Down"),               toString(textStyle.isUpsideDown()));
  output(OD_T("Underlined"),                toString(textStyle.isUnderlined()));
  output(OD_T("Overlined"),                 toString(textStyle.isOverlined()));

  popIndent();
}
/************************************************************************/
/* Output ACI colors                                                    */
/************************************************************************/
void OdGiDumperImpl::outputColors(const OdUInt16* colors, 
                                  OdInt32 numColors, 
                                  const OdString& name)
{
  if (colors)
  {
    output(OdString(name) + OD_T(" Colors"));
    pushIndent();
    OdInt32 i;
    for (i = 0; i < numColors; i++)
    {
      output(toString(name + OD_T("[%d]"), (int)i), toString(OD_T("ACI %d"), colors[i]));
    }
    popIndent();
  }
}
/************************************************************************/
/* Output true colors                                                   */
/************************************************************************/
void OdGiDumperImpl::outputTrueColors(const OdCmEntityColor* trueColors, 
                                      OdInt32 numColors,
                                      const OdString& name)
{
  if (trueColors)
  {
    output(name + OD_T(" TrueColors"));
    pushIndent();
    OdInt32 i;
    for (i = 0; i < numColors; i++)
    {
      output(toString(name + OD_T("[%d]"), (int)i), toString(trueColors[i]));
    }
    popIndent();
  }
}

/************************************************************************/
/* Output object ids                                                    */
/************************************************************************/
void OdGiDumperImpl::outputIds(OdDbStub** ids, 
                               OdInt32 numIds, 
                               const OdString& name, 
                               const OdString& table)
{
  if (ids)
  {
    output(name + OD_T(" ") + table);
    pushIndent();
    OdInt32 i;
    for (i = 0; i < numIds; i++)
    {
      OdDbObjectId id(ids[i]);
      output(toString(name + OD_T("[%d]"), (int)i), toString(id));
    }
    popIndent();
  }
}
/************************************************************************/
/* Output selection markers                                             */
/************************************************************************/
void OdGiDumperImpl::outputSelectionMarkers(const OdInt32* selectionMarkers, 
                                            OdInt32 numMarkers, 
                                            const OdString& name)
{
  if (selectionMarkers)
  {
    output(name + OD_T(" Selection Markers") );
    pushIndent();
    OdInt32 i;
    for (i = 0; i < numMarkers; i++)
    {
      output(toString(OdString(name) + OD_T("[%d]"), (int)i), toString((int)selectionMarkers[i]));
    }
  }
}

void OdGiDumperImpl::outputSelectionMarkers(const OdGsMarker* selectionMarkers, 
                                            OdInt32 numMarkers, 
                                            const OdString& name)
{
  if (selectionMarkers)
  {
    output(name + OD_T(" Selection Markers") );
    pushIndent();
    OdInt32 i;
    for (i = 0; i < numMarkers; i++)
    {
      output(toString(OdString(name) + OD_T("[%d]"), (int)i), toString((int)selectionMarkers[i]));
    }
  }
}

/************************************************************************/
/* Output visibility                                                    */
/************************************************************************/
void OdGiDumperImpl::outputVisibility(const OdUInt8* visibility, 
                                      OdInt32 count, 
                                      const OdString& name)
{
  if (visibility)
  {
    output(name + OD_T(" Visibility"));
    pushIndent();
    OdInt32 i;
    for (i = 0; i < count; i++)
    {
      OdString vis (OD_T("???"));
      switch (visibility[i])
      {
      case kOdGiInvisible   : vis = OD_T("kOdGiInvisible");   break;
      case kOdGiVisible     : vis = OD_T("kOdGiVisible");     break;
      case kOdGiSilhouette  : vis = OD_T("kOdGiSilhouette");  break;
      }
      output(toString(name + OD_T("[%d]"), (int)i), vis);
    }
    popIndent();
  }
}

/************************************************************************/
/* Output Face Data                                                     */
/************************************************************************/
void OdGiDumperImpl::outputFaceData(const OdGiFaceData* pFaceData, 
                                    OdInt32 numFaces)
{
  if (pFaceData)
  {
    outputColors(pFaceData->colors(),                     numFaces, OD_T("Face"));
    outputTrueColors(pFaceData->trueColors(),             numFaces, OD_T("Face"));
    outputIds(pFaceData->layerIds(),                      numFaces, OD_T("Face"), OD_T("Layers"));
    outputSelectionMarkers(pFaceData->selectionMarkers(), numFaces, OD_T("Face"));
    outputVisibility(pFaceData->visibility(),             numFaces, OD_T("Face"));
  }
}
/************************************************************************/
/* Output edge data                                                     */
/************************************************************************/
void OdGiDumperImpl::outputEdgeData(const OdGiEdgeData* pEdgeData, 
                                    OdInt32 numEdges)
{
  if (pEdgeData)
  {
    outputColors(pEdgeData->colors(),                     numEdges, OD_T("Edge"));
    outputTrueColors(pEdgeData->trueColors(),             numEdges, OD_T("Edge"));
    outputIds(pEdgeData->layerIds(),                      numEdges, OD_T("Edge"), OD_T("Layers"));
    outputIds(pEdgeData->linetypeIds(),                   numEdges, OD_T("Edge"), OD_T("Linetypes"));
    outputSelectionMarkers(pEdgeData->selectionMarkers(), numEdges, OD_T("Edge"));
    outputVisibility(pEdgeData->visibility(),             numEdges, OD_T("Edge"));
  }
}

/************************************************************************/
/* Output Vertex Data                                                   */
/************************************************************************/
void OdGiDumperImpl::outputVertexData(const OdGiVertexData* pVertexData, OdInt32 numVerts)
{
  if (pVertexData)
  {
    const OdGeVector3d* normals = pVertexData->normals();
    if (normals)
    {
      OdString orientation (OD_T("???"));
      switch (pVertexData->orientationFlag())
      {
      case kOdGiCounterClockwise    : orientation = OD_T("kOdGiCounterClockwise ");  break;
      case kOdGiNoOrientation       : orientation = OD_T("kOdGiNoOrientation");      break;
      case kOdGiClockwise           : orientation = OD_T("kOdGiClockwise");          break;
      }
      output(OD_T("Vertex Orientation Flag"), orientation);

      output(OD_T("Vertex Normals"));
      pushIndent();
      for (OdInt32 i = 0; i < numVerts; ++i)
      {
        output(toString(OD_T("Vertex[%d]"), (int)i), toString(normals[i]));
      }
      popIndent();

      outputTrueColors(pVertexData->trueColors(), numVerts, OD_T("Vertex"));
    }
  }
}