//#include "qgsrendererv2.h"
//#include "qgssymbolv2.h"
//#include "qgsfeature.h"

//#include <QMap>

#include "qgssinglesymbolrendererv2.h"

class QgsFeature;
class QgsSymbolV2;

//typedef QMap<int, QVariant> QgsAttributeMap;


class QgsLidarPointRgbRendererV2 : public QgsSingleSymbolRendererV2
{
public:
	QgsLidarPointRgbRendererV2();
	void startRender( QgsRenderContext& context, const QgsVectorLayer *vlayer );
	QgsSymbolV2 * symbolForFeature ( QgsFeature &feature );
private:
	void initSymbol();
	int mRedFieldIdx;
	int mGreenFieldIdx;
	int mBlueFieldIdx;
};