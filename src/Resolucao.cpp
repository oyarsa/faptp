#include "includes/parametros.h"
#include "Resolucao.h"

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
    init(pBlocosTamanho, pCamadasTamanho, pPerfisTamanho);
}

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho, std::string pSolucaoTxt) {
    solucaoTxt = pSolucaoTxt;

    init(pBlocosTamanho, pCamadasTamanho, pPerfisTamanho);
}

void Resolucao::init(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;
    perfisTamanho = pPerfisTamanho;

    carregarDados();

    initGrasp();
}

void Resolucao::initGrasp() {
    graspVizinhanca = RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS;
    graspVizinhos = RESOLUCAO_GRASP_ITERACAO_VIZINHOS_DEDAULT;

    graspTempoConstrucao = RESOLUCAO_GRASP_TEMPO_CONSTRUCAO_FATOR_DEFAULT;
}

void Resolucao::carregarDados() {
    std::string filename = JSON_INPUT;
    std::ifstream myfile(filename);

    if (myfile.is_open()) {

        myfile >> jsonRoot;
        myfile.close();

        carregarDadosProfessores();
        carregarDadosDisciplinas();
        carregarDadosProfessorDisciplinas();
        carregarAlunoPerfis();
    } else {
        std::cerr << "We had a problem reading file (" << filename << ")\n";
        throw 1;
    }
}

void Resolucao::carregarDadosProfessores() {
    const auto jsonProfessores = jsonRoot["professores"];

    for (auto i = 0; i < jsonProfessores.size(); i++) {
        const auto id = jsonProfessores[i]["id"].asString();
        const auto nome = jsonProfessores[i]["nome"].asString();

        professores[id] = new Professor(nome, id);
    }
}

void Resolucao::carregarDadosDisciplinas() {
    const auto& jsonDisciplinas = jsonRoot["disciplinas"];

    for (auto i = 0; i < jsonDisciplinas.size(); i++) {
        const auto id = jsonDisciplinas[i]["id"].asString();
        const auto nome = jsonDisciplinas[i]["nome"].asString();
        const auto curso = jsonDisciplinas[i]["curso"].asString();
        const auto capacidade = jsonDisciplinas[i]["capacidade"].asInt();

        const auto cargahoraria = jsonDisciplinas[i]["carga"].asInt();
        const auto periodo = jsonDisciplinas[i]["periodo"].asInt();
        const auto turma = jsonDisciplinas[i]["turma"].asString();

        std::vector<std::string> fPreRequisitos;

        Disciplina *disciplina = new Disciplina(nome, cargahoraria, periodo, curso, id, turma, capacidade);

        const auto& prerequisitos = jsonDisciplinas[i]["prerequisitos"];
        for (auto j = 0; j < prerequisitos.size(); j++) {
            disciplina->preRequisitos.push_back(prerequisitos[j].asString());
        }

        const auto& equivalentes = jsonDisciplinas[i]["equivalentes"];
        for (auto j = 0; j < equivalentes.size(); j++) {
            disciplina->equivalentes.push_back(equivalentes[j].asString());
        }
        disciplina->equivalentes.push_back(nome);
        
        disciplinas.push_back(disciplina);
    }

    disciplinas = ordenarDisciplinas();
}

void Resolucao::carregarDadosProfessorDisciplinas() {
    const auto& jsonProfessorDisciplinas = jsonRoot["professordisciplinas"];

    for (auto i = 0; i < jsonProfessorDisciplinas.size(); i++) {
        const auto id = jsonProfessorDisciplinas[i]["id"].asString();
        const auto professor = jsonProfessorDisciplinas[i]["professor"].asString();
        const auto disciplina = jsonProfessorDisciplinas[i]["disciplina"].asString();

        ProfessorDisciplina *professorDisciplina = new ProfessorDisciplina(professores[professor], disciplinas[disciplinasIndex[disciplina]], id);

        /**
         * TODO: recuperar peso do vínculo do professor com a disciplina do arquivo de entrada
         */
        double competenciaPeso = 1.0;
        professorDisciplina->professor->addCompetencia(disciplina, competenciaPeso);

        professorDisciplinas[id] = professorDisciplina;
    }
}

