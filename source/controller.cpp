#include "controller.h"

Controller::Controller(){
}

// realiza análise sintática e retorna passos intermediários
QList<QStringList> Controller::syntaxAnalysis(QString text, QString sentence){

    // tenta criar parser
    QList<QStringList> result = createParser(text);
    QStringList activations_sequence;

    // se parser foi criado com sucesso
    if(result[0][0].compare("true") == 0 ){
        int *x = new int();                                                     // posição do símbolo atual na sentença
        QHash<QString, QStringList> all_procedures = parser.getProcedures();    // hash que armazena todos procedimentos do parser
        QStringList list_sentence = sentence.split(" ");                        // sentence armazenada em uma lista

        // adiciona $ no final da sentença
        list_sentence.push_back("$");

        // busca instrução no procedimento main que chama o NT inicial
        QStringList main = all_procedures.value("main");
        QString instruction = main[2];
        instruction.remove(QRegExp("^      "));

        // NT initial que será chamado no procedimento main
        QString initial = instruction.split(" ")[0];

        // busca procedimento do NT inicial
        QStringList procedure = all_procedures.value(initial);

        // chama procedimento de NT inicial
        *x = 0;
        activations_sequence.push_back("Chamando " + initial + " (x)");
        activations_sequence = runProcedure(x, procedure, activations_sequence, list_sentence, all_procedures);

        // verifica se sentença é válida
        if(list_sentence[0].compare("&") == 0){
            QStringList set_first = first(initial);
            if(set_first.contains("&")){
                if(activations_sequence.last().compare("ERRO!") == 0)
                    activations_sequence.pop_back();
                activations_sequence.push_back("SENTENÇA VÁLIDA!");
            }
        }else{
            if(list_sentence[*x] == "$"){
                activations_sequence.push_back("SENTENÇA VÁLIDA!");
            }
            else{
                if(activations_sequence.last().compare("ERRO!") == 0)
                    activations_sequence.pop_back();
                activations_sequence.push_back("ERRO: Fim esperado");
            }
        }
        activations_sequence.push_front("\n##### ANÁLISE SINTÁTICA #####\n\nChamando main (x)");
    }

    result.push_back(activations_sequence);
    return result;
}

