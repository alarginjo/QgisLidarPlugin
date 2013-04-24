/***************************************************************************
     QgsLidarProfileGui.cpp
     --------------------------------------
    Date                 : Sun Dec 3 08:00:00 CET 2012
    Copyright            : (C) 2012 by Ivan Alargic
    Email                : alarginjo at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QSettings>
#include <QList>
#include <QIcon>

#include "qgisinterface.h"
#include "qgsvectorlayer.h"
#include "qgsmapcanvas.h"
#include "qgsmaplayerregistry.h"
#include "qgsmaptoolzoom.h"
#include "qgsmaptoolpan.h"
#include "qgsapplication.h"
#include "qgscategorizedsymbolrendererv2.h"
#include "qgssymbolv2.h"

#include "LidarData.h"
#include "profile.h"
#include "qgslidarpointrgbrendererv2.h"

#include "qgslidarprofilegui.h"

QgsLidarProfileDockWidget::QgsLidarProfileDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags )
    : QDockWidget( title, parent, flags )
{
  setObjectName( "LidarProfileDockWidget" ); // set object name so the position can be saved
}

QgsLidarProfileGui::QgsLidarProfileGui( QgisInterface* theQgisInterface, QWidget* parent, Qt::WFlags fl )
    : QMainWindow( parent, fl )
    , mIface( theQgisInterface )
    , mDock( 0 )
	, mLidarData( 0 )
	, mLayer( 0 )
{
	setupUi( this );

	createActions();
	createMapTools();

	//set map canvas properties
	mMapCanvas->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	mMapCanvas->setCanvasColor( Qt::white );
	mMapCanvas->setMinimumWidth( 400 );
	mMapCanvas->mapRenderer()->setDestinationCrs( QgsCoordinateReferenceSystem("epsg:3857") );//probably redudant
	mMapCanvas->clearExtentHistory(); // reset zoomnext/zoomlast

	dockThisWindow( true );
}

void QgsLidarProfileGui::dockThisWindow( bool dock )
{
  if ( mDock )
  {
    setParent( mIface->mainWindow(), Qt::Window );
    show();

    mIface->removeDockWidget( mDock );
    mDock->setWidget( 0 );
    delete mDock;
    mDock = 0;
  }

  if ( dock )
  {
    mDock = new QgsLidarProfileDockWidget( tr( "Lidar Profile" ), mIface->mainWindow() );
    mDock->setWidget( this );
    mIface->addDockWidget( Qt::BottomDockWidgetArea, mDock );
  }
}

QgsLidarProfileGui::~QgsLidarProfileGui()
{

	removeOldLayer();

	delete mToolZoomIn;
	delete mToolZoomOut;
	delete mToolPan;
	delete mToolClassifyAbove;
	delete mToolClassifyBelow;
	delete mToolClassifyClose;

}

void QgsLidarProfileGui::removeOldLayer()
{
  // delete layer (and don't signal it as it's our private layer)
  if ( mLayer )
  {
    QgsMapLayerRegistry::instance()->removeMapLayers(
      ( QStringList() << mLayer->id() ), false );
    mLayer = NULL;
  }
  mMapCanvas->refresh();
}

void QgsLidarProfileGui::createActions()
{
	// Classification actions
	mActionClassifyAboveLine->setIcon( QIcon( ":/icons/class_above48.png" ) );
	connect( mActionClassifyAboveLine, SIGNAL( triggered() ), this, SLOT( setClassifyAboveTool() ) );

	mActionClassifyBelowLine->setIcon( QIcon( ":/icons/class_below48.png" ) );
	connect( mActionClassifyBelowLine, SIGNAL( triggered() ), this, SLOT( setClassifyBelowTool() ) );

	mActionClassifyCloseToLine->setIcon( QIcon( ":/icons/class_close48.png" ) );
	connect( mActionClassifyCloseToLine, SIGNAL( triggered() ), this, SLOT( setClassifyCloseTool() ) );

	// Move forward
	mActionMoveProfileForward->setIcon( QIcon( ":/icons/arrow_up.png" ) );
	connect( mActionMoveProfileForward, SIGNAL( triggered() ), this, SLOT( moveForward() ) );

	mActionMoveProfileBackward->setIcon( QIcon( ":/icons/arrow_down.png" ) );
	connect( mActionMoveProfileBackward, SIGNAL( triggered() ), this, SLOT( moveBackward() ) );

	// View actions 
	mActionPan->setIcon( QgsApplication::getThemeIcon( "/mActionPan.png" ) );
	connect( mActionPan, SIGNAL( triggered() ), this, SLOT( setPanTool() ) );

	mActionZoomIn->setIcon( QgsApplication::getThemeIcon( "/mActionZoomIn.png" ) );
	connect( mActionZoomIn, SIGNAL( triggered() ), this, SLOT( setZoomInTool() ) );

	mActionZoomOut->setIcon( QgsApplication::getThemeIcon( "/mActionZoomOut.png" ) );
	connect( mActionZoomOut, SIGNAL( triggered() ), this, SLOT( setZoomOutTool() ) );

	mActionZoomToLayer->setIcon( QgsApplication::getThemeIcon( "/mActionZoomToLayer.png" ) );
	connect( mActionZoomToLayer, SIGNAL( triggered() ), this, SLOT( zoomToLayerTool() ) );

	mActionZoomLast->setIcon( QgsApplication::getThemeIcon( "/mActionZoomLast.png" ) );
	connect( mActionZoomLast, SIGNAL( triggered() ), this, SLOT( zoomToLast() ) );

	mActionZoomNext->setIcon( QgsApplication::getThemeIcon( "/mActionZoomNext.png" ) );
	connect( mActionZoomNext, SIGNAL( triggered() ), this, SLOT( zoomToNext() ) );

}

void QgsLidarProfileGui::createMapTools()
{
	// set up map tools
	mToolZoomIn = new QgsMapToolZoom( mMapCanvas, false /* zoomIn */ );
	mToolZoomIn->setAction( mActionZoomIn );

	mToolZoomOut = new QgsMapToolZoom( mMapCanvas, true /* zoomOut */ );
	mToolZoomOut->setAction( mActionZoomOut );

	mToolPan = new QgsMapToolPan( mMapCanvas );
	mToolPan->setAction( mActionPan );

	mToolClassifyAbove = NULL;
	//mToolClassifyAbove = new QgsMapToolClassifyAbove( mMapCanvas, ... )
	//mToolClassifyAbove->setAction( mActionClassifyAboveLine );

	mToolClassifyBelow = NULL;
	//mToolClassifyBelow = new QgsMapToolClassifyBelow( mMapCanvas, ... )
	//mToolClassifyAbove->setAction( mActionClassifyBellowLine );

	mToolClassifyClose = NULL;
	//mToolClassifyAbove = QgsMapToolClassifyClose( mMapCanvas, ... )
	//mToolClassifyAbove->setAction( mActionClassifyCloseToLine );

	QSettings s;
	int action = s.value( "/qgis/wheel_action", 2 ).toInt();
	double zoomFactor = s.value( "/qgis/zoom_factor", 2 ).toDouble();
	mMapCanvas->setWheelAction(( QgsMapCanvas::WheelAction ) action, zoomFactor );

}

