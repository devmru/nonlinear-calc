#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "muParser/muParser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_bstart_clicked();
    void clearscroll();
    void showAbout();

private:
    Ui::MainWindow *ui;
    mu::Parser p;
    double result;
    QString result_buffer;
    QString expression;
};

#endif // MAINWINDOW_H