// executa procedimento fazendo análise sintática e retorna passos intermediários
QStringList Controller::runProcedure(int *x, QStringList procedure, QStringList activations_sequence,
                                                  QStringList sentence, QHash<QString, QStringList> all_procedures){
    // inicia execução de procedimento
    int i = 1;
    while(i < procedure.length() && activations_sequence.last().compare("ERRO!") != 0 && *x < sentence.length()){
        bool matches = false;

        // instrução é do tipo "ALEX(x)"
        if(procedure[i].contains("ALEX(x)") && matches == false){
            matches = true;

            activations_sequence.push_back("Reconhece " + sentence[*x]);

            // reconhece x e busca próximo símbolo da sentença
            (*x)++;
        }

        // instrução é do tipo "else"
        if(procedure[i].contains("else") && matches == false){
            matches = true;

            // se chegou aqui é pq a execução do if correspondente a este else
            // não teve ERRO! e foi executado até a última linha
            // o else só é realmente executado qd a condição do if é falsa
            // e o if da um jump para a linha seguinte ao else
            return activations_sequence;
        }

        // instrução é do tipo "if x = 'a' then"
        if(procedure[i].contains("if x = ") && matches == false){
            matches = true;

            // busca terminal 'a'
            QString temp = procedure[i].split("if x = '")[1];
            QString terminal = temp.split("' then")[0];

            // se x == 'a'
            if(sentence[*x] != terminal){

                // busca else correspondente a este if
                int position = procedure[i].indexOf("if x = ");
                int j = i;
                for(j = i; j < procedure.length(); j++){

                    // se encontrou um else candidato ao else correto
                    if(procedure[j].contains("else")){

                        // se else está na mesma posição do if, isto é, é o else correto
                        int position_else = procedure[j].indexOf("else");
                        if(position_else == position){
                            i = j;

                            // se else leva para erro, retorna
                            if(procedure[i+1].contains("ERRO!")){
                                activations_sequence.push_back("ERRO!");
                                return activations_sequence;
                            }
                            break;
                        }
                    }
                }
                // se não achou else, então retorna
                if(j == procedure.length())
                    return activations_sequence;
            }
        }

        // instrução é do tipo "if x ∈ FIRST(sequence)"
        if(procedure[i].contains("if x ∈ FIRST") && matches == false){
            matches = true;

            // busca sequência na instrução
            QString temp = procedure[i].split("if x ∈ FIRST(")[1];
            QString sequence = temp.split(")")[0];

            // busca conjunto first da sequência
            QStringList seq_first = first(sequence);

            // se x pertence ao first da sequência
            if(seq_first.contains(sentence[*x]) == false){

                // busca else correspondente a este if
                int position = procedure[i].indexOf("if x ∈ FIRST");
                int j = i;
                for(j = i; j < procedure.length(); j++){

                    // se encontrou um else candidato ao else correto
                    if(procedure[j].contains("else")){

                        // se else está na mesma posição do if, isto é, é o else correto
                        int position_else = procedure[j].indexOf("else");
                        if(position_else == position){
                            i = j;

                            // se else leva para erro, retorna
                            if(procedure[i+1].contains("ERRO!")){
                                activations_sequence.push_back("ERRO!");
                                return activations_sequence;
                            }
                            break;
                        }
                    }
                }
                // se não achou else, então retorna
                if(j == procedure.length())
                    return activations_sequence;
            }
        }

        // se intrução é do tipo "ERRO!"
        if(procedure[i].contains("ERRO!") && matches == false){
            matches = true;

            // ignora, pois o ERRO! só é efetivamente executado quando se chega em um else
        }

        // se instrução é do tipo A (x)
        if(matches == false){
            // busca 'A' na instrução
            QString nonterminal = procedure[i];
            nonterminal.remove(" ");
            nonterminal.remove("(x)");

            // busca procedimento de 'A' e executa
            QStringList next_procedure = all_procedures.value(nonterminal);
            activations_sequence = runProcedure(x, next_procedure, activations_sequence, sentence, all_procedures);

            activations_sequence.push_back("Chamando " + nonterminal + " (x)");
        }

        i++;
    }

    return activations_sequence;
}

