#ifndef RECURSIVEDESCENTPARSER_H
#define RECURSIVEDESCENTPARSER_H

#include <QHash>
#include <QString>
#include <QStringList>

class RecursiveDescentParser
{
private:
    QHash<QString, QStringList> procedures;
public:
    RecursiveDescentParser();
    void setProcedures(QHash<QString, QStringList>);
    QHash<QString, QStringList> getProcedures();
};

#endif // RECURSIVEDESCENTPARSER_H
