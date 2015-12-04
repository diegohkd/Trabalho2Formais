#include "contextfreegrammar.h"

ContextFreeGrammar::ContextFreeGrammar(){
}

QList<QStringList> ContextFreeGrammar::getProductions(){
    return productions;
}

void ContextFreeGrammar::setProductions(QList<QStringList > productions){
    this->productions = productions;
    createAndSetNonTerminals();
    createAndSetTerminals();
}

// Inicializa conjunto de terminais
void ContextFreeGrammar::createAndSetTerminals(){
    Vt.clear();
    for(int i = 0; i < productions.length(); i++){
        for(int j = 1; j < productions[i].length(); j++){
            QStringList current_productions = productions[i][j].split(" ");
            for(int k = 0; k < current_productions.length(); k++){
                QString symbol = current_productions[k];
                //if(!(symbol.length() == 1 && symbol[0].isLetter() && symbol[0].isUpper()) && symbol.compare("&") != 0){
                if(!symbol[0].isUpper() && symbol[0] != '&'){
                    if(!Vt.contains(current_productions[k]))
                        Vt.push_back(current_productions[k]);
                }
            }
        }
    }
}

// inicializa conjunto de não terminais
void ContextFreeGrammar::createAndSetNonTerminals(){
    Vn.clear();
    for(int i = 0; i < productions.length(); i++){
        if(!Vn.contains(productions[i][0]))
            Vn.push_back(productions[i][0]);
    }
}

QStringList ContextFreeGrammar::getVt(){
    return Vt;
}

QStringList ContextFreeGrammar::getVn(){
    return Vn;
}

// busca posição de não-terminal na lista de produções
int ContextFreeGrammar::findNtIndex(QString nt){
    for(int i = 0; i < productions.length(); i++){
        if(productions[i][0].compare(nt) == 0)
            return i;
    }
    //not found
    return -1;
}

bool ContextFreeGrammar::productionContainsEmptySentence(QString left_side){
    int index = findNtIndex(left_side);
    for(int i = 1; i < productions[index].length(); i++){

        // se existe produção para &, então retorna true
        if(productions[index][i].compare("&") == 0){
            return true;
        }else{

            // verifica se deriva epsilon indiretamente
            QStringList current_production = productions[index][i].split(" ");
            if(Vn.contains(current_production[0]) && current_production[0].compare(left_side) != 0){

                // verifica cada simbolo da produção buscando derivação de epsilon indireta
                bool derivate_empty_sentence = true;
                int last_iteration;
                for(int j = 0; j < current_production.length() && derivate_empty_sentence == true ; j++){
                    QString current_symbol = current_production[j];
                    if(Vn.contains(current_symbol)){
                        derivate_empty_sentence = productionContainsEmptySentence(current_symbol);
                        if(derivate_empty_sentence == false)
                            return false;
                    }else
                        return false;
                    last_iteration = j;
                }
                if(last_iteration + 1 == current_production.length()){
                    return true;
                }
            }
        }
    }
    return false;

    /*int index = findNtIndex(left_side);
    for(int i = 1; i < productions[index].length(); i++){
        if(productions[index][i].compare("&") == 0)
            return true;
    }
    return false;*/
}

bool ContextFreeGrammar::productionContainsDirectEmptySentence(QString left_side){
    int index = findNtIndex(left_side);
        for(int i = 1; i < productions[index].length(); i++){
            if(productions[index][i].compare("&") == 0)
                return true;
        }
        return false;
}
