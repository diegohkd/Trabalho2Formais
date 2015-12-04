#include "mainwindow.h"
#include "ui_mainwindow.h"

//delete
#include "contextfreegrammar.h"
#include <QStringList>
#include <QList>
#include "controller.h"
#include <QFileDialog>
#include <qfile.h>
#include <qtextstream.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ll1_clicked()
{
    QString text = ui->plainTextEdit_GLC->toPlainText();

    if(text.compare("") != 0){
        QList<QStringList> result = c.isLL1(text);
        ui->textEdit_passos_interm->clear();
        for(int i=1; i < result.length();i++){
            for(int j=0; j < result[i].length();j++){
                ui->textEdit_passos_interm->insertPlainText(result[i][j]+"\n");
            }
        }
    }
}

void MainWindow::on_pushButton_parser_clicked()
{
    QString text = ui->plainTextEdit_GLC->toPlainText();

    if(text.compare("") != 0){
        ui->textEdit_passos_interm->clear();
        QList<QStringList> result = c.createParser(text);
        for(int i=1; i < result.length(); i++){
            for(int j = 0; j < result[i].length(); j++){
                ui->textEdit_passos_interm->insertPlainText(result[i][j]    +"\n");
            }
        }
    }
}

void MainWindow::on_pushButton_anal_sintatica_clicked()
{
    QList<QStringList> result;
    QString text = ui->plainTextEdit_GLC->toPlainText();
    QString sentence = ui->lineEdit_sentence->text();

    if(text.compare("") != 0 && sentence.compare("") != 0){
        ui->textEdit_passos_interm->clear();
        result = c.syntaxAnalysis(text, sentence);
        for(int i=1; i < result.length(); i++){
            for(int j = 0; j < result[i].length(); j++){
                ui->textEdit_passos_interm->insertPlainText(result[i][j]    +"\n");
            }
        }
    }
}

void MainWindow::on_actionSalvar_triggered()
{
    /*QString filename = QFileDialog::getSaveFileName(this, tr("Salvar"),QDir::currentPath(),tr("(*.tf)") );
    if( !filename.isNull() )
    {
        if(filename.contains(".tf") == false)
            filename += ".tf";
        QFile file(filename);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;

            QTextStream out(&file);
            out << ui->plainTextEdit_GLC->toPlainText();
    }*/
    QString filename = QFileDialog::getSaveFileName(this, tr("Salvar"),QDir::currentPath(),tr("(*.tf)") );
    if( !filename.isNull() )
    {
        if(filename.contains(".tf") == false)
            filename += ".tf";
        QFile file(filename);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;

            QTextStream out(&file);
            out << "<Gramatica Livre de Contexto>\n";
            out << ui->plainTextEdit_GLC->toPlainText() + "\n";
            out << "<Sentenca>\n";
            out << ui->lineEdit_sentence->text() + "\n";
    }
}

void MainWindow::on_actionAbrir_triggered()
{

    QString path = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir arquivo"), path, tr("(*.tf)"));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){

    }
    QTextStream stream ( &file );
    QString line;
    QString cfg = "", sentence = "";
    bool reading_CFG = false, reading_sentence = false;
    while(!stream.atEnd()) {
         line = stream.readLine();
         if(reading_CFG == true && line.compare("<Sentenca>") != 0){
             cfg += line + "\n";
         }
         if(reading_sentence == true && line.compare("<Gramatica Livre de Contexto>") != 0){
             sentence += line + "\n";
         }
         if(line.compare("<Gramatica Livre de Contexto>") == 0){
             reading_CFG = true;
             reading_sentence = false;
         }
         if(line.compare("<Sentenca>") == 0){
             reading_sentence = true;
             reading_CFG = false;
         }
    }
    file.close(); // when your done.

    sentence.remove(sentence.length()-1, 2);
    cfg.remove(cfg.length()-1, 2);

    sentence.replace("\n\n", "");
    cfg.replace("\n\n", "\n");
    //sentence.replace(" ", "");
    //cfg.replace(" ", "");
    cfg.remove(QRegExp("\n$"));

    if(sentence.length() > 0){
        ui->lineEdit_sentence->clear();
        ui->lineEdit_sentence->insert(sentence);
    }
    if(cfg.length() > 0){
        ui->plainTextEdit_GLC->clear();
        ui->plainTextEdit_GLC->insertPlainText(cfg);
    }
}
    /*
    QString path = QDir::currentPath();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir arquivo"), path, tr("(*.tf)"));

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){

    }
    QTextStream stream ( &file );
    QString line;
    QString cfg = "";
    while(!stream.atEnd()) {
         line = stream.readLine();
         cfg += line + "\n";
    }
    file.close(); // when your done.

    cfg.replace("\n\n", "");
    cfg.remove(QRegExp("\n$"));

    if(cfg.length() > 0){
        ui->plainTextEdit_GLC->clear();
        ui->plainTextEdit_GLC->insertPlainText(cfg);
    }
}*/
