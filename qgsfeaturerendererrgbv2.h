//#include "qgsrendererv2.h"
//#include "qgssymbolv2.h"
//#include "qgsfeature.h"

class QgsFeature;
class QgsSymbolV2;
class QgsAttributeMap;
class QgsFeatureRendererV2;


class QgsLidarPointRgbRendererV2 : public QgsFeatureRendererV2
{
public:
	QgsLidarPointRgbRendererV2();
	/**
	* Initialize rgb renderer for the vector layer. Takes indices of red,
	* green, and blue field.
	* \param theLidarVector		: vector layer
	*/
	void init(QgsVectorLayer * theLidarVector);
	QgsSymbolV2 * symbolForFeature (QgsFeature &feature);
private:
	mRedFieldIdx;
	mGreenFieldIdx;
	mBlueFieldIdx;
}