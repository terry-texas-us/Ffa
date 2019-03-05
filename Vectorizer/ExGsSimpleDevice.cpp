#include "..\\PegAeSys\\stdafx.h"

#include "..\\PegAeSys\\PegAEsysDoc.h"

#include "OdaCommon.h"

#include "DbObject.h"

#include "RxObjectImpl.h"

#include "ExGsSimpleDevice.h"
#include "GiDumperImpl.h"
#include "GiConveyorGeometryDumper.h"
#include "ColorMapping.h"
#include "toString.h"

ExGsSimpleDevice::ExGsSimpleDevice()
{
  /**********************************************************************/
  /* Set a palette with a white background.                             */
  /**********************************************************************/  
  setLogicalPalette(odcmAcadLightPalette(), 256);

  /**********************************************************************/
  /* Set the size of the vectorization window                           */
  /**********************************************************************/  
  onSize(OdGsDCRect(0,100,0,100));
}

OdGsDevicePtr ExGsSimpleDevice::createObject(DeviceType type)
{
	std::_tostream* target = &std::_tcout;

  OdGsDevicePtr pRes = OdRxObjectImpl<ExGsSimpleDevice, OdGsDevice>::createObject();
  ExGsSimpleDevice* pMyDev = static_cast<ExGsSimpleDevice*>(pRes.get());

  pMyDev->m_type = type;

  /**********************************************************************/
  /* Create the output geometry dumper                                  */
  /**********************************************************************/  
  pMyDev->m_pDumper = OdGiDumperImpl::createObject(target);
  
  /**********************************************************************/
  /* Create the destination geometry receiver                           */
  /**********************************************************************/  
  pMyDev->m_pDestGeometry = OdGiConveyorGeometryDumper::createObject(pMyDev->m_pDumper);

  return pRes;
}

/************************************************************************/
/* Creates a new OdGsView object, and associates it with this Device    */
/* object.                                                              */
/*                                                                      */
/* pInfo is a pointer to the Client View Information for this Device    */
/* object.                                                              */
/*                                                                      */
/* bEnableLayerVisibilityPerView specifies that layer visibility        */
/* per viewport is to be supported.                                     */
/************************************************************************/  
OdGsViewPtr ExGsSimpleDevice::createView(const OdGsClientViewInfo* pInfo, 
                                         bool bEnableLayerVisibilityPerView)
{
  /**********************************************************************/
  /* Create a View                                                      */
  /**********************************************************************/
  OdGsViewPtr pView = ExSimpleView::createObject();
  ExSimpleView* pMyView = static_cast<ExSimpleView*>(pView.get());

  /**********************************************************************/
  /* This call is required by DD 1.13+                                  */
  /**********************************************************************/
  pMyView->init(this, pInfo, bEnableLayerVisibilityPerView);

  /**********************************************************************/
  /* Directs the output geometry for the view to the                    */
  /* destination geometry object                                        */
  /**********************************************************************/
  pMyView->output().setDestGeometry(*m_pDestGeometry);

  return (OdGsView*)pMyView;
} 

/************************************************************************/
/* Returns the geometry receiver associated with this object.           */
/************************************************************************/  
OdGiConveyorGeometry* ExGsSimpleDevice::destGeometry()
{
  return m_pDestGeometry;
}

/************************************************************************/
/* Returns the geometry dumper associated with this object.             */
/************************************************************************/  
OdGiDumper* ExGsSimpleDevice::dumper()
{
  return m_pDumper;
}

void ExSimpleView::ownerDrawDc(const OdGePoint3d& origin,
                               const OdGeVector3d& u,
                               const OdGeVector3d& v,
                               const OdGiSelfGdiDrawable* /*pDrawable*/,
                               bool dcAligned,
                               bool allowClipping)
{
  // ownerDrawDc is not conveyor primitive. It means we must take all rendering processings
  // (transforms) by ourselves
  OdGeMatrix3d eyeToOutput = eyeToOutputTransform();
  OdGePoint3d originXformed(eyeToOutput * origin);
  OdGeVector3d uXformed(eyeToOutput * u), vXformed(eyeToOutput * v);

  OdGiDumper* pDumper = device()->dumper();

  pDumper->output(OD_T("ownerDrawDc"));
  
  // It's shown only in 2d mode.
  if(mode() == OdGsView::k2DOptimized)
  {
    pDumper->pushIndent();
    pDumper->output(OD_T("origin xformed"), toString(originXformed));
    pDumper->output(OD_T("u xformed"),      toString(uXformed));
    pDumper->output(OD_T("v xformed"),      toString(vXformed));
    pDumper->output(OD_T("dcAligned"),      toString(dcAligned));
    pDumper->output(OD_T("allowClipping"),  toString(allowClipping));
    pDumper->popIndent();
  }
}

/************************************************************************/
/* Called by the DWGdirect vectorization framework to update            */
/* the GUI window for this Device object                                */
/*                                                                      */
/* pUpdatedRect specifies a rectangle to receive the region updated     */
/* by this function.                                                    */
/*                                                                      */
/* The override should call the parent version of this function,        */
/* OdGsBaseVectorizeDevice::update().                                   */
/************************************************************************/
void ExGsSimpleDevice::update(OdGsDCRect* pUpdatedRect)
{
  OdGsBaseVectorizeDevice::update(pUpdatedRect);
}

/************************************************************************/
/* Called by each associated view object to set the current RGB draw    */
/* color.                                                               */
/************************************************************************/  
void ExGsSimpleDevice::draw_color(ODCOLORREF color)
{
  dumper()->output(OD_T("draw_color"), toRGBString(color));
}

