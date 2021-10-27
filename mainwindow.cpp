#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <kletka.h>

#include <QtDataVisualization>
#include <QtWidgets>
#include <cmath>

const byte MainWindow::coresCount = QThread::idealThreadCount();
const size_t MainWindow::maxX = 10; //x, y, z дожны быть > 0
const size_t MainWindow::maxY = 10;
const size_t MainWindow::maxZ = 10;
bool MainWindow::gameIsRun = 0;
bool MainWindow::canDie = 1;
bool MainWindow::_2D = 0;

size_t MainWindow::fieldBegin;
size_t MainWindow::fieldEnd;
const size_t MainWindow::fieldSize = (maxX+3)*(maxY+3)*(maxZ+3);
byte MainWindow::field[fieldSize];
byte MainWindow::neighbors[fieldSize];
size_t MainWindow::longestAxis;
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

    graph = new Q3DScatter();
    graph->setMeasureFps(1);
    //graph->setOrthoProjection(1);
    graph->axisX()->setTitle("X");
    graph->axisY()->setTitle("Y");
    graph->axisZ()->setTitle("Z");
    graph->axisX()->setRange(0, maxX);
    graph->axisY()->setRange(0, maxY);
    graph->axisZ()->setRange(0, maxZ);
    if (maxX > maxZ && maxX > maxY)
        longestAxis = maxX;
    else if (maxZ > maxX && maxZ > maxY)
        longestAxis = maxZ;
    else longestAxis = maxY;
    graph->setAspectRatio(longestAxis/maxY); //самая длинная на y
    graph->setHorizontalAspectRatio(maxX/maxZ); //x на z
    graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    Kletka::graph = graph;
    Kletka::KList.resize((maxX+3)*(maxY+3)*(maxZ+3)); //maxX+1 и тд это чтобы до нужного размера включительно и + 2 для запаса на расчеты
    Kletka::KList.fill(nullptr);
    fieldBegin = (maxX+1)*(maxY+1) + maxX+1 + 1;
    fieldEnd = fieldSize-fieldBegin;
    std::fill(std::begin(field),std::begin(field)+fieldSize,(byte)0);
    std::fill(std::begin(neighbors),std::begin(neighbors)+fieldSize,(byte)0);
    //qDebug()<<Kletka::KList.size();

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

    size_t id = QFontDatabase::addApplicationFont(":/fonts/Font-Awesome-Free-Solid-900.otf");
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
    for (size_t i = fieldBegin; i < fieldEnd; i += maxX+1) {
        for (size_t j = 0; j < maxX+1; ++j) {
            if (field[i+j]) {
                Kletka::KList.at(i+j)->draw();
                //qDebug()<<Kletka::KList.at(i)->getPosition()<<i<<Kletka::KList.at(i)->getSeries()->dataProxy()->itemCount();
            }
        }
    }
    ui->drawTime->setText("draw: "+QString::number(drawTimer.elapsed())+"ms");
}

