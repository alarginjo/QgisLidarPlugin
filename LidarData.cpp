#include <fstream>  
#include <iostream> 

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QTextStream>

#include <qgsrectangle.h>
#include <qgsgeometry.h>
#include <qgspoint.h>

#include "gdal_priv.h"
#include "cpl_conv.h" 
#include "cpl_string.h"
#include <ogr_spatialref.h>

#include "LidarData.h"


LidarData::LidarData():
	mLidarPoints( 0 ), 
	mMetadata(), 
	mFileName(),
	mGridIndex( NULL ), 
	mProfile(),
	mPointDensity( 0 )
{
}

LidarData::~LidarData()
{
	delete mGridIndex;
}

void LidarData::load(const QString& theFileName, QProgressDialog* p) 
{
	try 
	{
		//Take the name of LAS file
		mFileName = QString(theFileName);
		//Open the LAS file 
		std::ifstream ifs;
		ifs.open(mFileName.toStdString().c_str(), std::ios::in | std::ios::binary);
		liblas::ReaderFactory f;
		liblas::Reader reader = f.CreateWithStream(ifs);
		//Get the number of point in the file
		mMetadata.setFromLasHeader(reader.GetHeader());
		//Reserve the memory for the number of points in the LAS file
		quint32 numOfPoints = (quint32) mMetadata.numOfPoints();
		mLidarPoints.reserve(numOfPoints);
		//Create grid index
		mGridIndex = new GridIndex();
		QgsRectangle gridExtent(mMetadata.minX(), mMetadata.minY(),
								mMetadata.maxX(), mMetadata.maxY());
		mGridIndex->init(gridExtent, 1);
		//Read points and populate grid index
		quint32 pIndex = 0;

		//calculate step for progress dialog 
		unsigned int step = numOfPoints / 100;
		if ( p )
		{
			p->setMaximum( numOfPoints );
		}
		while ( reader.ReadNextPoint() )
		{
			pIndex = mLidarPoints.size();
			LidarPoint newLidarPoint;
			newLidarPoint.setFromLasPoint( reader.GetPoint() );
			mLidarPoints.append( newLidarPoint );
			mGridIndex->insertPoint( mLidarPoints, pIndex );
			if ( p && pIndex % step == 0 )
			{
				p->setValue( pIndex );
				if ( p->wasCanceled() )
				{
					mLidarPoints.clear();
					break;
				}
			}
		}
		if ( p )
		{
			p->setValue( numOfPoints );
		}

		calcPointDensity();

	} 
	catch (...)
	{
		throw;
	}
}

const QVector<LidarPoint>& LidarData::points() const
{
	return mLidarPoints;
}

LidarMetadata& LidarData::metadata()
{
	return mMetadata;
}

QString const& LidarData::fileName() const
{
	return mFileName;
}

const GridIndex * LidarData::gridIndex() const
{
	return mGridIndex;
}

double LidarData::pointDensity() const
{
	return mPointDensity;
}

void LidarData::printElevations(const QString fileName) const
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return;
	QTextStream out(&file);
	for(quint32 i=0; i < mGridIndex->cols(); i++)
		for(quint32 j=0; j < mGridIndex->rows(); j++)
		{
			out << "[" << j << ", " << i << "]:";
			const QList<quint32>& list = (mGridIndex->indices()[mGridIndex->index(j,i)]);
			if(list.isEmpty())
				out << "0 ";
			else
				out << mLidarPoints.at(list.last()).z() << " ";
			out << "\n";
		}

	file.close();
}

void LidarData::createLasFile() const
{
	std::ofstream ofs;
	ofs.open("D:\\Temp\\proba2.las", std::ios::out | std::ios::binary);
	liblas::Header header;
	header.SetDataFormatId(liblas::ePointFormat1); // Time only

	// Set coordinate system using GDAL support
	liblas::SpatialReference srs;
	srs.SetFromUserInput("EPSG:4326");

	header.SetSRS(srs);

	header.SetPointRecordsCount(100);

	liblas::Writer writer(ofs, header);
	// here the header has been serialized to disk into the *file.las*

	for(unsigned int i=0; i<100; i++)
	{
		liblas::Point point;
		point.SetCoordinates(19.0 + 0.01 * i, 45.0 + 0.01 * i, 100.0 + 0.01 * i);
		// fill other properties of point record

		writer.WritePoint(point);
	}

}

