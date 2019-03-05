#ifndef _TO_STRING_H
#define _TO_STRING_H

#include "OdaCommon.h"

#include "DbDatabase.h"
#include "DbFiler.h"

#include "DbMaterial.h"
#include "Db2dVertex.h"
#include "Db3dPolylineVertex.h"
#include "DbAbstractViewTableRecord.h"
#include "DbArcAlignedText.h"
#include "DbAttribute.h"
#include "DbBlockReference.h"
#include "DbBlockTable.h"
#include "DbBlockTableRecord.h"
#include "DbCurve.h"
#include "DbDictionary.h"
#include "DbDimAssoc.h"
#include "DbDimStyleTable.h"
#include "DbDimStyleTableRecord.h"
#include "DbEntity.h"
#include "DbHatch.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbLayout.h"
#include "DbLeader.h"
#include "DbLinetypeTable.h"
#include "DbLinetypeTableRecord.h"
#include "DbMText.h"
#include "DbMlineStyle.h"
#include "DbOle2Frame.h"
#include "DbPolyline.h"
#include "DbRasterImage.h"
#include "DbRasterImageDef.h"
#include "DbRegAppTable.h"
#include "DbRegAppTableRecord.h"
#include "DbSubentId.h"
#include "DbText.h"
#include "DbTextStyleTable.h"
#include "DbTextStyleTableRecord.h"
#include "DbUCSTable.h"
#include "DbUCSTableRecord.h"
#include "DbViewTable.h"
#include "DbViewTableRecord.h"
#include "DbViewport.h"
#include "DbViewportTable.h"
#include "DbViewportTableRecord.h"
#include "DbXrecord.h"
#include "Ge/Ge.h"
#include "Ge/GeExtents3d.h"
#include "Ge/GeScale2d.h"
#include "Ge/GeScale3d.h"
#include "Gi/Gi.h"
#include "Gi/GiRasterImage.h"
#include "Gi/GiGeometry.h"
#include "Gi/GiPolyline.h"
#include "Gi/GiTextStyle.h"
#include "Gs/Gs.h"
#include "TextDefs.h"
#include "DbPlaceHolder.h"
#include "Ge/GeEllipArc3d.h"
#include "Ve/AveDefs.h"
#include "Ve/AveMaterial.h"
#include "Ve/AveLight.h"

OdString shortenPath(OdString path, int maxPath = 40);
OdString toArcSymbolTypeString (const int                   val);
OdString toDegreeString(const double                        val);
OdString toHexString(const int                              val);
OdString toLooptypeString(const int                         val);
OdString toOdGiVisibilityString(const OdUInt8               val);
OdString toString(const OdArcTextAlignment                  val);
OdString toString(const OdCmColor&                          val);
OdString toString(const OdCmEntityColor&                    val);
OdString toString(const OdDb::DwgVersion                    val);
OdString toString(const OdDb::EndCaps                       val);
OdString toString(const OdDb::JoinStyle                     val);
OdString toString(const OdDb::LineWeight                    val);
OdString toString(const OdDb::MeasurementValue              val);
OdString toString(const OdDb::OsnapMode                     val);
OdString toString(const OdDb::Planarity                     val);
OdString toString(const OdDb::PlotStyleNameType             val);
OdString toString(const OdDb::ProxyImage                    val);
OdString toString(const OdDb::RenderMode                    val);
OdString toString(const OdDb::SubentType                    val);
OdString toString(const OdDb::TextHorzMode                  val);
OdString toString(const OdDb::TextVertMode                  val);
OdString toString(const OdDb::UnitsValue                    val);
OdString toString(const OdDb::Vertex2dType                  val);
OdString toString(const OdDb::Vertex3dType                  val);
OdString toString(const OdDb::Visibility                    val);
OdString toString(const OdDb::XrefStatus                    val);
OdString toString(const OdDbBlockTableRecord::BlockScaling  val);
OdString toString(const OdDbDate&                           val);
//OdString toString(const OdDbDimAssoc::Associativity         val);
//OdString toString(const OdDbDimAssoc::RotatedType           val);
OdString toString(const OdDbHandle&                         val);
OdString toString(const OdDbHatch::HatchObjectType          val);
OdString toString(const OdDbHatch::HatchPatternType         val);
OdString toString(const OdDbHatch::HatchStyle               val);
OdString toString(const OdDbMText::AttachmentPoint          val);
OdString toString(const OdDbMText::FlowDirection            val);
OdString toString(const OdDbObjectId                        val);
OdString toString(const OdDbOle2Frame::Type                 val);
OdString toString(const OdDbPlotSettings::ShadePlotType     val);
OdString toString(const OdDbPolyline::SegType               val);
OdString toString(const OdDbRasterImage::ClipBoundaryType   val);
OdString toString(const OdDbViewport::ShadePlotType         val);
OdString toString(const OdGe::EntityId                      val);
OdString toString(const OdGePoint2d&                        val);
OdString toString(const OdGePoint3d&                        val);
OdString toString(const OdGeScale2d&                        val);
OdString toString(const OdGeScale3d&                        val);
OdString toString(const OdGeVector2d&                       val);
OdString toString(const OdGeVector3d&                       val);
OdString toString(const OdGeVector3d*                      pVal);
OdString toString(const OdGiArcType                         val);
OdString toString(const OdGiFillType                        val);
OdString toString(const OdGiOrientationType                 val);
OdString toString(const OdGiPolyline::SegType               val);
OdString toString(const OdGiRasterImage::Units              val);
OdString toString(const OdGsView::RenderMode                val); 
OdString toString(const OdRxClass*                          val);
OdString toString(const OdString&                           val);
OdString toString(const OdChar*                             val);
OdString toString(const OdString& fmt, double               val);
OdString toString(const OdString& fmt, int                  val);
OdString toString(const bool                                val);
OdString toString(const OdDbLeader::AnnoType                val);
OdString toString(const double                              val);
OdString toString(const int                                 val);
OdString toString(const OdAve::MaterialType                 val);
OdString toRGBString(const OdUInt32&                        val);
OdString toString(const OdAveGraniteMaterial::GraniteColor  val);
OdString toString(const OdGsLight::LightType                val);

#endif