void Resolucao::carregarAlunoPerfis() {
    const auto& jsonAlunoPerfis = jsonRoot["alunoperfis"];

    for (auto i = 0; i < jsonAlunoPerfis.size(); i++) {
        const auto id = jsonAlunoPerfis[i]["id"].asString();
        const auto peso = jsonAlunoPerfis[i]["peso"].asDouble();
        const auto turma = jsonAlunoPerfis[i]["turma"].asString();
        const auto periodo = jsonAlunoPerfis[i]["periodo"].asInt();

        AlunoPerfil *alunoPerfil = new AlunoPerfil(peso, id, turma, periodo);

        const auto& jsonRestantes = jsonAlunoPerfis[i]["restantes"];
        for (auto j = 0; j < jsonRestantes.size(); j++) {
            Disciplina *disciplina = disciplinas[disciplinasIndex[jsonRestantes[j].asString()]];
            alunoPerfil->addRestante(disciplina);
        }
        alunoPerfil->restante = ordenarDisciplinas(alunoPerfil->restante);

        const auto& jsonCursadas = jsonAlunoPerfis[i]["cursadas"];
        for (auto j = 0; j < jsonCursadas.size(); j++) {
            std::string cursada = jsonCursadas[j].asString();
            alunoPerfil->addCursada(cursada);
        }

        alunoPerfis[id] = alunoPerfil;
    }
}

Resolucao::~Resolucao() {

}

void Resolucao::start(int pTipo) {
    start(pTipo, 0);
}

void Resolucao::start(int pTipo, double x) {
    if (solucaoTxt != "") {
        carregarSolucao();
    }

    gerarGrade(pTipo, x);
}

