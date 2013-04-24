/***************************************************************************
  qgislidarplugin.cpp
  [plugindescription]
  -------------------
         begin                : [PluginDate]
         copyright            : [(C) Your Name and Date]
         email                : [Your Email]

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//
// QGIS Specific includes
//

#include <qgisinterface.h>
#include <qgisgui.h>
#include <qgsvectorlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsvectordataprovider.h>
#include <qgsgeometry.h>
#include <qgsgenericprojectionselector.h>
#include <qgsrasterlayer.h>
#include <qgsrasterinterface.h>
#include <qgsproviderregistry.h>
#include <qgsrasterblock.h>
#include "qgsgdalprovider.h"
#include <qgsmaptool.h>
#include <qgsmapcanvas.h>

//
// Qt4 Related Includes
//

#include <QAction>
#include <QToolBar>
#include <QCoreApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QFileInfo>
#include <QProgressDialog>

//
//LibLAS Includes
//
#include <liblas/liblas.hpp>
#include <liblas/point.hpp>

#include "qgislidarplugin.h"
#include "profiletool.h"
#include "gdalutilities.h"

static const QString sName = QObject::tr( "Lidar" );
static const QString sDescription = QObject::tr( "Displaying and processing of Lidar data" );
static const QString sCategory = QObject::tr( "Raster" );
static const QString sPluginVersion = QObject::tr( "Version 0.1" );
static const QgisPlugin::PLUGINTYPE sPluginType = QgisPlugin::UI;
static const QString sPluginIcon = ":/icons/lidar.png";

//////////////////////////////////////////////////////////////////////
//
// THE FOLLOWING METHODS ARE MANDATORY FOR ALL PLUGINS
//
//////////////////////////////////////////////////////////////////////

/**
 * Constructor for the plugin. The plugin is passed a pointer
 * an interface object that provides access to exposed functions in QGIS.
 * @param theQGisInterface - Pointer to the QGIS interface object
 */
QGisLidarPlugin::QGisLidarPlugin( QgisInterface * theQgisInterface ):
    QgisPlugin( sName, sDescription, sCategory, sPluginVersion, sPluginType ),
    mQGisIface( theQgisInterface ), 
	mLidarProfileGui( NULL ), 
	mLidarToolsGui( NULL ),
	mLidarMenu ( NULL ), 
	mQActionPointer( NULL ), 
	mActionProfile( NULL ), 
	mToolProfile( NULL ), 
	mLidarData ( NULL )
{
	GDALUtilities::init(  );
}

QGisLidarPlugin::~QGisLidarPlugin()
{

}

/*
 * Initialize the GUI interface for the plugin - this is only called once when the plugin is
 * added to the plugin registry in the QGIS application.
 */
