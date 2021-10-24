#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Q3DScatter>

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
    QVector<int> needAn = {2, 3}; //AN - alive neighbours
    QVector<int> needAnToCreate = {3};

public:
    static const int maxX;
    static const int maxY;
    static const int maxZ;
    static bool gameIsRun;
    static bool canDie;
    static bool _2D;

    static QVector<bool> field;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void draw();
    int anyToDec(QString string, int n);

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