/************************************************************************/
/* Called by each associated view object to set the current ACI draw    */
/* color.                                                               */
/************************************************************************/  
void ExGsSimpleDevice::draw_color_index(OdUInt16 colorIndex)
{
  dumper()->output(OD_T("draw_color_index"), toString(colorIndex));
}

/************************************************************************/
/* Called by each associated view object to set the current draw        */
/* lineweight and pixel width                                           */
/************************************************************************/  
void ExGsSimpleDevice::draw_lineWidth(OdDb::LineWeight weight, int pixelLineWidth)
{
  dumper()->output(OD_T("draw_lineWidth"));
  dumper()->pushIndent();
  dumper()->output(OD_T("weight"), toString(weight));
  dumper()->output(OD_T("pixelLineWidth"), toString(pixelLineWidth));
  dumper()->popIndent();
}


/************************************************************************/
/* Called by the DWGdirect vectorization framework to give the          */
/* client application a chance to terminate the current                 */
/* vectorization process.  Returning true from this function will       */
/* stop the current vectorization operation.                            */
/************************************************************************/

bool ExSimpleView::regenAbort() const
{
  // return true here to abort the vectorization process
  return false;  
}

/************************************************************************/
/* Called by the DWGdirect vectorization framework to vectorize         */
/* each entity in this view.  This override allows a client             */
/* application to examine each entity before it is vectorized.          */
/* The override should call the parent version of this function,        */
/* OdGsBaseVectorizeView::draw().                                       */
/************************************************************************/
void ExSimpleView::draw(const OdGiDrawable* pDrawable)
{
	OdDbObjectPtr pEnt = OdDbObject::cast(pDrawable);


  /**********************************************************************/
  /* The parent class version of this function must be called.          */
  /**********************************************************************/
  OdGsBaseVectorizeView::draw(pDrawable);
}

/************************************************************************/
/* Flushes any queued graphics to the display device.                   */
/************************************************************************/
void ExSimpleView::update()
{
  /**********************************************************************/
  /* If geometry receiver is a simplifier, we must re-set the draw      */
  /* context for it                                                     */
  /**********************************************************************/
  (static_cast<OdGiGeometrySimplifier*>(device()->destGeometry()))->setDrawContext(drawContext());

  /**********************************************************************/
  /* Comment these functions to get primitives in eye coordinates.      */
  /**********************************************************************/
  OdGeMatrix3d eye2Screen(eyeToScreenMatrix());
  OdGeMatrix3d screen2Eye(eye2Screen.inverse());
  setEyeToOutputTransform(eye2Screen);

  // perform viewport clipping in eye coordinates:

  /**********************************************************************/
  /* Perform viewport clipping in eye coordinates.                      */
  /**********************************************************************/
  m_eyeClip.m_bClippingFront = isFrontClipped();
  m_eyeClip.m_bClippingBack = isBackClipped();
  m_eyeClip.m_dFrontClipZ = frontClip();
  m_eyeClip.m_dBackClipZ = backClip();
  m_eyeClip.m_vNormal = viewDir();
  m_eyeClip.m_ptPoint = target();
  m_eyeClip.m_Points.clear();

  OdGeVector2d halfDiagonal(fieldWidth() / 2.0, fieldHeight() / 2.0);
  
  if(m_nrcCounts.size() == 1) // polygons aren't supported yet
  {
    // polygonal clipping
    int i;
    for(i = 0; i < m_nrcCounts[0]; i ++)
    {
      OdGePoint3d screenPt(m_nrcPoints[i].x, m_nrcPoints[i].y, 0.0);
      screenPt.transformBy(screen2Eye);
      m_eyeClip.m_Points.append(OdGePoint2d(screenPt.x, screenPt.y));
    }
  }
  else
  {
    // rectangular clipping
    m_eyeClip.m_Points.append(OdGePoint2d::kOrigin - halfDiagonal);
    m_eyeClip.m_Points.append(OdGePoint2d::kOrigin + halfDiagonal);
  }

  m_eyeClip.m_xToClipSpace = getWorldToEyeTransform();
  
  pushClipBoundary(&m_eyeClip);

  OdGsBaseVectorizeView::update();

  popClipBoundary();
} // end ExSimpleView::update()


/************************************************************************/
/* Notification function called by the vectorization framework          */
/* whenever the rendering attributes have changed.                      */
/*                                                                      */
/* Retrieves the current vectorization traits from DWGdirect (color     */
/* lineweight, etc.) and sets them in this device.                      */
/************************************************************************/
void ExSimpleView::onTraitsModified()
{
//	bool b = isEntityTraitsDataChanged();
	OdGsBaseVectorizeView::onTraitsModified();
	const OdGiSubEntityTraitsData& currTraits = effectiveTraits();
	if(currTraits.trueColor().isByColor())
	{
		device()->draw_color(ODTOCOLORREF(currTraits.trueColor()));
	}
	else
	{
		device()->draw_color_index(currTraits.color());
	}	

	OdDb::LineWeight lw = currTraits.lineWeight();
	device()->draw_lineWidth(lw, lineweightToPixels(lw));
} // end ExSimpleView::onTraitsModified()


void ExSimpleView::beginViewVectorization()
{
  OdGsBaseVectorizeView::beginViewVectorization();
  device()->setupSimplifier( &m_pModelToEyeProc->eyeDeviation() );
}

void ExGsSimpleDevice::setupSimplifier(const OdGiDeviation* pDeviation)
{
  m_pDestGeometry->setDeviation(pDeviation);
}


//==========================================================================================