// tenta criar parser e retorna passos intermediários
QList<QStringList> Controller::createParser(QString text){
    createGLC(text);

    QList<QStringList> result = isLL1(text);
    QStringList ret;

    // se gramática é LL(1) então cria parser
    if(result[0][0].compare("true") == 0){
        QList<QStringList> productions = cfg.getProductions();
        QStringList current_productions;
        QStringList current_production;
        QHash<QString, QStringList> procedures;
        QStringList procedure, main;
        QString tab = "      ";
        QStringList Vn = cfg.getVn();

        // cria procedimento main
        main.push_back("procedure main ()");
        main.push_back(tab + "ALEX(x)");
        main.push_back(tab + productions[0][0] + " (x)");
        main.push_back(tab + "if x = '$' then");
        main.push_back(tab + tab + "OK!");
        main.push_back(tab + "else");
        main.push_back(tab + tab + "ERRO: (\"Fim esperado\")");

        procedures.insert("main", main);

        // cria procedimentos de cada não terminal
        for(int i = 0; i < productions.length(); i++){
            current_productions = productions[i];
            QString current_NT = current_productions[0];
            current_productions.pop_front();
            procedure.clear();

            // TODO - Criar função sort para garantir que todo terminal vem antes de não-terminal
            // ordena produções para criar primeiro instruções de produções que começam com terminal
            current_productions.sort(Qt::CaseInsensitive);

            procedure.push_back("procedure " + current_NT + " (x)");

            // cria if's (possivelmente) aninhados para as produções
            QStack<QString> all_tabs;
            QString c_tab;
            all_tabs.push(tab);
            for(int j = 0; j < current_productions.length(); j++){
                QString current_symbol = (QString)current_productions[j][0];
                if(current_symbol.compare("&") != 0){

                    current_production = current_productions[j].split(" ");

                    bool first_is_nonterminal = false;
                    // se PRIMEIRO símbolo da produção é não-terminal
                    if(Vn.contains(current_symbol) == true){
                        first_is_nonterminal = true;
                    }

                    // itera por todos símbolos da produção atual adicionando suas instruções ao procedimento
                    int n_inner_if = 0;
                    for(int k = 0; k < current_production.length(); k++){
                        c_tab = all_tabs.top();
                        current_symbol = (QString)current_production[k];

                        // se símbolo atual for terminal
                        if(Vn.contains(current_symbol) == false){
                            procedure.push_back(c_tab + "if x = '" + current_symbol + "' then");

                            c_tab += tab;
                            all_tabs.push(c_tab);

                            procedure.push_back(c_tab  + "ALEX(x)");
                            n_inner_if++;
                        }else{
                            // símbolo é não-terminal

                            // se o não-terminal é o primeiro símbolo da produção
                            if(first_is_nonterminal == true){
                                c_tab = all_tabs.top();
                                procedure.push_back(c_tab + "if x ∈ FIRST(" + current_productions[j] + ") then");
                                c_tab += tab;
                                all_tabs.push(c_tab);
                                procedure.push_back(c_tab + current_symbol + " (x)");
                                n_inner_if++;
                                first_is_nonterminal = false;
                            }else
                                procedure.push_back(c_tab + current_symbol + " (x)");
                        }
                        // se final de produção, remove tab da pilha e adiciona else's com erro
                        if(k == current_production.length() - 1){
                            while(n_inner_if > 1){
                                all_tabs.pop();
                                c_tab = all_tabs.top();
                                procedure.push_back(c_tab + "else");
                                procedure.push_back(c_tab + tab + "ERRO!");
                                n_inner_if--;
                            }
                        }
                    }

                    // adiciona else se existem mais produções a serem verificadas
                    if(j < current_productions.length() - 1){
                        all_tabs.pop();
                        c_tab = all_tabs.top();
                        procedure.push_back(c_tab + "else");
                        all_tabs.push(c_tab + tab);
                    }
                }
            }

            if(productions[i].contains("&") == false){
                all_tabs.pop();
                c_tab = all_tabs.top();
                procedure.push_back(c_tab + "else");
                procedure.push_back(c_tab + tab + "ERRO!");
            }

            procedures.insert(productions[i][0], procedure);
        }
        parser.setProcedures(procedures);

        main.push_front("\n##### PARSER #####\n");
        ret += main;
        for(int i=0; i < Vn.length(); i++){
            QStringList pro = procedures.value(Vn[i]);
            ret.push_back("\n");
            for(int j=0; j < pro.length(); j++){
                ret.push_back(pro[j]);
            }
        }
        result.push_back(ret);
    }
    return result;
}

