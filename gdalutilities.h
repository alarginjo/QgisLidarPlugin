/**/

#include <QString>
#include <QMap>

#include "qgscoordinatereferencesystem.h"

#include "gdal.h"

class GDALUtilities 
{
public:
	static void init();
	static bool GDALUtilities::createRasterFile(QString& theFilename, 
											QString& theFormat,  
											GDALDataType theType, 
											int theBands, 
											int theRows, 
											int theCols, 
											void ** theData,
											double * theGeoTransform, 
											const QgsCoordinateReferenceSystem * theCrs,
											double theNodataValue = 0);
private:
	///list of formats that can be created by gdal
	QMap<QString, QString> mSupportedFormats;
};