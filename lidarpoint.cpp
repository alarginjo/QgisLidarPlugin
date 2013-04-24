
#include <qgsgeometry.h>

#include "lidarpoint.h"

LidarPoint::LidarPoint(): mX(0), mY(0), mZ(0), mIntensity(0), mClassification(0),
	mRed(0), mGreen(0), mBlue(0)
{
}

LidarPoint::~LidarPoint()
{
}

qreal LidarPoint::x() const
{
	return mX;
}

void LidarPoint::setX(qreal theX)
{
	mX = theX;
}

qreal LidarPoint::y() const
{
	return mY;
}

void LidarPoint::setY(qreal theY)
{
	mY = theY;
}

qreal LidarPoint::z() const
{
	return mZ;
}

void LidarPoint::setZ(qreal theZ)
{
	mZ = theZ;
}

quint8 LidarPoint::intensity() const
{
	return mIntensity;
}

void LidarPoint::setIntensity(quint8 theIntensity)
{
	mIntensity = theIntensity;
}

quint8 LidarPoint::classification() const
{
	return mClassification;
}

void LidarPoint::setClassification(quint8 theClassification)
{
	mClassification = theClassification;
}

quint16 LidarPoint::red() const
{
	return mRed;
}

void LidarPoint::setRed(quint16 theRed)
{
	mRed = theRed;
}

quint16 LidarPoint::green() const
{
	return mGreen;
}

void LidarPoint::setGreen(quint16 theGreen)
{
	mGreen = theGreen;
}

quint16 LidarPoint::blue() const
{
	return mBlue;
}

void LidarPoint::setBlue(quint16 theBlue)
{
	mBlue = theBlue;
}

QColor LidarPoint::color() const
{
	return QColor(mRed >> 8, mGreen >> 8, mBlue >> 8);
}

QgsFeature * LidarPoint::toQgsFeature() const
{
	QgsFeature * fet = new QgsFeature();
	fet->setGeometry(QgsGeometry::fromPoint(QgsPoint(mX, mY)));
	QgsAttributeMap attMap;
	attMap[0] = QVariant(mZ);
	attMap[1] = QVariant(mIntensity);
	attMap[2] = QVariant(mClassification);
	attMap[3] = QVariant(mRed);
	attMap[4] = QVariant(mGreen);
	attMap[5] = QVariant(mBlue);
	fet->setAttributeMap( attMap );
	return fet;
}
liblas::Point * LidarPoint::toLasPoint() const
{
	liblas::Point * lasPoint = new liblas::Point();
	lasPoint->SetCoordinates(mX, mY, mZ);
	lasPoint->SetIntensity(mIntensity);
	lasPoint->SetClassification(mClassification);
	lasPoint->SetColor(liblas::Color(mRed, mGreen, mBlue));
	return lasPoint;
}

void LidarPoint::setFromLasPoint(liblas::Point const& theLasPoint)
{
	mX = theLasPoint.GetX();
	mY = theLasPoint.GetY();
	mZ = theLasPoint.GetZ();
	mIntensity = theLasPoint.GetIntensity();
	mClassification = theLasPoint.GetClassification().GetClass();
	mRed = theLasPoint.GetColor().GetRed();
	mGreen = theLasPoint.GetColor().GetGreen();
	mBlue = theLasPoint.GetColor().GetBlue();
}

bool LidarPoint::inPolygon(const QVector<QgsPoint>& theVertices) const
{
	quint32 i;
	quint32 j;
	bool result = false;
	for (i = 0, j = theVertices.size()-1; i < theVertices.size(); j = i++) 
	{
		if( ((theVertices[i].y() > mY) != (theVertices[j].y() > mY)) && 
			(mX < (theVertices[j].x() - theVertices[i].x()) * (mY - theVertices[i].y()) / (theVertices[j].y() - theVertices[i].y()) + theVertices[i].x()) )
				result = !result;
	}
	return result;
}

bool LidarPoint::inPolygon(QgsGeometry * thePolygon) const
{
	bool result;

	QgsGeometry * pointGeom = QgsGeometry::fromPoint( QgsPoint( mX, mY ) );
	result = pointGeom->within( thePolygon );
	delete pointGeom;

	return result;
}