void Resolucao::carregarSolucao() {
    const auto& jsonHorario = jsonRoot["horario"];
    
    Solucao *solucao = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);
    int bloco, dia, camada;
    
    for (auto i = 0; i < jsonHorario.size(); i++) {
        bloco = jsonHorario[i]["horario"].asInt();
        dia = jsonHorario[i]["semana"].asInt();
        camada = jsonHorario[i]["camada"].asInt();

        ProfessorDisciplina *professorDisciplina = professorDisciplinas[jsonHorario[i]["professordisciplina"].asString()];
        if (verbose) 
            std::cout << "D:" << dia << " - B:" << bloco << " - C:" << camada << " - PDSP:" << professorDisciplina->disciplina->nome << "  - P:";
        solucao->horario->insert(dia, bloco, camada, professorDisciplina);
    }
    
    if (verbose)
        std::cout << "-----------------------------------------" << std::endl;
    solucoes.push_back(solucao);
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas() {
    disciplinas = ordenarDisciplinas(disciplinas);

    atualizarDisciplinasIndex();

    return disciplinas;
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas(std::vector<Disciplina*> pDisciplinas) {
    std::vector<Disciplina*>::iterator dIter = pDisciplinas.begin();
    std::vector<Disciplina*>::iterator dIterEnd = pDisciplinas.end();

    std::sort(dIter, dIterEnd, DisciplinaCargaHorariaDesc());

    return pDisciplinas;
}

void Resolucao::atualizarDisciplinasIndex() {
    Disciplina *disciplina;

    std::vector<Disciplina*>::iterator dIter = disciplinas.begin();
    std::vector<Disciplina*>::iterator dIterEnd = disciplinas.end();

    disciplinasIndex.clear();
    for (int i = 0; dIter != dIterEnd; ++dIter, i++) {
        disciplina = *dIter;
        disciplinasIndex[disciplina->id] = i;
    }
}

int Resolucao::gerarGrade(int pTipo, double x) {
    switch (pTipo) {
        case RESOLUCAO_GERAR_GRADE_TIPO_GULOSO:

            return gerarGradeTipoGuloso();

            break;

        case RESOLUCAO_GERAR_GRADE_TIPO_GRASP:

            return gerarGradeTipoGrasp(x);

            break;

        case RESOLUCAO_GERAR_GRADE_TIPO_COMBINATORIO:

            return gerarGradeTipoCombinacaoConstrutiva();

            break;
    }

    return 0;
}

int Resolucao::gerarGradeTipoGuloso() {
    std::vector<Solucao*>::iterator sIter = solucoes.begin();
    std::vector<Solucao*>::iterator sIterEnd = solucoes.end();
    Solucao *solucao;

    Horario *horario;
    Grade *apGrade;

    std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
    std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
    AlunoPerfil *alunoPerfil;

    std::vector<Disciplina*>::iterator dIter;
    std::vector<Disciplina*>::iterator dIterEnd;
    Disciplina *disciplina;
    std::vector<Disciplina*> apRestante;
    std::vector<std::string> apCursadas;

    for (; sIter != sIterEnd; ++sIter) {
        solucao = *sIter;
        horario = solucao->horario;

        for (; apIter != apIterEnd; ++apIter) {
            if (verbose)
                std::cout << apIter->first << std::endl;
            alunoPerfil = alunoPerfis[apIter->first];

            apGrade = new Grade(blocosTamanho, alunoPerfil, horario, disciplinas, disciplinasIndex);

            apRestante = alunoPerfil->restante;
            apCursadas = alunoPerfil->cursadas;

            dIter = apRestante.begin();
            dIterEnd = apRestante.end();

            for (; dIter != dIterEnd; ++dIter) {
                disciplina = *dIter;

                apGrade->insert(disciplina);
            }

            solucao->insertGrade(apGrade);
        }

        if (verbose)
            std::cout << solucao->getObjectiveFunction();
    }
}

Grade* Resolucao::gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep, int deep, int current) {
    Grade *bestGrade = pGrade->clone();
    Grade *currentGrade;

    double bestFO, currentFO;

    bool viavel;

    for (int i = current; i < disciplinasRestantes.size(); i++) {
        currentGrade = pGrade->clone();

        if (verbose) 
            std::cout << "Nivel: " << (deep) << " Disciplina: " << i << " (" << disciplinasRestantes[i]->id << ")" << std::endl;

        viavel = currentGrade->insert(disciplinasRestantes[i]);
        if (viavel) {

            if (deep != maxDeep) {
                currentGrade = gerarGradeTipoCombinacaoConstrutiva(currentGrade, disciplinasRestantes, maxDeep, (deep + 1), (i + 1));
            }

            bestFO = bestGrade->getObjectiveFunction();
            currentFO = currentGrade->getObjectiveFunction();
            if (bestFO < currentFO) {
                bestGrade = currentGrade;
            }

            if (deep == 0) {
                //std::cout << "----------------------------" << std::endl;
            }
        } else {
            if (verbose) 
                std::cout << "[inviavel]" << std::endl;
        }
    }
    if (deep == 0) {
        //std::cout << "############################" << std::endl;
    }

    return bestGrade;
}

Grade* Resolucao::gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep) {
    return gerarGradeTipoCombinacaoConstrutiva(pGrade, disciplinasRestantes, maxDeep, 0, 0);
}

int Resolucao::gerarGradeTipoCombinacaoConstrutiva() {
    std::vector<Solucao*>::iterator sIter = solucoes.begin();
    std::vector<Solucao*>::iterator sIterEnd = solucoes.end();
    Solucao *solucao;

    Horario *horario;
    Grade *apGrade;

    std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
    std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
    AlunoPerfil *alunoPerfil;

    std::vector<Disciplina*> apRestante;

    for (; sIter != sIterEnd; ++sIter) {
        solucao = *sIter;
        horario = solucao->horario;

        for (; apIter != apIterEnd; ++apIter) {
            if (verbose)
                std::cout << "[" << apIter->first << "]" << std::endl;
            alunoPerfil = alunoPerfis[apIter->first];

            apRestante = alunoPerfil->restante;

            apGrade = gerarGradeTipoCombinacaoConstrutiva(new Grade(blocosTamanho, alunoPerfil, horario,
                                                                    disciplinas, disciplinasIndex), apRestante, apRestante.size());

            solucao->insertGrade(apGrade);
            
            const auto& escolhidas = apGrade->disciplinasAdicionadas;
            
            for (const auto disc : escolhidas) {
                disc->alocados++;
                if (disc->alocados >= disc->capacidade) {
                    disc->ofertada = false;
                }
            }

        }
        if (verbose) 
            std::cout << solucao->getObjectiveFunction();
    }
}

