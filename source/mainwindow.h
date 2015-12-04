#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"
#include <QMainWindow>

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
    void on_pushButton_ll1_clicked();

    void on_pushButton_parser_clicked();

    void on_pushButton_anal_sintatica_clicked();

    void on_actionSalvar_triggered();

    void on_actionAbrir_triggered();

private:
    Ui::MainWindow *ui;
    Controller c;
};

#endif // MAINWINDOW_H