QString LidarData::_createRaster() const
{ 
	QString result;

	const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;

	GDALAllRegister();

    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    if( poDriver != NULL )
	{
		papszMetadata = poDriver->GetMetadata();
		if( CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
		{
			QFileInfo myFileInfo(mFileName);
			QString destFilename = myFileInfo.dir().absolutePath();
			destFilename.append("/"); 
			destFilename.append(myFileInfo.baseName());
			destFilename.append(".tif");
			result = destFilename;

			GDALDataset *poDstDS;       
			char **papszOptions = NULL;

			char * pszDstFilename = "D:/Temp/test1.tif"; //destFilename.toLocal8Bit().data();

			/*poDstDS = poDriver->Create( pszDstFilename, mGridIndex->cols(), mGridIndex->rows(), 1, GDT_Float32, 
										papszOptions );*/

			poDstDS = poDriver->Create( pszDstFilename, 100, 100, 1, GDT_Float32, 
										papszOptions );

			double adfGeoTransform[6] = { mGridIndex->extent().xMinimum(), mGridIndex->cellSize(), 0, mGridIndex->extent().yMaximum(), 0, - mGridIndex->cellSize() };
			OGRSpatialReference oSRS;
			char *pszSRS_WKT = NULL;
			GDALRasterBand *poBand;
	

			poDstDS->SetGeoTransform( adfGeoTransform );
		    
			pszSRS_WKT = mMetadata.crs()->toWkt().toLocal8Bit().data();
			poDstDS->SetProjection( pszSRS_WKT );
			CPLFree( pszSRS_WKT );

			/*float * abyRaster = new float[mGridIndex->cols()*mGridIndex->rows()];
			for(quint32 i=0; i<mGridIndex->rows(); i++)
				for(quint32 j=0; j<mGridIndex->cols(); j++)
					if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
						abyRaster[i*mGridIndex->cols()+j] = (float) mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).z();
					else
						abyRaster[i*mGridIndex->cols()+j] = 0;*/

			float * abyRaster = new float[100*100];
			for(int i=0; i<100; i++)
				for(int j=0; j<100; j++)
					//if (mGridIndex->indices()[i*100+j].size() > 0)
						abyRaster[i*100+j] = (float) 100;
					//else
						//abyRaster[i*100+j] = 0;

			poBand = poDstDS->GetRasterBand(1);
			/*poBand->RasterIO( GF_Write, 0, 0, mGridIndex->cols(), mGridIndex->rows(), 
							  abyRaster, mGridIndex->cols(), mGridIndex->rows(), GDT_Float32, 0, 0 ); */ 
			poBand->RasterIO( GF_Write, 0, 0, 100, 100, 
							  abyRaster, 100, 100, GDT_Float32, 0, 0 );

			delete [] abyRaster;

			/* Once we're done, close properly the dataset */
			GDALClose( (GDALDatasetH) poDstDS );
		}
	}
	
	return result;
}

QString LidarData::createRaster() const
{
	QString result;
	const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    char **papszMetadata;

	GDALAllRegister();

    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    if( poDriver == NULL )
        return result;

    papszMetadata = poDriver->GetMetadata();
    if( !CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
        return result;

	GDALDataset *poDstDS;       
    char **papszOptions = NULL;

	QFileInfo myFileInfo(mFileName);
	QString destFilename = myFileInfo.dir().absolutePath();
	if ( !destFilename.endsWith("/") )
		destFilename.append("/"); 
	destFilename.append(myFileInfo.baseName());
	destFilename.append(".tif");
	char * pszDstFilename = new char[destFilename.size() + 1];
	strcpy(pszDstFilename, destFilename.toLocal8Bit().data());
	result = destFilename;

    poDstDS = poDriver->Create( pszDstFilename, mGridIndex->cols(), mGridIndex->rows(), 1, GDT_Float32, 
										papszOptions );
	delete pszDstFilename;

	double adfGeoTransform[6] = { mGridIndex->extent().xMinimum(), mGridIndex->cellSize(), 0, mGridIndex->extent().yMaximum(), 0, - mGridIndex->cellSize() };
    OGRSpatialReference oSRS;
    GDALRasterBand *poBand;

	float * abyRaster = new float[mGridIndex->cols()*mGridIndex->rows()];
	for(quint32 i=0; i<mGridIndex->rows(); i++)
		for(quint32 j=0; j<mGridIndex->cols(); j++)
			if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
				abyRaster[i*mGridIndex->cols()+j] = (float) mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).z();
			else
				abyRaster[i*mGridIndex->cols()+j] = 0;

    poDstDS->SetGeoTransform( adfGeoTransform );
	
	char *pszSRS_WKT = new char[mMetadata.crs()->toWkt().size() + 1];
	strcpy(pszSRS_WKT, mMetadata.crs()->toWkt().toLocal8Bit().data());
    poDstDS->SetProjection( pszSRS_WKT );
    delete pszSRS_WKT;

    poBand = poDstDS->GetRasterBand(1);
    poBand->RasterIO( GF_Write, 0, 0, mGridIndex->cols(), mGridIndex->rows(), 
							  abyRaster, mGridIndex->cols(), mGridIndex->rows(), GDT_Float32, 0, 0 );  

	delete [] abyRaster;

    /* Once we're done, close properly the dataset */
    GDALClose( (GDALDatasetH) poDstDS );

	return result;
	
}

QVector<quint32> LidarData::pointsInPolygonV1(const QgsPolygon& thePolygon) const
{
	QVector<quint32> result;
	
	//bounding box of the polygon in world coords
	//qreal minx, maxx, miny, maxy;

	//bounding box of the polygon in grid coords
	qint32 minCol, maxCol, minRow, maxRow;

	//indicator that polygon is at least partialy inside grid extents
	//bool inGrid = false;

	if ( !thePolygon.isEmpty() )
	{
		//prepare vertices according to point-in-poly algorithm 
		//http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html#Listing the Vertices
		//QVector<QgsPoint> vertices;
		//vertices.append(QgsPoint(0,0));

		if( !thePolygon[0].isEmpty() )
		{
			//minx = thePolygon[0][0].x();
			//maxx = thePolygon[0][0].x();
			//miny = thePolygon[0][0].y();
			//maxy = thePolygon[0][0].y();
		

		/*for( quint32 i=0; i < thePolygon.size(); i++)
		{
			for ( quint32 j=0; j < thePolygon[i].size(); j++)
			{
				vertices.append(thePolygon[i][j]);

				if (thePolygon[i][j].x() < minx)
					minx = thePolygon[i][j].x();
				else
					if (thePolygon[i][j].x() > maxx)
						maxx = thePolygon[i][j].x();
				if (thePolygon[i][j].y() < miny)
					miny = thePolygon[i][j].y();
				else
					if (thePolygon[i][j].y() > maxy)
						maxy = thePolygon[i][j].y();

				if ( thePolygon[i][j].x() >= mGridIndex->extent().xMinimum() && 
						thePolygon[i][j].x() <= mGridIndex->extent().xMaximum() &&
						thePolygon[i][j].y() >= mGridIndex->extent().yMinimum() &&
						thePolygon[i][j].y() <= mGridIndex->extent().yMaximum() )
				{
					inGrid = true;
				}
			}
			vertices.append(QgsPoint(0,0));
		}*/
			QgsGeometry * polygonGeom = QgsGeometry::fromPolygon( thePolygon );
			QgsRectangle polygonBBox = polygonGeom->boundingBox();

			if ( polygonGeom->intersects( mGridIndex->extent() ) )
			{
				//find minimal bounding box of outer polygon in grid coords
				minCol = mGridIndex->x2col( polygonBBox.xMinimum() );
				if ( minCol < 0 )
					minCol = 0;
				QgsDebugMsg( QString::number( minCol ) );
				maxCol = mGridIndex->x2col( polygonBBox.xMaximum() );
				if ( maxCol > mGridIndex->cols() )
					maxCol = mGridIndex->cols();
				QgsDebugMsg( QString::number( maxCol ) );
				minRow = mGridIndex->y2row( polygonBBox.yMaximum() );
				if ( minRow < 0 )
					minRow = 0;
				QgsDebugMsg( QString::number( minRow ) );
				maxRow = mGridIndex->y2row( polygonBBox.yMinimum() );
				if ( maxRow > mGridIndex->rows() )
					maxRow = mGridIndex->rows();
				QgsDebugMsg( QString::number( maxRow ) );

				for( quint32 i=minRow; i < maxRow; i++)
					for( quint32 j=minCol; j <maxCol; j++)
					{
						QgsRectangle cell;
						cell.setXMinimum( mGridIndex->extent().xMinimum() + j * mGridIndex->cellSize() );
						cell.setXMaximum( mGridIndex->extent().xMinimum() + (j + 1) * mGridIndex->cellSize() );
						cell.setYMinimum( mGridIndex->extent().yMaximum() - (i + 1) * mGridIndex->cellSize() );
						cell.setYMaximum( mGridIndex->extent().yMaximum() - i * mGridIndex->cellSize() );
						if( polygonGeom->intersects( cell ) )
						{
							for( quint32 k=0; k < mGridIndex->indices()[mGridIndex->index(i,j)].size(); k++)
							{
								if( mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).inPolygon( polygonGeom ) )//( mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).inPolygon( vertices ) )
								{
									//QgsDebugMsg("\nPoint: (" + QString::number(mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).x())
									//	+ ", " + QString::number(mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).y()) 
									//	+ ", " + QString::number(mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).z() ));
									result.append(mGridIndex->indices()[mGridIndex->index(i,j)].at(k));
								}
							}
						}
					}

				delete polygonGeom;

			}
		}
	}

	return result;
	
}