// verifica se gramática é LL1 e retorna lista com passos intermediários
// em que o primeiro item diz se é ou não LL1
QList<QStringList> Controller::isLL1(QString text){
    QList<QStringList> result_interm;

    QList<QStringList> result;
    createGLC(text);

    // verifica se tem recursão a esquerda
    result = containsLeftRecursion();
    if(!result.isEmpty()){
        QStringList result_out;
        QString r = "Contem recursão a esquerda:";
        result_out.push_back(r);
        for(int i=0; i < result.length();i++){
            QString in;
            in = result[i][0]+" = \t{ ";
            for(int j=1; j < result[i].length();j++){
                in = in+result[i][j]+" ";
            }
            in=in+"}";
            result_out.push_back(in);
        }
        result_out.push_back("\nA gramática não é LL(1)!");
        result_interm.push_back(result_out);

        QStringList boo;
        QString x = "false";
        boo.push_back(x);
        result_interm.push_front(boo);

        return result_interm;
    }
    // verifica se não está fatorada
    result = isCFGFactored();
    if(!result.isEmpty()){
        QStringList result_out;
        QString r = "Não está fatorada, possui não-determinismo:";
        result_out.push_back(r);
        for(int i=0; i < result.length();i++){
            QString in;
            in = result[i][0]+" = \t{ ";
            for(int j=1; j < result[i].length();j++){
                in = in+result[i][j]+" ";
            }
            in=in+"}";
            result_out.push_back(in);
        }
        result_out.push_back("\nA gramática não é LL(1)!");
        result_interm.push_back(result_out);

        QStringList boo;
        QString x = "false";
        boo.push_back(x);
        result_interm.push_front(boo);

        return result_interm;
    }

    // verifica terceira condição para ser LL1
    QList<QStringList> productions = cfg.getProductions();
    QList<QStringList> all_firsts = createSetOfAllFirst();
    QList<QStringList> all_follows = follow();
    bool is_not_LL1 = false;
    for(int i = 0; i < productions.length(); i++){
        for(int j = 0; j < all_follows[i].length(); j++){
            if(all_firsts[i].contains("&") && all_firsts[i].contains(all_follows[i][j]))
                is_not_LL1 = true;
        }
    }

    QStringList result_output;
    QString f = "FIRST:";
    result_output.push_back(f);
    for(int i=0; i<productions.length(); i++){
        QString prod = productions[i][0]+" = \t{ ";
        for(int j = 0; j < all_firsts[i].length(); j++){
            prod = prod+all_firsts[i][j]+" ";
        }
        prod = prod+"}";
        result_output.push_back(prod);
    }
    QString fl = "\nFOLLOW:";
    result_output.push_back(fl);
    for(int i=0; i<productions.length(); i++){
        QString prod = productions[i][0]+" = \t{ ";
        for(int j = 0; j < all_follows[i].length(); j++){
            prod = prod+all_follows[i][j]+" ";
        }
        prod = prod+"}";
        result_output.push_back(prod);
    }

    QStringList boo;
    if(is_not_LL1 == true){
        result_output.push_back("A terceira condição falhou. Não é LL(1)!");
        QString x = "false";
        boo.push_back(x);
        result_interm.push_front(boo);
    }else{
        result_output.push_back("\nA Gramática é LL(1)!");
        QString x = "true";
        boo.push_back(x);
        result_interm.push_front(boo);
    }
    result_interm.push_back(result_output);

    return result_interm;
}

// busca conjunto fist de uma sequência
QStringList Controller::first(QString sequence){
    QStringList Vt = cfg.getVt();
    QStringList Vn = cfg.getVn();

    // busca first de todos NT's para auxiliar na busca do first da sequencia
    QList<QStringList> all_first = createSetOfAllFirst();

    QStringList first;
    QStringList seq = sequence.split(" ");

    // se primeiro símbolo é terminal, só first é só o símbolo
    if(Vt.contains(seq[0]))
        first.push_back(seq[0]);
    else{
        // primeiro símbolo é NT, então busca first de NT e possivelmente
        // first dos elementos que seguem na sequência

        QString current_symbol = seq[0];
        int i = 0;
        bool contains_empty_sentence = true;
        while(i < seq.length() && Vn.contains(current_symbol) && contains_empty_sentence){
            int index = cfg.findNtIndex(current_symbol);
            for(int j = 0; j < all_first[index].length(); j++){
                if(!first.contains(all_first[index][j]))
                    first.push_back(all_first[index][j]);
            }
            i++;
            if(i < seq.length() && first.contains("&"))
                current_symbol = seq[i];
            else
                contains_empty_sentence = false;
        }
        if(Vt.contains(current_symbol) && !first.contains(current_symbol))
            first.push_back(current_symbol);
    }
    return first;
}

