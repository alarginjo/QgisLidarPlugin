#include "qgslidartoolsgui.h"

#include <QWidget>

#include "LidarData.h"

#include "qgisinterface.h"


QgsLidarToolsGui::QgsLidarToolsGui( QgisInterface* theQgisInterface, QWidget* parent, Qt::WFlags fl ) 
	: mIface( theQgisInterface )
	, mLidarData( NULL )
{
	setupUi( this );

	mIface->addDockWidget( Qt::LeftDockWidgetArea, this );
}
	
QgsLidarToolsGui::~QgsLidarToolsGui()
{

}

void QgsLidarToolsGui::setLidarData( LidarData * theLidarData )
{
	mLidarData = theLidarData;

	fillInfoFields();
}

void QgsLidarToolsGui::fillInfoFields()
{
	if ( mLidarData )
	{
		mEditNumOfPoints->setText( QString::number( mLidarData->metadata().numOfPoints() ) );
		mEditNorth->setText( QString::number( mLidarData->metadata().maxY(), 'f', 3 ) );
		mEditWest->setText( QString::number( mLidarData->metadata().minX(), 'f', 3 ) );
		mEditEast->setText( QString::number( mLidarData->metadata().maxX(), 'f', 3 ) );
		mEditSouth->setText( QString::number( mLidarData->metadata().minY(), 'f', 3 ) );
		mEditZMin->setText( QString::number( mLidarData->metadata().minZ(), 'f', 3 ) );
		mEditZMax->setText( QString::number( mLidarData->metadata().maxZ(), 'f', 3 ) );
		mEditGridCellSize->setText( QString::number( mLidarData->gridIndex()->cellSize(), 'f', 3 ) );
		mEditPointDensity->setText( QString::number( mLidarData->pointDensity(), 'f', 3 ) );
	}
}

void QgsLidarToolsGui::clearInfoFields()
{
	mEditNumOfPoints->setText( "" );
	mEditNorth->setText( "" );
	mEditWest->setText( "" );
	mEditEast->setText( "" );
	mEditSouth->setText( "" );
	mEditZMin->setText( "" );
	mEditZMax->setText( "" );
	mEditGridCellSize->setText( "" );
	mEditPointDensity->setText( "" );
}