void QGisLidarPlugin::initGui()
{

  //create Action
  /*if ( mQGisIface )
  {
    //find raster menu
    QString pluginText = QCoreApplication::translate( "QgisApp", "&Raster" );
    QMainWindow* mainWindow = qobject_cast<QMainWindow*>( mQGisIface->mainWindow() );
    if ( !mainWindow )
    {
      return;
    }

    QMenuBar* menuBar = mainWindow->menuBar();
    if ( !menuBar )
    {
      return;
    }

    QMenu* pluginMenu = 0;
    QList<QAction *> menuBarActions = menuBar->actions();
    QList<QAction *>::iterator menuActionIt =  menuBarActions.begin();
    for ( ; menuActionIt != menuBarActions.end(); ++menuActionIt )
    {
      if (( *menuActionIt )->menu() && ( *menuActionIt )->menu()->title() == pluginText )
      {
        pluginMenu = ( *menuActionIt )->menu();
        pluginMenu->addSeparator();
        break;
      }
    }

    if ( !pluginMenu )
    {
      return;
    }

    mLidarMenu = new QMenu( tr( "Lidar" ), pluginMenu );
    mLidarMenu->setIcon( QIcon( ":/qgislidarplugin/lidar.png" ) );
    mLidarMenu->addAction( tr( "Load Points" ), this, SLOT( loadLidarPoints(lidarData) ) );
    mLidarMenu->addAction( tr( "Test Action" ), this, SLOT( test() ) );
    // mLidarMenu->addAction( tr( "Hillshade" ), this, SLOT( hillshade() ) );
    // mLidarMenu->addAction( tr( "Relief" ), this, SLOT( relief() ) );
    // mLidarMenu->addAction( tr( "Ruggedness index" ), this, SLOT( ruggedness() ) );
    pluginMenu->addMenu( mLidarMenu );
  }*/

	// Create the action for tool
	mQActionPointer = new QAction( QIcon(), tr( "Load Points" ), this );
	// Set the what's this text
	mQActionPointer->setWhatsThis( tr( "Loads Lidar points from LAS file." ) );
	// Connect the action to the run
	connect( mQActionPointer, SIGNAL( triggered() ), this, SLOT( loadLidarPoints() ) );
	mQGisIface->addPluginToMenu( tr( "&Lidar" ), mQActionPointer );

	// Create the action for tool
	mActionProfile = new QAction( QIcon(), tr( "Create Profile" ), this );
	// Set the what's this text
	mActionProfile->setWhatsThis( tr( "Create cross section profile of specified depth along specified line." ) );
	// Connect the action to the run
	connect( mActionProfile, SIGNAL( triggered() ), this, SLOT( setProfileTool() ) );
	mQGisIface->addPluginToMenu( tr( "&Lidar" ), mActionProfile );

	mToolProfile = new ProfileTool( mQGisIface->mapCanvas() );
	mToolProfile->setAction( mActionProfile );

	mLidarProfileGui = new QgsLidarProfileGui(mQGisIface, mQGisIface->mainWindow());
	mLidarProfileGui->show();
	mLidarProfileGui->setFocus();

	mLidarToolsGui = new QgsLidarToolsGui(mQGisIface, mQGisIface->mainWindow());
	mLidarToolsGui->show();
	mLidarToolsGui->setFocus();

	connect( mToolProfile, SIGNAL( profileCreated(QgsGeometry *, QgsGeometry *, QgsVector&) ), this, SLOT( displayProfile(QgsGeometry *, QgsGeometry *, QgsVector&) ) ); 

}

void QGisLidarPlugin::setProfileTool()
{
  mQGisIface->mapCanvas()->setMapTool( mToolProfile );
}

void QGisLidarPlugin::loadLidarPoints() 
{
	QMainWindow* mainWindow = qobject_cast<QMainWindow*>( mQGisIface->mainWindow() );
	try 
	{
		mLidarData = new LidarData();
		QString fileName = QFileDialog::getOpenFileName(0,
			tr("Open Lidar Points"), "Z:\\", tr("Las Files (*.las)"));

		if(fileName.endsWith(".las")) {
			QProgressDialog p("Loading points...", "Cancel", 0, 100, mainWindow);
			p.setWindowModality(Qt::WindowModal);
			mLidarData->load(fileName, &p);
			if( mLidarData->metadata().crs() == NULL || !mLidarData->metadata().crs()->isValid() )
			{
				QgsGenericProjectionSelector mySelector( mQGisIface->mainWindow() );
				QString authId = "EPSG:4326";
				mySelector.setSelectedAuthId( authId );
				if ( mySelector.exec() )
				{
					authId = mySelector.selectedAuthId();
				}
				mLidarData->metadata().setCrs(authId);
			}

			QgsRasterLayer * rasterLayer = createRasterLayer( QGisLidarPlugin::Elevation );
			QList< QgsMapLayer * > layerList = QList< QgsMapLayer * >();
			layerList.append(static_cast< QgsMapLayer * >( rasterLayer ));
			QgsMapLayerRegistry::instance()->addMapLayers(layerList);

			//set Lidar data to profile gui 
			if ( mLidarProfileGui )
				mLidarProfileGui->setLidarData( mLidarData );

			//set Lidar data to lidar tools gui
			if ( mLidarToolsGui )
				mLidarToolsGui->setLidarData( mLidarData );
		}
		else
			QMessageBox::warning(mainWindow, tr("Read Lidar data"), 
								tr("Only the .las files are curently supported."), 
								QMessageBox::Ok);
	}
	catch (std::exception const& e)
	{
		QMessageBox::warning(mainWindow, tr("Read Lidar data"), 
							tr(e.what()), 
							QMessageBox::Ok);
	}
}