Grade* Resolucao::gerarGradeTipoGraspConstrucao(Grade* pGrade, double alfa) {
    std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

    return gerarGradeTipoGraspConstrucao(pGrade, alfa, professorDisciplinasIgnorar);
}

Grade* Resolucao::gerarGradeTipoGraspConstrucao(Grade* pGrade, double alfa, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar) {
    AlunoPerfil *alunoPerfil;

    std::vector<std::string> apCursadas;
    std::vector<Disciplina*> apRestante;
    std::vector<Disciplina*>::iterator dIterStart;
    std::vector<Disciplina*>::iterator dIterEnd;
    std::vector<Disciplina*>::iterator current;
    Disciplina *disciplina;

    Util util;

    int adicionados;
    int disponivel = (SEMANA - 2) * camadasTamanho;

    int rand;
    int distancia;

    alunoPerfil = pGrade->alunoPerfil;

    apCursadas = alunoPerfil->cursadas;
    apRestante = std::vector<Disciplina*>(alunoPerfil->restante.begin(), alunoPerfil->restante.end());

    dIterStart = apRestante.begin();
    dIterEnd = apRestante.end();

    adicionados = 0;
    while (apRestante.size() != 0 && dIterStart != dIterEnd && disponivel != adicionados) {

        distancia = getIntervaloAlfaGrasp(apRestante, alfa);
        rand = util.randomBetween(0, distancia);
        current = dIterStart + rand;
        disciplina = *current;

        if (pGrade->insert(disciplina, professorDisciplinasIgnorar)) {
            adicionados++;
        }
        apRestante.erase(current);

        dIterStart = apRestante.begin();
    }

    return pGrade;
}

