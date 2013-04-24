
#ifndef QGSLIDARPROFILEGUI_H
#define QGSLIDARPROFILEGUI_H

#include "ui_qgslidarprofileguibase.h"
#include <QtGui/QDockWidget>

class QgisInterface;
class QgsMapTool;
class LidarData;

class QgsLidarProfileDockWidget : public QDockWidget
{
	Q_OBJECT
public:
	QgsLidarProfileDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
};

class QgsLidarProfileGui : public QMainWindow, private Ui::QgsLidarProfileGuiBase
{
	Q_OBJECT

public:
	QgsLidarProfileGui( QgisInterface* theQgisInterface, QWidget* parent = 0, Qt::WFlags fl = 0 );
	~QgsLidarProfileGui();
	void setLidarData(LidarData * theLidarData);
	void setProfile();

private slots:
	//view
	void setZoomInTool();
	void setZoomOutTool();
	void zoomToLayerTool();
	void zoomToLast();
	void zoomToNext();
	void setPanTool();

	//classification
	void setClassifyAboveTool();
	void setClassifyBelowTool();
	void setClassifyCloseTool();

	//profile position
	void moveForward();
	void moveBackward();

	//Layer display
	void setLayerRendererByClass();

	void setLayerRendererByColor();

private:
	void createActions();
	void createMapTools();
	void removeOldLayer();
	/**Docks / undocks this window*/
    void dockThisWindow( bool dock );

	QgisInterface * mIface;

	QgsMapTool * mToolZoomIn;
    QgsMapTool * mToolZoomOut;
    QgsMapTool * mToolPan;
	QgsMapTool * mToolClassifyAbove;
	QgsMapTool * mToolClassifyBelow;
	QgsMapTool * mToolClassifyClose;

	QDockWidget * mDock;

	LidarData * mLidarData;

	QgsVectorLayer * mLayer;
};

	

#endif