#include "mainwindow.h"
#include "kletka.h"

int Kletka::count = 0;
QVector<Kletka*> Kletka::KList;
Q3DScatter *Kletka::graph;
Kletka::Kletka(double x, double y, double z, QWidget *parent) : QMainWindow(parent)
{
    series = new QScatter3DSeries;
    series->setMesh(QAbstract3DSeries::MeshCube);
    QScatterDataItem data = QVector3D(x, y, z);
    series->dataProxy()->addItem(data);
    if (MainWindow::maxX > MainWindow::maxZ && MainWindow::maxX > MainWindow::maxY)
        series->setItemSize((float)10/4/MainWindow::maxX);
    else if (MainWindow::maxY > MainWindow::maxZ && MainWindow::maxY > MainWindow::maxX)
        series->setItemSize((float)10/4/MainWindow::maxY);
    else series->setItemSize((float)10/4/MainWindow::maxZ);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    //может будут проверять и удалять дубли здесь вместо генерации новых клеток в другом месте (их кол-во упадет немного)
    posInList = numsInOne(x, y, z);
    if (KList[posInList] != nullptr)
        delete KList[posInList];
    KList[posInList] = this;
    count++;
    //qDebug()<<numsInOne(x, y, z)<<x<<y<<z<<KList.length();//MainWindow::maxY<<MainWindow::maxZ;
}

Kletka::~Kletka()
{
    delSeriesFromGraph();
    KList[posInList] = nullptr;
    delete series;
    count--;
    //qDebug()<<count;
}

void Kletka::delSeriesFromGraph()
{
    for (int i = 0; i < series->dataProxy()->itemCount(); i++) //ух ёёё.. понадобилось 3 часа чтобы понять чего не хватает
        series->dataProxy()->removeItems(0, 1);
    graph->removeSeries(series);
}

void Kletka::draw()
{
    graph->addSeries(series);
}

void Kletka::setPosition(double x, double y, double z)
{
    series->dataProxy()->setItem(0, QVector3D(x, y, z));
}

int Kletka::numsInOne(int x, int y, int z, int maxY, int maxZ) {
    if (maxY == -1)
        maxY = MainWindow::maxY+1;
    if (maxZ == -1)
        maxZ = MainWindow::maxZ+1;
    return x*maxY*maxZ+y*maxZ+z;
}

QVector3D Kletka::getPosition(){return series->dataProxy()->itemAt(0)->position();}

QScatter3DSeries *Kletka::getSeries(){return series;}
