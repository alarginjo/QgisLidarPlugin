#include "qgsrendererv2.h"
#include "qgssymbolv2.h"
#include "qgsfeature.h"
#include "qgsvectorlayer.h"
#include "qgsmarkersymbollayerv2.h"
#include "qgssymbollayerv2utils.h"
#include "qgssinglesymbolrendererv2.h"
#include "qgslogger.h"

#include "qgslidarpointrgbrendererv2.h"

QgsLidarPointRgbRendererV2::QgsLidarPointRgbRendererV2():
	QgsSingleSymbolRendererV2( QgsMarkerSymbolV2::createSimple( QgsStringMap() )  ), 
	mRedFieldIdx( -1 ), 
	mGreenFieldIdx( -1 ),
	mBlueFieldIdx( -1 )
{
	initSymbol();
}

void QgsLidarPointRgbRendererV2::startRender( QgsRenderContext& context, const QgsVectorLayer *vlayer)
{
	mRedFieldIdx = vlayer->fieldNameIndex("red");
	mGreenFieldIdx = vlayer->fieldNameIndex("green");
	mBlueFieldIdx = vlayer->fieldNameIndex("blue");
	QgsSingleSymbolRendererV2::startRender(context, vlayer);
}


QgsSymbolV2 * QgsLidarPointRgbRendererV2::symbolForFeature (QgsFeature &feature)
{
	int red = 0;
	int green = 0;
	int blue = 0;
	
	if ( mRedFieldIdx != -1 )
	{
		red = feature.attributeMap()[mRedFieldIdx].toInt() / 256;
	}
	if ( mGreenFieldIdx != -1 )
	{
		green = feature.attributeMap()[mGreenFieldIdx].toInt() / 256;
	}
	if ( mBlueFieldIdx != -1 )
	{
		blue = feature.attributeMap()[mBlueFieldIdx].toInt() / 256;
	}
	
	/*QgsStringMap properties;

	properties["color"] = QString::number( red ) + ","
		+ QString::number( green ) + "," 
		+ QString::number( blue );
	properties["color_border"] = properties["color"];
	properties["size"] = "0.5";*/
	//QgsSymbolLayerV2 * symbolLayer = QgsSimpleMarkerSymbolLayerV2::create( properties );
	//QColor pointColor( red, green, blue );
	//static_cast<QgsSimpleMarkerSymbolLayerV2 *> ( mSymbol->symbolLayer( 0 ) )->setColor( pointColor ); 
	//static_cast<QgsSimpleMarkerSymbolLayerV2 *> ( mSymbol->symbolLayer( 0 ) )->setBorderColor( pointColor );
		

//QgsSymbolV2 * symbol = QgsSingleSymbolRendererV2::symbolForFeature( feature );

	return mSymbol;
}

void QgsLidarPointRgbRendererV2::initSymbol()
{
 	QgsStringMap properties;
	properties["color"] = "0,0,0";
	properties["color_border"] = "0,0,0";
	properties["size"] = "0.5";
	mSymbol = QgsMarkerSymbolV2::createSimple( properties );
}