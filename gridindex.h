/***************************************************************************
    gridindex.h
    -------------------
    begin                : Jan 21, 2004
    copyright            : (C) 2004 by Tim Sutton
    email                : tim@linfiniti.com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//
//Qt includes
//
#include <QVector>

//
//Qgis includes
//
#include <qgsrectangle.h>

#include "lidarpoint.h"
#include "lidarmetadata.h"

//
// forward declarations
//

class QgsRectangle;

class GridIndex
{
public:
	GridIndex();
	~GridIndex();
	//Getters
	QgsRectangle const& extent() const;
	const QList<quint32> * indices() const;
	quint32 rows() const;
	quint32 cols() const;
	qreal cellSize() const;
	//Initializer
	void init(const QgsRectangle& theExtent, const qreal theCellSize);
	/** Inserts new point index in grid. 
	* \param thePoints	: vector of Lidar points
	* \param theIndex	: index of point in vector
	*/
	void insertPoint(const QVector<LidarPoint>& thePoints, const quint32 theIndex);
	//Utility functions
	quint32 x2col(const qreal theX) const;
	quint32 y2row(const qreal theY) const;
	quint32 index(const quint32 theRow, const quint32 theCol) const;
	//Test
	void print(const QString& fileName) const;
private:

	//! Extent of the grid
	QgsRectangle mExtent;
	//! Array of lists of integers. One dimensional implementation of two dimensional array.
	QList<quint32> *mIndices;
	//! Number of rows
	quint32 mRows;
	//! Number of columns
	quint32 mCols;
	//! Size of cell
	qreal mCellSize;
};