#include <qgslogger.h>

#include "lidarmetadata.h"

LidarMetadata::LidarMetadata(): 
	mNumOfPoints(0), 
	mMinX(0), 
	mMaxX(0), 
	mMinY(0), 
	mMaxY(0), 
	mMinZ(0), 
	mMaxZ(0), 
	mPointClasses()
{
	mPointClasses.insert(0, "Created");
	mPointClasses.insert(1, "Unclassified");
	mPointClasses.insert(2, "Ground");
	mPointClasses.insert(3, "Low Vegetation");
	mPointClasses.insert(4, "Medium Vegetation");
	mPointClasses.insert(5, "High Vegetation");
	mPointClasses.insert(6, "Building");
	mPointClasses.insert(7, "Low Point");
	mPointClasses.insert(8, "Model Key Point");
	mPointClasses.insert(9, "Water");
	mPointClasses.insert(10, "Reserved");
	mPointClasses.insert(11, "Reserved");
	mPointClasses.insert(12, "Overlap Points");
	for(quint8 i=13; i<32; i++)
		mPointClasses.insert(i, "Reserved");

	mCrs = NULL;
}

LidarMetadata::~LidarMetadata()
{
	delete mCrs;
}

quint32 LidarMetadata::numOfPoints() const
{
	return mNumOfPoints;
}

void LidarMetadata::setNumOfPoints(const quint32 theNumOfPoints)
{
	mNumOfPoints = theNumOfPoints;
}

qreal LidarMetadata::minX() const
{
	return mMinX;
}

void LidarMetadata::setMinX(const qreal theMinX)
{
	mMinX = theMinX;
}

qreal LidarMetadata::maxX() const
{
	return mMaxX;
}

void LidarMetadata::setMaxX(const qreal theMaxX)
{
	mMaxX = theMaxX;
}

qreal LidarMetadata::minY() const
{
	return mMinY;
}

void LidarMetadata::setMinY(const qreal theMinY)
{
	mMinY = theMinY;
}

qreal LidarMetadata::maxY() const
{
	return mMaxY;
}

void LidarMetadata::setMaxY(const qreal theMaxY)
{
	mMaxY = theMaxY;
}

qreal LidarMetadata::minZ() const
{
	return mMinZ;
}

void LidarMetadata::setMinZ(const qreal theMinZ)
{
	mMinZ = theMinZ;
}

qreal LidarMetadata::maxZ() const
{
	return mMaxZ;
}

void LidarMetadata::setMaxZ(const qreal theMaxZ)
{
	mMaxZ = theMaxZ;
}

QgsCoordinateReferenceSystem * LidarMetadata::crs() const
{
	return mCrs;
}

void LidarMetadata::setCrs(const QString theCrsDesc)
{
	if (theCrsDesc != "")
		mCrs = new QgsCoordinateReferenceSystem(theCrsDesc);
}

void LidarMetadata::setFromLasHeader(liblas::Header const& theHeader)
{
	mNumOfPoints = theHeader.GetPointRecordsCount();
	mMinX = theHeader.GetMinX();
	mMaxX = theHeader.GetMaxX();
	mMinY = theHeader.GetMinY();
	mMaxY = theHeader.GetMaxY();
	mMinZ = theHeader.GetMinZ();
	mMaxZ = theHeader.GetMaxZ();
	QString crsDesc = QString::fromStdString(theHeader.GetSRS().GetProj4());
	if (crsDesc != "")
		mCrs = new QgsCoordinateReferenceSystem();
	//The caller should check if CRS is valid and if it is not, call projection selector
}

liblas::Header * LidarMetadata::createLasHeader() const
{
	liblas::Header * lasHeader = new liblas::Header();
	
	lasHeader->SetPointRecordsCount(mNumOfPoints);
	lasHeader->SetMin(mMinX, mMinY, mMinZ);
	lasHeader->SetMax(mMaxX, mMaxY, mMaxZ);

	liblas::SpatialReference lasSrs;
	lasSrs.SetProj4(mCrs->toProj4().toStdString());
	lasHeader->SetSRS(lasSrs);

	return lasHeader;
}

const QMap<quint8, QString>& LidarMetadata::pointClasses() const
{
	return mPointClasses;
}