
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include "gridindex.h"


GridIndex::GridIndex():
	mExtent(), mIndices (NULL), mRows(0), mCols(0), mCellSize(0)
{
}

GridIndex::~GridIndex()
{
	delete [] mIndices;
}

QgsRectangle const& GridIndex::extent() const 
{
	return mExtent;
}

const QList<quint32> * GridIndex::indices() const
{
	return mIndices;
}

quint32 GridIndex::rows() const
{
	return mRows;
}

quint32 GridIndex::cols() const
{
	return mCols;
}

qreal GridIndex::cellSize() const
{
	return mCellSize;
}

quint32 GridIndex::x2col(const qreal theX) const
{
	return (quint32)((theX - mExtent.xMinimum()) / mCellSize); 
}

quint32 GridIndex::y2row(const qreal theY) const
{
	return (quint32)((mExtent.yMaximum() - theY) / mCellSize);
}

quint32 GridIndex::index(const quint32 theRow, const quint32 theCol) const
{
	return theRow * mCols + theCol;
}

void GridIndex::init(const QgsRectangle& theExtent, const qreal theCellSize)
{
	mExtent = theExtent;
	mCellSize = theCellSize;
	mRows = (quint32) ((mExtent.yMaximum() - mExtent.yMinimum()) / mCellSize) + 1;
	mCols = (quint32) ((mExtent.xMaximum() - mExtent.xMinimum()) / mCellSize) + 1;
	try
	{
		mIndices = new QList<quint32>[mRows * mCols];
	}
	catch (...)
	{
		throw;
	}
}

void GridIndex::insertPoint(const QVector<LidarPoint>& thePoints, const quint32 theIndex)
{
	quint32 c = x2col(thePoints.at(theIndex).x());
	quint32 r = y2row(thePoints.at(theIndex).y());
	QMutableListIterator<quint32> it(mIndices[index(r, c)]);
	if( !it.hasNext() )
	{
		//insert first item
		it.insert(theIndex);
	}
	else
	{
		while(it.hasNext()) 
		{
			if(thePoints.at(it.peekNext()).z() >= thePoints.at(theIndex).z())
			{
				//inser in the middle
				it.insert(theIndex);
				break;
			}
			else
			{
				it.next();
			}
		}
		if( !it.hasNext() )
			//insert last item
			it.insert(theIndex);
	}
}

void GridIndex::print(const QString& fileName) const
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return;
	QTextStream out(&file);
	for(quint32 i=0; i<mCols; i++)
		for(quint32 j=0; j<mRows; j++)
		{
			out << "[" << j << ", " << i << "]:";
			QListIterator<quint32> it(mIndices[index(j,i)]);
			while(it.hasNext())
				out << it.next() << " ";
			out << "\n";
		}

	file.close();
}