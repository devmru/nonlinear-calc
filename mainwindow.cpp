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
//Вычисление функции
double MainWindow::fun(double x){
    QString f = expression;
    QString z = QString::number(x);
    f.replace("x","("+z+")");
    f.replace("e","_e");
    mu::string_type expression_buffer;
    QByteArray expression_array = f.toLocal8Bit();
    char *expression_chars = expression_array.data();
    expression_buffer = expression_chars;
    p.SetExpr(expression_chars);
    return p.Eval();
}
//Первая производная
double MainWindow::derv1_f(double x,double exp){
    return (fun(x+exp)-fun(x))/exp;
}
//Вторая производная
double MainWindow::derv2_f(double x,double exp){
    return ((fun(x + exp) - fun((x + exp) + exp))/((x + exp) - ((x + exp) + exp)) - (fun(x) - fun(x+exp))/(x - (x + exp)))/exp;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->newras, SIGNAL(triggered()), this, SLOT(clearscroll()));
    connect(ui->about, SIGNAL(triggered()), this, SLOT(showAbout()));
    ui->sbtoch->setMinimum(0.00001);
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
      xi[i] =(i/10.0)-abs(xot);
      try
      {
      yi[i] = fun(i/10.0-abs(xot));
      }
      catch(mu::Parser::exception_type &e)
      {
          rez->setText("Ошибка!");
          lay->removeWidget(graf);
          lay->removeWidget(rez);
          QMessageBox messageBox;
          messageBox.critical(0,"Ошибка","Произошла ошибка!");
          messageBox.setFixedSize(500,200);
          return;
      }
    }
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
    switch (cbmetod) {
    case 0:{
        QString oldrez = rez->text();
        rez->setText(oldrez+"\nВыбран метод дихотомии");
        //Table
        QTableWidget * trez = new QTableWidget;
        trez->setColumnCount(4);
        trez->setRowCount(1);
        trez->setStyleSheet("border:none");
        trez->setHorizontalHeaderLabels(QStringList()<<"a"<<"b"<<"Xn=(a+b)/2"<<"b-Xn");
        //Insert in table
        double a = rasot;
        double b =rasdo;
        int ni = 0;
        double razbixn = 0;
        double xn;
        do{
            xn = (a+b)/2;
            double fb = fun(b);
            double fxn = fun(xn);
            razbixn = b-xn;
            trez->setRowCount(ni+1);
            QTableWidgetItem * itma = new QTableWidgetItem;
            QTableWidgetItem * itmb = new QTableWidgetItem;
            QTableWidgetItem * itmxn = new QTableWidgetItem;
            QTableWidgetItem * itmrazbixn = new QTableWidgetItem;
            itma->setText(QString::number(a));
            itmb->setText(QString::number(b));
            itmxn->setText(QString::number(xn));
            itmrazbixn->setText(QString::number(razbixn));
            trez->setItem(ni, 0, itma);
            trez->setItem(ni, 1, itmb);
            trez->setItem(ni, 2, itmxn);
            trez->setItem(ni, 3, itmrazbixn);
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
        //Убираю скролл у таблиц
        int ht = (trez->model()->rowCount()-1)+trez->horizontalHeader()->height();
        int wt = (trez->model()->columnCount()-1)+trez->verticalHeader()->width();
        for(int row = 0; row < trez->model()->rowCount(); row++)
        ht = ht + trez->rowHeight(row);
        for(int column = 0; column < trez->model()->columnCount(); column++)
        wt = wt + trez->columnWidth(column);
        trez->setMinimumHeight(ht);
        trez->setMinimumWidth(wt);
        lay->addWidget(trez);
        QLabel * rezend = new QLabel;
        rezend->setTextInteractionFlags(Qt::TextSelectableByMouse);
        rezend->setText("Искомый корень x≈"+QString::number(xn)+" Вычисления проводились с точностью "+QString::number(sbtoch));
        lay->addWidget(rezend);
        break;}
    case 1:{
        QString otvet;
        double x0;
        QString oldrez = rez->text();
        if (fun(rasot)==0) {
            otvet = "\nПоскольку F("+QString::number(rasot)+")=0, то корень x="+QString::number(rasot);
            rez->setText(oldrez+"\nВыбран метод Ньютона(касательных)"+otvet);
            break;
        }
        else if (fun(rasdo)==0) {
            otvet = "\nПоскольку F("+QString::number(rasdo)+")=0, то корень x="+QString::number(rasdo);
            rez->setText(oldrez+"\nВыбран метод Ньютона(касательных)"+otvet);
            break;
        }
        if (fun(rasot)*fun(rasdo)>0) {
            otvet = "\nf("+QString::number(rasot)+")="+QString::number(fun(rasot))+""
                    "\nf("+QString::number(rasdo)+")="+QString::number(fun(rasdo))+""
                    "\nВ данном интервале ["+QString::number(rasot)+";"+QString::number(rasdo)+"] нет корней, так как"
                      " ("+QString::number(fun(rasot))+"*"+QString::number(fun(rasdo))+">0)";
            rez->setText(oldrez+"\nВыбран метод Ньютона(касательных)"+otvet);
            break;

        }
        else if (fun(rasot)*fun(rasdo)<0) {
            otvet = "\nПоскольку f("+QString::number(rasot)+")*f("+QString::number(rasdo)+")<0, то корень лежит в пределах ["+QString::number(rasot)+";"+QString::number(rasdo)+"]"
                    "\nВычисляем занчения функции в точке а="+QString::number(rasot)+""
                    "\nf("+QString::number(rasot)+")="+QString::number(fun(rasot))+""
                    "\nf''("+QString::number(rasot)+")="+QString::number(derv2_f(rasot,sbtoch));
            if (fun(rasot)*derv2_f(rasot,sbtoch)>0) {
                otvet = otvet+"\nПоскольку f(a)*f''(a)>0, то x0=a="+QString::number(rasot);
                x0 =rasot;
            }
            else if (fun(rasot)*derv2_f(rasot,sbtoch)<0) {
                otvet = otvet+"\nПоскольку f(a)*f''(a)>0, то x0=b="+QString::number(rasdo);
                x0 =rasdo;
            }
            rez->setText(oldrez+"\nВыбран метод Ньютона(касательных)"+otvet);
            //Table
            QTableWidget * trez = new QTableWidget;
            trez->setColumnCount(4);
            trez->setRowCount(1);
            trez->setStyleSheet("border:none");
            trez->setHorizontalHeaderLabels(QStringList()<<"x"<<"f(x)"<<"f'(x)"<<"f(x)/f'(x)");
            double xn=x0,fx,dfx,h,x;
            int ni=0;
            do{
                fx=fun(xn);
                dfx=derv1_f(xn,sbtoch);
                h=fx/dfx;
                trez->setRowCount(ni+1);
                QTableWidgetItem * itmxn = new QTableWidgetItem;
                QTableWidgetItem * itmfx = new QTableWidgetItem;
                QTableWidgetItem * itmdfx = new QTableWidgetItem;
                QTableWidgetItem * itmh = new QTableWidgetItem;
                itmxn->setText(QString::number(xn));
                itmfx->setText(QString::number(fx));
                itmdfx->setText(QString::number(dfx));
                itmh->setText(QString::number(h));
                trez->setItem(ni, 0, itmxn);
                trez->setItem(ni, 1, itmfx);
                trez->setItem(ni, 2, itmdfx);
                trez->setItem(ni, 3, itmh);
                x=xn;
                xn=xn-h;
                ni++;
            }while (fabs(h)>sbtoch);
            //Убираю скролл у таблиц
            int ht = (trez->model()->rowCount()-1)+trez->horizontalHeader()->height();
            int wt = (trez->model()->columnCount()-1)+trez->verticalHeader()->width();
            for(int row = 0; row < trez->model()->rowCount(); row++)
            ht = ht + trez->rowHeight(row);
            for(int column = 0; column < trez->model()->columnCount(); column++)
            wt = wt + trez->columnWidth(column);
            trez->setMinimumHeight(ht);
            trez->setMinimumWidth(wt);
            lay->addWidget(trez);
            QLabel * rezend = new QLabel;
            rezend->setTextInteractionFlags(Qt::TextSelectableByMouse);
            rezend->setText("Искомый корень x≈"+QString::number(x)+" Вычисления проводились с точностью "+QString::number(sbtoch));
            lay->addWidget(rezend);
        }
        break;
        }
    }
    ui->scrollContents->setLayout(lay);
    }
}

void MainWindow::clearscroll(){
      qDeleteAll(ui->scrollContents->children());
      ui->scrollContents->resize(0,0);
}
void MainWindow::showAbout(){
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
