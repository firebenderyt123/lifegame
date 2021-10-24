#ifndef KLETKA_H
#define KLETKA_H

#include <QMainWindow>
#include <Q3DScatter>
#include <QScatter3DSeries>

using namespace QtDataVisualization;

class Kletka : public QMainWindow
{
    Q_OBJECT

    QScatter3DSeries *series;

public:
    static Q3DScatter *graph;

    static int count;
    static QVector<Kletka*> KList;
    int posInList;

    explicit Kletka(double x, double y, double z, QWidget *parent = nullptr);
    ~Kletka();

    void delSeriesFromGraph();

    void draw();
    void setPosition(double x, double y, double z);

    static int numsInOne(int x, int y, int z, int maxY = -1, int maxZ = -1);

    QVector3D getPosition();
    QScatter3DSeries *getSeries();
};

#endif // KLETKA_H