QgsVectorLayer* QGisLidarPlugin::createQgsVectorLayer(quint32 thePointDensity)
{
	//Get the points
	QVector<LidarPoint> const& lidarPoints = mLidarData->points();

	//Get the coordinate system
	//Set up uri
	QString uri = QString("Point");
	QString crs("");
	if( !mLidarData->metadata().crs()->isValid() )
	{
		 QgsGenericProjectionSelector mySelector( mQGisIface->mainWindow() );
		 QString authId = "epsg";
		 long crsId = 4326;
		 mySelector.setSelectedAuthId( authId );
		 mySelector.setSelectedCrsId( crsId );
		 if ( mySelector.exec() )
		 {
		   authId = mySelector.selectedAuthId();
		   crsId = mySelector.selectedCrsId();
		 }
		 crs.append("crs=");
		 crs.append(authId);
		 crs.append(":");
		 crs.append(crsId);
		 //crs.append(QString::number(newEpsgId));
		 uri.append("?");
		 uri.append(crs);
	}
	else
	{
		crs.append("crs=proj4:");
		crs.append(mLidarData->metadata().crs()->toProj4());
		uri.append(QString("?"));
		uri.append(crs);
	}

//	mCrs.createFromString(crs);

	uri.append(QString("&field=z:double&field=intensity:integer&field=classification:integer"));
	uri.append(QString("&field=red:integer&field=green:integer&field=blue:integer"));
	//Set the layer name
	QString name = mLidarData->fileName().mid(mLidarData->fileName().lastIndexOf("/") + 1);
	//Create vector layer
	QgsVectorLayer* lidarLayer = new QgsVectorLayer(uri, name, "memory");
	QgsVectorDataProvider *pr = lidarLayer->dataProvider();
	QgsFeatureList fetList;
	QgsFeature * fet;
	//Add every Nth (N=pointDensity) point to vector layer 
	for(quint32 i=0; i < (quint32) lidarPoints.size(); i+=thePointDensity)
	{
		fet = lidarPoints.at(i).toQgsFeature();
		fetList.append(*fet);
	}
	pr->addFeatures( fetList );
	lidarLayer->updateExtents();
	return lidarLayer;
}

void QGisLidarPlugin::createQgsRasterLayer()
{
/*	const GridIndex * grid = mLidarData->gridIndex();

	double * geoTransform = new double[6];
	geoTransform[0] = grid->extent().xMinimum();
	geoTransform[1] = grid->cellSize();
	geoTransform[2] = 0;
	geoTransform[3] = grid->extent().yMaximum();
	geoTransform[4] = 0;
	geoTransform[5] = - grid->cellSize();

	QgsGdalProvider * gdalProvider = (QgsGdalProvider *) QgsProviderRegistry::instance()->provider("gdal", NULL);
	gdalProvider->setDataSourceUri("D:/Temp/gdal.tif");
	gdalProvider->create( tr("GTiff"), 1, QgsRasterBlock::Float32, grid->cols(), grid->rows(), 
						 geoTransform, *(mLidarData->metadata().crs()));
	
	float * gridData = new float[grid->rows() * grid->cols()];

	//Get the points
	QVector<LidarPoint> const& lidarPoints = mLidarData->points();

	QFile file("D:/Temp/elev.txt");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return;
	QTextStream out(&file);
	for(unsigned int i=0; i<grid->cols(); i++)
		for(unsigned int j=0; j<grid->rows(); j++)
		{
			const QList<quint32>& pIndices = grid->indices()[grid->index(j,i)];

			if (pIndices.isEmpty())
				gridData[grid->index(j,i)] = 0;
			else
			{
				out << lidarPoints.at(pIndices.last()).x() << " : " << lidarPoints.at(pIndices.last()).y() << " : " << lidarPoints.at(pIndices.last()).z() << "\n";
				gridData[grid->index(j,i)] = lidarPoints.at(pIndices.last()).z();
				out << "[" << j << "' " << i << "] :" << gridData[grid->index(j,i)] << "\n";
			}
		
		}
	file.close();

	gdalProvider->write(gridData, 1, grid->cols(), grid->rows(), 0, 0);

	delete [] gridData;

	rasterLayer = new QgsRasterLayer(gdalProvider->dataSourceUri(), 
									  mLidarData->fileName(),
									  gdalProvider->name());*/
}

void QGisLidarPlugin::displayProfile(QgsGeometry * theLine, QgsGeometry * thePolygon, QgsVector& theVector)
{
	if( mLidarData )
	{
		QgsPolyline line = theLine->asPolyline();

		QgsPolygon polygon = thePolygon->asPolygon();

		mLidarData->createProfile(line, polygon, theVector);

		mLidarProfileGui->setProfile();
	}

}