// busca conjunto follow de todos NT's
QList<QStringList> Controller::follow(){
    QList<QStringList> all_productions = cfg.getProductions();
    QStringList Vt = cfg.getVt();
    QStringList Vn = cfg.getVn();
    QList<QStringList> follow;
    QList<QStringList> intermediate;

    //monta a tabela
    for(int i = 0; i < Vn.length(); i++){
        QStringList line;
        follow.push_back(line);
    };
    //Passo 1
    QString cifrao = "$";
    follow[0].push_back(cifrao);

    QStringList fResult;
    QString f;
 //  Passo 2
    for(int i = 0; i < Vn.length(); i++) {
        QStringList line = all_productions[i];

        for(int k = 1; k < line.length(); k++){
            QString prod = line[k];
            QStringList current_production = prod.split(" ");

            for(int l = 0; l < current_production.length();l++){
                QString current_symbol = current_production[l];

                bool isNt = Vn.contains(current_symbol);

                if(isNt && l != current_production.length()-1){
                    f = "";

                    f.push_back(current_production[l+1]);
                    int index = cfg.findNtIndex(current_symbol);

                    for(int j = l+2; j < current_production.length(); j++){
                        f.push_back(" "+current_production[j]);
                    }
                    fResult = first(f);
                    for(int c = 0; c < fResult.length();c++){
                        if(!follow[index].contains(fResult[c]) && !fResult[c].contains("&")){
                            if(Vn.contains(fResult[c]) == false)
                                follow[index].push_back(fResult[c]);
                        }
                    }
                }
            }
        }
    }

    //Passo 3
    //verifica se o follow mudou
    while(intermediate != follow){
        intermediate = follow;

        //pega linha
        for(int i = 0; i < Vn.length(); i++){
             QStringList line = all_productions[i];

             //pega produção
             for(int k = 1; k < line.length(); k++){
                QString prod = line[k];
                QStringList current_production = prod.split(" ");

                //pega do ultimo simbolo até o simbolo que satisfaça a condição de ser NT e o first conter epslon
                int lastSymbol = current_production.length()-1;
                bool eEmptyS = true;
                bool isVn = Vn.contains(current_production[lastSymbol]);
                while(isVn && eEmptyS){
                    int index = cfg.findNtIndex(current_production[lastSymbol]);

                    //coloca em follow do NT da linha, o follow do lastSymbol NT; follow[index] = B follow[i] = A A <- B
                    for(int c = 0; c < follow[i].length();c++){
                        QStringList B = follow[i];
                        QString symbol = B[c];
                        if(!follow[index].contains(symbol)) {
                            follow[index].push_back(symbol);
                        }
                    }
                    //verifica se epson esta contido em first de lastSymbol NT
                    if(!first(current_production[lastSymbol]).contains("&")){
                        eEmptyS = false;
                    }
                    lastSymbol--;
                    if(lastSymbol >= 0){
                        isVn = Vn.contains(current_production[lastSymbol]);
                    } else {
                        isVn = false;
                    }
                }
             }
        }
    }

   return follow;
}

// retorna o conjunto firstNT de nt incluindo nt como o primeiro elemento
QStringList Controller::firstNT(QStringList all_first_NT, QStringList *pending_NT, QString nt){
    int current_index;
    QList<QStringList> all_productions;
    QStringList all_NTs;

    current_index = cfg.findNtIndex(nt);
    all_productions = cfg.getProductions();
    all_NTs = cfg.getVn();

    // busca pelos firstNT's do NT atual
    for(int i = 1; i < all_productions[current_index].length(); i++){
        QStringList current_production = all_productions[current_index][i].split(" ");
        QString first_element = (QString)current_production[0];

        // se primeiro elemento da produção é NT
        if(all_NTs.contains(first_element)){
            if(!pending_NT->contains(first_element) && !all_first_NT.contains(first_element))
                pending_NT->push_back(first_element);

            // se primeiro elemento tem uma produção para a sentença vazia
            QString current_NT = first_element;
            int i_current_prod = 0;
            while(all_NTs.contains(current_NT) && cfg.productionContainsEmptySentence(current_NT) && i_current_prod < current_production.length()){
                if(cfg.productionContainsEmptySentence(current_NT)){
                    i_current_prod++;

                    // se próximo símbolo é NT
                    if(i_current_prod < current_production.length() && all_NTs.contains((QString)current_production[i_current_prod])){
                        current_NT = current_production[i_current_prod];
                        if(!pending_NT->contains(current_NT) && !all_first_NT.contains(first_element))
                            pending_NT->push_back(current_NT);
                    }
                }
            }
        }
    }

    if(!pending_NT->isEmpty()){
        nt = pending_NT->at(0);
        pending_NT->pop_front();
        if(!all_first_NT.contains(nt))
            all_first_NT.push_back(nt);
        all_first_NT = firstNT(all_first_NT, pending_NT, nt);
    }
    return all_first_NT;
}

