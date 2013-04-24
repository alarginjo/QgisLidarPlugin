/***************************************************************************
    qgslidarplugin.h 

	Plugin for visualization and processing of Lidar data.

    -------------------
    begin                : Nov 1, 2012
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
/***************************************************************************
 *   QGIS Programming conventions:
 *
 *   mVariableName - a class level member variable
 *   sVariableName - a static class level member variable
 *   variableName() - accessor for a class member (no 'get' in front of name)
 *   setVariableName() - mutator for a class member (prefix with 'set')
 *
 *   Additional useful conventions:
 *
 *   theVariableName - a method parameter (prefix with 'the')
 *   myVariableName - a locally declared variable within a method ('my' prefix)
 *
 *   DO: Use mixed case variable names - myVariableName
 *   DON'T: separate variable names using underscores: my_variable_name (NO!)
 *
 * **************************************************************************/
#ifndef QGISLIDARPLUGIN_H
#define QGISLIDARPLUGIN_H

//QT4 includes
#include <QObject>

//QGIS includes
#include "../qgisplugin.h"
#include <qgscoordinatereferencesystem.h>

//forward declarations
class QgisInterface;
class QgsVectorLayer;
class QgsRasterLayer;
class QgsRubberBand;
class QMenu;
class QAction;
class QgsMapTool;

#include "LidarData.h"
#include "qgslidarprofilegui.h"
#include "qgslidartoolsgui.h"

/**
* \class QGisLidarPlugin
* \brief Lidar plugin for QGIS
* [description]
*/
class QGisLidarPlugin: public QObject, public QgisPlugin
{
    Q_OBJECT
  public:

	  enum RasterLayerType {
		  Elevation,
		  Color,
		  Intensity,
		  Classification,
		  Hillshade
	  };

    //////////////////////////////////////////////////////////////////////
    //
    //                MANDATORY PLUGIN METHODS FOLLOW
    //
    //////////////////////////////////////////////////////////////////////

    /**
    * Constructor for a plugin. The QgisInterface pointer is passed by
    * QGIS when it attempts to instantiate the plugin.
    * @param theInterface Pointer to the QgisInterface object.
     */
    QGisLidarPlugin( QgisInterface * theInterface );
    //! Destructor
    virtual ~QGisLidarPlugin();
	//! init the gui
	virtual void initGui();
    //! unload the plugin
    void unload();

	///create raster layer and insert in map
	QgsRasterLayer * loadRasterLayer( RasterLayerType theType );
	void showRasterLayer( RasterLayerType theType );
	void hideRasterLayer( RasterLayerType theType );

	QgsVectorLayer* createQgsVectorLayer(quint32 thePointDensity);
	void createQgsRasterLayer();
	//Testing methods
	void testCrs();

	public slots:

		void displayProfile(QgsGeometry * theLine, QgsGeometry * thePolygon, QgsVector& theVector);

	private slots:

		void loadLidarPoints();
		void setProfileTool();

	private:

		QgsRasterLayer * createRasterLayer( RasterLayerType theType );

    ////////////////////////////////////////////////////////////////////
    //
    // MANDATORY PLUGIN PROPERTY DECLARATIONS  .....
    //
    ////////////////////////////////////////////////////////////////////

		int mPluginType;
		//! Pointer to the QGIS interface object
		QgisInterface *mQGisIface;


    ////////////////////////////////////////////////////////////////////
    //
    // ADD YOUR OWN PROPERTY DECLARATIONS AFTER THIS POINT.....
    //
    ////////////////////////////////////////////////////////////////////
		
		///pointer to profile dock widget
		QgsLidarProfileGui * mLidarProfileGui;

		///pointer to Lidar tools dock widget
		QgsLidarToolsGui * mLidarToolsGui;

		///pointer to the Lidar menu
		QMenu *mLidarMenu;


		///load Lidar points action
		QAction * mQActionPointer;
		///draw rubber band action
		QAction * mActionProfile;
		///draw rubber band tool
		QgsMapTool * mToolProfile;

		///Lidar raster layers
		//QMap<RasterLayerType, QgsRasterLayer *> mRasterLayers;

		///Lidar data
		LidarData* mLidarData;
		
};

#endif //QGISLIDARPLUGIN_H
