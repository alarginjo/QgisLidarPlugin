//
//Qt includes
//
#include <QString>
#include <QVector>
#include <QProgressDialog>

//
//QGIS Includes
//
#include <qgsrasterlayer.h>

//
//LASlib includes
//

#include "liblas/liblas.hpp"

#include "lidarpoint.h"
#include "lidarmetadata.h"
#include "gridindex.h"
#include "profile.h"

class LidarData 
{
public:
	LidarData();
	~LidarData();
	/** Loads Lidar data from file and creates grid index with default cell size of 1m.
	* \param theFileName		: name of input file
	* \param p					: pointer to progress dialog
	*/
	void load(const QString& theFileName, QProgressDialog* p);
	const QVector<LidarPoint>& points() const;
	LidarMetadata& metadata();
	QString const& fileName() const;
	void recreateGridIndex( double theCellSize );
	const GridIndex * gridIndex() const;
	const Profile& profile() const;
	double pointDensity() const;
	//TODO create pyramid of vector layers
	
	//Create grid index
	void createGridIndex( double theCellSize );
	//Create raster
	QString createRaster() const;
	//Create profile
	void createProfile( const QgsPolyline& theLine, const QgsPolygon& thePolygon, const QgsVector& theVector );
	//Extract points inside polygon
	QVector<quint32> pointsInPolygonV1(const QgsPolygon& thePolygon) const;
	QVector<quint32> pointsInPolygonV2(const QgsPolygon& thePolygon) const;
	//Test
	void printElevations(const QString fileName) const;
	void createLasFile() const;
	QString _createRaster() const;

	//create data for raster display
	double ** LidarData::createElevationMatrix() const;
	unsigned char ** LidarData::createColorMatrix() const;
	unsigned char ** LidarData::createClassificationMatrix() const;
	unsigned short ** LidarData::createIntensityMatrix() const;
private:
	void calcPointDensity();

	QVector<LidarPoint> mLidarPoints;
	LidarMetadata mMetadata;
	QString mFileName;
	GridIndex * mGridIndex;
	Profile mProfile;
	double mPointDensity;
};

