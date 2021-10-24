#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <kletka.h>

#include <QtDataVisualization>
#include <QtWidgets>
#include <cmath>

const int MainWindow::maxX = 10; //x, y, z дожны быть > 0
const int MainWindow::maxY = 10;
const int MainWindow::maxZ = 10;
bool MainWindow::gameIsRun = 0;
bool MainWindow::canDie = 1;
bool MainWindow::_2D = 0;

QVector<bool> MainWindow::field;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer();
    timer->setInterval(0);
    connect(timer, &QTimer::timeout, this, [=](){
        nextGeneration();
    });

    graph = new Q3DScatter;
    graph->setMeasureFps(1);
    //graph->setOrthoProjection(1);
    graph->axisX()->setTitle("X");
    graph->axisY()->setTitle("Y");
    graph->axisZ()->setTitle("Z");
    graph->axisX()->setRange(0, maxX);
    graph->axisY()->setRange(0, maxY);
    graph->axisZ()->setRange(0, maxZ);
    if (maxX > maxZ && maxX > maxY) graph->setAspectRatio(maxX/maxY); //самая длинная на y
    else if (maxZ > maxX && maxZ > maxY) graph->setAspectRatio(maxZ/maxY);
    else graph->setAspectRatio(1);
    graph->setHorizontalAspectRatio(maxX/maxZ); //x на z
    graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    Kletka::graph = graph;
    Kletka::KList.resize(Kletka::numsInOne(maxX, maxY, maxZ)+1);
    Kletka::KList.fill(nullptr);
    field.resize(Kletka::numsInOne(maxX, maxY, maxZ)+1);
    field.fill(0);

    QWidget *container = QWidget::createWindowContainer(graph);
    ui->verticalLayout->insertWidget(1, container);
    //ui->verticalLayout->addLayout(ui->buttonsLayout, 1);

    ui->delay->setValue(timer->interval());

    ui->radio2D->setChecked(_2D);
    if (canDie) ui->dieKletkamBtn->setCheckState(Qt::Checked);
    else ui->dieKletkamBtn->setCheckState(Qt::Unchecked);

    connect(ui->delay, SIGNAL(valueChanged(int)), this, SLOT(delayChanged(int)));
    connect(ui->radio2D, SIGNAL(clicked()), this, SLOT(xDchanged()));
    connect(ui->radio3D, SIGNAL(clicked()), this, SLOT(xDchanged()));
    connect(ui->dieKletkamBtn, SIGNAL(clicked()), this, SLOT(canDieChanged()));

    int id = QFontDatabase::addApplicationFont(":/fonts/Font-Awesome-Free-Solid-900.otf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    QFont font;
    font.setFamily(family);
    font.setPixelSize(32);

    ui->prevStepBtn->setFont(font);
    ui->playBtn->setFont(font);
    ui->nextStepBtn->setFont(font);

    ui->prevStepBtn->setText("\uf048");
    ui->playBtn->setText("\uf04b");
    ui->nextStepBtn->setText("\uf051");

    connect(ui->generateBtn, SIGNAL(clicked()), this, SLOT(generateClicked()));
    connect(ui->prevStepBtn, SIGNAL(clicked()), this, SLOT(prevGenClicked()));
    connect(ui->playBtn, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(ui->nextStepBtn, SIGNAL(clicked()), this, SLOT(nextGenClicked()));

    generateRandomField();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete graph;
}

void MainWindow::draw()
{
    QElapsedTimer drawTimer;
    drawTimer.start();
    ui->FPS->setText(QString::number(graph->currentFps())+" FPS");
    for (int i = 0; i < field.size(); i++) {
        if (field.at(i) == 1) {
            Kletka::KList.at(i)->draw();
            //qDebug()<<Kletka::KList.at(i)->getPosition()<<i<<Kletka::KList.at(i)->getSeries()->dataProxy()->itemCount();
        }
    }
    ui->drawTime->setText("draw: "+QString::number(drawTimer.elapsed())+"ms");
}

void MainWindow::generateRandomField()
{
    for (int i = 0; i < field.size(); i++) {
        if (field.at(i) == 1) {
            delete Kletka::KList.at(i);
            field[i] = 0;
        }
    }
    qDebug()<<Kletka::count<<graph->seriesList()<<graph->selectedSeries();
    int kCount = (int)((maxX+1)*(maxY+1)*(maxZ+1)*0.1);
    if (kCount <= 1)
        kCount = QRandomGenerator::global()->bounded(1, (maxX+1)*(maxY+1)*(maxZ+1));
    else
        kCount = QRandomGenerator::global()->bounded(1, kCount);

    int x, y, z;
    for (int i = 0; i < kCount; i++) {
        x = QRandomGenerator::global()->bounded((int)graph->axisX()->min(), maxX+1);
        y = QRandomGenerator::global()->bounded((int)graph->axisY()->min(), maxY+1);
        if (_2D == 0)
            z = QRandomGenerator::global()->bounded((int)graph->axisZ()->min(), maxZ+1);
        else z = 0;
        new Kletka(x, y, z);
        field[Kletka::numsInOne(x, y, z)] = 1;
    }
    draw();
}

void MainWindow::delayChanged(int i){timer->setInterval(i);}

void MainWindow::xDchanged()
{
    //QRadioButton *checkBox = qobject_cast<QRadioButton*>(sender());
    if (ui->radio2D->isChecked()) _2D = 1;
    else _2D = 0;
//    if ((checkBox == ui->radio2D && _2D == 0) || (checkBox == ui->radio3D && _2D == 1))
//        generateRandomField();
}
void MainWindow::canDieChanged(){canDie = !canDie;}

void MainWindow::generateClicked()
{
    generateRandomField();
}

void MainWindow::prevGenClicked()
{
    prevGeneration();
}

void MainWindow::playClicked()
{
    gameIsRun = !gameIsRun;
    play();
}

void MainWindow::nextGenClicked()
{
    timer->stop();
    gameIsRun = 0;
    timer->stop();
    ui->playBtn->setText("\uf04b");
    nextGeneration();
}

void MainWindow::prevGeneration()
{
    //надо будет сделать обратный nextGeneration
}

void MainWindow::play()
{
    if (gameIsRun && Kletka::count > 0) {
        ui->playBtn->setText("\uf04c");
        timer->start();
    } else {
        timer->stop();
        ui->playBtn->setText("\uf04b");
    }
}

void MainWindow::nextGeneration()
{
    QElapsedTimer calcTimer;
    calcTimer.start();

    int n, m, aliveNeigbours;
    QVector<bool> fieldCopy = field;
    for (int k = graph->axisZ()->min(); k <= maxZ*(!_2D); k++) {
        for (int j = graph->axisY()->min(); j <= maxY; j++) {
            for (int i = graph->axisX()->min(); i <= maxX; i++) {
                n = Kletka::numsInOne(i, j, k);
                aliveNeigbours = 0;

                if (_2D) {
                    for (int y = j-1; y <= j+1; y++)
                    {
                        for (int x = i-1; x <= i+1; x++)
                        {
                            if (x >= 0 && x <= maxX && y >= 0 && y <= maxY && !(x == i && y == j)) {
                                m = Kletka::numsInOne(x, y, 0);
                                if (fieldCopy.at(m) == 1){ //если сосед живой
                                    aliveNeigbours++;
                                }
                            }
                        }
                    }
                } else {
                    for (int z = k-1; z <= k+1; z++)
                    {
                        for (int y = j-1; y <= j+1; y++)
                        {
                            for (int x = i-1; x <= i+1; x++)
                            {
                                if (x >= 0 && x <= maxX && y >= 0 && y <= maxY && z >= 0 && z <= maxZ && !(x == i && y == j && z == k)) {
                                    m = Kletka::numsInOne(x, y, z);
                                    if (fieldCopy.at(m) == 1){ //если сосед живой
                                        aliveNeigbours++;
                                    }
                                }
                            }
                        }
                    }
                }
                if (fieldCopy.at(n) == 1) //живая клетка
                {
                    if (canDie) { //смерть клетки если можно
                        bool gg = 0;
                        for (int a = 0; a < needAn.size(); a++)
                            if (aliveNeigbours == needAn[a]) { //кол-во живых соседов столько сколько надо
                                gg = 1;
                                break;
                            }
                        if (!gg) {
                            field[n] = 0;
                            delete Kletka::KList.at(n);
                        }
                    }
                }
                else //мертвая клетка
                {
                    bool gg = 0;
                    for (int a = 0; a < needAnToCreate.size(); a++)
                        if (aliveNeigbours == needAnToCreate[a]) { //кол-во живых соседов столько сколько надо
                            gg = 1;
                            break;
                        }
                    if (gg) { //оживление клетки
                        new Kletka(i, j, k);
                        field[n] = 1;
                    }
                }
            }
        }
    }
    ui->calcTime->setText("calc: "+QString::number(calcTimer.elapsed())+"ms");

    draw();
}

