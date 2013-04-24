/***************************************************************************
  lidarmetadata.h  -  Metadata of Lidar data
                             -------------------
    begin                : October, 2012
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

#ifndef LIDARMETADATA
#define LIDARMETADATA

//
// Qt Includes
//
#include <QMap>
#include <QString>

//
// QGIS Includes
//
#include <qgscoordinatereferencesystem.h>

#include "liblas/liblas.hpp"

class LidarMetadata
{
public:
	LidarMetadata();
	~LidarMetadata();
	quint32 numOfPoints() const;
	void setNumOfPoints(const quint32 theNumOfPoints);
	qreal minX() const;
	void setMinX(const qreal theMinX);
	qreal maxX() const;
	void setMaxX(const qreal theMaxX);
	qreal minY() const;
	void setMinY(const qreal theMinY);
	qreal maxY() const;
	void setMaxY(const qreal theMaxY);
	qreal minZ() const;
	void setMinZ(const qreal theMinZ);
	qreal maxZ() const;
	void setMaxZ(const qreal theMaxZ);
	QgsCoordinateReferenceSystem * crs() const;
	void setCrs(const QString theCrsDesc);
	const QMap<quint8, QString>& pointClasses() const;
	void setPointClasses(QMap<quint8, QString> const& thePointClasses);
	void addPointClass(const quint8 theCode, const QString theDescription);
	void removePointClass(const quint8 theCode);
	void setFromLasHeader(liblas::Header const& theHeader);
	liblas::Header * createLasHeader() const;
private:
	quint32 mNumOfPoints;
	qreal mMinX;
	qreal mMaxX;
	qreal mMinY;
	qreal mMaxY;
	qreal mMinZ;
	qreal mMaxZ;
	QgsCoordinateReferenceSystem * mCrs;
	QMap<quint8, QString> mPointClasses;
	//TODO implement the methods for the scale and offset
	//qreal mScale;
	//qreal mOffset;
};

#endif //LIDARMETADATA