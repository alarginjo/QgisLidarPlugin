/***************************************************************************
  profile.cpp  -  profile of Lidar points along speciffied line
                             -------------------
    begin                : November, 2012
    copyright            : (C) 2012 by Ivan Alargic
    email                : alarginjo at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgslogger.h"
#include "qgsvectordataprovider.h"
#include "profile.h"

Profile::Profile() : mPointIndices(), 
	mVectLayer( NULL ),
	mLine(),
	mPolygon(),
	mVector()
{
}

void Profile::createProfile(const QgsPolyline& theLine, 
							const QgsPolygon& thePolygon,
							const QgsVector& theVector,
							const QVector<LidarPoint>& thePoints, 
							const QVector<quint32>& thePointIndices) 
{
	mLine = theLine;
	mPolygon = thePolygon;
	mVector = theVector;
	//2D CRS is selected only for display. Point coordinates are meaningless in that CRS.
	//QString uri = "Point?crs=epsg:3857&field=intensity:integer&field=classification:integer";
	//uri.append("&field=red:integer&field=green:integer&field=blue:integer");
	QString uri = "Point?crs=epsg:3857&field=intensity:integer&field=classification:integer";
	uri.append("&field=red:integer&field=green:integer&field=blue:integer");
	mVectLayer = new QgsVectorLayer(uri, "profile", "memory");

	QgsVectorDataProvider *pr = mVectLayer->dataProvider();

	QgsPoint startPoint = mLine[0];
	QgsPoint endPoint = mLine[1];

	QgsFeatureList fets;

	for(quint32 i=0; i < thePointIndices.size(); i++)
	{
		QgsFeature fet;
		qreal xy = sqrt(pow((thePoints[thePointIndices[i]].x() - startPoint.x()),2) +
			pow((thePoints[thePointIndices[i]].y() - startPoint.y()),2));
		//QgsDebugMsg( QString::number(xy) );
		qreal z = thePoints[thePointIndices[i]].z();
		if( z <= 50 ) 
			QgsDebugMsg( QString::number(z) );
		//QgsDebugMsg("\nProfile point: " + QString::number(xy) + ", " + QString::number(z));
		fet.setGeometry(QgsGeometry::fromPoint(QgsPoint( xy, z)));
		QgsAttributeMap attMap;
		attMap[0] = QVariant(thePoints[thePointIndices[i]].intensity());
		attMap[1] = QVariant(thePoints[thePointIndices[i]].classification());
		attMap[2] = QVariant(thePoints[thePointIndices[i]].red());
		attMap[3] = QVariant(thePoints[thePointIndices[i]].green());
		attMap[4] = QVariant(thePoints[thePointIndices[i]].blue());
		fet.setAttributeMap( attMap );
		fets.append( fet );
	}

	pr->addFeatures( fets );

	QgsDebugMsg("\nFeatures:" + QString::number( mVectLayer->featureCount() ));

	mVectLayer->updateExtents();
}

Profile::~Profile()
{
}

QgsVectorLayer * Profile::vectorLayer() const
{
	return mVectLayer;
}

const QgsPolyline& Profile::line() const
{
	return mLine;
}

const QgsPolygon& Profile::polygon() const
{
	return mPolygon;
}

const QgsVector& Profile::vector() const
{
	return mVector;
}