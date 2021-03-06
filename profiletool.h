/***************************************************************************
 qgsmaptoolselectpolygon.h  -  map tool for selecting features by polygon
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

#ifndef PROFILETOOL_H
#define PROFILETOOL_H

#include "qgsmaptool.h"
#include "qgspoint.h"

class QgsMapCanvas;
class QgsRubberBand;
class QgsGeometry;


class ProfileTool :
	public QgsMapTool
{
	Q_OBJECT
public:
	ProfileTool(QgsMapCanvas * canvas);
	~ProfileTool();
	void canvasMoveEvent( QMouseEvent * e );
	void canvasPressEvent( QMouseEvent * e );

signals:
	void profileCreated(QgsGeometry * theLine, QgsGeometry * thePolygon, QgsVector& theVector);
    
private:
   
	QgsRubberBand* mRubberBand;
	QgsGeometry * mLine;
	QgsGeometry * mPolygon;
	QgsVector mOffsetVector;
};

#endif //PROFILETOOL_H
