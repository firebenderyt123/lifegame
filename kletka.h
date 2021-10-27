#ifndef KLETKA_H
#define KLETKA_H

#include <Q3DScatter>
#include <QScatter3DSeries>

using namespace QtDataVisualization;

class Kletka
{
    QScatter3DSeries *series;

public:
    static Q3DScatter *graph;

    static size_t count;
    static QVector<Kletka*> KList;
    size_t posInList;

    Kletka(size_t x, size_t y, size_t z);
    Kletka(size_t n);
    ~Kletka();

    void delSeriesFromGraph();

    void draw();
    void setPosition(size_t x, size_t y, size_t z);

    static size_t numsInOne(size_t x, size_t y, size_t z);
    static QVector3D oneInNums(size_t k);

    QVector3D getPosition();
    QScatter3DSeries *getSeries();
};

#endif // KLETKA_H