QVector<quint32> LidarData::pointsInPolygonV2(const QgsPolygon& thePolygon) const
{
	QVector<quint32> result;
	
	//bounding box of the polygon in world coords
	qreal minx, maxx, miny, maxy;

	//bounding box of the polygon in grid coords
	qint32 minCol, maxCol, minRow, maxRow;

	//indicator that polygon is at least partialy inside grid extents
	//bool inGrid = false;

	if ( !thePolygon.isEmpty() )
	{
		//prepare vertices according to point-in-poly algorithm 
		//http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html#Listing the Vertices
		QVector<QgsPoint> vertices;
		vertices.append(QgsPoint(0,0));

		if( !thePolygon[0].isEmpty() )
		{
			minx = thePolygon[0][0].x();
			maxx = thePolygon[0][0].x();
			miny = thePolygon[0][0].y();
			maxy = thePolygon[0][0].y();
		

			for( quint32 i=0; i < thePolygon.size(); i++)
			{
				for ( quint32 j=0; j < thePolygon[i].size(); j++)
				{
					vertices.append(thePolygon[i][j]);

					/*if (thePolygon[i][j].x() < minx)
						minx = thePolygon[i][j].x();
					else
						if (thePolygon[i][j].x() > maxx)
							maxx = thePolygon[i][j].x();
					if (thePolygon[i][j].y() < miny)
						miny = thePolygon[i][j].y();
					else
						if (thePolygon[i][j].y() > maxy)
							maxy = thePolygon[i][j].y();

					if ( thePolygon[i][j].x() >= mGridIndex->extent().xMinimum() && 
							thePolygon[i][j].x() <= mGridIndex->extent().xMaximum() &&
							thePolygon[i][j].y() >= mGridIndex->extent().yMinimum() &&
							thePolygon[i][j].y() <= mGridIndex->extent().yMaximum() )
					{
						inGrid = true;
					}*/
				}
				vertices.append(QgsPoint(0,0));
			}
			QgsGeometry * polygonGeom = QgsGeometry::fromPolygon( thePolygon );
			QgsRectangle polygonBBox = polygonGeom->boundingBox();

			if ( polygonGeom->intersects( mGridIndex->extent() ) )
			{
				//find minimal bounding box of outer polygon in grid coords
				minCol = mGridIndex->x2col( polygonBBox.xMinimum() );
				if ( minCol < 0 )
					minCol = 0;
				QgsDebugMsg( QString::number( minCol ) );
				maxCol = mGridIndex->x2col( polygonBBox.xMaximum() );
				if ( maxCol > mGridIndex->cols() )
					maxCol = mGridIndex->cols();
				QgsDebugMsg( QString::number( maxCol ) );
				minRow = mGridIndex->y2row( polygonBBox.yMaximum() );
				if ( minRow < 0 )
					minRow = 0;
				QgsDebugMsg( QString::number( minRow ) );
				maxRow = mGridIndex->y2row( polygonBBox.yMinimum() );
				if ( maxRow > mGridIndex->rows() )
					maxRow = mGridIndex->rows();
				QgsDebugMsg( QString::number( maxRow ) );

				for( quint32 i=minRow; i < maxRow; i++)
					for( quint32 j=minCol; j <maxCol; j++)
					{
						QgsRectangle cell;
						cell.setXMinimum( mGridIndex->extent().xMinimum() + j * mGridIndex->cellSize() );
						cell.setXMaximum( mGridIndex->extent().xMinimum() + (j + 1) * mGridIndex->cellSize() );
						cell.setYMinimum( mGridIndex->extent().yMaximum() - (i + 1) * mGridIndex->cellSize() );
						cell.setYMaximum( mGridIndex->extent().yMaximum() - i * mGridIndex->cellSize() );
						if( polygonGeom->intersects( cell ) )
						{
							for( quint32 k=0; k < mGridIndex->indices()[mGridIndex->index(i,j)].size(); k++)
							{
								if( mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).inPolygon( vertices ) )//( mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).inPolygon( vertices ) )
								{
									//QgsDebugMsg("\nPoint: (" + QString::number(mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).x())
									//	+ ", " + QString::number(mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).y()) 
									//	+ ", " + QString::number(mLidarPoints.at(mGridIndex->indices()[mGridIndex->index(i,j)].at(k)).z() ));
									result.append(mGridIndex->indices()[mGridIndex->index(i,j)].at(k));
								}
							}
						}
					}

				delete polygonGeom;

			}
		}
	}

	return result;
}

