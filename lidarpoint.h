/***************************************************************************
  lidarpoint.h  -  Definition of Lidar point
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

#ifndef LIDARPOINT
#define LIDARPOINT

//
// Qt Includes
//
#include <QMap>
#include <QString>
#include <QColor>

//
// QGIS Includes
//
#include <qgsfeature.h>

#include "liblas/liblas.hpp"

class LidarPoint 
{
public:
	LidarPoint();
	~LidarPoint();
	qreal x() const;
	void setX(qreal theX);
	qreal y() const;
	void setY(qreal theY);
	qreal z() const;
	void setZ(qreal theZ);
	quint8 intensity() const;
	void setIntensity(quint8 theIntensity);
	quint8 classification() const;
	void setClassification(const quint8 theClassification);
	quint16 red() const;
	void setRed(quint16 theRed);
	quint16 green() const;
	void setGreen(quint16 theGreen);
	quint16 blue() const;
	void setBlue(quint16 theBlue);
	QColor color() const;
	QgsFeature * toQgsFeature() const;
	liblas::Point * toLasPoint() const;
	void setFromLasPoint(liblas::Point const& theLasPoint);

	bool LidarPoint::inPolygon(const QVector<QgsPoint>& theVertices) const;
	bool LidarPoint::inPolygon(QgsGeometry * thePolygon) const;
private:
	qreal mX;
	qreal mY;
	qreal mZ;
	quint16 mIntensity;
	quint8 mClassification;
	quint16 mRed;
	quint16 mGreen;
	quint16 mBlue;
};

#endif //LIDARPOINT