void QgsLidarProfileGui::setLidarData(LidarData * theLidarData)
{
	mLidarData = theLidarData;
}

void QgsLidarProfileGui::setProfile()
{
	removeOldLayer();

	mLayer = mLidarData->profile().vectorLayer();
	
	setLayerRendererByClass();

	// so layer is not added to legend
	QgsMapLayerRegistry::instance()->addMapLayers(
		QList<QgsMapLayer *>() << mLayer, false );
	// add layer to map canvas
	QList<QgsMapCanvasLayer> layers;
	layers.append( QgsMapCanvasLayer( mLayer ) );
	mMapCanvas->setLayerSet( layers );

	mMapCanvas->setExtent( mLayer->extent() );
	mMapCanvas->refresh();
	mIface->mapCanvas()->refresh();
	mMapCanvas->clearExtentHistory(); // reset zoomnext/zoomlast

	if( mDock )
	{
		if ( !mDock->isVisible() )
			show();
	}
	else
	{
		if( !isVisible() )
			show();
	}
}

// View slots
void QgsLidarProfileGui::setPanTool()
{
  mMapCanvas->setMapTool( mToolPan );
}

void QgsLidarProfileGui::setZoomInTool()
{
  mMapCanvas->setMapTool( mToolZoomIn );
}

void QgsLidarProfileGui::setZoomOutTool()
{
  mMapCanvas->setMapTool( mToolZoomOut );
}

void QgsLidarProfileGui::zoomToLayerTool()
{
  if ( mLayer )
  {
    mMapCanvas->setExtent( mLayer->extent() );
    mMapCanvas->refresh();
  }
}

void QgsLidarProfileGui::zoomToLast()
{
  mMapCanvas->zoomToPreviousExtent();
}

void QgsLidarProfileGui::zoomToNext()
{
  mMapCanvas->zoomToNextExtent();
}

//Classify slots
void QgsLidarProfileGui::setClassifyAboveTool()
{
	mMapCanvas->setMapTool( mToolClassifyAbove );
}

void QgsLidarProfileGui::setClassifyBelowTool()
{
	mMapCanvas->setMapTool( mToolClassifyBelow );
}

void QgsLidarProfileGui::setClassifyCloseTool()
{
	mMapCanvas->setMapTool( mToolClassifyClose );
}

//Move slots
void QgsLidarProfileGui::moveForward()
{
	if( mLidarData )
	{
	}
}

void QgsLidarProfileGui::moveBackward()
{
	if( mLidarData )
	{
	}
}

void QgsLidarProfileGui::setLayerRendererByClass()
{
	if( mLayer )
	{
		QgsCategoryList categories;
		QStringList colors = QColor::colorNames();
		QMapIterator<quint8, QString> iter( mLidarData->metadata().pointClasses() );
		while( iter.hasNext() )
		{
			iter.next();
			if( iter.value() != "Reserved" )
			{
				QgsStringMap properties;
				QColor color (255, 0, 255);
				//switch
				switch( iter.key() )
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

				properties["color"] = QString::number( color.red()) + ","
					+ QString::number( color.green()) + "," 
					+ QString::number( color.blue());
				properties["color_border"] = properties["color"];
				properties["size"] = "0.5";
				QgsMarkerSymbolV2 * symbol = QgsMarkerSymbolV2::createSimple( properties );
				QgsDebugMsg( symbol->type() );
				QgsDebugMsg( QString::number( symbol->size() ) );
				QgsRendererCategoryV2 category( iter.key(), symbol, iter.value() );
				categories.append( category );
			}
		}

		QgsFeatureRendererV2 * renderer = new QgsCategorizedSymbolRendererV2( "classification", categories );

		mLayer->setRendererV2( renderer );
	}
}

void QgsLidarProfileGui::setLayerRendererByColor()
{
	if( mLayer )
	{
		QgsFeatureRendererV2 * renderer = new QgsLidarPointRgbRendererV2();
		mLayer->setRendererV2( renderer );
	}
}