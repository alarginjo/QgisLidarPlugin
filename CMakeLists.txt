########################################################
# Files

SET (LIDAR_SRCS
	lidarpoint.cpp
	lidarmetadata.cpp
	gridindex.cpp
	profile.cpp
	LidarData.cpp
	profiletool.cpp
	qgslidarpointrgbrendererv2.cpp
	gdalutilities.cpp
	qgslidarprofilegui.cpp
	qgslidartoolsgui.cpp
    qgislidarplugin.cpp
)

SET (LIDAR_UIS 
    qgslidarprofileguibase.ui
    qgslidartoolsbase.ui
    )

SET (LIDAR_MOC_HDRS
	lidarpoint.h
	lidarmetadata.h
	gridindex.h
	profile.h
	LidarData.h
	profiletool.h
	qgslidarpointrgbrendererv2.h
	gdalutilities.h
	qgslidarprofilegui.h
	qgslidartoolsgui.h
	qgislidarplugin.h
)

SET (LIDAR_RCCS  qgslidarplugin.qrc)
########################################################
# Build

QT4_WRAP_UI (LIDAR_UIS_H  ${LIDAR_UIS})

QT4_WRAP_CPP (LIDAR_MOC_SRCS  ${LIDAR_MOC_HDRS})

QT4_ADD_RESOURCES(LIDAR_RCC_SRCS ${LIDAR_RCCS})

ADD_LIBRARY (qgislidarplugin MODULE 
  ${LIDAR_SRCS}
  ${LIDAR_MOC_SRCS} 
  ${LIDAR_RCC_SRCS}
  ${LIDAR_UIS_H})

# SET(Boost_DEBUG 1)
SET(Boost_INCLUDE_DIRS "c:/boost/include/boost-1_51")
SET(Boost_LIBRARIES  "c:/boost/lib")
#SET (LIBLAS_INCLUDE_DIR "c:/OSGeo4W/include/liblas")
#SET (LIBLAS_LIBRARY "c:/OSGeo4W/lib/liblas.lib")

#SET (BOOST_ROOT "c:/boost")

#FIND_PACKAGE ( Boost 1.51.0 )
#MESSAGE(STATUS ${Boost_INCLUDE_DIRS})


FIND_PATH ( LIBLAS_INCLUDE_DIR liblas
	"c:/OSGeo4W/"
	"$ENV{INCLUDE}"
	"$ENV{LIB_DIR}/include"
	)
MESSAGE(STATUS ${LIBLAS_INCLUDE_DIR})

FIND_LIBRARY( LIBLAS_LIBRARY liblas liblas_c PATHS
	"c:/OSGeo4W/lib"
	"$ENV{LIB}"
	"$ENV{LIB_DIR}/lib"
)

MESSAGE(STATUS ${CMAKE_CURRENT_BINARY_DIR})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_BINARY_DIR}
     ${CMAKE_CURRENT_BINARY_DIR}/../../ui
     ${GDAL_INCLUDE_DIR}
     ${LIBLAS_INCLUDE_DIR}
     ${Boost_INCLUDE_DIRS}
     ../../core 
     ../../core/raster 
     ../../core/symbology-ng 
     ../../gui
     ../../analysis/raster
     ../../providers/gdal
     ../../ui
     ..
     .
)

MESSAGE(STATUS ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(
  qgislidarplugin
  qgis_analysis
  qgis_core
  qgis_gui
  ${LIBLAS_LIBRARY}
  ${Boost_LIBRARIES}/libboost_thread-vc90-mt-1_51.lib
  ${Boost_LIBRARIES}/libboost_system-vc90-mt-1_51.lib
  ${Boost_LIBRARIES}/libboost_chrono-vc90-mt-1_51.lib
)


########################################################
# Install

INSTALL(TARGETS qgislidarplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR}
  )
