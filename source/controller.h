#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "contextfreegrammar.h"
#include "recursivedescentparser.h"
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QStack>

class Controller
{
private:
    ContextFreeGrammar cfg;
    RecursiveDescentParser parser;

    void createGLC(QString);
    QStringList first(QString);
    QList<QStringList> follow();
    QList<QStringList> containsLeftRecursion();
    QList<QStringList> isCFGFactored();
    QStringList firstNT(QStringList, QStringList*, QString);
    bool containsIndirectLeftRecursion();
    QList<QStringList> createSetOfAllFirst();
    QStringList runProcedure(int*, QStringList, QStringList, QStringList, QHash<QString, QStringList>);
    int findPositionElse(int, QStringList);
    QStringList validateSentence(QString, RecursiveDescentParser, ContextFreeGrammar);
public:
    Controller();
    QList<QStringList> isLL1(QString);
    QList<QStringList> createParser(QString);
    QList<QStringList> syntaxAnalysis(QString, QString);
};

#endif // CONTROLLER_H