void LidarData::createProfile( const QgsPolyline& theLine, const QgsPolygon& thePolygon, const QgsVector& theVector ) 
{
	QVector<quint32> pointsInside = pointsInPolygonV2( thePolygon );
	QgsDebugMsg("Profile line: \n From: ");
	QgsDebugMsg(QString::number(theLine[0].x()) + ", " + QString::number(theLine[0].y()));
	QgsDebugMsg(" To: ");
	QgsDebugMsg(QString::number(theLine[1].x()) + ", " + QString::number(theLine[1].y()));
	mProfile.createProfile( theLine, thePolygon, theVector, mLidarPoints, pointsInside );
}

const Profile& LidarData::profile() const
{
	return mProfile;
}

double ** LidarData::createElevationMatrix() const
{
	double ** raster = new double*[1];
	raster[0] = new double[mGridIndex->cols() * mGridIndex->rows()];
	for(quint32 i=0; i<mGridIndex->rows(); i++)
	{
		for(quint32 j=0; j<mGridIndex->cols(); j++)
		{
			if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
			{
				raster[0][i*mGridIndex->cols()+j] = mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).z();
			}
			else
			{
				raster[0][i*mGridIndex->cols()+j] = 0;
			}
		}
	}

	return raster;
}

unsigned char ** LidarData::createColorMatrix() const
{
	unsigned char ** raster = new unsigned char*[3];
	for( int i = 0; i < 3; i++)
	{
		raster[i] =	new unsigned char[mGridIndex->cols() * mGridIndex->rows()];
	}
	for(quint32 i=0; i<mGridIndex->rows(); i++)
	{
		for(quint32 j=0; j<mGridIndex->cols(); j++)
		{
			if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
			{
				raster[0][i*mGridIndex->cols()+j] = static_cast<unsigned char> 
													( mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).red() 
													/ 256 );
				raster[1][i*mGridIndex->cols()+j] = static_cast<unsigned char> 
													( mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).green() 
													/ 256 );
				raster[2][i*mGridIndex->cols()+j] = static_cast<unsigned char>
													( mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).blue() 
													/ 256 );
			}
			else
			{
				raster[0][i*mGridIndex->cols()+j] = 0;
				raster[1][i*mGridIndex->cols()+j] = 0;
				raster[2][i*mGridIndex->cols()+j] = 0;
			}
		}
	}

	return raster;
}

