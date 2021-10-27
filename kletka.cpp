#include "mainwindow.h"
#include "kletka.h"

#include <cmath>

size_t Kletka::count = 0;
QVector<Kletka*> Kletka::KList;
Q3DScatter *Kletka::graph;
Kletka::Kletka(size_t x, size_t y, size_t z) : series(new QScatter3DSeries), posInList(numsInOne(x, y, z))
{
    series->setMesh(QAbstract3DSeries::MeshCube);
    series->dataProxy()->addItem(QScatterDataItem(QVector3D(x-1, y-1, z-1)));
    series->setItemSize((float)2.5/MainWindow::longestAxis);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    ++count;
    //qDebug()<<numsInOne(x, y, z)<<x<<y<<z<<KList.length();//MainWindow::maxY<<MainWindow::maxZ;
}
Kletka::Kletka(size_t n) : series(new QScatter3DSeries), posInList(n)
{
    series->setMesh(QAbstract3DSeries::MeshCube);
    series->dataProxy()->addItem(QScatterDataItem(oneInNums(n)-QVector3D(1, 1, 1)));
    series->setItemSize((float)2.5/MainWindow::longestAxis);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    ++count;
    //qDebug()<<numsInOne(x, y, z)<<x<<y<<z<<KList.length();//MainWindow::maxY<<MainWindow::maxZ;
}

Kletka::~Kletka()
{
    delSeriesFromGraph();
    KList[posInList] = nullptr;
    delete series;
    --count;
}

void Kletka::delSeriesFromGraph()
{
    series->dataProxy()->removeItems(0, 1); //ух ёёё.. понадобилось 3 часа чтобы понять чего не хватает
    graph->removeSeries(series);
}

void Kletka::draw()
{
    graph->addSeries(series);
}

void Kletka::setPosition(size_t x, size_t y, size_t z)
{
    series->dataProxy()->setItem(0, QVector3D(x, y, z));
}

size_t Kletka::numsInOne(size_t x, size_t y, size_t z) {
    size_t maxY = MainWindow::maxY+3;
    size_t maxX = MainWindow::maxX+3;
    return z*maxY*maxX+y*maxX+x;
}

QVector3D Kletka::oneInNums(size_t k) {
    size_t maxY = MainWindow::maxY+3;
    size_t maxX = MainWindow::maxX+3;
    //k -= MainWindow::fieldBegin-1;
    return QVector3D(floor(k%maxX), (size_t)floor(k/maxX)%maxY, floor(k/(maxY*maxX)));
}

QVector3D Kletka::getPosition(){return series->dataProxy()->itemAt(0)->position();}

QScatter3DSeries *Kletka::getSeries(){return series;}