Solucao* Resolucao::gerarGradeTipoGraspConstrucao(Solucao* pSolucao, double alfa) {
    Horario *horario;
    Grade *apGrade;

    std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
    std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
    AlunoPerfil *alunoPerfil;

    horario = pSolucao->horario;

    for (; apIter != apIterEnd; ++apIter) {
        if (verbose)
            std::cout << apIter->first << std::endl;
        alunoPerfil = alunoPerfis[apIter->first];

        apGrade = new Grade(blocosTamanho, alunoPerfil, horario, disciplinas, disciplinasIndex);

        gerarGradeTipoGraspConstrucao(apGrade, alfa);

        pSolucao->insertGrade(apGrade);
    }

    return pSolucao;
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamentoAleatorio(Solucao* pSolucao, double alfa) {
    Solucao *bestSolucao = pSolucao->clone();
    Solucao *currentSolucao;

    std::map<std::string, AlunoPerfil*>::iterator apIter;
    std::map<std::string, AlunoPerfil*>::iterator apIterEnd;
    AlunoPerfil *alunoPerfil;

    Grade *grade;

    std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

    Util util;

    int random;
    int disciplinasSize;
    int disciplinasRemoveMax;
    int disciplinasRemoveRand;
    double bestFO, currentFO;

    bestFO = bestSolucao->getObjectiveFunction();

    for (int i = 0; i < graspVizinhos; i++) {
        currentSolucao = pSolucao->clone();

        apIter = alunoPerfis.begin();
        apIterEnd = alunoPerfis.end();

        if (verbose)
            std::cout << "------NGH" << i << std::endl;

        for (; apIter != apIterEnd; ++apIter) {
            alunoPerfil = alunoPerfis[apIter->first];

            grade = currentSolucao->grades[alunoPerfil->id];

            disciplinasSize = grade->disciplinasAdicionadas.size();
            disciplinasRemoveMax = ceil(disciplinasSize * 1);
            disciplinasRemoveRand = util.randomBetween(1, disciplinasRemoveMax);

            for (int j = 0; j < disciplinasRemoveRand; j++) {
                ProfessorDisciplina *professorDisciplinaRemovido = NULL;

                disciplinasSize = grade->disciplinasAdicionadas.size();
                random = util.randomBetween(0, disciplinasSize);
                grade->remove(grade->disciplinasAdicionadas[random], professorDisciplinaRemovido);

                // Se houve uma remoção
                if (professorDisciplinaRemovido != NULL) {
                    professorDisciplinasIgnorar.push_back(professorDisciplinaRemovido);
                }
            }

            grade = gerarGradeTipoGraspConstrucao(grade, alfa, professorDisciplinasIgnorar);
        }

        currentFO = currentSolucao->getObjectiveFunction();
        if (verbose)
            std::cout << std::endl << "------NGH" << i << ": L(" << bestFO << ") < C(" << currentFO << ")" << std::endl;
        if (bestFO < currentFO) {

            bestSolucao = currentSolucao;
            bestFO = currentFO;
            if (verbose)
                std::cout << "------NGH new best: " << bestFO << std::endl;
            i = 0;
        }
    }

    return bestSolucao;
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamentoCrescente(Solucao* pSolucao) {
    Solucao *bestSolucao = pSolucao->clone();
    Solucao *currentSolucao;

    std::map<std::string, AlunoPerfil*>::iterator apIter;
    std::map<std::string, AlunoPerfil*>::iterator apIterEnd;
    AlunoPerfil *alunoPerfil;

    Disciplina* disciplinaRemovida;
    std::vector<Disciplina*> disciplinasRemovidas;
    std::vector<Disciplina*> disciplinasRestantes;
    std::vector<Disciplina*>::iterator drIter;
    std::vector<Disciplina*>::iterator drIterEnd;

    Grade *bestGrade, *currentGrade;

    Util util;

    int random;

    double bestFO, currentFO;

    apIter = alunoPerfis.begin();
    apIterEnd = alunoPerfis.end();

    for (; apIter != apIterEnd; ++apIter) {

        alunoPerfil = alunoPerfis[apIter->first];
        bestGrade = bestSolucao->grades[alunoPerfil->id];

        for (int i = 0; i < graspVizinhos; i++) {
            currentSolucao = pSolucao->clone();
            currentGrade = currentSolucao->grades[alunoPerfil->id];

            if (verbose)
                std::cout << std::endl << "------NGH" << i << std::endl;

            disciplinasRemovidas.clear();
            disciplinasRestantes = std::vector<Disciplina*>(alunoPerfil->restante.begin(), alunoPerfil->restante.end());

            for (int j = 0; j < (i + 1); j++) {
                random = util.randomBetween(0, currentGrade->disciplinasAdicionadas.size());
                if (random == -1) {
                    break;
                }
                disciplinaRemovida = currentGrade->remove(currentGrade->disciplinasAdicionadas[random]);
                if (disciplinaRemovida != NULL) {
                    disciplinasRemovidas.push_back(disciplinaRemovida);
                }
            }

            disciplinasRestantes.erase(std::remove_if(disciplinasRestantes.begin(), disciplinasRestantes.end(), DisciplinasRemoveDisciplinas(disciplinasRemovidas)), disciplinasRestantes.end());
            disciplinasRestantes.erase(std::remove_if(disciplinasRestantes.begin(), disciplinasRestantes.end(), DisciplinasRemoveDisciplinas(currentGrade->disciplinasAdicionadas)), disciplinasRestantes.end());

            gerarGradeTipoCombinacaoConstrutiva(currentGrade, disciplinasRestantes, i);

            bestFO = bestGrade->getObjectiveFunction();
            currentFO = currentGrade->getObjectiveFunction();
            if (verbose)
                std::cout << "------NGH" << i << ": L(" << bestFO << ") < C(" << currentFO << ")" << std::endl;
            if (bestFO < currentFO) {
                bestSolucao = currentSolucao;
                bestFO = currentFO;
                if (verbose)
                    std::cout << "------NGH new best: " << bestFO << std::endl;
                i = 0;
            }
        }
    }

    return bestSolucao;
}

int Resolucao::gerarGradeTipoGrasp(double alfa) {
    std::vector<Solucao*>::iterator sIter = solucoes.begin();
    std::vector<Solucao*>::iterator sIterEnd = solucoes.end();
    Solucao *solucao;
    Solucao *currentSolucao;
    Solucao *bestSolucao;

    double bestFO, currentFO;

    Util util;

    clock_t t0;
    double diff = 0;

    int const RESOLUCAO_GRASP_TEMPO_CONSTRUCAO = ceil(graspTempoConstrucao * alunoPerfis.size() * disciplinas.size());

    for (int i = 1; sIter != sIterEnd; ++sIter, i++) {

        if (verbose) 
            std::cout << "HORARIO (Solucao) " << i << ":" << std::endl;

        solucao = *sIter;
        bestSolucao = solucao->clone();
        bestFO = 0;

        while (diff <= RESOLUCAO_GRASP_TEMPO_CONSTRUCAO) {
            currentSolucao = solucao->clone();

            t0 = clock();

            gerarGradeTipoGraspConstrucao(currentSolucao, alfa);

            if (verbose)
                std::cout << "----FIT: " << currentSolucao->getObjectiveFunction() << std::endl;

            switch (graspVizinhanca) {
                case RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS:
                    gerarGradeTipoGraspRefinamentoAleatorio(currentSolucao, alfa);
                    break;
                case RESOLUCAO_GRASP_VIZINHOS_CRESCENTE:
                    gerarGradeTipoGraspRefinamentoCrescente(currentSolucao);
                    break;
            }

            if (verbose)
                std::cout << "----FIT(NGH):" << currentSolucao->getObjectiveFunction() << std::endl;

            diff += util.timeDiff(clock(), t0);

            currentFO = currentSolucao->getObjectiveFunction();
            if (bestFO < currentFO) {
                bestSolucao = currentSolucao->clone();
                bestFO = currentFO;
                diff = 0;

                if (verbose) 
                    std::cout << "----NGH is the new best (gerarGradeTipoGrasp)" << std::endl;
            }
            if (verbose) 
                std::cout << "-------------------------------------------------" << std::endl;
        }

        if (verbose) 
            std::cout << "BEST FIT: " << bestSolucao->getObjectiveFunction() << std::endl;
        
        if (!verbose) {
            const auto& grades = bestSolucao->grades;
            for (const auto& par : grades) {
                const auto gradeAtual = par.second;
                std::cout << gradeAtual->alunoPerfil->id << ":\n";
                const auto& discEscolhidas = gradeAtual->disciplinasAdicionadas;
                for (const auto disc : discEscolhidas) {
                    std::cout << disc->nome << " ";
                }
                std::cout << " " << gradeAtual->getObjectiveFunction() << "\n";
            }
        }
            
    }
}

std::vector<Disciplina*>::iterator Resolucao::getLimiteIntervaloGrasp(std::vector<Disciplina*> pApRestante, double alfa) {
    std::vector<Disciplina*>::iterator dIter = pApRestante.begin();
    std::vector<Disciplina*>::iterator dIterEnd = pApRestante.end();
    std::vector<Disciplina*>::iterator dIterLimit = dIter;

    int bestFIT = (*dIter)->cargaHoraria;
    int worstFIT = (pApRestante.back())->cargaHoraria;

    int currentFIT = 0;
    int acceptFIT = bestFIT - ceil((1 - alfa) * (bestFIT - worstFIT));

    for (; dIter != dIterEnd; ++dIter) {
        currentFIT = (*dIter)->cargaHoraria;
        if (currentFIT < acceptFIT) {
            break;
        }
        dIterLimit = dIter;
    }

    return dIterLimit;
}

int Resolucao::getIntervaloAlfaGrasp(std::vector<Disciplina*> pApRestante, double alfa) {
    std::vector<Disciplina*>::iterator dIter = pApRestante.begin();
    std::vector<Disciplina*>::iterator dIterEnd = pApRestante.end();

    int distancia = 0;

    int bestFIT = (*dIter)->cargaHoraria;
    int worstFIT = (pApRestante.back())->cargaHoraria;

    int currentFIT = 0;
    int acceptFIT = bestFIT - ceil((1 - alfa) * (bestFIT - worstFIT));

    for (; dIter != dIterEnd; ++dIter) {
        currentFIT = (*dIter)->cargaHoraria;
        if (currentFIT < acceptFIT) {
            break;
        }
        distancia++;
    }

    return distancia;
}