unsigned char ** LidarData::createClassificationMatrix() const
{
	unsigned char ** raster = new unsigned char*[3];
	for( int i = 0; i < 3; i++)
	{
		raster[i] =	new unsigned char[mGridIndex->cols() * mGridIndex->rows()];
	}
	for(quint32 i=0; i<mGridIndex->rows(); i++)
	{
		for(quint32 j=0; j<mGridIndex->cols(); j++)
		{
			if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
			{
				QColor color;
				switch( mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).classification() )
				{
					case 1: //Unclassified
						color = QColor( Qt::black );
						break;
					case 2: //Ground
						color = QColor( Qt::magenta );
						break;
					case 3: //Low vegetation
						color = QColor( Qt::green );
						break;
					case 4: //Medium vegetation
						color = QColor( Qt::darkGreen );
						break;
					case 5: //High vegetation
						color = QColor( Qt::yellow );
						break;
					case 6: //Buildings
						color = QColor( Qt::red );
						break;
					case 7: //Low points
						color = QColor( Qt::darkBlue );
						break;
					case 8: //Key points
						color = QColor( Qt::darkRed );
						break;
					case 9: //Water
						color = QColor( Qt::blue  );
						break;
					case 12: //Overlap
						color = QColor( Qt::cyan );
						break;
					default:
						color = QColor( Qt::white );
				}
				raster[0][i*mGridIndex->cols()+j] = color.red();
				raster[1][i*mGridIndex->cols()+j] = color.green();
				raster[2][i*mGridIndex->cols()+j] = color.blue();
			}
			else
			{
				raster[0][i*mGridIndex->cols()+j] = 0;
				raster[1][i*mGridIndex->cols()+j] = 0;
				raster[2][i*mGridIndex->cols()+j] = 0;
			}
		}
	}

	return raster;
}

