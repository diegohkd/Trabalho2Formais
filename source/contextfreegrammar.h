#ifndef CONTEXTFREEGRAMMAR_H
#define CONTEXTFREEGRAMMAR_H

#include <QList>
#include <QString>
#include <QStringList>

class ContextFreeGrammar
{
private:
    QList<QStringList> productions;
    QStringList Vt;
    QStringList Vn;

    void createAndSetTerminals();
    void createAndSetNonTerminals();
public:
    ContextFreeGrammar();
    QList<QStringList> getProductions();
    void setProductions(QList<QStringList>);
    QStringList getVt();
    QStringList getVn();
    int findNtIndex(QString);
    bool productionContainsEmptySentence(QString);
    bool productionContainsDirectEmptySentence(QString);
};

#endif // CONTEXTFREEGRAMMAR_H