// cria gramática livre de contexto
void Controller::createGLC(QString text){
    QList<QStringList> gr;
    QStringList list = text.split("\n");
    QStringList prods;
    for(int i = 0; i < list.count(); i++){
        list[i].remove(QRegExp(" *$"));
        //Separa o lado esquerdo do direito
        QStringList e = list[i].split(" -> ");

        //if(e[e.length()-1].compare(" ") == 0)
        //    e[e.length()-1].remove(e[e.length()-1].length()-1, 1);

        //separa as prods do lado direito e coloca no QList<QStringList[i]>
        prods = e[1].split(" | ");
        //coloca na primeira posição o lado esquerdo
        prods.push_front(e[0]);

        gr.push_back(prods);
    }
    cfg.setProductions(gr);
}

// verifica se existe recursão a esquerda e retorna
// uma lista vazia se não tem recursão a esquerda ou uma lista
// com uma lista de NT's que geram recursão a esquerda
// direta e/ou uma lista de NT's que geram recursão a esquerda
// indireta
// Obs: usa a mesma ideia do algoritmo de firstNT
QList<QStringList> Controller::containsLeftRecursion(){
    QList<QStringList> all_productions;
    QStringList all_NTs;
    QList<QStringList> result;
    QStringList direct_recursion;
    QStringList indirect_recursion;

    all_productions = cfg.getProductions();
    all_NTs = cfg.getVn();

    direct_recursion.push_back("Direta");
    indirect_recursion.push_back("Indireta");

    for(int l = 0; l < all_productions.length(); l++){
        QStringList all_first_NT;
        QStringList *pending_NT = new QStringList();
        QString nt;
        bool repeat;
        int current_index;
        QString initial_NT;

        nt = all_productions[l][0];
        initial_NT = nt;
        repeat = true;

        while(repeat == true){
            current_index = cfg.findNtIndex(nt);

            // busca por firstNT's do NT atual
            for(int i = 1; i < all_productions[current_index].length(); i++){
                QStringList current_productions = all_productions[current_index][i].split(" ");
                QString first_element = (QString)current_productions[0];

                // se primeiro elmento da produção é NT
                if(all_NTs.contains(first_element)){

                    // se encontrou recursão a esquerda
                    if(first_element.compare(initial_NT) == 0){
                        // se é recursão a esquerda direta
                        if(nt.compare(initial_NT) == 0){
                            if(!direct_recursion.contains(nt))
                                direct_recursion.push_back(nt);
                        }
                        else{
                            // é recursão a esquerda indireta
                            if(!indirect_recursion.contains(nt))
                                indirect_recursion.push_back(nt);
                        }
                    }

                    if(!pending_NT->contains(first_element) && !all_first_NT.contains(first_element))
                        pending_NT->push_back(first_element);

                    // se primeiro elemento tem uma produção para sentença vazia
                    QString current_NT = first_element;
                    int i_current_prod = 0;
                    while(all_NTs.contains(current_NT) && cfg.productionContainsDirectEmptySentence(current_NT) && i_current_prod < current_productions.length()){
                        if(cfg.productionContainsDirectEmptySentence(current_NT)){
                            i_current_prod++;

                            // se próximo símbolo é NT
                            if(i_current_prod < current_productions.length() && all_NTs.contains((QString)current_productions[i_current_prod])){
                                current_NT = current_productions[i_current_prod];

                                // se encontrou recursão a esquerda indireta
                                if(current_NT.compare(initial_NT) == 0){
                                    if(!indirect_recursion.contains(nt))
                                        indirect_recursion.push_back(nt);
                                }
                                if(!pending_NT->contains(current_NT) && !all_first_NT.contains(current_NT))
                                    pending_NT->push_back(current_NT);
                            }
                        }
                    }
                }
            }

            if(!pending_NT->isEmpty()){
                nt = pending_NT->at(0);
                pending_NT->pop_front();
                if(!all_first_NT.contains(nt))
                    all_first_NT.push_back(nt);
            }else
                repeat = false;
        }
    }
    if(direct_recursion.length() > 1)
        result.push_back(direct_recursion);
    if(indirect_recursion.length() > 1)
        result.push_back(indirect_recursion);
    return result;
}

