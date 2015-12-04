#include "recursivedescentparser.h"

RecursiveDescentParser::RecursiveDescentParser()
{
}

void RecursiveDescentParser::setProcedures(QHash<QString, QStringList> procedures){
    this->procedures = procedures;
}

QHash<QString, QStringList> RecursiveDescentParser::getProcedures(){
    return procedures;
}
