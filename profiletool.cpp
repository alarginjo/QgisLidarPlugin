/***************************************************************************
qgsmaptoolselectpolygon.cpp  -  map tool for selecting features by polygon
---------------------
begin                : May 2010
copyright            : (C) 2010 by Jeremy Palmer
email                : jpalmer at linz dot govt dot nz
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include "qgsgeometry.h"
#include "qgsrubberband.h"
#include "qgsmapcanvas.h"
#include "qgis.h"

#include <QMouseEvent>

#include "profiletool.h"


ProfileTool::ProfileTool( QgsMapCanvas* canvas )
    : QgsMapTool( canvas ), mRubberBand( NULL ), mLine( NULL ), mPolygon( NULL ), mOffsetVector( 0,0 )
{
}

ProfileTool::~ProfileTool()
{
  delete mRubberBand;
  delete mLine;
  delete mPolygon;
}

void ProfileTool::canvasPressEvent( QMouseEvent * e )
{
  if ( mRubberBand == NULL )
  {
    mRubberBand = new QgsRubberBand( mCanvas, false );
  }
  if ( e->button() == Qt::LeftButton )
  {
    if ( mRubberBand->numberOfVertices() < 2)
	{
		mRubberBand->addPoint( toMapCoordinates( e->pos() ) );
	}
	else //number of vertices greater than 0
	{
		if ( mRubberBand->numberOfVertices() == 2)
		{
			//save the created line
			mLine = mRubberBand->asGeometry();
			QgsPoint p1 = mLine->vertexAt(0);
			QgsPoint p2 = mLine->vertexAt(1);
			if( !(p1 == p2) ) 
			{
				//delete line rubber band
				delete mRubberBand;
				//create polygonal rubber band by offseting line on both sides
				mRubberBand = new QgsRubberBand( mCanvas, true );
				//calculate perpendicular vector
				QgsVector p1p2Vector( p2.x() - p1.x(), p2.y() - p1.y() );
				mOffsetVector = p1p2Vector.perpVector();
				mOffsetVector = mOffsetVector.normal() * 0.1;
				//construct the polygon of 0.2 units width in map coordinates
				QgsPoint p11( p1.x() + mOffsetVector.x(), p1.y() + mOffsetVector.y());
				mRubberBand->addPoint(p11  /*, false */);
				QgsPoint p21( p2.x() + mOffsetVector.x(), p2.y() + mOffsetVector.y());
				mRubberBand->addPoint(p21  /*, false */);
				QgsPoint p22( p2.x() - mOffsetVector.x(), p2.y() - mOffsetVector.y());
				mRubberBand->addPoint(p22  /*, false */);
				QgsPoint p12( p1.x() - mOffsetVector.x(), p1.y() - mOffsetVector.y());
				mRubberBand->addPoint(p12  /*, false */);
			}
			else
			{
				delete mLine;
				mLine = NULL;
			}
		}
	}
	
  }
  else
  {
    if ( mRubberBand->numberOfVertices() > 2 )
    {
      mPolygon = mRubberBand->asGeometry();
	  //TODO: do something
	  emit profileCreated(mLine, mPolygon, mOffsetVector);
	  delete mPolygon;
	  delete mLine;
	  mPolygon = 0;
	  mLine = 0;
    }
    mRubberBand->reset( true );
    delete mRubberBand;
    mRubberBand = NULL;
  }
}

void ProfileTool::canvasMoveEvent( QMouseEvent * e )
{
  if ( mRubberBand == NULL )
  {
    return;
  }
  if ( mRubberBand->numberOfVertices() <= 2 )
  {
    mRubberBand->removeLastPoint( 0 );
    mRubberBand->addPoint( toMapCoordinates( e->pos() ) );
  }
  else
  {
	QgsPoint cursorMapPos = toMapCoordinates( e->pos() );
	QgsVector p2CursorVector( cursorMapPos.x() - mLine->vertexAt(1).x(), cursorMapPos.y() - mLine->vertexAt(1).y() );
	if ( p2CursorVector.length() != 0 )
	{
		//new_offset_vector = offset_vector_normal * dot_product(line_to_cursor_vector, offset_vector_normal)
		mOffsetVector = mOffsetVector.normal() * ( p2CursorVector * mOffsetVector.normal() );
		mRubberBand->reset( true );
		QgsPoint p1 = mLine->vertexAt(0);
		QgsPoint p2 = mLine->vertexAt(1);
		QgsPoint p11( p1.x() + mOffsetVector.x(), p1.y() + mOffsetVector.y());
		mRubberBand->addPoint(p11 /*, false */);
		QgsPoint p21( p2.x() + mOffsetVector.x(), p2.y() + mOffsetVector.y());
		mRubberBand->addPoint(p21 /*, false */);
		QgsPoint p22( p2.x() - mOffsetVector.x(), p2.y() - mOffsetVector.y());
		mRubberBand->addPoint(p22 /*, false */);
		QgsPoint p12( p1.x() - mOffsetVector.x(), p1.y() - mOffsetVector.y());
		mRubberBand->addPoint(p12 /*, false */);
	}
  } 
}

