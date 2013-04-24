#include "gdal_priv.h"
#include "cpl_conv.h" 
#include "cpl_string.h"

#include "gdalutilities.h"

void GDALUtilities::init()
{
	GDALAllRegister();

//Maybe available in future
/*	int nDrivers = GDALGetDriverCount();
	for ( int i = 0; i < nDrivers; ++i )
	{
		GDALDriverH driver = GDALGetDriver( i );
		if ( driver != NULL )
		{
			char** driverMetadata = GDALGetMetadata( driver, NULL );
			if ( CSLFetchBoolean( driverMetadata, GDAL_DCAP_CREATE, false ) )
			{
				QString driverExtension = GDALGetMetadataItem( driver, GDAL_DMD_EXTENSION, NULL );
				mSupportedFormats.insert( QString( GDALGetDriverShortName( driver ) ), driverExtension );
			}
		}
	}*/
}

bool GDALUtilities::createRasterFile(QString& theFilename, 
									 QString& theFormat,  
									 GDALDataType theType, 
									 int theBands, 
									 int theRows, 
									 int theCols, 
									 void ** theData,
									 double * theGeoTransform, 
									 const QgsCoordinateReferenceSystem * theCrs,
									 double theNodataValue)
{
	if ( theBands <= 0 )
		return false;

	if ( theRows <= 0 )
		return false;

	if ( theCols <= 0 )
		return false;

	if ( !theData )
		return false;

/*	bool formatSupported = false;
	QMapIterator<QString, QString> i(mSupportedFormats);
	while (i.hasNext()) 
	{
		i.next();
		if( theFormat == i.key())
		{
			formatSupported = true;
			break;
		}
	}

	if ( !formatSupported )
		return false;

*/

	//GDALAllRegister();

	GDALDriver * driver;

	//set format
	char * format = new char[theFormat.size() + 1];
	strcpy( format, theFormat.toLocal8Bit().data() );

    driver = GetGDALDriverManager()->GetDriverByName(format);

	if( driver == NULL )
        return false;

	char ** metadata = driver->GetMetadata();
    if( !CSLFetchBoolean( metadata, GDAL_DCAP_CREATE, FALSE ) )
        return false;
		
	GDALDataset * dstDS;     

	//set options
	char ** options = NULL;
	options = CSLSetNameValue( options, "COMPRESS", "LZW" );

	//if it is a GeoTIFF format set correct compression options
	if ( !strcmp( format, "GTiff" ) )
	{
		if( theType == GDT_Byte )
		{
			options = CSLSetNameValue( options, "PREDICTOR", "1" );
		}
		else
		{
			if ( theType == GDT_UInt16 || theType == GDT_Int16  
				|| theType == GDT_UInt32 || theType == GDT_Int32 )
			{
				options = CSLSetNameValue( options, "PREDICTOR", "2" );
			} 
			else 
			{
				options = CSLSetNameValue( options, "PREDICTOR", "3" );
			}
		}
	}
	
	//set filename
	char * dstFilename = new char[theFilename.size() + 1];
	strcpy( dstFilename, theFilename.toLocal8Bit().data() );

	dstDS = driver->Create( dstFilename, theCols, theRows, theBands, theType, 
								options );
	delete dstFilename;
	delete [] options;

	//set geotransform
	dstDS->SetGeoTransform( theGeoTransform );

	//set CRS
	char * crsWkt = new char[theCrs->toWkt().size() + 1];
	strcpy( crsWkt, theCrs->toWkt().toLocal8Bit().data());
    dstDS->SetProjection( crsWkt );
    delete crsWkt;

	GDALRasterBand * band;

	for( int i=1; i <= theBands; i++ )
	{
		band = dstDS->GetRasterBand( i );
		band->SetNoDataValue( theNodataValue );
		band->RasterIO( GF_Write, 0, 0, theCols, theRows, theData[ i-1 ], 
						theCols, theRows, theType, 0, 0);
	}

	GDALClose( (GDALDatasetH) dstDS );

	return true;

}