// verifica se a gramática está fatorada e retorna
// uma lista vazia se está fatorada ou uma lista
// com uma lista de terminais que geram não-determinismo
// direto e/ou uma lista de terminais que geram não-determinismo
// indireto
// Obs: usa a mesma ideia do algoritmo de first
QList<QStringList> Controller::isCFGFactored(){
    QList<QStringList> all_productions = cfg.getProductions();
    QList<QStringList> all_first;
    QStringList Vt = cfg.getVt();
    QStringList Vn = cfg.getVn();

    QList<QStringList> result;
    QStringList direct_non_determinism;
    QStringList indirect_non_determinism;
    direct_non_determinism.push_back("Direto");
    indirect_non_determinism.push_back("Indireto");

    // busca first de cada não-terminal
    for(int i = 0; i < all_productions.length(); i++){
        QStringList temp;
        temp.push_back(all_productions[i][0]);
        QStringList first_NT = firstNT(temp, new QStringList(), all_productions[i][0]);

        QStringList current_first;
        QString initial_NT = all_productions[i][0];
        int index;
        // adiciona first de todos os first_NT do não-terminal atual
        for(int j = 0; j < first_NT.length(); j++){
            index = cfg.findNtIndex(first_NT[j]);

            // verifica cada produção de um first_NT
            for(int k = 1; k < all_productions[index].length(); k++){
                QString production = all_productions[index][k];
                QStringList current_productions = production.split(" ");
                QString first_element = current_productions[0];

                // verifica se o primeiro elemento da produção é terminal
                if(!Vn.contains(first_element)){

                     if(!current_first.contains(first_element)){
                         if(first_element.compare("&") == 0){
                             if(first_NT[j].compare(all_productions[i][0]) == 0)
                                 current_first.push_back(first_element);
                         }else
                             current_first.push_back(first_element);
                     }else{
                         // se não for & então foi encontrado não fatoração
                         if(first_element.compare("&") != 0){
                             if(j == 0){
                                if(!direct_non_determinism.contains(first_NT[0]))
                                    direct_non_determinism.push_back(first_NT[0]);
                             }else{
                                 if(!indirect_non_determinism.contains(first_NT[0]))
                                     indirect_non_determinism.push_back(first_NT[0]);
                             }
                         }
                     }
                }else{
                    // caso o primeiro elemento da produção seja não terminal, então verifica se o não terminal
                    // ou uma sequencia de não terminais geram palavra vazia
                    QString current_NT = first_element;
                    int i_current_prod = 0;
                    bool found_terminal = false;
                    while(Vn.contains(current_NT) && cfg.productionContainsEmptySentence(current_NT)
                          && i_current_prod < current_productions.length() && found_terminal == false){
                        if(cfg.productionContainsEmptySentence(current_NT)){
                            i_current_prod++;
                            if(i_current_prod < current_productions.length()){
                                // if next character is nonterminal
                                if(Vn.contains(current_productions[i_current_prod])){
                                    current_NT = current_productions[i_current_prod];
                                }
                                // if next character is terminal
                                if(Vt.contains(current_productions[i_current_prod])){
                                    found_terminal = true;
                                    if(!current_first.contains(current_productions[i_current_prod]))
                                        current_first.push_back(current_productions[i_current_prod]);
                                    else{
                                        // se não for & então foi encontrado não fatoração
                                        if(first_element.compare("&") != 0){
                                            if(j == 0){
                                               if(!direct_non_determinism.contains(first_NT[0]))
                                                   direct_non_determinism.push_back(first_NT[0]);
                                            }
                                            else{
                                                if(!indirect_non_determinism.contains(first_NT[0]))
                                                    indirect_non_determinism.push_back(first_NT[0]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // se a produção possui somente não terminais que geravam a palavra vazia
                    if(i_current_prod == current_productions.length()){
                        if(!current_first.contains("&") && first_NT[j].compare(all_productions[i][0]) == 0)
                            current_first.push_back("&");
                    }
                }
            }
        }
        all_first.push_back(current_first);
        current_first.clear();
    }
    if(direct_non_determinism.length() > 1)
        result.push_back(direct_non_determinism);
    if(indirect_non_determinism.length() > 1)
        result.push_back(indirect_non_determinism);
    return result;
}

// cria conjunto first de todos NT's
QList<QStringList> Controller::createSetOfAllFirst(){
    QList<QStringList> all_productions = cfg.getProductions();
    QList<QStringList> all_first;
    QStringList Vt = cfg.getVt();
    QStringList Vn = cfg.getVn();

    for(int i = 0; i < all_productions.length(); i++){
        QStringList temp;
        QStringList current_first;
        temp.push_back(all_productions[i][0]);
        QStringList first_NT = firstNT(temp, new QStringList(), all_productions[i][0]);

        if(cfg.productionContainsEmptySentence(all_productions[i][0]) && current_first.contains("&") == false)
            current_first.push_back("&");

        int index;
        // adiciona first de todos os first_NT
        for(int j = 0; j < first_NT.length(); j++){
            index = cfg.findNtIndex(first_NT[j]);

            // verifica cada produção de um first_NT
            for(int k = 1; k < all_productions[index].length(); k++){
                QString production = all_productions[index][k];
                QStringList current_productions = production.split(" ");
                QString first_element = current_productions[0];

                // verifica se o primeiro elemento da produção é terminal
                if(!Vn.contains(first_element) && !current_first.contains(first_element)){
                    if(first_element.compare("&") == 0){
                        if(first_NT[j].compare(all_productions[i][0]) == 0)
                            current_first.push_back(first_element);
                    }else
                        current_first.push_back(first_element);
                }else{
                    // caso o primeiro elemento da produção seja não terminal, então verifica se o não terminal
                    // ou uma sequencia de não terminais geram palavra vazia
                    QString current_NT = first_element;
                    int i_current_prod = 0;
                    bool found_terminal = false;
                    while(Vn.contains(current_NT) && cfg.productionContainsEmptySentence(current_NT)
                          && i_current_prod < current_productions.length() && found_terminal == false){
                        if(cfg.productionContainsEmptySentence(current_NT)){
                            i_current_prod++;
                            if(i_current_prod < current_productions.length()){
                                // if next character is nonterminal
                                if(Vn.contains(current_productions[i_current_prod])){
                                    current_NT = current_productions[i_current_prod];
                                }
                                if(Vt.contains(current_productions[i_current_prod])){
                                    found_terminal = true;
                                    if(!current_first.contains(current_productions[i_current_prod])
                                            && Vn.contains(current_productions[i_current_prod]) == false)
                                        current_first.push_back(current_productions[i_current_prod]);
                                }
                            }
                        }
                    }
                    // se a produção possui somente não terminais que geravam a palavra vazia
                    if(i_current_prod == current_productions.length()){
                        if(!current_first.contains("&") && first_NT[j].compare(all_productions[i][0]) == 0)
                            current_first.push_back("&");
                    }
                }
            }
        }
        all_first.push_back(current_first);
        current_first.clear();
    }
    return all_first;
}
