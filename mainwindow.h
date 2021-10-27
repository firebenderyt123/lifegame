#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Q3DScatter>
#include <QElapsedTimer>

#include <kletka.h>

using namespace QtDataVisualization;

class ObjectsDeleterThread;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QTimer *timer = nullptr;
    QVector<byte> needAn = {2, 3}; //AN - alive neighbours
    QVector<byte> needAnToCreate = {3};

public:
    static const byte coresCount;
    static const size_t maxX;
    static const size_t maxY;
    static const size_t maxZ;
    static bool gameIsRun;
    static bool canDie;
    static bool _2D;

    static size_t fieldBegin;
    static size_t fieldEnd;
    static const size_t fieldSize;
    static byte field[];
    static byte neighbors[];
    static size_t longestAxis;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void draw();
    size_t anyToDec(QString string, size_t n);

    void generateNeighbors();
    void generateRandomField();
    void prevGeneration();
    void play();
    void nextGeneration();

public slots:
    void delayChanged(int);
    void xDchanged();
    void canDieChanged();
    void generateClicked();
    void prevGenClicked();
    void playClicked();
    void nextGenClicked();

private:
    Ui::MainWindow *ui;
    Q3DScatter *graph;
};
#endif // MAINWINDOW_H