QgsRasterLayer * QGisLidarPlugin::loadRasterLayer( RasterLayerType theType )
{
/*	switch( theType )
	{
		case QGisLidarPlugin::Elevation:
			double * elevations = 
	}*/

	return NULL;
}

QgsRasterLayer * QGisLidarPlugin::createRasterLayer( RasterLayerType theType )
{
	QString filename = mLidarData->fileName();
	filename.chop( QFileInfo( filename ).suffix().size() + 1 );

	QString format = "GTiff";
	GDALDataType type;
	int bands = 0;
	int rows = mLidarData->gridIndex()->rows();
	int cols = mLidarData->gridIndex()->cols();
	double geotransform[6] = { mLidarData->gridIndex()->extent().xMinimum(), 
							mLidarData->gridIndex()->cellSize(), 
							0, 
							mLidarData->gridIndex()->extent().yMaximum(), 
							0, 
							- mLidarData->gridIndex()->cellSize() };
	QgsCoordinateReferenceSystem * crs = mLidarData->metadata().crs();
	void ** data = NULL;

	bool layerCreated = false;
	switch (theType) 
	{
	case QGisLidarPlugin::Elevation :
		type = GDT_Float64;
		bands = 1;
		filename.append("_elev.tif");
		data = reinterpret_cast<void **>( mLidarData->createElevationMatrix() );
		layerCreated = GDALUtilities::createRasterFile( filename, format, type, bands,
														rows, cols, data, geotransform, crs);
		delete [] data[0];
		delete [] data;
		break;
	case QGisLidarPlugin::Color :
		type = GDT_Byte;
		bands = 3;
		filename.append("_rgb.tif");
		data = reinterpret_cast<void **>( mLidarData->createColorMatrix() );
		layerCreated = GDALUtilities::createRasterFile( filename, format, type, bands,
														rows, cols, data, geotransform, crs);
		delete [] data[0];
		delete [] data[1];
		delete [] data[2];
		delete [] data;
		break;
	}

	QgsRasterLayer * rasterLayer = NULL;
	if ( layerCreated )
	{
		rasterLayer = new QgsRasterLayer( filename, QFileInfo( filename ).baseName() );
	}

	return rasterLayer;
}

// Unload the plugin by cleaning up the GUI
void QGisLidarPlugin::unload()
{
	if ( mQGisIface )
	{
		//delete mLidarMenu;
		delete mLidarProfileGui;
		delete mLidarToolsGui;
		delete mQActionPointer;
		delete mActionProfile;
		delete mToolProfile; //Looks like it is deleted by Qgis main window map canvas
		delete mLidarData;
	}
}


void QGisLidarPlugin::testCrs()
{

	if( mLidarData )
	{
		QFile file("C:/Temp/testcrs.txt");
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		out << "WKT: \n\t";
		out << mLidarData->metadata().numOfPoints() << "\n";
		out << mLidarData->metadata().crs()->toWkt() << "\n\n";
		out << "Proj4: \n\t";
		out << mLidarData->metadata().crs()->toProj4() << "\n\n";
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//  THE FOLLOWING CODE IS AUTOGENERATED BY THE PLUGIN BUILDER SCRIPT
//    YOU WOULD NORMALLY NOT NEED TO MODIFY THIS, AND YOUR PLUGIN
//      MAY NOT WORK PROPERLY IF YOU MODIFY THIS INCORRECTLY
//
//
//////////////////////////////////////////////////////////////////////////


/**
 * Required extern functions needed  for every plugin
 * These functions can be called prior to creating an instance
 * of the plugin class
 */
// Class factory to return a new instance of the plugin class
QGISEXTERN QgisPlugin * classFactory( QgisInterface * theQgisInterfacePointer )
{
  return new QGisLidarPlugin( theQgisInterfacePointer );
}
// Return the name of the plugin - note that we do not user class members as
// the class may not yet be insantiated when this method is called.
QGISEXTERN QString name()
{
  return sName;
}

// Return the description
QGISEXTERN QString description()
{
  return sDescription;
}

// Return the category
QGISEXTERN QString category()
{
  return sCategory;
}

// Return the type (either UI or MapLayer plugin)
QGISEXTERN int type()
{
  return sPluginType;
}

// Return the version number for the plugin
QGISEXTERN QString version()
{
  return sPluginVersion;
}

QGISEXTERN QString icon()
{
  return sPluginIcon;
}

// Delete ourself
QGISEXTERN void unload( QgisPlugin * thePluginPointer )
{
  delete thePluginPointer;
}
