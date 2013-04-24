
#ifndef QGSLIDARTOOLSGUI_H
#define QGSLIDARTOOLSGUI_H

#include "ui_qgslidartoolsbase.h"
#include <QtGui/QDockWidget>

class QgisInterface;
class LidarData;

class QgsLidarToolsGui : public QDockWidget, private Ui::QgsLidarToolsBase
{
	Q_OBJECT
	public:
		QgsLidarToolsGui( QgisInterface* theQgisInterface, QWidget* parent = 0, Qt::WFlags fl = 0 );
		~QgsLidarToolsGui();

		void setLidarData( LidarData * theLidarData );

		void fillInfoFields();
		void clearInfoFields();

	public slots:

	private slots:

	private:
		QgisInterface * mIface;

		LidarData * mLidarData;
};

#endif //QGSLIDARTOOLSGUI_H
