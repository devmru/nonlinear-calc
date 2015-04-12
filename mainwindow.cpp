#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "muParser/muParser.h"
#include "math.h"
#include <QVBoxLayout>
#include "qcustomplot.h"
#include <QWidget>
#include <QString>
#include <QAction>
#include <QDebug>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->newras, SIGNAL(triggered()), this, SLOT(clearscroll()));
    connect(ui->about, SIGNAL(triggered()), this, SLOT(showAbout()));
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
    double rasot = ui->rasot->value();
    double rasdo = ui->rasdo->value();
    int cbmetod = ui->cbmetod->currentIndex();
    double sbtoch = ui->sbtoch->value();
    expression = ui->leuravn->text();
    if(expression!= 0){
    QVBoxLayout * lay = new QVBoxLayout();
    QLabel * rez = new QLabel;
    rez->setTextInteractionFlags(Qt::TextSelectableByMouse);
    rez->setText("Вы ввели: "+expression);
    QCustomPlot * graf = new QCustomPlot();
    graf->setObjectName("graf");
    graf->setMinimumHeight(500);
    lay->addWidget(graf);
    lay->addWidget(rez);
    // генерируем данные
    int nitr = (abs(xot)+abs(xdo))*10;
    QVector<double> xi(nitr), yi(nitr); // для гладкости)
    for (int i=0; i<nitr; ++i)
    {
      xi[i] = i/10.0-abs(xot);
      try
      {
      QString f = expression;
      double x = i/10.0-abs(xot);
      QString z = QString::number(x);
      f.replace("x",z);
      mu::string_type expression_buffer;
      QByteArray expression_array = f.toLocal8Bit();
      char *expression_chars = expression_array.data();
      expression_buffer = expression_chars;
      p.SetExpr(expression_buffer);
      yi[i] = p.Eval(); //Подсчитанное решение
      }
      catch(mu::Parser::exception_type &e)
      {
          rez->setText("Ошибка!");
          lay->removeWidget(graf);
          lay->removeWidget(rez);
          QMessageBox messageBox;
          messageBox.critical(0,"Ошибка","Произошла ошибка!");
          messageBox.setFixedSize(500,200);
          break;
      }
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
    graf->setAttribute(Qt::WA_NoMousePropagation, false);
    graf->graph(0)->setData(xi, yi);
    // задаем имена осей координат
    graf->xAxis->setLabel("x");
    graf->yAxis->setLabel("y");
    // задаем размеры осей
    graf->xAxis->setRange(xot, xdo);
    graf->yAxis->setRange(yot, ydo);
    graf->replot();

    if(cbmetod==0){
        QString oldrez = rez->text();
        rez->setText(oldrez+"\nВыбран метод дихотомии");
        QTableWidget * tabledihot = new QTableWidget;
        tabledihot->setColumnCount(4);
        tabledihot->setRowCount(1);
        tabledihot->setHorizontalHeaderLabels(QStringList()<<"a"<<"b"<<"Xn=(a+b)/2"<<"b-Xn");
//Insert in table
        double a = rasot;
        double b =rasdo;
        int ni = 0;
        double razbixn = 0;
        double xn;
        do{
            xn = (a+b)/2;
            //f ot b
            QString exprb = expression;
            exprb.replace("x",QString::number(b));
            mu::string_type exprb_buffer;
            QByteArray exprb_array = exprb.toLocal8Bit();
            char *exprb_chars = exprb_array.data();
            exprb_buffer = exprb_chars;
            p.SetExpr(exprb_buffer);
            double fb = p.Eval();

            //f ot xn
            QString exprxn = expression;
            exprxn.replace("x",QString::number(xn));
            mu::string_type exprxn_buffer;
            QByteArray exprxn_array = exprxn.toLocal8Bit();
            char *exprxn_chars = exprxn_array.data();
            exprxn_buffer = exprxn_chars;
            p.SetExpr(exprxn_buffer);
            double fxn = p.Eval();
            razbixn = b-xn;
            tabledihot->setRowCount(ni+1);
            QTableWidgetItem * itma = new QTableWidgetItem;
            QTableWidgetItem * itmb = new QTableWidgetItem;
            QTableWidgetItem * itmxn = new QTableWidgetItem;
            QTableWidgetItem * itmrazbixn = new QTableWidgetItem;
            itma->setText(QString::number(a));
            itmb->setText(QString::number(b));
            itmxn->setText(QString::number(xn));
            itmrazbixn->setText(QString::number(razbixn));
            tabledihot->setItem(ni, 0, itma);
            tabledihot->setItem(ni, 1, itmb);
            tabledihot->setItem(ni, 2, itmxn);
            tabledihot->setItem(ni, 3, itmrazbixn);
            if((fb*fxn)<0)
            {
                a = xn;
            }
            else
            {
                b = xn;
            }
            ni++;
        }while (razbixn > sbtoch);

        //Убираю скролл у таблицы
        int ht = (tabledihot->model()->rowCount()-1)+tabledihot->horizontalHeader()->height();
        int wt = (tabledihot->model()->columnCount()-1)+tabledihot->verticalHeader()->width();
        for(int row = 0; row < tabledihot->model()->rowCount(); row++)
        ht = ht + tabledihot->rowHeight(row);
        for(int column = 0; column < tabledihot->model()->columnCount(); column++)
        wt = wt + tabledihot->columnWidth(column);
        tabledihot->setMinimumHeight(ht);
        tabledihot->setMinimumWidth(wt);
        lay->addWidget(tabledihot);
        QLabel * rezend = new QLabel;
        rezend->setTextInteractionFlags(Qt::TextSelectableByMouse);
        rezend->setText("Искомый корень x≈"+QString::number(xn)+" Вычисления проводились с точностью "+QString::number(sbtoch));
        lay->addWidget(rezend);
    }

    ui->scrollContents->setLayout(lay);
    }
}
void MainWindow::clearscroll()
{
      qDeleteAll(ui->scrollContents->children());
      ui->scrollContents->resize(0,0);
}
void MainWindow::showAbout()
{
    QDialog *Dialog = new QDialog(this);
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *label_2;
    QLabel *label_3;

    if (Dialog->objectName().isEmpty())
        Dialog->setObjectName(QString::fromUtf8("Dialog"));
    Dialog->resize(400, 300);
    gridLayout = new QGridLayout(Dialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    buttonBox = new QDialogButtonBox(Dialog);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    buttonBox->setCenterButtons(false);

    gridLayout->addWidget(buttonBox, 2, 0, 1, 1);

    label = new QLabel(Dialog);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 1);

    groupBox = new QGroupBox(Dialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    gridLayout_2 = new QGridLayout(groupBox);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout_2->addWidget(label_3, 0, 0, 1, 1);

    gridLayout->addWidget(groupBox, 1, 0, 1, 1);

    QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));
    QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));

    QMetaObject::connectSlotsByName(Dialog);

    Dialog->setWindowTitle(QApplication::translate("Dialog", "О программе", 0));
         label->setText(QApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
 "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
 "p, li { white-space: pre-wrap; }\n"
 "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
 "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:18pt; font-weight:600;\">Nonlinear-calc</span></p>\n"
 "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">version 1.0</span></p>\n"
 "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
 "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px;"
                         " margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Copyright \302\251 2015 Maxim Sadakov</span></p>\n"
 "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"https://github.com/devmru/nonlinear-calc/\"><span style=\" font-size:8pt; text-decoration: underline; color:#0000ff;\">github.com</span></a>"
 "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"mailto:proger@dev-m.ru\"><span style=\" font-size:8pt; text-decoration: underline; color:#0000ff;\">proger@dev-m.ru</span></a>"
 "</p></body></html>", 0));
         groupBox->setTitle(QApplication::translate("Dialog", "Libraries", 0));
         label_2->setText(QApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
 "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
 "p, li { white-space: pre-wrap; }\n"
 "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
 "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">muParser v1.34</span><span style=\" font-size:10pt;\"> (a fast math expression parser)</span></p>\n"
 "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Copyright \302\251 2004-2010 Ingo Berg</span></p>\n"
 "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://muparser.sourceforge.net/\"><span style=\" font-size:8pt; text"
                         "-decoration: underline; color:#0000ff;\">muparser.sourceforge.net</span></a></p></body></html>", 0));
         label_3->setText(QApplication::translate("Dialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
 "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
 "p, li { white-space: pre-wrap; }\n"
 "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
 "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Qt Framework v4.6.3</span><span style=\" font-size:10pt;\"> (a C++ toolkit)</span></p>\n"
 "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Copyright \302\251 2010 Nokia Corporation</span></p>\n"
 "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"http://qt.nokia.com/\"><span style=\" font-size:8pt; text-decoration: underli"
                         "ne; color:#0000ff;\">qt.nokia.com</span></a></p></body></html>", 0));

         Dialog->exec();
}