void MainWindow::generateRandomField()
{
    size_t num, n;

    if (_2D) {
        for (size_t k = 2; k <= maxZ+1; ++k) { //удаляем клетки (z > 0)
            for (size_t j = 1; j <= maxY+1; ++j) {
                for (size_t i = 1; i <= maxX+1; ++i) {
                    n = Kletka::numsInOne(i, j, k);
                    field[n] = (byte)0;
                    delete Kletka::KList.at(n);
                }
            }
        }
    }

    for (size_t k = 1; k <= maxZ*(!_2D)+1; ++k) {
        for (size_t j = 1; j <= maxY+1; ++j) {
            for (size_t i = 1; i <= maxX+1; ++i) {
                num = QRandomGenerator::global()->bounded(0, 2);
                n = Kletka::numsInOne(i, j, k);
                if (num && Kletka::KList.at(n) == nullptr) {
                    Kletka::KList[n] = new Kletka(i, j, k);
                    //qDebug()<<n<<Kletka::oneInNums(n)<<i<<j<<k;
                } else if (!num && Kletka::KList.at(n) != nullptr)
                    delete Kletka::KList.at(n);
                field[n] = (byte)num;
                //qDebug()<<i<<j<<k<<n<<num<<Kletka::KList[n];
            }
        }
    }
    //qDebug()<<Kletka::count;
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

    size_t n;
    size_t z = (maxX+3)*(maxY+3);
    size_t y = maxX+3;
    ulong* ptr;
    byte* fieldPtr = field;
    byte* neighborsPtr = neighbors;

    for (size_t n = 0; n < fieldSize; ++n)
    {
        *(ulong*)(neighborsPtr + n) = 0;
    }

    for (size_t n = fieldBegin; n < fieldEnd; ++n)
    {
        ptr = (ulong*)(neighborsPtr + n);
        *ptr += fieldPtr[n - 1 - y - z];
        *ptr += fieldPtr[n - y - z];
        *ptr += fieldPtr[n + 1 - y - z];
        *ptr += fieldPtr[n - 1 - z];
        *ptr += fieldPtr[n - z];
        *ptr += fieldPtr[n + 1 - z];
        *ptr += fieldPtr[n - 1 + y - z];
        *ptr += fieldPtr[n + y - z];
        *ptr += fieldPtr[n + 1 + y - z];

        *ptr += fieldPtr[n - 1 - y];
        *ptr += fieldPtr[n - y];
        *ptr += fieldPtr[n + 1 - y];
        *ptr += fieldPtr[n - 1];
        *ptr += fieldPtr[n + 1];
        *ptr += fieldPtr[n - 1 + y];
        *ptr += fieldPtr[n + y];
        *ptr += fieldPtr[n + 1 + y];

        *ptr += fieldPtr[n - 1 - y + z];
        *ptr += fieldPtr[n - y + z];
        *ptr += fieldPtr[n + 1 - y + z];
        *ptr += fieldPtr[n - 1 + z];
        *ptr += fieldPtr[n + z];
        *ptr += fieldPtr[n + 1 + z];
        *ptr += fieldPtr[n - 1 + y + z];
        *ptr += fieldPtr[n + y + z];
        *ptr += fieldPtr[n + 1 + y + z];
        //qDebug()<<n<<&fieldPtr[n - 1 - y - z]<<fieldPtr[n - 1 - y - z]<<&ptr[n]<<ptr[n];
    }

//    for (size_t k = 1; k <= maxZ*(!_2D)+1; k++) {
//        for (size_t j = 1; j <= maxY+1; j++) {
//            for (size_t i = 1; i <= maxX+1; i++) {
//                n = Kletka::numsInOne(i,j,k);
//                neighbors[n] = (byte)(
//                    field[n - 1 - y - z] + field[n - y - z] + field[n + 1 - y - z] +
//                    field[n - 1 - z] + field[n - z] + field[n + 1 - z] +
//                    field[n - 1 + y - z] + field[n + y - z] + field[n + 1 + y - z] +

//                    field[n - 1 - y] + field[n - y] + field[n + 1 - y] +
//                    field[n - 1] + field[n + 1] +
//                    field[n - 1 + y] + field[n + y] + field[n + 1 + y] +

//                    field[n - 1 - y + z] + field[n - y + z] + field[n + 1 - y + z] +
//                    field[n - 1 + z] + field[n + z] + field[n + 1 + z] +
//                    field[n - 1 + y + z] + field[n + y + z] + field[n + 1 + y + z]);
//                //qDebug()<<&neighbors[n];
////                qDebug()<<i<<j<<k<<neighbors[n]<<n;
////                qDebug()<<
////                        field[n - 1 - y]<<field[n - y]<<field[n + 1 - y]<<
////                        field[n - 1]<<field[n + 1]<<
////                        field[n - 1 + y]<<field[n + y]<<field[n + 1 + y];
////                qDebug()<<n - 1 - y<<n - y<<n + 1 - y<<n - 1<< n + 1<< n - 1 + y<<n + y<<n + 1 + y;
////                qDebug()<<Kletka::numsInOne(i, j, k);
//            }
//        }
//    }
    bool keepAlive;
    bool makeNewLife;
//    if (_2D) {
//        for (size_t n = fieldBegin; n <= maxX+3+y*(maxY+2)+z; ++n) {
//            keepAlive = field[n] & needAn.contains(neighbors[n]);
//            makeNewLife = !field[n] & needAnToCreate.contains(neighbors[n]);
//            field[n] = (byte)((makeNewLife | keepAlive) ? 1 : 0);
//            if (field[n] && Kletka::KList.at(n) == nullptr)
//                Kletka::KList[n] = new Kletka(n);
//            else if (canDie && !field[n] && Kletka::KList.at(n) != nullptr)
//                delete Kletka::KList.at(n);
//            //qDebug()<<field[n]<<n;
//        }
//    } else {
//        for (size_t n = fieldBegin; n <= fieldEnd; ++n) {
//            keepAlive = field[n] & needAn.contains(neighbors[n]);
//            makeNewLife = !field[n] & needAnToCreate.contains(neighbors[n]);
//            field[n] = (byte)((makeNewLife | keepAlive) ? 1 : 0);
//            if (field[n] && Kletka::KList.at(n) == nullptr)
//                Kletka::KList[n] = new Kletka(n);
//            else if (canDie && !field[n] && Kletka::KList.at(n) != nullptr)
//                delete Kletka::KList.at(n);
//            //qDebug()<<field[n]<<n;
//        }
//    }
    for (size_t k = 1; k <= maxZ*(!_2D)+1; ++k) {
        for (size_t j = 1; j <= maxY+1; ++j) {
            for (size_t i = 1; i <= maxX+1; ++i) {
                n = Kletka::numsInOne(i,j,k);
                keepAlive = field[n] & needAn.contains(neighbors[n]);
                makeNewLife = !field[n] & needAnToCreate.contains(neighbors[n]);
                field[n] = (byte)((makeNewLife | keepAlive) ? 1 : 0);
                if (field[n] && Kletka::KList.at(n) == nullptr)
                    Kletka::KList[n] = new Kletka(i, j, k);
                else if (canDie && !field[n] && Kletka::KList.at(n) != nullptr)
                    delete Kletka::KList.at(n);
                //qDebug()<<field[n]<<n;
            }
        }
    }
    //qDebug()<<Kletka::KList;
    ui->calcTime->setText("calc: "+QString::number(calcTimer.elapsed())+"ms");

    draw();
}

