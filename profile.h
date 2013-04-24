/***************************************************************************
  profile.h  -  profile of Lidar points along speciffied line
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

#ifndef PROFILE
#define PROFILE

#include <QVector>

#include "qgsgeometry.h"
#include "qgsvectorlayer.h"

#include "lidarpoint.h"

class Profile
{
public:
	Profile();
	~Profile();
	/**
	* Creates a profile of Lidar points along specified line.
	* \param theLine			: the centerline of profile ( it has starting and ending point )
	* \param thePolygon			: the area of profile
	* \param theVector			: the vector of profile
	* \param thePoints			: the Lidar points
	* \param thePointIndices	: the indices of points that are inside profile area 
	*/
	void createProfile(const QgsPolyline& theLine, 
		const QgsPolygon& thePolygon,
		const QgsVector& theVector,
		const QVector<LidarPoint>& thePoints, 
		const QVector<quint32>& thePointIndices);
	QgsVectorLayer * vectorLayer() const;
	//geters
	const QgsPolyline& line() const;
	const QgsPolygon& polygon() const;
	const QgsVector& vector() const;
	//Methods for selection of points based on height
	const QVector<quint32>& belowLine(const QgsPolyline& theLine) const;
	const QVector<quint32>& aboveLine(const QgsPolyline& theLine) const;
	const QVector<quint32>& nearLine(const QgsPolyline& theLine) const;
private:
	QVector<quint32> mPointIndices;
	QgsVectorLayer * mVectLayer;
	//!centerline of profile
	QgsPolyline mLine;
	//!area of profile
	QgsPolygon mPolygon;
	//!vector of profile
	/*!vector which starts at the end point of the centerline, is perpendicular
	* to centerline and have length equal to half of profile depth
	*/
	QgsVector mVector;
};

#endif PROFILE