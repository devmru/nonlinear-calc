#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"
#include <QVBoxLayout>
#include "qcustomplot.h"
#include <QWidget>
#include <QString>
#include <QAction>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->newras, SIGNAL(triggered()), this, SLOT(clearscroll()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bstart_clicked()
{
    qDeleteAll(ui->scrollContents->children());
    ui->scrollContents->resize(0,0);

    int xot = ui->sbxot->value();
    int xdo = ui->sbxdo->value();
    int yot = ui->sbyot->value();
    int ydo = ui->sbydo->value();
    int rasot = ui->rasot->value();
    int rasdo = ui->rasdo->value();
    int cbmetod = ui->cbmetod->currentIndex();
    QString leuravn = ui->leuravn->text();



    QVBoxLayout * lay = new QVBoxLayout();
    QLabel * rez = new QLabel;
    rez->setText("sssss");
    QCustomPlot * graf = new QCustomPlot();
    graf->setObjectName("graf");
    graf->setMinimumHeight(500);
    lay->addWidget(graf);
    lay->addWidget(rez);
    ui->scrollContents->setLayout(lay);

    // генерируем данные
    int nitr = (xot*10)*2;
    QVector<double> x(nitr), y(nitr); // для гладкости)
    for (int i=0; i<nitr; ++i)
    {
      x[i] = i/10.0-xot;
      y[i] = leuravn; // моя формула
    }

//    QVector<double> Ticks;  // шаг
//    int i = -10;
//    while(i<=10) {
//        Ticks << i;
//        i+=1;
//    }
//    graf->xAxis->setAutoTicks(false); // выключаем автоматические отсчеты
//    graf->xAxis->setTickVector(Ticks);  // задаем созданный нами вектор
//    graf->yAxis->setAutoTicks(false); // выключаем автоматические отсчеты
//    graf->yAxis->setTickVector(Ticks);  // задаем созданный нами вектор

    // создаем график и добавляем данные:
    graf->addGraph();
    graf->graph(0)->setData(x, y);
    // задаем имена осей координат
    graf->xAxis->setLabel("x");
    graf->yAxis->setLabel("y");
    // задаем размеры осей
    graf->xAxis->setRange(-xot, xdo);
    graf->yAxis->setRange(-yot, ydo);
    graf->replot();
}
void MainWindow::clearscroll()
{
      qDeleteAll(ui->scrollContents->children());
      ui->scrollContents->resize(0,0);
}
void MainWindow::showAbout()
{

}