unsigned short ** LidarData::createIntensityMatrix() const
{
	unsigned short ** raster = new unsigned short*[1];
	raster[0] = new unsigned short[mGridIndex->cols() * mGridIndex->rows()];
	for(quint32 i=0; i<mGridIndex->rows(); i++)
	{
		for(quint32 j=0; j<mGridIndex->cols(); j++)
		{
			if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
			{
				raster[0][i*mGridIndex->cols()+j] = mLidarPoints.at(mGridIndex->indices()[i*mGridIndex->cols()+j].last()).intensity();
			}
			else
			{
				raster[0][i*mGridIndex->cols()+j] = 0;
			}
		}
	}

	return raster;
}

void LidarData::calcPointDensity()
{
	int nNonEmptyCells = 0;
	for(quint32 i=0; i<mGridIndex->rows(); i++)
	{
		for(quint32 j=0; j<mGridIndex->cols(); j++)
		{
			if (mGridIndex->indices()[i*mGridIndex->cols()+j].size() > 0)
			{
				nNonEmptyCells++;
			}
		}
	}

	mPointDensity = mLidarPoints.size() / ( nNonEmptyCells * mGridIndex->cellSize() * mGridIndex->cellSize() );
}

void LidarData::createGridIndex( double theCellSize )
{
	delete mGridIndex;
	mGridIndex = new GridIndex();

	QgsRectangle gridExtent(mMetadata.minX(), mMetadata.minY(),
							mMetadata.maxX(), mMetadata.maxY());
	mGridIndex->init( gridExtent, theCellSize );

	for ( unsigned int i=0; i < mLidarPoints.size(); i++ )
	{
		mGridIndex->insertPoint( mLidarPoints , i );
	}
 	
}

