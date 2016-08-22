#include <fstream>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <iterator>
#include <iomanip>
#include <set>

#include <gsl.h>

#ifdef MODELO
    #include <modelo-grade/arquivos.h>
    #include <modelo-grade/modelo_solver.h>
#endif

#include "parametros.h"
#include "Resolucao.h"
#include "Algorithms.h"
#include "Util.h"
#include "Semana.h"
#include "Aleatorio.h"
#include "Output.h"
#include "SA.h"
#include "ILS.h"

Resolucao::Resolucao(const Configuracao& c)
    : horarioPopulacaoInicial(c.popInicial_)
      , horarioTorneioPares(c.numTorneioPares_)
      , horarioTorneioPopulacao(c.numTorneioPop_)
      , horarioCruzamentoPorcentagem(c.porcentCruz_)
      , horarioTipoCruzamento(c.tipoCruz_)
      , horarioTipoMutacao(c.tipoMut_)
      , horarioIteracao(c.numIter_)
      , horarioMutacaoProbabilidade(c.mutProb_)
      , horarioMutacaoTentativas(c.mutTentativas_)
      , gradeTipoConstrucao(c.tipoConstr_)
      , gradeAlfa(c.graspAlfa_)
      , gradeGraspVizinhanca(c.tipoVizinhanca_)
      , gradeGraspVizinhos(c.numVizinhos_)
      , gradeGraspTempoConstrucao(c.graspTempo_)
      , porcentagemSolucoesAleatorias(c.porcSolucoesAleatorias_)
      , numMaximoIteracoesSemEvolucaoAG(c.numMaxIterSemEvoAG_)
      , maxIterSemEvoGrasp(c.numMaxIterSemEvoGRASP_)
      , blocosTamanho(c.blocoTam_)
      , camadasTamanho(c.camadaTam_)
      , perfisTamanho(c.perfilTam_)
      , arquivoEntrada(c.filename_)
      , professores()
      , disciplinas()
      , periodoXdisciplina()
      , alunoPerfis()
      , professorDisciplinas()
      , solucao(nullptr)
      , jsonRoot()
#ifdef MODELO
      , curso(nullptr)
      , alunos()
      , tempoLimiteModelo(c.tempoLimMod_)
#endif
{
    carregarDados();
}

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho,
                     Configuracao::TipoGrade pTipoConstrucao, std::string pArquivoEntrada)
    : gradeTipoConstrucao(pTipoConstrucao)
      , blocosTamanho(pBlocosTamanho)
      , camadasTamanho(pCamadasTamanho)
      , perfisTamanho(pPerfisTamanho)
      , arquivoEntrada(pArquivoEntrada)
#ifdef MODELO
      , curso(nullptr)
      , alunos()
#endif
{
    carregarDados();
}

Resolucao::~Resolucao()
{
    delete solucao;

    while (!disciplinas.empty()) {
        delete disciplinas.back();
        disciplinas.pop_back();
    }

    for (auto& par : professores) {
        delete par.second;
    }

    for (auto& par : alunoPerfis) {
        delete par.second;
    }

    for (auto& par : professorDisciplinas) {
        delete par.second;
    }
}

#ifdef MODELO
fagoc::Curso& Resolucao::getCurso()
{
    return *curso;
}

const std::vector<fagoc::Aluno>& Resolucao::getAlunos() const
{
    return alunos;
}
#endif

std::string Resolucao::getLog() const
{
    return log.str();
}

void Resolucao::carregarDados()
{
    std::ifstream myfile(arquivoEntrada);

    if (myfile.is_open()) {
        myfile >> jsonRoot;
        myfile.close();

        carregarDadosDisciplinas();
        carregarDadosProfessores();
        carregarAlunoPerfis();

#ifdef MODELO
        if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
            auto p = fagoc::ler_json(arquivoEntrada);
            curso.reset(new fagoc::Curso(std::move(p.first)));
            alunos = move(p.second);
        }
#endif
    } else {
        std::cerr << "We had a problem reading file (" << arquivoEntrada << ")\n";
        throw 1;
    }

    for (auto i = 0; i < numcruz; i++) {
        contadorIguaisCruz[i] = contadorMelhoresCruz[i] = 0;
        tempoTotalCruz[i] = 0;
    }

    contadorIguaisMut[0] = contadorIguaisMut[1] = 0;
    contadorMelhoresMut[0] = contadorMelhoresMut[1] = 0;
    tempoTotalMut[0] = tempoTotalMut[1] = 0;
    tempoTotalElit = tempoTotalSelec = 0;
}

void Resolucao::carregarDadosProfessores()
{
    const auto& jsonProfessores = jsonRoot["professores"];

    for (auto k = 0u; k < jsonProfessores.size(); k++) {
        std::string id = jsonProfessores[k]["id"].asString();
        std::string nome = jsonProfessores[k]["nome"].asString();

        professores[id] = new Professor(nome, id);
        auto& diasDisponiveis = professores[id]->diasDisponiveis;
        auto& numDisp = professores[id]->numDisponibilidade;

        if (jsonProfessores[k].isMember("creditoMaximo") == 1) {
            professores[id]->setCreditoMaximo(jsonProfessores[k]["creditoMaximo"].asInt());
        }

        if (jsonProfessores[k].isMember("disponibilidade") == 1) {
            const auto& disponibilidade = jsonProfessores[k]["disponibilidade"];
            diasDisponiveis.resize(disponibilidade.size());
            for (auto i = 0u; i < disponibilidade.size(); i++) {
                diasDisponiveis[i].resize(disponibilidade[i].size());
                for (auto j = 0u; j < disponibilidade[i].size(); j++) {
                    diasDisponiveis[i][j] = disponibilidade[i][j].asBool();
                }

                numDisp += accumulate(begin(diasDisponiveis[i]), end(diasDisponiveis[i]), 0);
            }
        }

        const auto& competencias = jsonProfessores[k]["competencias"];
        for (auto j = 0u; j < competencias.size(); j++) {

            std::vector<Disciplina*>::iterator it;
            std::string disciplinaId = competencias[j].asString();

            it = find_if(disciplinas.begin(), disciplinas.end(),
                         DisciplinaFindDisciplinaId(disciplinaId));
            if (it != disciplinas.end()) {
                (*it)->addProfessorCapacitado(professores[id]);
            }
        }
    }
}

void Resolucao::carregarDadosDisciplinas()
{
    const auto& jsonDisciplinas = jsonRoot["disciplinas"];

    for (auto i = 0u; i < jsonDisciplinas.size(); i++) {
        const auto id = jsonDisciplinas[i]["id"].asString();
        const auto nome = jsonDisciplinas[i]["nome"].asString();
        const auto curso = jsonDisciplinas[i]["curso"].asString();
        const auto capacidade = jsonDisciplinas[i]["capacidade"].asInt();

        const auto cargahoraria = jsonDisciplinas[i]["carga"].asInt();
        const auto turma = jsonDisciplinas[i]["turma"].asString();

        const auto periodo = jsonDisciplinas[i]["periodo"].asString();
        const auto periodoMinimo = jsonDisciplinas[i]["periodominimo"].asString();

        Disciplina* disciplina = new Disciplina(nome, cargahoraria, periodo, curso, id, turma, capacidade, periodoMinimo);
        disciplina->ofertada = jsonDisciplinas[i]["ofertada"].asBool();

        const auto& corequisitos = jsonDisciplinas[i]["corequisitos"];
        for (auto j = 0u; j < corequisitos.size(); j++) {
            disciplina->coRequisitos.insert(corequisitos[j].asString());
        }

        const auto& prerequisitos = jsonDisciplinas[i]["prerequisitos"];
        for (auto j = 0u; j < prerequisitos.size(); j++) {
            disciplina->preRequisitos.insert(prerequisitos[j].asString());
        }

        const auto& equivalentes = jsonDisciplinas[i]["equivalentes"];
        for (auto j = 0u; j < equivalentes.size(); j++) {
            disciplina->equivalentes.insert(equivalentes[j].asString());
        }
        disciplina->equivalentes.insert(nome);

        disciplinas.push_back(disciplina);
        if (disciplina->ofertada) {
            periodoXdisciplina[curso + periodo].push_back(disciplina);
        }
    }

    disciplinas = ordenarDisciplinas();
}

void Resolucao::carregarDadosProfessorDisciplinas()
{
    const auto& jsonProfessorDisciplinas = jsonRoot["professordisciplinas"];

    for (auto i = 0u; i < jsonProfessorDisciplinas.size(); i++) {
        const auto id = jsonProfessorDisciplinas[i]["id"].asString();
        const auto professor = jsonProfessorDisciplinas[i]["professor"].asString();
        const auto disciplina = jsonProfessorDisciplinas[i]["disciplina"].asString();

        ProfessorDisciplina* professorDisciplina = new ProfessorDisciplina(
            professores[professor],
            disciplinas[disciplinasIndex[disciplina]],
            id);

        /**
         * TODO: recuperar peso do v�nculo do professor com a disciplina do arquivo de entrada
         */
        double competenciaPeso = 1.0;
        professorDisciplina->professor->addCompetencia(disciplina, competenciaPeso);

        professorDisciplinas[id] = professorDisciplina;
    }
}

void Resolucao::carregarAlunoPerfis()
{
    const auto& jsonAlunoPerfis = jsonRoot["alunoperfis"];

    for (auto i = 0u; i < jsonAlunoPerfis.size(); i++) {
        const auto id = jsonAlunoPerfis[i]["id"].asString();
        const auto peso = jsonAlunoPerfis[i]["peso"].asDouble();
        const auto turma = jsonAlunoPerfis[i]["turma"].asString();
        const auto periodo = jsonAlunoPerfis[i]["periodo"].asString();

        AlunoPerfil* alunoPerfil = new AlunoPerfil(peso, id, turma, periodo);

        const auto& jsonRestantes = jsonAlunoPerfis[i]["restantes"];
        for (auto j = 0u; j < jsonRestantes.size(); j++) {
            Disciplina* disciplina = disciplinas[disciplinasIndex[jsonRestantes[j].asString()]];
            alunoPerfil->addRestante(disciplina);
        }

        const auto& jsonCursadas = jsonAlunoPerfis[i]["cursadas"];
        for (auto j = 0u; j < jsonCursadas.size(); j++) {
            std::string cursada = jsonCursadas[j].asString();
            alunoPerfil->addCursada(cursada);
        }

        std::vector<Disciplina*> aprovadas;
        const auto& restantes = alunoPerfil->restante;
        std::remove_copy_if(begin(disciplinas), end(disciplinas), std::back_inserter(aprovadas),
                            [&restantes](Disciplina* d) {
                                return find_if(begin(restantes), end(restantes), [&d](const std::string& x) {
                                                   return x == d->id;
                                               }) != end(restantes);
                            });

        auto& aprovadasNomes = alunoPerfil->aprovadas;

        for (const auto& aprovada : aprovadas) {
            aprovadasNomes.insert(aprovada->id);
        }

        alunoPerfis[id] = alunoPerfil;
    }
}

double Resolucao::start()
{
    return start(true);
}

double Resolucao::start(bool input)
{
    if (input) {
        carregarSolucao();
        return gerarGrade();
    }

    return gerarHorarioAG()->getFO();
}

void Resolucao::carregarSolucao()
{
    const auto& jsonHorario = jsonRoot["horario"];

    carregarDadosProfessorDisciplinas();

    Solucao* solucaoLeitura = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);
    int bloco {};
    int dia {};
    int camada {};

    for (auto i = 0u; i < jsonHorario.size(); i++) {
        bloco = jsonHorario[i]["horario"].asInt();
        dia = jsonHorario[i]["semana"].asInt();
        camada = jsonHorario[i]["camada"].asInt();

        ProfessorDisciplina* professorDisciplina = professorDisciplinas[jsonHorario[i]["professordisciplina"].asString()];
        if (verbose)
            std::cout << "D:" << dia << " - B:" << bloco << " - C:" << camada << " - PDSP:" << professorDisciplina->disciplina->nome << "  - P:";
        solucaoLeitura->horario->insert(dia, bloco, camada, professorDisciplina);
    }

    if (verbose)
        std::cout << "-----------------------------------------" << std::endl;
    solucao = solucaoLeitura;
}

Disciplina* Resolucao::getDisciplinaByName(const std::string& nomeDisc)
{
    return disciplinas[disciplinasIndex[nomeDisc]];
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas()
{
    disciplinas = ordenarDisciplinas(disciplinas);

    atualizarDisciplinasIndex();

    return disciplinas;
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas(std::vector<Disciplina*> pDisciplinas)
{
    sort(begin(pDisciplinas), end(pDisciplinas), DisciplinaCargaHorariaDesc());
    return pDisciplinas;
}

void Resolucao::atualizarDisciplinasIndex()
{
    // auto dIter = disciplinas.begin();
    // disciplinasIndex.clear();

    // for (int i = 0; dIter != end(disciplinas); ++dIter, i++) {
    //     disciplinasIndex[(*dIter)->id] = i;
    // }

    for (size_t i = 0; i < disciplinas.size(); i++) {
        disciplinasIndex[disciplinas[i]->id] = i;
    }
}

std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamento(const std::vector<Solucao*>& parVencedor)
{
    std::vector<Solucao*> filhos {};

    switch (horarioTipoCruzamento) {
        case Configuracao::TipoCruzamento::construtivo_reparo: {
            auto filhos1 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[0], parVencedor[1]);
            auto filhos2 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[1], parVencedor[0]);
            filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
            filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
            break;
        }

        case Configuracao::TipoCruzamento::simples: {
            auto filhos1 = gerarHorarioAGCruzamentoSimples(parVencedor[0], parVencedor[1]);
            filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
            break;
        }

        case Configuracao::TipoCruzamento::substitui_bloco: {
            auto filhos1 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[0], parVencedor[1]);
            auto filhos2 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[1], parVencedor[0]);
            filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
            filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
            break;
        }

        case Configuracao::TipoCruzamento::ordem: {
            auto filho1 = crossoverOrdem(*parVencedor[0], *parVencedor[1]);
            auto filho2 = crossoverOrdem(*parVencedor[1], *parVencedor[0]);
            if (filho1) {
                filhos.push_back(filho1);
            }
            if (filho2) {
                filhos.push_back(filho2);
            }
            break;
        }

        case Configuracao::TipoCruzamento::ciclo: {
            auto filho1 = crossoverCiclo(*parVencedor[0], *parVencedor[1]);
            auto filho2 = crossoverCiclo(*parVencedor[1], *parVencedor[0]);
            if (filho1) {
                filhos.push_back(filho1);
            }
            if (filho2) {
                filhos.push_back(filho2);
            }
            break;
        }

        case Configuracao::TipoCruzamento::pmx: {
            auto filho1 = crossoverPMX(*parVencedor[0], *parVencedor[1]);
            auto filho2 = crossoverPMX(*parVencedor[1], *parVencedor[0]);
            if (filho1) {
                filhos.push_back(filho1);
            }
            if (filho2) {
                filhos.push_back(filho2);
            }
            break;
        }
    }

    return filhos;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoExper(const std::vector<Solucao*>& parVencedor, Configuracao::TipoCruzamento tipoCruz)
{
    std::vector<Solucao*> filhos {};
    auto idx = 0;

    switch (tipoCruz) {
        case Configuracao::TipoCruzamento::construtivo_reparo: {
            auto filhos1 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[0], parVencedor[1]);
            auto filhos2 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[1], parVencedor[0]);
            filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
            filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
            idx = 0;
            break;
        }

        case Configuracao::TipoCruzamento::simples: {
            auto filhos1 = gerarHorarioAGCruzamentoSimples(parVencedor[0], parVencedor[1]);
            filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
            idx = 1;
            break;
        }

        case Configuracao::TipoCruzamento::substitui_bloco: {
            auto filhos1 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[0], parVencedor[1]);
            auto filhos2 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[1], parVencedor[0]);
            filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
            filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
            idx = 2;
            break;
        }

        case Configuracao::TipoCruzamento::ordem: {
            auto filho1 = crossoverOrdem(*parVencedor[0], *parVencedor[1]);
            auto filho2 = crossoverOrdem(*parVencedor[1], *parVencedor[0]);
            if (filho1)
                filhos.push_back(filho1);
            if (filho2)
                filhos.push_back(filho2);
            idx = 3;
            break;
        }

        case Configuracao::TipoCruzamento::ciclo: {
            auto filho1 = crossoverCiclo(*parVencedor[0], *parVencedor[1]);
            auto filho2 = crossoverCiclo(*parVencedor[1], *parVencedor[0]);
            if (filho1)
                filhos.push_back(filho1);
            if (filho2)
                filhos.push_back(filho2);
            idx = 4;
            break;
        }

        case Configuracao::TipoCruzamento::pmx: {
            auto filho1 = crossoverPMX(*parVencedor[0], *parVencedor[1]);
            auto filho2 = crossoverPMX(*parVencedor[1], *parVencedor[0]);
            if (filho1)
                filhos.push_back(filho1);
            if (filho2)
                filhos.push_back(filho2);
            idx = 5;
            break;
        }
    }

    for (auto& filho : filhos) {
        auto fofilho = filho->getFO();
        auto fopai1 = parVencedor[0]->getFO();
        auto fopai2 = parVencedor[1]->getFO();

        if (fofilho > fopai1 || fofilho > fopai2) {
            contadorMelhoresCruz[idx]++;
            if (fofilho > populacao[0]->getFO()) {
                std::cout << "Melhor solucao. Operador: C" << idx + 1 << "\n";
                log << "Melhor solucao. Operador: C" << idx + 1 << "\n";
            }
        } else if (fofilho == fopai1 || fofilho == fopai2) {
            contadorIguaisCruz[idx]++;
        }
    }

    return filhos;
}

Solucao* Resolucao::gerarHorarioAG()
{
    const auto numCruz = std::max(
        1,
        static_cast<int>(horarioPopulacaoInicial * horarioCruzamentoPorcentagem));

    tempoInicio = Util::now();
    populacao = gerarHorarioAGPopulacaoInicial2();
    foAlvo = populacao[0]->getFO();
    iteracaoAlvo = -1;
    tempoAlvo = Util::chronoDiff(Util::now(), tempoInicio);

    constexpr auto uma_hora = 60 /* min */ * 60 /* sec */ * 1000 /* ms */;

    for (auto i = 0;
         i - iteracaoAlvo <= numMaximoIteracoesSemEvolucaoAG
         && Util::chronoDiff(Util::now(), tempoInicio) < uma_hora;
         i++) {
        ultimaIteracao = i;
        logPopulacao(populacao, i);

        gerarHorarioAGEfetuaCruzamento(populacao, numCruz);
        gerarHorarioAGEfetuaMutacao(populacao);
        gerarHorarioAGSobrevivenciaElitismo(populacao);
        gerarHorarioAGVerificaEvolucao(populacao, i);
    }

    // Captura a melhor solução da população, deletando o resto
    gerarHorarioAGSobrevivenciaElitismo(populacao, 1);
    auto solucaoAG = populacao[0];

    // Se as grades foram resolvidas pelo modelo, suas matrizes estão vazias,
    // o que causa problemas com a escrita do html.
    if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
        reinsereGrades(solucaoAG);
    }

    solucao = solucaoAG;

    return solucaoAG;
}

Solucao* Resolucao::gerarHorarioAG2()
{
    tempoInicio = std::chrono::steady_clock::now();

    populacao = gerarHorarioAGPopulacaoInicial2();
    foAlvo = populacao[0]->getFO();
    iteracaoAlvo = -1;
    tempoAlvo = Util::chronoDiff(Util::now(), tempoInicio);

    //for (auto i = 0; i < horarioIteracao; i++) {
    for (auto i = 0; i - iteracaoAlvo <= numMaximoIteracoesSemEvolucaoAG; i++) {
        ultimaIteracao = i;
        logPopulacao(populacao, i);

        auto selecbegin = Util::now();
        auto pais = gerarHorarioAGTorneioPar(populacao);
        auto totalselc = Util::chronoDiff(Util::now(), selecbegin);
        log << "Tempo selecao: " << totalselc << "ms\n";
        tempoTotalSelec += totalselc;

        // Aplica os operadores de cruzamento

        //auto simplesbegin = Util::now();
        //gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::simples, pais);
        //auto simplestotal = Util::chronoDiff(Util::now(), simplesbegin);
        //log << "Tempo C2: " << simplestotal << "ms\n";
        //tempoTotalCruz[1] += simplestotal;

        //auto subblocobegin = Util::now();
        //gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::substitui_bloco, pais);
        //auto subblocototal = Util::chronoDiff(Util::now(), subblocobegin);
        //log << "Tempo C3: " << subblocototal << "ms\n";
        //tempoTotalCruz[2] += subblocototal;

        //auto constrbegin = Util::now();
        //gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::construtivo_reparo, pais);
        //auto constrtotal = Util::chronoDiff(Util::now(), constrbegin);
        //log << "Tempo C1: " << constrtotal << "ms\n";
        //tempoTotalCruz[0] += constrtotal;

        auto ordembegin = Util::now();
        gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::ordem, pais);
        auto ordemtotal = Util::chronoDiff(Util::now(), ordembegin);
        log << "Tempo C4: " << ordemtotal << "ms\n";
        tempoTotalCruz[3] += ordemtotal;

        //auto ciclobegin = Util::now();
        //gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::ciclo, pais);
        //auto ciclototal = Util::chronoDiff(Util::now(), ciclobegin);
        //log << "Tempo C5: " << ciclototal << "ms\n";
        //tempoTotalCruz[4] += ciclototal;

        auto pmxbegin = Util::now();
        gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::pmx, pais);
        auto pmxtotal = Util::chronoDiff(Util::now(), pmxbegin);
        log << "Tempo C6: " << pmxtotal << "ms\n";
        tempoTotalCruz[5] += pmxtotal;

        // Aplica mutação

        auto disctempo = Util::now();
        gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substiui_disciplina);
        auto disctotal = Util::chronoDiff(Util::now(), disctempo);
        log << "Tempo M1: " << disctotal << "ms\n";
        tempoTotalMut[0] += disctotal;

        //auto profbegin = Util::now();
        //gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substitui_professor);
        //auto proftotal = Util::chronoDiff(Util::now(), profbegin);
        //log << "Tempo M2: " << proftotal << "ms\n";
        //tempoTotalMut[1] += proftotal;

        // Aplica o elitismo, deletando as que sobraram
        auto elitbegin = Util::now();
        gerarHorarioAGSobrevivenciaElitismo(populacao);
        auto elittotal = Util::chronoDiff(Util::now(), elitbegin);
        log << "Tempo Elitismo: " << elittotal << "ms\n\n";
        tempoTotalElit += elittotal;

        // Verifica se a nova população possui uma solução melhor que a anterior
        gerarHorarioAGVerificaEvolucao(populacao, i);
    }

    // Captura a melhor solução da população, deletando o resto
    gerarHorarioAGSobrevivenciaElitismo(populacao, 1);
    auto solucaoAG = populacao[0];

    // Se as grades foram resolvidas pelo modelo, suas matrizes estão vazias,
    // o que causa problemas com a escrita do html.
    if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
        reinsereGrades(solucaoAG);
    }

    solucao = solucaoAG;

    return solucaoAG;
}

Solucao* Resolucao::gerarHorarioAG3()
{
    tempoInicio = std::chrono::steady_clock::now();

    populacao = gerarHorarioAGPopulacaoInicial2();
    foAlvo = populacao[0]->getFO();
    iteracaoAlvo = -1;
    tempoAlvo = Util::chronoDiff(Util::now(), tempoInicio);

    for (auto i = 0; i - iteracaoAlvo <= numMaximoIteracoesSemEvolucaoAG; i++) {
        ultimaIteracao = i;
        logPopulacao(populacao, i);

        // Aplica os operadores de cruzamento
        auto selecbegin = Util::now();
        auto pais = gerarHorarioAGTorneioPar(populacao);
        auto totalselc = Util::chronoDiff(Util::now(), selecbegin);
        log << "Tempo selecao: " << totalselc << "ms\n";
        tempoTotalSelec += totalselc;

        auto simplesbegin = Util::now();
        gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::simples, pais);
        auto simplestotal = Util::chronoDiff(Util::now(), simplesbegin);
        log << "Tempo C2: " << simplestotal << "ms\n";
        tempoTotalCruz[1] += simplestotal;

        selecbegin = Util::now();
        pais = gerarHorarioAGTorneioPar(populacao);
        totalselc = Util::chronoDiff(Util::now(), selecbegin);
        log << "Tempo selecao: " << totalselc << "ms\n";
        tempoTotalSelec += totalselc;

        auto subblocobegin = Util::now();
        gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::substitui_bloco, pais);
        auto subblocototal = Util::chronoDiff(Util::now(), subblocobegin);
        log << "Tempo C3: " << subblocototal << "ms\n";
        tempoTotalCruz[2] += subblocototal;

        selecbegin = Util::now();
        pais = gerarHorarioAGTorneioPar(populacao);
        totalselc = Util::chronoDiff(Util::now(), selecbegin);
        log << "Tempo selecao: " << totalselc << "ms\n";
        tempoTotalSelec += totalselc;

        auto constrbegin = Util::now();
        gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::construtivo_reparo, pais);
        auto constrtotal = Util::chronoDiff(Util::now(), constrbegin);
        log << "Tempo C1: " << constrtotal << "ms\n";
        tempoTotalCruz[0] += constrtotal;

        selecbegin = Util::now();
        pais = gerarHorarioAGTorneioPar(populacao);
        totalselc = Util::chronoDiff(Util::now(), selecbegin);
        log << "Tempo selecao: " << totalselc << "ms\n";
        tempoTotalSelec += totalselc;

        auto ordembegin = Util::now();
        gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::ordem, pais);
        auto ordemtotal = Util::chronoDiff(Util::now(), ordembegin);
        log << "Tempo C4: " << ordemtotal << "ms\n";
        tempoTotalCruz[3] += ordemtotal;

        // Aplica mutação

        auto disctempo = Util::now();
        gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substiui_disciplina);
        auto disctotal = Util::chronoDiff(Util::now(), disctempo);
        log << "Tempo M1: " << disctotal << "ms\n";
        tempoTotalMut[0] += disctotal;

        auto profbegin = Util::now();
        gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substitui_professor);
        auto proftotal = Util::chronoDiff(Util::now(), profbegin);
        log << "Tempo M2: " << proftotal << "ms\n";
        tempoTotalMut[1] += proftotal;

        // Aplica o elitismo, deletando as que sobraram

        auto elitbegin = Util::now();
        gerarHorarioAGSobrevivenciaElitismo(populacao);
        auto elittotal = Util::chronoDiff(Util::now(), elitbegin);
        log << "Tempo Elitismo: " << elittotal << "ms\n";
        tempoTotalElit += elittotal;

        // Verifica se a nova população possui uma solução melhor que a anterior
        gerarHorarioAGVerificaEvolucao(populacao, i);
    }

    // Captura a melhor solução da população, deletando o resto
    gerarHorarioAGSobrevivenciaElitismo(populacao, 1);
    auto solucaoAG = populacao[0];

    // Se as grades foram resolvidas pelo modelo, suas matrizes estão vazias,
    // o que causa problemas com a escrita do html.
    if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
        reinsereGrades(solucaoAG);
    }

    solucao = solucaoAG;

    return solucaoAG;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGPopulacaoInicial()
{
    std::vector<Solucao*> solucoesAG {};

    Disciplina* disciplinaAleatoria {nullptr};
    Professor* professorSelecionado {nullptr};

    std::string dId {};
    std::string pId {};
    std::string pdId {};

    int randInt {};
    bool inserted = false;
    bool professorPossuiCreditos {};

    std::map<std::string, int> creditosUtilizadosProfessor {};
    std::map<std::string, int> colisaoProfessor {};

    /**
     * TODO: ordenar os professores com menos blocos dispon�veis para dar aula
     * Priorizar esses professores para que eles tenham as aulas montadas
     * antes dos outros professores
     */
    while (static_cast<int>(solucoesAG.size()) != horarioPopulacaoInicial) {
        Solucao* solucaoLocal = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);
        int i = 0;

        creditosUtilizadosProfessor.clear();
        colisaoProfessor.clear();
        for (const auto& par : periodoXdisciplina) {
            auto discs = par.second;
            std::map<std::string, int> creditosUtilizadosDisciplina;
            std::map<std::string, ProfessorDisciplina*> disciplinaXprofessorDisciplina;
            int dia = 0, bloco = 0;

            bool finding = false;

            std::map<std::string, int> infactiveis;

            colisaoProfessor.clear();

            while (discs.size() != 0) {
                randInt = 0;

                /**
                 * TODO: implementar aleatoriedade uniforme
                 */
                switch (0) {
                    case 0:
                        randInt = aleatorio::randomInt() % discs.size();
                        break;
                    case 1:
                        break;
                }

                disciplinaAleatoria = discs[randInt];
                dId = disciplinaAleatoria->getId();

                // Se a disciplina ainda n�o tem professor alocado
                if (!disciplinaXprofessorDisciplina[dId]) {

                    do {
                        randInt = aleatorio::randomInt() % disciplinaAleatoria->professoresCapacitados.size();

                        professorSelecionado = disciplinaAleatoria->professoresCapacitados[randInt];
                        pId = professorSelecionado->getId();

                        professorPossuiCreditos = (professorSelecionado->creditoMaximo != 0
                            && professorSelecionado->creditoMaximo
                            < (creditosUtilizadosProfessor[pId] + disciplinaAleatoria->getCreditos()));
                    } while (professorPossuiCreditos);

                    int profNum = randInt;

                    creditosUtilizadosProfessor[pId] += disciplinaAleatoria->getCreditos();

                    pdId = "pr" + pId + "di" + dId;
                    if (!professorDisciplinas[pdId]) {
                        professorDisciplinas[pdId] = new ProfessorDisciplina(disciplinaAleatoria->professoresCapacitados[profNum], disciplinaAleatoria, pdId);
                    }

                    disciplinaXprofessorDisciplina[dId] = professorDisciplinas[pdId];
                }

                if (dia > (dias_semana_util - 1)) {
                    finding = true;
                }

                if (finding) {
                    inserted = false;
                    std::vector<int> empties = solucaoLocal->horario->getAllEmpty(i);
                    int positions[3] {};

                    for (std::vector<int>::iterator it = empties.begin(); it != empties.end(); ++it) {

                        solucaoLocal->horario->get3DMatrix(*it, positions);

                        dia = positions[1];
                        bloco = positions[0];

                        inserted = solucaoLocal->horario->insert(dia, bloco, i, disciplinaXprofessorDisciplina[dId]);
                        if (inserted) {
                            break;
                        }

                        //std::cout << i << " " << par.first << " (" << (*it) << " - " << dia << " - " << bloco << ") [" << disciplinaAleatoria->nome << "]" << std::endl;
                    }

                    infactiveis[dId]++;

                    //std::cerr << infactiveis[dId] << ") Deu merda! " << par.first << " [" << disciplinaAleatoria->nome << "]" << std::endl;
                    if (infactiveis[dId] > 4) {

                        solucaoLocal->horario->clearDisciplina(disciplinaXprofessorDisciplina[dId], i);

                        discs.erase(find_if(discs.begin()
                                            , discs.end()
                                            , DisciplinaFindDisciplina(disciplinaAleatoria)));

                        continue;
                    }
                } else {

                    inserted = solucaoLocal->horario->insert(dia, bloco, i, disciplinaXprofessorDisciplina[dId]);
                }

                if (inserted) {

                    bloco++;
                    creditosUtilizadosDisciplina[dId]++;
                    if (!finding && ((bloco % 2) == 1 && disciplinaAleatoria->getCreditos() > creditosUtilizadosDisciplina[dId])) {
                        inserted = solucaoLocal->horario->insert(dia, bloco, i, disciplinaXprofessorDisciplina[dId]);
                        if (inserted) {
                            bloco++;
                            creditosUtilizadosDisciplina[dId]++;
                        }
                    }
                } else {
                    colisaoProfessor[dId]++;

                    if (colisaoProfessor[dId] == horarioProfessorColisaoMax) {

                        if (bloco % 2 == 1) {
                            bloco += 1;
                        } else {
                            bloco += 2;
                        }

                        colisaoProfessor[dId] = 0;
                    }
                }

                if (bloco >= blocosTamanho) {
                    bloco = 0;
                    dia++;
                }

                //std::cout << "\n" << disciplinaAleatoria->getCreditos() << " == " << creditosUtilizadosDisciplina[dId] << std::endl;
                if (disciplinaAleatoria->getCreditos() == creditosUtilizadosDisciplina[dId]) {

                    discs.erase(find_if(discs.begin()
                                        , discs.end()
                                        , DisciplinaFindDisciplina(disciplinaAleatoria)));
                }
            }

            i++;
        }

        auto t1 = Util::now();
        gerarGrade(solucaoLocal);
        auto tend = Util::chronoDiff(Util::now(), t1);
        std::cout << "t: " << tend << "\n";
        //gerarGradeTipoGrasp(solucaoLocal, false);
        //solucoesAG.push_back(solucaoLocal);
        Util::insert_sorted(solucoesAG, solucaoLocal, SolucaoComparaMaior());
    }

    return solucoesAG;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGTorneioPar(std::vector<Solucao*> solucoesPopulacao)
{
    auto pai1 = selecaoTorneio(solucoesPopulacao);
    auto pai2 = selecaoTorneio(solucoesPopulacao);

    return {pai1, pai2};
}

Solucao* Resolucao::gerarHorarioAGTorneio(std::vector<Solucao*> solucoesPopulacao) const
{
    std::vector<Solucao*> torneioCandidatos {};
    Solucao* vencedor {nullptr};
    double vencedorFO {0};

    auto populacaoTorneioMax = static_cast<size_t>(horarioTorneioPopulacao)
            * solucoesPopulacao.size();

    while (torneioCandidatos.size() <= populacaoTorneioMax && solucoesPopulacao.size() != 0) {
        auto randInt = aleatorio::randomInt() % solucoesPopulacao.size();

        auto randomFO = solucoesPopulacao[randInt]->getFO();
        if (vencedorFO < randomFO) {
            vencedor = solucoesPopulacao[randInt];
            vencedorFO = randomFO;
        }

        // Remove elemento sorteado
        solucoesPopulacao.erase(
            remove(solucoesPopulacao.begin(), solucoesPopulacao.end(), solucoesPopulacao[randInt]),
            solucoesPopulacao.end());
    }

    return vencedor;
}

Solucao* Resolucao::gerarHorarioAGTorneio2(std::vector<Solucao*>& pop) const
{
    auto primeiro = Util::randomChoice(pop);
    auto segundo = Util::randomChoice(pop);

    if (primeiro->getFO() > segundo->getFO()) {
        return primeiro;
    } else {
        return segundo;
    }
}

// Construtivo-reparo foi o primeiro operador de cruzamento implementado, que tenta inserir uma disciplina
// de cada vez e reparar a solução se uma restrição for violada
std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoConstrutivoReparo(Solucao* solucaoPai1, Solucao* solucaoPai2)
{
    //puts("Constr");
    //printf("pais: %g %g\n", solucaoPai1->fo, solucaoPai2->fo);
    std::vector<Solucao*> filhos;
    std::vector<ProfessorDisciplina*> matrizBackup;

    auto camadasMax = static_cast<int>(horarioCruzamentoCamadas * camadasTamanho);
    int numFilhos = 0;

    int camadaPeriodo, diaSemana, blocoHorario;
    int posicao, posicaoX;
    bool success;

    ProfessorDisciplina* e;
    ProfessorDisciplina* g;

    Solucao* filho;

    do {

        filho = new Solucao(*solucaoPai1);

        for (int i = 0; i < camadasMax; i++) {

            camadaPeriodo = aleatorio::randomInt() % camadasTamanho;

            for (int j = 0, tentativas = 0; j < horarioCruzamentoDias;) {
                success = false;

                // De segunda a s�bado
                diaSemana = aleatorio::randomInt() % 6;
                blocoHorario = aleatorio::randomInt() % blocosTamanho;

                posicao = filho->horario->getPosition(diaSemana, blocoHorario, camadaPeriodo);

                e = solucaoPai2->horario->at(posicao);
                g = filho->horario->at(posicao);

                // Backup
                matrizBackup = filho->horario->matriz;

                if (e == g) {
                    // nada a fazer
                    success = true;
                } else if (e == NULL) {
                    // Remove G
                    posicaoX = filho->horario->getPosition(diaSemana, blocoHorario, camadaPeriodo);
                    filho->horario->matriz[posicaoX] = NULL;

                    success = gerarHorarioAGCruzamentoAleatorioReparo(filho, diaSemana, blocoHorario, camadaPeriodo);
                    if (!success) {
                        filho->horario->matriz = matrizBackup;
                    }
                } else if (g == NULL) {
                    // Recupera primeira ocorrencia da disciplina E
                    posicaoX = filho->horario->getFirstDisciplina(e->disciplina);
                    // Se o professor da primeira ocorrencia for o mesmo professor da disciplina E
                    if (filho->horario->matriz[posicaoX]->professor == e->professor) {
                        // Remove primeira ocorrencia de E
                        filho->horario->matriz[posicaoX] = NULL;

                        success = filho->horario->insert(diaSemana, blocoHorario, camadaPeriodo, e);
                        if (!success) {
                            filho->horario->matriz = matrizBackup;
                        }
                    }
                } else {
                    // Recupera primeira ocorrencia da disciplina E
                    posicaoX = filho->horario->getFirstDisciplina(e->disciplina);
                    // Se o professor da primeira ocorrencia for o mesmo professor da disciplina E
                    if (filho->horario->matriz[posicaoX]->professor == e->professor) {

                        // Remove primeira ocorrencia de E
                        filho->horario->matriz[posicaoX] = NULL;

                        // insere E
                        success = filho->horario->insert(diaSemana, blocoHorario, camadaPeriodo, e);
                        if (!success) {
                            success = gerarHorarioAGCruzamentoAleatorioReparo(filho, diaSemana, blocoHorario, camadaPeriodo);
                            if (!success) {
                                filho->horario->matriz = matrizBackup;
                            }
                        }
                    }
                }

                if (success || tentativas >= horarioCruzamentoTentativasMax) {
                    j++;
                    tentativas = 0;
                }
            }
        }
        gerarGrade(filho);
        //printf("filho: %g\n", filho->getFO());
        filhos.push_back(filho);
        numFilhos++;
    } while (numFilhos <= horarioCruzamentoFilhos);
    //puts("\n");

    return filhos;
}

int Resolucao::cruzaCamada(Solucao*& filho, const Solucao* pai, int camada) const
{
    // Se o cruzamento falhar, filho continua o mesmo de antes
    auto fallback = new Solucao(*filho);
    filho->fo = -1;
    filho->horario->hash_ = 0;
    // Apaga camada do filho
    for (auto i = 0; i < dias_semana_util; i++) {
        for (auto j = 0; j < blocosTamanho; j++) {
            filho->horario->clearSlot(i, j, camada);
        }
    }

    // Tenta inserir as disciplinas da camada do pai. Se alguma falhar, volta
    // ao fallback e encerra
    for (auto i = 0; i < dias_semana_util; i++) {
        for (auto j = 0; j < blocosTamanho; j++) {
            auto pos = filho->horario->getPosition(i, j, camada);
            auto alocPai = pai->horario->matriz[pos];
            if (!alocPai || filho->horario->insert(i, j, camada, alocPai)) {
                continue;
            }

            if (verbose) {
                std::cout << "Problemas no cruzamento tipo simples\n";
            }

            delete filho;
            filho = fallback;
            return 0;
        }
    }
    delete fallback;
    return 1;
}

// Simples é o novo, que simplesmente tenta substituir todos os perídos
// do filho pelos do pai a partir de certo ponto de corte. Se durante uma
// operação em uma dessas camadas for violada alguma restrição, aquela camada
// retorna ao original, e a próxima será tentada
std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoSimples(Solucao* pai1, Solucao* pai2)
{
    auto camadaCruz = aleatorio::randomInt() % camadasTamanho;
    auto filho1 = new Solucao(*pai1);
    auto filho2 = new Solucao(*pai2);

    std::vector<Solucao*> solucoes;
    auto success1 = 0;
    auto success2 = 0;

    success1 += cruzaCamada(filho1, pai2, camadaCruz);
    success2 += cruzaCamada(filho2, pai1, camadaCruz);

    if (success1 > 0) {
        gerarGrade(filho1);
        solucoes.push_back(filho1);
    } else {
        delete filho1;
    }
    if (success2 > 0) {
        gerarGrade(filho2);
        solucoes.push_back(filho2);
    } else {
        delete filho2;
    }

    return solucoes;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoSubstBloco(Solucao* solucaoPai1, Solucao* solucaoPai2)
{
    //puts("subsbloco");
    //printf("pais: %g %g\n", solucaoPai1->fo, solucaoPai2->fo);
    int dia, bloco, camada;
    std::size_t pos;
    auto filho1 = new Solucao(*solucaoPai1);
    auto filho2 = new Solucao(*solucaoPai2);
    auto& mat1 = filho1->horario->matriz;
    auto& mat2 = filho2->horario->matriz;
    auto fallback1 = new Solucao(*filho1);
    auto fallback2 = new Solucao(*filho2);

    // Escolhe posição de cruzamento. Se estiver no meio um bloco tenta de novo
    do {
        pos = aleatorio::randomInt() % mat1.size();
        int coord[3];
        filho1->horario->get3DMatrix(pos, coord);
        dia = coord[1];
        bloco = coord[0];
        camada = coord[2];
    } while (bloco % 2 == 1);

    struct Gene
    {
        ProfessorDisciplina* disc; // professorDisciplina da célula escolhida
        int posGene; // posição original da célula no pai
        int posCruz; // posição final da célula no filho
        bool disponivel; // registra se a célula já encontrou um compatível

        Gene(ProfessorDisciplina* p, int g, int c, bool d)
            : disc(p), posGene(g), posCruz(c), disponivel(d) {}
    };

    // Insere as disciplinas após o ponto de corte na camada original
    // Modifica então para nulo, para poder inserir as novas alocações no slot
    auto lista1 = std::vector<Gene> {};
    auto lista2 = std::vector<Gene> {};
    for (auto d = dia; d < dias_semana_util; d++) {
        for (auto h = d == dia ? bloco : 0; h < blocosTamanho; h++) {
            auto i = filho1->horario->getPosition(d, h, camada);
            if (mat1[i]) {
                lista1.emplace_back(mat1[i], i, -1, true);
                mat1[i] = nullptr;
            }
            if (mat2[i]) {
                lista2.emplace_back(mat2[i], i, -1, true);
                mat2[i] = nullptr;
            }
        }
    }

    // Procura o primeiro gene compatível com o atual na lista 2, e os marca
    // para troca
    for (auto& gene : lista1) {
        auto it = std::find_if(begin(lista2), end(lista2), [&](Gene& g) {
                                   return g.disc->disciplina == gene.disc->disciplina;
                               });
        if (it == end(lista2)) {
            continue;
        }

        gene.posCruz = it->posGene;
        it->posCruz = gene.posGene;
        gene.disponivel = false;
        it->disponivel = false;
    }

    // Separa uma lista em duas: blocos (disciplinas que estão alocadas em blocos)
    // e isoladas (o resto)
    auto sepBlocos = [this](std::vector<Gene>& lista, const std::vector<ProfessorDisciplina*>& mat) {
        auto blocos = std::vector<Gene> {};
        auto isoladas = std::vector<Gene> {};
        for (auto i = 0u; i < lista.size(); i++) {
            auto p = lista[i].posGene;
            if (p < blocosTamanho - 1 && mat[p] == mat[p + 1]) {
                blocos.emplace_back(std::move(lista[i]));
            } else {
                isoladas.emplace_back(std::move(lista[i]));
            }
        }

        return std::make_pair(blocos, isoladas);
    };

    // Separa as listas 1 e 2
    auto ret1 = sepBlocos(lista1, mat1);
    auto blocos1 = std::move(ret1.first);
    auto isoladas1 = std::move(ret1.second);
    auto ret2 = sepBlocos(lista2, mat2);
    auto blocos2 = std::move(ret2.first);
    auto isoladas2 = std::move(ret2.second);

    // Separa uma lista entre compatíveis (aqueles que acharam um equivalente)
    // e não compatíveis
    auto sepCompat = [](std::vector<Gene>& lista) {
        auto compat = std::vector<Gene> {};
        auto incompat = std::vector<Gene> {};

        for (auto&& gene : lista) {
            if (gene.disponivel) {
                incompat.emplace_back(std::move(gene));
            } else {
                compat.emplace_back(std::move(gene));
            }
        }

        return std::make_pair(compat, incompat);
    };

    // Separa as isoladas e as blocos da 1 e da 2
    auto retBloco1 = sepCompat(blocos1);
    auto blocos1Comp = retBloco1.first;
    auto blocos1Incomp = retBloco1.second;
    auto retBloco2 = sepCompat(blocos2);
    auto blocos2Comp = retBloco2.first;
    auto blocos2Incomp = retBloco2.second;

    auto retIso1 = sepCompat(isoladas1);
    auto iso1Comp = retIso1.first;
    auto iso1Incomp = retIso1.second;
    auto retIso2 = sepCompat(isoladas2);
    auto iso2Comp = retIso2.first;
    auto iso2Incomp = retIso2.second;

    // Tenta inserir um bloco no horário de acordo com a posição determinada anteriormente
    auto insertBlocoComp = [](Horario* hor, std::vector<Gene>& blocos) {
        for (auto& b : blocos) {
            int coord[3];
            hor->get3DMatrix(b.posCruz, coord);
            if (!hor->insert(coord[1], coord[0], coord[2], b.disc)
                || !hor->insert(coord[1], coord[0] + 1, coord[2], b.disc)) {
                return false;
            }
        }
        return true;
    };

    // Tenta inserir uma isolada no horário de acordo com a posição determinada anteriormente
    auto insertIsoComp = [](Horario* hor, std::vector<Gene>& blocos) {
        for (auto& b : blocos) {
            if (!b.disc) {
                continue;
            }
            int coord[3];
            hor->get3DMatrix(b.posCruz, coord);
            if (!hor->insert(coord[0], coord[1], coord[2], b.disc)) {
                return false;
            }
        }
        return true;
    };

    // Para blocos incompatíveis, tenta-se achar um lugar no horário que caibam os dois juntos
    // Se não for encontrado, são adicionados na lista de isolados
    auto insertBlocoIncom = [&](Horario* hor, std::vector<Gene>& blocos, std::vector<Gene>& iso) {
        for (auto& b : blocos) {
            // Tenta inserir o bloco na matriz
            if (![&](Gene& g) {
                for (auto d = dia; d < dias_semana_util; d++) {
                    for (auto h = d == dia ? bloco : 0; h < blocosTamanho; h++) {
                        if (h < blocosTamanho - 1 && !hor->matriz[h] && !hor->matriz[h + 1] &&
                            hor->insert(d, h, camada, g.disc)) {
                            return true;
                        }
                    }
                }
                return false;
                // Não conseguindo, insere nas isoladas (duas instancias)
            }(b)) {
                iso.push_back(b);
                iso.emplace_back(std::move(b));
            }
        }
        return true;
    };

    // Insere uma isolada incompatível, procurando uma posição vazia na grade
    auto insertIsoIncom = [&](Horario* hor, std::vector<Gene>& blocos) {
        for (auto& b : blocos) {
            if (![&](Gene& g) {
                for (auto d = dia; d < dias_semana_util; d++) {
                    for (auto h = d == dia ? bloco : 0; h < blocosTamanho; h++) {
                        if (!hor->matriz[h] && hor->insert(d, h, camada, g.disc)) {
                            return true;
                        }
                    }
                }
                return false;
            }(b)) {
                return false;
            }
        }
        return true;
    };

    // Verifica se conseguiu terminar o cruzamento. Se não, volta para o fallback
    if (insertBlocoComp(filho1->horario, blocos2Comp) &&
        insertBlocoIncom(filho1->horario, blocos2Incomp, iso2Incomp) &&
        insertIsoComp(filho1->horario, iso2Comp) &&
        insertIsoIncom(filho1->horario, iso2Incomp)) {
        delete fallback1;
    } else {
        delete filho1;
        filho1 = fallback1;
    }

    if (insertBlocoComp(filho2->horario, blocos1Comp) &&
        insertBlocoIncom(filho2->horario, blocos1Incomp, iso2Incomp) &&
        insertIsoComp(filho2->horario, iso1Comp) &&
        insertIsoIncom(filho2->horario, iso1Incomp)) {
        delete fallback2;
    } else {
        delete filho2;
        filho2 = fallback2;
    }

    gerarGrade(filho1);
    gerarGrade(filho2);
    //printf("filho: %g\n", filho1->fo);
    //printf("filho: %g\n\n", filho2->fo);
    return {filho1, filho2};
}


bool Resolucao::gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG)
{
    bool success = false;
    ProfessorDisciplina* g = solucaoFilho->horario->at(diaG, blocoG, camadaG);

    for (int blocoReparo = 0; blocoReparo < blocosTamanho; blocoReparo++) {
        if (solucaoFilho->horario->at(diaG, blocoReparo, camadaG) == NULL) {
            success = solucaoFilho->horario->insert(diaG, blocoReparo, camadaG, g);
            if (success) {
                return true;
            }
        }
    }

    return false;
}

bool Resolucao::gerarHorarioAGCruzamentoAleatorioReparo(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG)
{
    if (gerarHorarioAGCruzamentoAleatorioReparoBloco(solucaoFilho, diaG, blocoG, camadaG)) {
        return true;
    }

    for (int diaReparo = 0; diaReparo < 6; diaReparo++) {
        if (diaReparo == diaG) {
            continue;
        }

        if (gerarHorarioAGCruzamentoAleatorioReparoBloco(solucaoFilho, diaReparo, blocoG, camadaG)) {
            return true;
        }
    }

    return false;
}

void Resolucao::gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& pop)
{
    gerarHorarioAGSobrevivenciaElitismo(pop, horarioPopulacaoInicial);
}

void Resolucao::gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& pop, int populacaoMax) const
{
    for (size_t i = populacaoMax; i < pop.size(); i++) {
        delete pop[i];
    }

    pop.resize(populacaoMax);
}

std::vector<Solucao*> Resolucao::gerarHorarioAGMutacao(std::vector<Solucao*>& pop)
{
    std::vector<Solucao*> genesX;

    for (auto j = 0u; j < pop.size(); j++) {
        auto porcentagem = aleatorio::randomInt() % 100 / 100.0;

        if (porcentagem <= horarioMutacaoProbabilidade) {
            //printf("pai: %g\n", populacao[j]->getFO());
            auto solucaoTemp = gerarHorarioAGMutacao(pop[j]);

            if (solucaoTemp) {
                //printf("filho: %g\n\n", solucaoTemp->getFO());
                genesX.push_back(solucaoTemp);
            }
        }
    }

    return genesX;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGMutacaoExper(std::vector<Solucao*>& pop, Configuracao::TipoMutacao tipoMut)
{
    std::vector<Solucao*> genesX;
    Solucao* solucaoTemp = nullptr;

    double porcentagem {};

    // Muta��o dos populacao
    for (auto j = 0u; j < pop.size(); j++) {
        porcentagem = (aleatorio::randomInt() % 100) / 100.0;

        if (porcentagem <= horarioMutacaoProbabilidade) {
            switch (tipoMut) {
                case Configuracao::TipoMutacao::substitui_professor:
                    solucaoTemp = gerarHorarioAGMutacaoSubstProf(*pop[j]);
                    break;
                case Configuracao::TipoMutacao::substiui_disciplina:
                    solucaoTemp = gerarHorarioAGMutacaoSubstDisc(pop[j]);
                    break;
            }

            if (solucaoTemp) {
                auto idx = 0;
                switch (tipoMut) {
                    case Configuracao::TipoMutacao::substiui_disciplina:
                        idx = 0;
                        break;
                    case Configuracao::TipoMutacao::substitui_professor:
                        idx = 1;
                        break;
                }
                if (solucaoTemp->getFO() > pop[j]->getFO()) {
                    contadorMelhoresMut[idx]++;
                } else if (solucaoTemp->getFO() == pop[j]->getFO()) {
                    contadorIguaisMut[idx]++;
                }
                genesX.push_back(solucaoTemp);
            }
        }
    }

    return genesX;
}

Solucao* Resolucao::gerarHorarioAGMutacaoSubstDisc(Solucao* pSolucao)
{
    //printf("pai: %g\n", pSolucao->fo);

    for (auto i = 0; i < horarioMutacaoTentativas; i++) {
        auto mut = std::make_unique<Solucao>(*pSolucao);
        auto camadaX = aleatorio::randomInt() % camadasTamanho;

        auto diaX1 = aleatorio::randomInt() % dias_semana_util;
        auto blocoX1 = 2 * Util::randomBetween(0, blocosTamanho / 2);
        auto x1 = mut->horario->getPosition(diaX1, blocoX1, camadaX);

        auto diaX2 = aleatorio::randomInt() % dias_semana_util;
        auto blocoX2 = 2 * Util::randomBetween(0, blocosTamanho / 2);
        auto x2 = mut->horario->getPosition(diaX2, blocoX2, camadaX);

        if (swapBlocos(*mut, x1, x2) && swapBlocos(*mut, x1 + 1, x2 + 1)) {
            gerarGrade(mut.get());
            return mut.release();
        }
    }

    return nullptr;
}

bool Resolucao::swapBlocos(Solucao& sol, int posX1, int posX2) const
{
    int diaX1, blocoX1, diaX2, blocoX2, camadaX;
    std::tie(diaX1, blocoX1, camadaX) = sol.horario->getCoords(posX1);
    std::tie(diaX2, blocoX2, std::ignore) = sol.horario->getCoords(posX2);

    auto pdX1 = sol.horario->at(posX1);
    auto pdX2 = sol.horario->at(posX2);

    // Não faz sentido trocar duas Pd iguais. Se ambas forem nulas,
    // também tenta de novo
    if (pdX1 == pdX2) {
        return false;
    }
    // Se o slot  de x1 for vazio
    if (!pdX1) {
        if (sol.horario->insert(diaX1, blocoX1, camadaX, pdX2)) {
            sol.horario->clearSlot(diaX2, blocoX2, camadaX);
            return true;
        }
        return false;
    }
    // Se o slot  de x2 for vazio
    if (!pdX2) {
        if (sol.horario->insert(diaX2, blocoX2, camadaX, pdX1)) {
            sol.horario->clearSlot(diaX1, blocoX1, camadaX);
            return true;
        }
        return false;
    }

    // Se ambos forem preenchidos
    sol.horario->clearSlot(diaX1, blocoX1, camadaX);
    sol.horario->clearSlot(diaX2, blocoX2, camadaX);

    if (sol.horario->insert(diaX1, blocoX1, camadaX, pdX2)
        && sol.horario->insert(diaX2, blocoX2, camadaX, pdX1)) {
        return true;
    }
    return false;
}

Solucao* Resolucao::gerarHorarioAGMutacao(Solucao* pSolucao)
{
    switch (horarioTipoMutacao) {
        case Configuracao::TipoMutacao::substiui_disciplina:
            return gerarHorarioAGMutacaoSubstDisc(pSolucao);
        case Configuracao::TipoMutacao::substitui_professor:
            return gerarHorarioAGMutacaoSubstProf(*pSolucao);
    }
    return nullptr;
}

double Resolucao::gerarGradeTipoGrasp2(Solucao* sol)
{
    auto total = 0;
    std::unordered_map<long long, Grade*> gradesGeradas;

    for (auto& par : alunoPerfis) {
        auto aluno = par.second;

        Grade* novaGrade = nullptr;
        if (gradesGeradas[aluno->getHash()]) {
            novaGrade = new Grade(*gradesGeradas[aluno->getHash()]);
            novaGrade->aluno = aluno;
        } else {
            novaGrade = GRASP(aluno, sol);
            gradesGeradas[aluno->getHash()] = novaGrade;
        }

        //printf("Aluno: %s\nDiscs: ", aluno->id.c_str());
        //for (auto& disc : novaGrade->disciplinasAdicionadas) {
        //    printf("%s ", disc->nome.c_str());
        //}
        //puts("\n");

        total += static_cast<int>(novaGrade->getFO());
        sol->insertGrade(novaGrade);
    }
    sol->fo = total;

    return total;
}

double Resolucao::gerarGrade()
{
    return gerarGrade(solucao);
}

double Resolucao::gerarGrade(Solucao* pSolucao)
{
    for (auto& par : pSolucao->grades) {
        delete par.second;
    }
    pSolucao->grades.clear();
    pSolucao->gradesLength = 0;

    switch (gradeTipoConstrucao) {
        case Configuracao::TipoGrade::guloso:
            return gerarGradeTipoGuloso(pSolucao);

        case Configuracao::TipoGrade::grasp:
            return gerarGradeTipoGrasp2(pSolucao);

        case Configuracao::TipoGrade::combinatorio:
            return gerarGradeTipoCombinacaoConstrutiva(pSolucao);

        case Configuracao::TipoGrade::modelo:
            return gerarGradeTipoModelo(pSolucao);
    }

    return 0;
}

double Resolucao::gerarGradeTipoGuloso(Solucao*& pSolucao)
{
    Horario* horario {nullptr};
    Grade* apGrade {nullptr};

    auto apIter = alunoPerfis.begin();
    auto apIterEnd = alunoPerfis.end();

    horario = pSolucao->horario;

    for (; apIter != apIterEnd; ++apIter) {
        if (verbose)
            std::cout << apIter->first << std::endl;
        auto alunoPerfil = alunoPerfis[apIter->first];

        apGrade = new Grade(blocosTamanho, alunoPerfil, horario, disciplinas, disciplinasIndex);

        auto apRestante = alunoPerfil->restante;
        auto apCursadas = alunoPerfil->cursadas;

        auto dIter = apRestante.begin();
        auto dIterEnd = apRestante.end();

        for (; dIter != dIterEnd; ++dIter) {
            apGrade->insert(getDisciplinaByName(*dIter));
        }

        pSolucao->insertGrade(apGrade);
    }

    return pSolucao->getFO();
}

Grade* Resolucao::gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, int maxDeep, int deep,
                                                      std::unordered_set<std::string>::const_iterator current)
{
    Grade* bestGrade = new Grade(*pGrade);
    Grade* currentGrade {nullptr};

    auto disciplinasRestantes = pGrade->aluno->restante;
    double bestFO {};
    double currentFO {};

    bool viavel {};

    for (auto it = current; it != std::end(disciplinasRestantes); ++it) {
        currentGrade = new Grade(*pGrade);

        if (verbose)
            std::cout << "Nivel: " << (deep) << " Disciplina: " << *it << ")\n";

        viavel = currentGrade->insert(getDisciplinaByName(*it));
        if (viavel) {

            if (deep != maxDeep) {
                currentGrade = gerarGradeTipoCombinacaoConstrutiva(currentGrade, maxDeep, deep + 1, ++it);
            }

            bestFO = bestGrade->getFO();
            currentFO = currentGrade->getFO();
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

Grade* Resolucao::gerarGradeTipoCombinacaoConstrutiva(
    Grade* pGrade, int maxDeep)
{
    auto restantes = pGrade->aluno->restante;
    return gerarGradeTipoCombinacaoConstrutiva(pGrade, maxDeep, 0,
                                               std::begin(restantes));
}

double Resolucao::gerarGradeTipoCombinacaoConstrutiva(Solucao*& pSolucao)
{
    Horario* horario {nullptr};
    Grade* apGrade {nullptr};

    auto apIter = alunoPerfis.begin();
    auto apIterEnd = alunoPerfis.end();

    horario = pSolucao->horario;

    for (; apIter != apIterEnd; ++apIter) {
        if (verbose)
            std::cout << "[" << apIter->first << "]" << std::endl;
        auto alunoPerfil = alunoPerfis[apIter->first];

        auto apRestante = alunoPerfil->restante;

        apGrade = gerarGradeTipoCombinacaoConstrutiva(new Grade(
                                                          blocosTamanho, alunoPerfil, horario,
                                                          disciplinas, disciplinasIndex), apRestante.size());

        pSolucao->insertGrade(apGrade);

        const auto& escolhidas = apGrade->disciplinasAdicionadas;

        for (const auto disc : escolhidas) {
            disc->alocados++;
            if (disc->alocados >= disc->capacidade) {
                disc->ofertada = false;
            }
        }

    }

    return pSolucao->getFO();
}

void Resolucao::gerarGradeTipoGraspConstrucao(Grade* pGrade)
{
    std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

    gerarGradeTipoGraspConstrucao(pGrade, professorDisciplinasIgnorar);
}

void Resolucao::gerarGradeTipoGraspConstrucao(Grade* pGrade,
                                              std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar)
{
    auto disponivel = (dias_semana_util - 2) * camadasTamanho;
    auto alunoPerfil = pGrade->aluno;
    auto apRestante = alunoPerfil->restanteOrd;
    auto dIterStart = apRestante.begin();

    auto adicionados = 0;
    while (apRestante.size() != 0 && dIterStart != apRestante.end()
        && disponivel != adicionados) {
        auto distancia = getIntervaloAlfaGrasp(apRestante);
        auto rand = Util::randomBetween(0, distancia);
        auto current = apRestante.begin();
        std::advance(current, rand);
        auto disciplina = *current;

        if (pGrade->insert(disciplina, professorDisciplinasIgnorar)) {
            adicionados++;
        }
        apRestante.erase(current);

        dIterStart = apRestante.begin();
    }
}

void Resolucao::gerarGradeTipoGraspConstrucao(Solucao* pSolucao)
{
    Horario* horario {nullptr};
    Grade* apGrade {nullptr};

    auto apIter = alunoPerfis.begin();
    auto apIterEnd = alunoPerfis.end();
    AlunoPerfil* alunoPerfil {nullptr};

    horario = pSolucao->horario;

    for (; apIter != apIterEnd; ++apIter) {

        if (verbose)
            std::cout << apIter->first << std::endl;
        alunoPerfil = alunoPerfis[apIter->first];

        apGrade = new Grade(blocosTamanho, alunoPerfil, horario, disciplinas,
                            disciplinasIndex);

        gerarGradeTipoGraspConstrucao(apGrade);

        pSolucao->insertGrade(apGrade);
    }
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamentoAleatorio(Solucao* pSolucao)
{
    Solucao* bestSolucao = new Solucao(*pSolucao);
    Solucao* currentSolucao {nullptr};

    AlunoPerfil* alunoPerfil {nullptr};

    Grade* grade {nullptr};

    std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

    int random {};
    int disciplinasSize {};
    int disciplinasRemoveMax {};
    int disciplinasRemoveRand {};
    double bestFO {};
    double currentFO {};

    bestFO = bestSolucao->getFO();

    for (int i = 0; i < gradeGraspVizinhos; i++) {
        currentSolucao = new Solucao(*pSolucao);

        auto apIter = alunoPerfis.begin();
        auto apIterEnd = alunoPerfis.end();

        if (verbose)
            std::cout << "------NGH" << i << std::endl;

        for (; apIter != apIterEnd; ++apIter) {
            alunoPerfil = alunoPerfis[apIter->first];

            grade = currentSolucao->grades[alunoPerfil->id];

            disciplinasSize = grade->disciplinasAdicionadas.size();
            disciplinasRemoveMax = static_cast<int>(ceil(disciplinasSize * 1));
            disciplinasRemoveRand = Util::randomBetween(1, disciplinasRemoveMax);

            for (int j = 0; j < disciplinasRemoveRand; j++) {
                ProfessorDisciplina* professorDisciplinaRemovido = NULL;

                disciplinasSize = grade->disciplinasAdicionadas.size();
                random = Util::randomBetween(0, disciplinasSize);
                grade->remove2(grade->disciplinasAdicionadas[random], professorDisciplinaRemovido);

                // Se houve uma remo��o
                if (professorDisciplinaRemovido != NULL) {
                    professorDisciplinasIgnorar.push_back(professorDisciplinaRemovido);
                }
            }

            gerarGradeTipoGraspConstrucao(grade, professorDisciplinasIgnorar);
        }
        currentFO = currentSolucao->getFO();
        if (verbose)
            std::cout << std::endl << "------NGH" << i << ": L(" << bestFO << ") < C(" << currentFO << ")" << std::endl;

        if (bestFO < currentFO) {
            delete bestSolucao;
            bestSolucao = currentSolucao;
            bestFO = currentFO;

            if (verbose)
                std::cout << "------NGH new best: " << bestFO << std::endl;
            i = 0;
        } else {
            delete currentSolucao;
        }
    }

    return bestSolucao;
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamentoCrescente(Solucao* pSolucao)
{
    Solucao* bestSolucao = new Solucao(*pSolucao);
    Solucao* currentSolucao {nullptr};

    AlunoPerfil* alunoPerfil {nullptr};

    Disciplina* disciplinaRemovida {nullptr};
    std::vector<Disciplina*> disciplinasRemovidas {};

    Grade* bestGrade {nullptr};
    Grade* currentGrade {nullptr};

    int random {};

    double bestFO {};
    double currentFO {};

    auto apIter = alunoPerfis.begin();
    auto apIterEnd = alunoPerfis.end();

    for (; apIter != apIterEnd; ++apIter) {

        alunoPerfil = alunoPerfis[apIter->first];
        bestGrade = bestSolucao->grades[alunoPerfil->id];

        for (int i = 0; i < gradeGraspVizinhos; i++) {
            currentSolucao = new Solucao(*pSolucao);
            currentGrade = currentSolucao->grades[alunoPerfil->id];

            if (verbose)
                std::cout << std::endl << "------NGH" << i << std::endl;

            disciplinasRemovidas.clear();
            auto disciplinasRestantes = alunoPerfil->restante;

            for (int j = 0; j < (i + 1); j++) {
                random = Util::randomBetween(0, currentGrade->disciplinasAdicionadas.size());
                if (random == -1) {
                    break;
                }
                disciplinaRemovida = currentGrade->remove(currentGrade->disciplinasAdicionadas[random]);
                if (disciplinaRemovida) {
                    disciplinasRemovidas.push_back(disciplinaRemovida);
                }
            }

            RemoveDisciplinasNome(disciplinasRestantes, disciplinasRemovidas);
            RemoveDisciplinasNome(disciplinasRestantes, currentGrade->disciplinasAdicionadas);
            gerarGradeTipoCombinacaoConstrutiva(currentGrade, i);

            bestFO = bestGrade->getFO();
            currentFO = currentGrade->getFO();
            if (verbose)
                std::cout << "------NGH" << i << ": L(" << bestFO << ") < C(" << currentFO << ")" << std::endl;
            if (bestFO < currentFO) {
                delete bestSolucao;
                bestSolucao = currentSolucao;
                bestFO = currentFO;

                if (verbose)
                    std::cout << "------NGH new best: " << bestFO << std::endl;
                i = 0;
            } else {
                delete currentSolucao;
            }
        }
    }

    return bestSolucao;
}

double Resolucao::gerarGradeTipoGrasp()
{
    return gerarGradeTipoGrasp(solucao);
}

double Resolucao::gerarGradeTipoGrasp(Solucao*& pSolucao)
{
    Solucao* currentSolucao {nullptr};
    Solucao* temp {nullptr};

    double bestFO {0};
    double currentFO {};

    clock_t t0 {};
    double diff {0};

    if (verbose)
        std::cout << "HORARIO (Solucao) :" << std::endl;

    int iteracoes = 0;
    while (diff <= gradeGraspTempoConstrucao) {
        currentSolucao = new Solucao(*pSolucao);

        t0 = clock();
        gerarGradeTipoGraspConstrucao(currentSolucao);

        if (verbose)
            std::cout << "----FIT: " << currentSolucao->getFO() << std::endl;

        temp = currentSolucao;

        switch (gradeGraspVizinhanca) {
            case Configuracao::TipoVizinhos::aleatorios:
                currentSolucao = gerarGradeTipoGraspRefinamentoAleatorio(currentSolucao);
                break;
            case Configuracao::TipoVizinhos::crescente:
                currentSolucao = gerarGradeTipoGraspRefinamentoCrescente(currentSolucao);
                break;
        }
        if (temp->id != currentSolucao->id)
            delete temp;

        if (verbose)
            std::cout << "----FIT(NGH):" << currentSolucao->getFO() << std::endl;

        diff += Util::timeDiff(clock(), t0);

        currentFO = currentSolucao->getFO();
        if (bestFO < currentFO) {
            delete pSolucao;
            pSolucao = currentSolucao;
            bestFO = currentFO;
            diff = 0;

            if (verbose)
                std::cout << "----NGH is the new best (gerarGradeTipoGrasp)" << std::endl;
        } else {
            delete currentSolucao;
        }
        if (verbose)
            std::cout << "-------------------------------------------------" << std::endl;

        iteracoes++;
    }
    //std::cout << iteracoes << std::endl;

    return pSolucao->getFO();
}

int Resolucao::getIntervaloAlfaGrasp(const std::vector<Disciplina*>& apRestante)
{
    auto bestFIT = apRestante.front()->cargaHoraria;
    auto worstFIT = apRestante.back()->cargaHoraria;
    auto deltaFIT = bestFIT - worstFIT;
    auto acceptFIT = bestFIT - Util::fast_ceil((1 - gradeAlfa) * deltaFIT);

    auto it = std::lower_bound(begin(apRestante), end(apRestante), acceptFIT,
                               [](Disciplina* d, int accept) {
                                   return d->cargaHoraria >= accept;
                               });

    return std::distance(begin(apRestante), it);
}

void Resolucao::showResult()
{
    showResult(solucao);
}

void Resolucao::showResult(Solucao* pSolucao)
{
    const auto& grades = pSolucao->grades;
    for (const auto& par : grades) {
        const auto gradeAtual = par.second;
        std::cout << gradeAtual->aluno->id << ":\n";
        const auto& discEscolhidas = gradeAtual->disciplinasAdicionadas;
        for (const auto disc : discEscolhidas) {

            std::cout << disc->nome << " ";
        }
        std::cout << " " << gradeAtual->getFO() << "\n";
    }
    std::cout << "\nFO da solucao: " << pSolucao->getFO() << "\n";
}

Solucao* Resolucao::getSolucao()
{
    return solucao;
}

void Resolucao::teste()
{
    std::unique_ptr<Solucao> sol {};
    puts("Gerando solucao");
    while (!sol) {
        sol = std::move(gerarSolucaoAleatoria());
    }
    printf("hash: %u\n", sol->getHash());

    // bench grasp
    auto n = 5;
    auto t_grasp = 0ll;
    auto fo_grasp = 0.0;
    auto max_fo_grasp = 0.0;
    auto min_fo_grasp = std::numeric_limits<double>::max();
    gradeTipoConstrucao = Configuracao::TipoGrade::grasp;
    for (auto i = 0; i < n; i++) {
        auto begin = Util::now();
        auto curr_fo = gerarGrade(sol.get());

        max_fo_grasp = std::max(curr_fo, max_fo_grasp);
        min_fo_grasp = std::min(curr_fo, min_fo_grasp);
        fo_grasp += curr_fo;

        auto end = Util::now();
        auto total = Util::chronoDiff(end, begin);
        t_grasp += total;
        std::cout << "Tempo: " << total << "\n";
        std::cout << "Fo: " << curr_fo << "\n\n";
    }
    std::cout << "Media tempo:" << t_grasp / (1. * n) << "\n";
    std::cout << "Media FO:   " << fo_grasp / n << "\n";
    std::cout << "Menor fo:   " << min_fo_grasp << "\n";
    std::cout << "Maior fo:   " << max_fo_grasp << "\n\n\n";

    auto savePath = Util::join_path({"teste", "fo" + std::to_string(fo_grasp / n)
        + "grasp"});
    Output::write(sol.get(), savePath);

    //bench modelo
    gradeTipoConstrucao = Configuracao::TipoGrade::modelo;
    auto t1 = Util::now();
    auto r = gerarGrade(sol.get());
    auto t2 = Util::now();
    auto t = Util::chronoDiff(t2, t1);
    std::cout << "Tempo mod: " << t << "\n";
    std::cout << "FO modelo: " << r << "\n";
    //
    //reinsereGrades(sol);
    //savePath = Util::join_path({"teste", "fo" + std::to_string(r) + "modelo"});
    //Output::write(sol, savePath);
    //showResult(sol);
}

double Resolucao::gerarGradeTipoModelo(Solucao* pSolucao)
{
#ifdef MODELO
    auto horarioBin = converteHorario(pSolucao);
    auto total = 0.0;
    std::unordered_map<long long, Grade*> gradesGeradas;

    for (const auto& aluno : alunos) {
        auto currAluno = alunoPerfis[aluno.nome()];
        Grade* novaGrade = nullptr;

        if (gradesGeradas[currAluno->getHash()]) {
            novaGrade = new Grade(*gradesGeradas[currAluno->getHash()]);
            novaGrade->aluno = currAluno;
        } else {
            novaGrade = new Grade(blocosTamanho, currAluno, pSolucao->horario,
                                  disciplinas, disciplinasIndex);
            fagoc::Modelo_solver solver(*curso, aluno, horarioBin,
                                        tempoLimiteModelo);
            solver.solve();
            const auto& solucao = solver.solucao();
            novaGrade->fo = solucao.funcao_objetivo;

            auto& adicionadas = novaGrade->disciplinasAdicionadas;
            for (const auto& disc : solucao.nomes_disciplinas) {
                adicionadas.push_back(disciplinas[disciplinasIndex[disc]]);
            }
            gradesGeradas[currAluno->getHash()] = novaGrade;
        }

        total += novaGrade->fo;
        pSolucao->insertGrade(novaGrade);
    }
    pSolucao->fo = total;

    return total;
#else
    return 0;
#endif
}

std::vector<std::vector<char>>
Resolucao::converteHorario(Solucao* pSolucao) const
{
#ifdef MODELO
    const auto& matriz = pSolucao->horario->matriz;
    auto numDisciplinas = disciplinas.size();
    auto numHorarios = SEMANA * blocosTamanho;
    std::vector<std::vector<char>> horarioBin(
        numHorarios, std::vector<char>(numDisciplinas, 0));
    int posicoes[3];

    for (auto i = 0u; i < matriz.size(); i++) {
        if (!matriz[i]) {
            continue;
        }
        auto index = curso->id_to_indice()[matriz[i]->disciplina->id];
        pSolucao->horario->get3DMatrix(i, posicoes);
        auto dia = posicoes[1];
        auto bloco = posicoes[0];
        auto posicao = (dia * blocosTamanho) + bloco;

        horarioBin[posicao][index] = 1;
    }
    return horarioBin;
#else
    return {};
#endif
}

void Resolucao::logExperimentos()
{
    char str[101];
    sprintf(str, "Tempo total selecao: %llu", tempoTotalSelec);
    log << str << "\n";
    puts(str);

    sprintf(str, "Tempo total elitismo: %llu", tempoTotalElit);
    log << str << "\n";
    puts(str);

    for (auto i = 0; i < numcruz; i++) {
        sprintf(str, "C%d: tempo total: %llu iguais: %d melhores: %d", i + 1,
                tempoTotalCruz[i], contadorIguaisCruz[i], contadorMelhoresCruz[i]);
        log << str << "\n";
        puts(str);
    }
    for (auto i = 0; i < 2; i++) {
        sprintf(str, "M%d: tempo total: %llu iguais: %d melhores: %d", i + 1,
                tempoTotalMut[i], contadorIguaisMut[i], contadorMelhoresMut[i]);
        log << str << "\n";
        puts(str);
    }

    log << "\n\n";
    puts("\n");
}

Solucao*
Resolucao::gerarHorarioAGMutacaoSubstProf(const Solucao& pSolucao) const
{
    for (auto i = 0; i < horarioMutacaoTentativas; i++) {
        auto mut = std::make_unique<Solucao>(pSolucao);

        // Determina aleatoriamente qual disciplina terá seu professor substituído
        auto idx = aleatorio::randomInt() % mut->horario->matriz.size();
        auto profDisc = mut->horario->matriz[idx];
        if (!profDisc) {
            continue;
        }

        std::unordered_set<Professor*> professoresCapacitados(
            begin(profDisc->disciplina->professoresCapacitados),
            end(profDisc->disciplina->professoresCapacitados));
        if (professoresCapacitados.size() < 2) {
            continue;
        }

        // Encontra um professor substituto, que não seja o original
        professoresCapacitados.erase(profDisc->professor);
        auto rand = aleatorio::randomInt() % professoresCapacitados.size();
        auto current = begin(professoresCapacitados);
        std::advance(current, rand);
        auto prof = *current;

        // Remove todas as ocorrências da alocãção, guardando suas posições
        int coord[3];
        mut->horario->get3DMatrix(idx, coord);
        auto camada = coord[2];
        std::vector<std::pair<int, int>> posicoesAloc;

        for (auto d = 0; d < dias_semana_util; d++) {
            for (auto h = 0; h < blocosTamanho; h++) {
                auto linearPos = mut->horario->getPosition(d, h, camada);
                if (mut->horario->matriz[linearPos] == profDisc) {
                    posicoesAloc.emplace_back(d, h);
                    mut->horario->matriz[linearPos] = nullptr;
                }
            }
        }

        // Modifica a alocação para o novo professor
        profDisc->professor = prof;
        // Reinsere com a nova alocação
        auto success = true;
        for (const auto& pos : posicoesAloc) {
            success = mut->horario->insert(pos.first, pos.second, camada,
                                           profDisc);
            if (!success) {
                break;
            }
        }
        if (success) {
            return mut.release();
        }
    }
    return nullptr;
}

void Resolucao::logPopulacao(const std::vector<Solucao*>& pop, int iter)
{
    // Imprime FO e Hash de todas as soluções e determina frequência dos hashes
    std::unordered_map<std::size_t, int> freq;
    printf("iteracao: %d\n", iter);
    log << "iteracao: " << iter << "\n";

    printf("%-8s %s\n", "FO", "Hash");
    log << std::left << std::setw(8) << "FO" << " Hash\n";
    for (const auto& individuo : pop) {
        auto hash = std::to_string(individuo->getHash());
        log << std::left << std::setw(8) << individuo->getFO()
                << " " << hash << "\n";
        printf("%-8g %s\n", individuo->getFO(),
               hash.c_str());
        freq[individuo->getHash()]++;
    }

    printf("\n%-11s %-5s %s\n", "Hash", "Freq", "%");
    log << std::left << std::setw(11) << "Hash" << std::setw(5)
            << "Freq" << " %\n";

    for (const auto& par : freq) {
        auto porc = par.second * 1. / pop.size() * 100;
        log << std::left << std::setw(11) << std::left << par.first
                << std::setw(5) << par.second << porc << "\n";
        printf("%-11u %-5d %g\n", par.first, par.second, porc);
    }

    log << "\n\n";
    printf("\n\n");
}

std::vector<Solucao*> Resolucao::gerarHorarioAGPopulacaoInicial2()
{
    /*    std::vector<Solucao*> populacaoInical;
    
        auto solucoes = gerarSolucoesAleatorias(horarioPopulacaoInicial);
        Util::insert_sorted(populacaoInical, begin(solucoes), end(solucoes),
                            SolucaoComparaMaior());
    
        return populacaoInical;*/

    auto solucoes = gerarSolucoesAleatorias2(horarioPopulacaoInicial);
    std::sort(begin(solucoes), end(solucoes), SolucaoComparaMaior());
    return solucoes;
}

bool Resolucao::
gerarCamada(Solucao* sol, int camada, std::vector<Disciplina*> discs,
            std::unordered_map<std::string, int>& creditos_alocados_prof)
{
    auto num_discs = discs.size();
    auto num_disc_visitadas = 0u;
    std::vector<bool> disc_visitadas(num_discs, false);

    while (num_disc_visitadas < num_discs) {
        int rnd_disc;
        do {
            rnd_disc = aleatorio::randomInt() % num_discs;
        } while (disc_visitadas[rnd_disc]);

        disc_visitadas[rnd_disc] = true;
        num_disc_visitadas++;
        auto disc = discs[rnd_disc];

        auto success = geraProfessorDisciplina(sol, disc, camada, creditos_alocados_prof);
        if (!success) {
            return false;
        }
    }
    return true;
}

bool Resolucao::geraProfessorDisciplina(
    Solucao* sol,
    Disciplina* disc,
    int camada,
    std::unordered_map<std::string, int>& creditos_alocados_prof
)
{
    auto& profs = disc->professoresCapacitados;
    auto num_profs = profs.size();
    auto num_prof_visitados = 0u;
    std::vector<bool> prof_visitados(num_profs, false);
    auto success = false;

    while (!success && num_prof_visitados < num_profs) {
        int rnd_prof;
        do {
            rnd_prof = aleatorio::randomInt() % num_profs;
        } while (prof_visitados[rnd_prof]);

        prof_visitados[rnd_prof] = true;
        num_prof_visitados++;
        auto prof = profs[rnd_prof];

        if (prof->creditoMaximo != 0 &&
            prof->creditoMaximo < creditos_alocados_prof[prof->nome]
            + disc->cargaHoraria) {
            continue;
        }
        success = geraAlocacao(sol, disc, prof, camada);
    }
    return success;
}

bool Resolucao::geraAlocacao(Solucao* sol, Disciplina* disc, Professor* prof, int camada)
{
    //puts("aloc");
    //printf("camada: %d\n", camada);
    //printf("disc: %s\nprof: %s\n", disc->nome.c_str(), prof->nome.c_str());
    //printf("id: %s\n", disc->id.c_str());
    auto pdId = "pr" + prof->id + "di" + disc->id;
    if (!professorDisciplinas[pdId]) {
        professorDisciplinas[pdId] = new ProfessorDisciplina(prof, disc);
    }
    auto pd = professorDisciplinas[pdId];
    sol->horario->discCamada[disc->id] = camada;

    auto creditos_alocados_disc = 0;
    auto num_slots = dias_semana_util * blocosTamanho;
    auto num_slot_visitados = 0;
    std::vector<std::vector<bool>> slots_visitados(
        dias_semana_util, std::vector<bool>(blocosTamanho, false));

    while (creditos_alocados_disc < disc->cargaHoraria && num_slot_visitados < num_slots) {
        auto restante = disc->cargaHoraria - creditos_alocados_disc;
        int bloco {};
        int dia {};
        auto success_prof = false;

        while (!success_prof && num_slot_visitados < num_slots) {
            do {
                dia = aleatorio::randomInt() % dias_semana_util;
                // Se restante for maior que um, deve alocar um bloco de duas disciplinas
                // em um horário par. Para isso se gera um número até a metade do número
                // de blocos e multiplica por dois. Ex: 4 blocos, serão gerados números 0 ou 1,
                // que possibilitam o bloco 0 ou 2.
                if (restante > 1) {
                    bloco = 2 * (aleatorio::randomInt() % (blocosTamanho / 2));
                } else {
                    // Se não for o caso, qualquer bloco serve
                    bloco = aleatorio::randomInt() % blocosTamanho;
                }
            } while (slots_visitados[dia][bloco]);

            if (restante == 1) {
                slots_visitados[dia][bloco] = true;
                num_slot_visitados++;
                success_prof = sol->horario->insert(dia, bloco, camada, pd);
                if (success_prof) {
                    creditos_alocados_disc++;
                }
            } else {
                slots_visitados[dia][bloco] = true;
                slots_visitados[dia][bloco + 1] = true;
                num_slot_visitados += 2;
                auto success_bloco1 = sol->horario->insert(dia, bloco,
                                                           camada, pd);
                auto success_bloco2 = sol->horario->insert(dia, bloco + 1,
                                                           camada, pd);
                success_prof = success_bloco1 && success_bloco2;
                if (success_prof) {
                    creditos_alocados_disc += 2;
                } else {
                    if (success_bloco1) {
                        sol->horario->clearSlot(dia, bloco, camada);
                    }
                    if (success_bloco2) {
                        sol->horario->clearSlot(dia, bloco + 1, camada);
                    }
                }
            }
        }
    }

    auto succ = creditos_alocados_disc == disc->cargaHoraria;
    if (!succ) {
        //puts("Problema Aloc");
        //printf("cred: %d aloc: %d\n", disc->cargaHoraria, creditos_alocados_disc);
    }
    //puts("");

    return succ;
}

std::unique_ptr<Solucao> Resolucao::gerarSolucaoAleatoria()
{
    auto solucaoRnd = std::make_unique<Solucao>(blocosTamanho, camadasTamanho,
                                                perfisTamanho);
    std::unordered_map<std::string, int> creditos_alocados_prof;

    auto num_periodos = periodoXdisciplina.size();
    auto num_per_visitados = 0u;
    std::vector<bool> periodos_visitados(num_periodos, false);

    // Percorre todos os períodos aleatoriamente
    while (num_per_visitados < num_periodos) {
        // Escolhe um período. Escolhe outro se já for percorrido.
        int rnd_per;
        do {
            rnd_per = aleatorio::randomInt() % num_periodos;
        } while (periodos_visitados[rnd_per]);

        periodos_visitados[rnd_per] = true;
        num_per_visitados++;

        // Encontra o período referente ao número, copia suas disciplinas
        auto it = periodoXdisciplina.begin();
        std::advance(it, rnd_per);
        solucaoRnd->camada_periodo[rnd_per] = it->first;
        auto& discs = it->second;

        //printf("c: %s\n", it->first.c_str());
        auto success = gerarCamada(solucaoRnd.get(), rnd_per, discs,
                                   creditos_alocados_prof);
        if (!success) {
            return nullptr;
        }
    }
    return solucaoRnd;
}

std::vector<Solucao*> Resolucao::gerarSolucoesAleatorias(int numSolucoes)
{
    std::vector<Solucao*> solucoes;
    for (auto i = 0; i < numSolucoes; i++) {
        std::unique_ptr<Solucao> currSolucao {};
        while (!currSolucao) {
            currSolucao = std::move(gerarSolucaoAleatoria());
        }
        gerarGrade(currSolucao.get());
        solucoes.push_back(currSolucao.release());
    }
    //puts("\n");
    return solucoes;
}

std::vector<Solucao*> Resolucao::gerarSolucoesAleatorias2(int numSolucoes)
{
    std::vector<Solucao*> solucoes(numSolucoes);
    for (auto& s : solucoes) {
        s = gerarSolucaoAleatoriaNotNull().release();
    }
    return solucoes;
}

std::unique_ptr<Solucao> Resolucao::gerarSolucaoAleatoriaNotNull()
{
    std::unique_ptr<Solucao> s {};
    while (!s) {
        s = std::move(gerarSolucaoAleatoria());
    }
    return s;
}

void Resolucao::reinsereGrades(Solucao* sol) const
{
    // Aqui as disciplinas são inseridas na matriz. É necessário copiar as
    // adicionadas e depois resetá-las para a inserção funcionar.
    for (auto& par : sol->grades) {
        auto& grade = *par.second;
        std::vector<Disciplina*> disc_add_copy {};
        std::swap(disc_add_copy, grade.disciplinasAdicionadas);
        for (auto& disc : disc_add_copy) {
            grade.insert(disc, {}, true);
        }
    }
}

// void Resolucao::
// gerarHorarioAGEfetuaCruzamento(std::vector<Solucao*>& populacao, int numCruz)
// {
//     for (auto j = 0; j < numCruz; j++) {
//         auto parVencedor = gerarHorarioAGTorneioPar(populacao);
//         auto filhos = gerarHorarioAGCruzamento(parVencedor);
//         Util::insert_sorted(populacao, begin(filhos), end(filhos),
//                             SolucaoComparaMaior());
//     }
// }

void Resolucao::
gerarHorarioAGEfetuaCruzamento(std::vector<Solucao*>& pop, int numCruz)
{
    std::vector<Solucao*> prole {};

    for (auto j = 0; j < numCruz; j++) {
        auto parVencedor = gerarHorarioAGTorneioPar(pop);
        auto filhos = gerarHorarioAGCruzamento(parVencedor);
        prole.insert(end(prole), begin(filhos), end(filhos));
    }

    Util::insert_sorted(pop, begin(prole), end(prole),
                        SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGEvoluiPopulacaoExper(
    std::vector<Solucao*>& pop,
    Configuracao::TipoCruzamento tipoCruz,
    std::vector<Solucao*>& pais
)
{
    auto filhos = gerarHorarioAGCruzamentoExper(pais, tipoCruz);
    Util::insert_sorted(pop, begin(filhos), end(filhos),
                        SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGEfetuaMutacao(std::vector<Solucao*>& pop)
{
    auto geneX = gerarHorarioAGMutacao(pop);
    Util::insert_sorted(pop, begin(geneX), end(geneX),
                        SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGEfetuaMutacaoExper(
    std::vector<Solucao*>& pop,
    Configuracao::TipoMutacao tipoMut
)
{
    auto geneX = gerarHorarioAGMutacaoExper(pop, tipoMut);
    Util::insert_sorted(pop, begin(geneX), end(geneX),
                        SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGVerificaEvolucao(
    std::vector<Solucao*>& pop,
    int iteracaoAtual
)
{
    auto& best = *populacao[0];

    if (best.getFO() > foAlvo) {
        foAlvo = best.getFO();
        hashAlvo = best.getHash();
        iteracaoAlvo = iteracaoAtual;
        tempoAlvo = Util::chronoDiff(std::chrono::steady_clock::now(),
                                     tempoInicio);
        //puts("Nova melhor solucao!");
        log << "Nova melhor solucao!\n";
        char str[101];
        sprintf(str, "%-8g %u\n", best.getFO(), best.getHash());
        //puts(str);
        log << str;
    }
}

std::unique_ptr<Solucao> Resolucao::gerarHorarioSA_ILS(
    const SA& sa, const ILS& ils)
{
    auto s = gerarSolucaoAleatoriaNotNull();
    auto s_sa = sa.gerar_horario(*s);
    auto s_ils = ils.gerar_horario(*s_sa);
    return s_ils;
}

std::unique_ptr<Grade>
Resolucao::gradeAleatoria(AlunoPerfil* alunoPerfil, Solucao* sol)
{
    auto novaGrade = std::make_unique<Grade>(
        blocosTamanho, alunoPerfil, sol->horario, disciplinas, disciplinasIndex);
    auto horariosMax = (dias_semana_util - 2) * blocosTamanho;
    auto adicionados = 0;
    auto restantes = alunoPerfil->restanteOrd;

    while (!restantes.empty() && adicionados < horariosMax) {
        auto current = getRandomDisc(restantes);

        auto success = novaGrade->insert(current);
        if (success) {
            adicionados++;
        }
        restantes.erase(std::remove(begin(restantes), end(restantes), current),
                        end(restantes));
    }

    return novaGrade;
}

std::unique_ptr<Grade> Resolucao::vizinhoGrasp(const Grade& grade)
{
    auto currGrade = std::make_unique<Grade>(grade);
    currGrade->fo = -1;

    const auto& adicionadas = currGrade->disciplinasAdicionadas;
    Disciplina* discremovida {nullptr};

    if (!adicionadas.empty()) {
        auto rand = aleatorio::randomInt() % adicionadas.size();
        discremovida = currGrade->remove(adicionadas[rand]);
    }

    auto restantes = currGrade->aluno->restanteOrd;

    while (!restantes.empty()) {
        auto current = getRandomDisc(restantes);
        auto discJaAdicionada = std::find(
            begin(adicionadas), end(adicionadas), current) != end(adicionadas);

        if (current != discremovida && !discJaAdicionada) {
            auto ok = currGrade->insert(current);
            if (ok) {
                break;
            }
        }

        restantes.erase(std::remove(begin(restantes), end(restantes), current),
                        end(restantes));

        /*if (current == discremovida || discJaAdicionada) {
            restantes.erase(std::remove(begin(restantes), end(restantes), current),
                            end(restantes));
            continue;
        }

        if (currGrade->insert(current)) {
            break;
        }

        restantes.erase(std::remove(begin(restantes), end(restantes), current),
                        end(restantes));*/
    }

    return currGrade;
}

void Resolucao::buscaLocal(std::unique_ptr<Grade>& grade)
{
    for (auto i = 0; i < gradeGraspVizinhos; i++) {
        auto vizinho = vizinhoGrasp(*grade);
        if (vizinho->getFO() > grade->getFO()) {
            grade = std::move(vizinho);
        }
    }
}

Grade* Resolucao::GRASP(AlunoPerfil* alunoPerfil, Solucao* sol)
{
    auto best = gradeAleatoria(alunoPerfil, sol);
    auto ultimaMelhoriaIter = -1;

    for (auto i = 0; i - ultimaMelhoriaIter < maxIterSemEvoGrasp; i++) {
        auto candidata = gradeAleatoria(alunoPerfil, sol);
        buscaLocal(candidata);

        if (candidata->getFO() > best->getFO()) {
            ultimaMelhoriaIter = i;
            best = std::move(candidata);
        }
    }

    return best.release();
}

std::unique_ptr<Solucao> Resolucao::gerarHorarioSA_ILS(long long timeout)
{
    SA sa {*this, 0.97, 1, 10'000, 5, timeout, {
            {Vizinhanca::ES, 25},
            {Vizinhanca::EM, 43},
            {Vizinhanca::RS, 20},
            {Vizinhanca::RM, 10},
            {Vizinhanca::KM, 2}
            }};

    ILS ils {*this, 10'000, 10, 1, 10, timeout};

    return gerarHorarioSA_ILS(sa, ils);
}

std::vector<ProfessorDisciplina*>
Resolucao::getSubTour(const Solucao& pai, int xbegin, int xend) const
{
    std::vector<ProfessorDisciplina*> subtour(begin(pai.horario->matriz) + xbegin,
                                              begin(pai.horario->matriz) + xend);
    return subtour;
}

Solucao* Resolucao::crossoverOrdemCamada(
    const Solucao& pai1,
    const Solucao& pai2,
    int camadaCruz)
{
    // Gera o ponto de cruzamento e encontra as coordenadas dele na matriz 3D
    int xbegin, xend;
    std::tie(xbegin, xend) = getCrossoverPoints(pai1, camadaCruz);
    int bloco, dia, camada;
    std::tie(bloco, dia, camada) = pai1.horario->getCoords(xbegin);

    // Gera o vector de genes entre os pontos de cruzamento
    auto genes = getSubTour(pai1, xbegin, xend);
    auto filho = std::make_unique<Solucao>(pai2);

    // A camada em que o cruzamento será efetuado é limpa
    filho->horario->clearCamada(camada);

    // Percorre os genes do subtour, inserindo-os na camada. Se não for possível,
    // o cruzamento não possui filho
    auto sucesso = insereSubTour(genes, *filho, xbegin);
    if (!sucesso) {
        return nullptr;
    }

    // Encontra a posição linear do começo da camada
    auto comecoCamada = filho->horario->getPosition(0, 0, camada);
    // E calcula o número de horários na semana
    auto tamCamada = dias_semana_util * blocosTamanho;

    // O pai2 será percorrido logo após o xend, resetando no final da camada
    // de volta ao início dela, até chegar no xbegin, inserindo as disciplinas
    // que já não estão no intervalo
    auto pai2idx = xend;
    for (auto i = xend; i < tamCamada + xend; i++) {
        // Encontra um índice válido (warpando quando chega ao final da camada)
        auto pai1idx = Util::warpIntervalo(i, tamCamada, comecoCamada);
        auto it = begin(genes);

        // Procura a primeira disciplina do pai2 que não está nos genes do cruzamento
        while ((it = procura_gene(genes, pai2.horario->matriz[pai2idx]))
            != end(genes)) {
            // Apaga dos genes para não haver outra colisão, e passa para a próxima
            // posição no pai2
            genes.erase(it);
            pai2idx = Util::warpIntervalo(pai2idx + 1, tamCamada, comecoCamada);
        }

        // Adquire as coordenadas da disciplina no pai1
        std::tie(bloco, dia, std::ignore) = filho->horario->getCoords(pai1idx);
        auto currPd = pai2.horario->matriz[pai2idx];

        // Tenta inserir a disciplina do pai2 nas coordenadas do pai1
        // Se falhar, o cruzamento não gera filhos
        if (currPd && !filho->horario->insert(dia, bloco, camada, currPd)) {
            return nullptr;
        }
    }

    gerarGrade(filho.get());
    return filho.release();
}

Solucao* Resolucao::crossoverOrdem(const Solucao& pai1, const Solucao& pai2)
{
    //puts("\n\nordem");
    //printf("pais: %g %g\n", pai1.fo, pai2.fo);

    std::vector<bool> camadas_visitadas(camadasTamanho);
    auto num_camadas_visitadas = 0;

    while (num_camadas_visitadas < camadasTamanho) {
        const auto camada = [&] {
            int n {};
            do {
                n = Util::randomBetween(0, camadasTamanho);
            } while (camadas_visitadas[n]);
            return n;
        }();

        // int camada {};
        // do {
        //     camada = Util::randomBetween(0, camadasTamanho);
        // } while (camadas_visitadas[camada]);

        num_camadas_visitadas++;
        camadas_visitadas[camada] = true;

        auto filho = crossoverOrdemCamada(pai1, pai2, camada);
        if (filho) {
            //printf("filho: %g\n\n", filho->fo);
            return filho;
        }
    }

    //puts("Nenhum filho");
    return nullptr;
}

std::pair<int, int> Resolucao::getCrossoverPoints(
    const Solucao& pai,
    int camada
) const
{
    auto dia1 = Util::randomBetween(0, dias_semana_util);
    auto dia2 = Util::randomBetween(0, dias_semana_util);

    auto bloco1 = Util::randomBetween(0, blocosTamanho);
    auto bloco2 = Util::randomBetween(0, blocosTamanho);

    auto x1 = pai.horario->getPosition(dia1, bloco1, camada);
    auto x2 = pai.horario->getPosition(dia2, bloco2, camada);

    auto xbegin = std::min(x1, x2);
    auto xend = std::max(x1, x2);

    return {xbegin, xend};
}

bool Resolucao::insereSubTour(
    const std::vector<ProfessorDisciplina*>& genes,
    Solucao& filho,
    int xbegin
)
{
    for (auto i = 0u; i < genes.size(); i++) {
        int bloco, dia, camada;
        std::tie(bloco, dia, camada) = filho.horario->getCoords(i + xbegin);
        auto currPd = genes[i];

        if (currPd && !filho.horario->insert(dia, bloco, camada, currPd)) {
            return false;
        }
    }
    return true;
}

Solucao* Resolucao::crossoverPMX(const Solucao& pai1, const Solucao& pai2)
{
    //puts("\n\nPMX");
    //printf("pais: %g %g\n", pai1.fo, pai2.fo);

    std::vector<bool> camadas_visitadas(camadasTamanho);
    auto num_camadas_visitadas = 0;

    while (num_camadas_visitadas < camadasTamanho) {
        const auto camada = [&] {
            int n {};
            do {
                n = Util::randomBetween(0, camadasTamanho);
            } while (camadas_visitadas[n]);
            return n;
        }();

        // int camada;
        // do {
        //     camada = Util::randomBetween(0, camadasTamanho);
        // } while (camadas_visitadas[camada]);

        num_camadas_visitadas++;
        camadas_visitadas[camada] = true;

        auto filho = crossoverPMXCamada(pai1, pai2, camada);
        if (filho) {
            //printf("filho: %g\n", filho->getFO());
            return filho;
        }
    }

    //printf("Nenhum filho\n");
    return nullptr;
}

Solucao* Resolucao::crossoverPMXCamada(
    const Solucao& pai1, 
    const Solucao& pai2, 
    int camadaCruz
)
{
    const auto camada_tam = dias_semana_util * blocosTamanho;
    std::vector<bool> preenchidos(camada_tam);

    // Gera o ponto de cruzamento e encontra as coordenadas dele na matriz 3D
    int xbegin, xend;
    std::tie(xbegin, xend) = getCrossoverPoints(pai1, camadaCruz);
    int bloco, dia, camada;
    std::tie(bloco, dia, camada) = pai1.horario->getCoords(xbegin);

    auto comeco_camada = pai1.horario->getPosition(0, 0, camada);

    // Gera o vector de genes entre os pontos de cruzamento
    auto genes = getSubTour(pai1, xbegin, xend);
    auto filho = std::make_unique<Solucao>(pai2);
    filho->horario->clearCamada(camada);

    // Percorre os genes do subtour, inserindo-os na camada. Se não for possível,
    // o cruzamento não gera filho
    auto sucesso = insereSubTour(genes, *filho, xbegin);
    if (!sucesso) {
        return nullptr;
    }

    for (auto i = xbegin; i < xend; i++) {
        preenchidos[i - comeco_camada] = true;
    }

    // Insere as disciplinas de pai2 que estavam no ponto de cruzamento
    // mas que não foram inseridas de pai1
    auto comeco_camada_it = begin(pai2.horario->matriz) + comeco_camada;
    auto fim_camada_it = comeco_camada_it + camada_tam;

    for (auto i = xbegin; i < xend; i++) {
        auto currPd = pai2.horario->at(i);

        // Verifica se já foi copiado (estava nos genes transmitidos)
        // Se sim, ele é removido dos genes (para caso haja repetição de genes
        // ele não entrar nessa condicional mais de uma vez), e passa para a
        // próxima iteração
        auto it = begin(genes);
        if ((it = procura_gene(genes, currPd)) != end(genes)) {
            genes.erase(it);
            continue;
        }

        auto idxPai2 = i;
        // Enquanto idxPai2 estiver no ponto de crossover
        while (xbegin <= idxPai2 && idxPai2 < xbegin) {
            // Localiza o valor dessa posição no pai 1
            auto pdPai1 = pai1.horario->matriz[idxPai2];
            // Localiza o mesmo valor no pai 2
            auto pdPai2It = std::find_if(comeco_camada_it, fim_camada_it,
                                         [&pdPai1](ProfessorDisciplina* pd) {
                                             return pd_equals(pdPai1, pd);
                                         });
            // Se o índice ainda faz parte do ponto de crossover, executa
            // para o novo valor.
            idxPai2 = std::distance(begin(pai2.horario->matriz), pdPai2It);
        }
        std::tie(bloco, dia, camada) = filho->horario->getCoords(idxPai2);

        if (currPd && !filho->horario->insert(dia, bloco, camada, currPd)) {
            return nullptr;
        }
        preenchidos[idxPai2 - comeco_camada] = true;
    }

    // Preenche as lacunas que restaram no horário
    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            auto indice_horario = filho->horario->getPosition(d, b, camada);
            auto indice_camada = indice_horario - comeco_camada;

            if (preenchidos[indice_camada]) {
                continue;
            }

            auto pdPai2 = pai2.horario->matriz[indice_horario];
            if (pdPai2 && !filho->horario->insert(d, b, camada, pdPai2)) {
                return nullptr;
            }
            preenchidos[indice_camada] = true;
        }
    }

    gerarGrade(filho.get());
    return filho.release();
}

Solucao* Resolucao::crossoverCicloCamada(
    const Solucao& pai1,
    const Solucao& pai2, 
    int camadaCruz
)
{
    auto tam_camada = dias_semana_util * blocosTamanho;
    std::vector<int> pos_visitados(tam_camada);
    auto num_visitados = 0;
    auto num_ciclos = 0;
    auto comeco_camada = pai1.horario->getPosition(0, 0, camadaCruz);

    // Enquanto houverem posições não visitadas
    while (num_visitados < tam_camada) {
        // Escolhe aleatoriamente um ponto em pai1
        int ponto;
        do {
            ponto = Util::randomBetween(0, tam_camada);
        } while (pos_visitados[ponto]);

        // Marca como vistado no novo ciclo
        pos_visitados[ponto] = ++num_ciclos;
        num_visitados++;
        // Encontra o valor referenciado
        auto valor_inicial = pai1.horario->matriz[comeco_camada + ponto];
        // Enncontra o respectivo no outro pai
        auto valpai2 = pai2.horario->matriz[comeco_camada + ponto];

        // Se forem iguais, nem começa a procurar um ciclo
        if (pd_equals(valor_inicial, valpai2)) {
            break;
        }

        do {
            // Valor respectivo no outro pai
            valpai2 = pai2.horario->at(comeco_camada + ponto);
            auto found = false;
            // Encontra tal valor no pai1
            for (auto i = 0; i < tam_camada; i++) {
                if (pos_visitados[i]) {
                    continue;
                }

                auto curr_pd = pai1.horario->matriz[i + comeco_camada];
                if (pd_equals(curr_pd, valpai2)) {
                    ponto = i;
                    found = true;
                    break;
                }
            }

            if (!found) {
                return nullptr;
            }

            // Marca o novo ponto para esse ciclo
            pos_visitados[ponto] = num_ciclos;
            num_visitados++;

            // Continua rodando até achar no pai2 um igual ao valor inicial do pai1
        } while (pai2.horario->matriz[comeco_camada + ponto] != valor_inicial);
    }

    // Gera máscara inicial
    std::vector<int> mask(num_ciclos);
    for (auto& x : mask) {
        x = Util::randomBetween(0, 2);
    }

    // Gera máscara do crossover
    std::vector<int> xmask(tam_camada);
    for (auto i = 0u; i < xmask.size(); i++) {
        auto ciclo = pos_visitados[i] - 1;
        xmask[i] = mask[ciclo];
    }

    auto filho = std::make_unique<Solucao>(pai1);
    filho->horario->clearCamada(camadaCruz);

    // Efetua cruzamento
    for (auto i = 0; i < tam_camada; i++) {
        auto curr_slot = comeco_camada + i;
        ProfessorDisciplina* pd {nullptr};

        int dia, bloco, camada;
        std::tie(dia, bloco, camada) = filho->horario->getCoords(curr_slot);

        switch (xmask[i]) {
            case 0:
                pd = pai1.horario->at(curr_slot);
                break;
            case 1:
                pd = pai2.horario->at(curr_slot);
                break;
        }

        if (pd && !filho->horario->insert(dia, bloco, camada, pd)) {
            return nullptr;
        }
    }

    gerarGrade(filho.get());
    return filho.release();
}

Solucao* Resolucao::crossoverCiclo(const Solucao& pai1, const Solucao& pai2)
{
    std::vector<bool> camadas_visitadas(camadasTamanho);
    auto num_camadas_visitadas = 0;

    while (num_camadas_visitadas < camadasTamanho) {
        const auto camada = [&] {
            int n {};
            do {
                n = Util::randomBetween(0, camadasTamanho);
            } while (camadas_visitadas[n]);
            return n;
        }();

        num_camadas_visitadas++;
        camadas_visitadas[camada] = true;

        auto filho = crossoverCicloCamada(pai1, pai2, camada);
        if (filho) {
            return filho;
        }
    }

    return nullptr;
}

Disciplina*
Resolucao::getRandomDisc(const std::vector<Disciplina*>& restantes)
{
    auto distancia = getIntervaloAlfaGrasp(restantes);
    auto rand = aleatorio::randomInt() % distancia;

    return restantes[rand];
}

Solucao* Resolucao::selecaoTorneio(const std::vector<Solucao*>& pop) const
{
    auto best = Util::randomChoice(pop);

    for (auto i = 1; i < horarioTorneioPopulacao; i++) {
        auto challenger = Util::randomChoice(pop);
        if (challenger->getFO() > best->getFO()) {
            best = challenger;
        }
    }

    return best;
}

std::unique_ptr<Solucao> Resolucao::event_swap(const Solucao& sol) const
{
    for (auto i = 0; i < horarioMutacaoTentativas; i++) {
        auto viz = std::make_unique<Solucao>(sol);

        auto camada = Util::randomBetween(0, camadasTamanho);

        auto diaX1 = Util::randomBetween(0, dias_semana_util);
        auto blocoX1 = 2 * Util::randomBetween(0, blocosTamanho / 2);
        auto e1 = viz->horario->getPosition(diaX1, blocoX1, camada);

        auto diaX2 = Util::randomBetween(0, dias_semana_util);
        auto blocoX2 = 2 * Util::randomBetween(0, blocosTamanho / 2);
        auto e2 = viz->horario->getPosition(diaX2, blocoX2, camada);

        if (swapBlocos(*viz, e1, e2) && swapBlocos(*viz, e1 + 1, e2 + 1)) {
            return viz;
        }
    }

    return std::make_unique<Solucao>(sol);
}

std::unique_ptr<Solucao> Resolucao::event_move(const Solucao& sol) const
{
    for (auto i = 0; i < horarioMutacaoTentativas; i++) {
        auto viz = std::make_unique<Solucao>(sol);

        auto camada = Util::randomBetween(0, camadasTamanho);
        auto dia = Util::randomBetween(0, dias_semana_util);
        auto bloco = 2 * Util::randomBetween(0, blocosTamanho / 2);

        auto e1 = viz->horario->at(dia, bloco, camada);
        auto e2 = viz->horario->at(dia, bloco + 1, camada);

        if (!e1 && !e2) {
            continue;
        }

        viz->horario->clearSlot(dia, bloco, camada);
        viz->horario->clearSlot(dia, bloco + 1, camada);

        for (auto d = 0; d < dias_semana_util; d++) {
            for (auto b = 0; b < blocosTamanho / 2; b += 2) {
                if (d == dia && b == bloco) {
                    continue;
                }

                if (viz->horario->at(d, b, camada)
                    || viz->horario->at(d, b + 1, camada)) {
                    continue;
                }

                auto ok_e1 = viz->horario->insert(d, b, camada, e1);
                auto ok_e2 = viz->horario->insert(d, b + 1, camada, e2);

                if (ok_e1 && ok_e2) {
                    return viz;
                }
            }
        }
    }

    return std::make_unique<Solucao>(sol);
}

std::unique_ptr<Solucao> Resolucao::resource_move(const Solucao& sol) const
{
    for (auto i = 0; i < horarioMutacaoTentativas; i++) {
        auto viz = std::make_unique<Solucao>(sol);

        // Determina aleatoriamente qual disciplina terá seu professor substituído
        int aloc_pos{};
        ProfessorDisciplina* aloc{nullptr};
        std::tie(aloc_pos, aloc) = get_random_notnull_aloc(*viz);

        std::unordered_set<Professor*> professores_capacitados(
            begin(aloc->disciplina->professoresCapacitados),
            end(aloc->disciplina->professoresCapacitados));

        if (professores_capacitados.size() == 1) {
            continue;
        }

        // Encontra um professor substituto, que não seja o original
        professores_capacitados.erase(aloc->professor);
        auto novo_prof = Util::randomChoice(professores_capacitados);

        // Remove todas as ocorrências da alocãção, guardando suas posições
        int camada{};
        tie(std::ignore, std::ignore, camada) = sol.horario->getCoords(aloc_pos);
        auto posicoes_aloc = remove_aloc_memorizando(*viz, aloc, camada);

        // Modifica a alocação para o novo professor
        aloc->professor = novo_prof;
        // Reinsere com a nova alocação
        auto ok = reinsere_alocacoes(*viz, posicoes_aloc, aloc, camada);
        if (ok) {
            return viz;
        }
    }

    return std::make_unique<Solucao>(sol);
}

std::unique_ptr<Solucao> Resolucao::resource_swap(const Solucao& sol) const
{
    for (auto i = 0; i < horarioMutacaoTentativas; i++) {
        auto viz = std::make_unique<Solucao>(sol);

        // Determina os eventos e1 e e2 que terão seus professores trocados
        // (e1 e e2 precisam ser diferentes)
        int pos_e1{};
        ProfessorDisciplina* e1{nullptr};
        std::tie(pos_e1, e1) = get_random_notnull_aloc(sol);

        int pos_e2{};
        ProfessorDisciplina* e2{nullptr};
        std::tie(pos_e2, e2) = [&] {
            int pos{};
            ProfessorDisciplina* e{nullptr};
            do {
                std::tie(pos, e) = get_random_notnull_aloc(sol);
            } while (e == e1);
            return std::make_pair(pos, e);
        }();

        // Estabelece se o professor de e1 pode lecionar e2 e vice-versa
        if (!is_professor_habilitado(*e1->getDisciplina(), e2->getProfessor())
            || !is_professor_habilitado(*e2->getDisciplina(), e1->getProfessor())) {
            continue;
        }

        // Remove e1 e e2 da matriz
        int camada_e1{};
        tie(std::ignore, std::ignore, camada_e1) = sol.horario->getCoords(pos_e1);
        auto posicoes_e1 = remove_aloc_memorizando(*viz, e1, camada_e1);

        int camada_e2{};
        tie(std::ignore, std::ignore, camada_e2) = sol.horario->getCoords(pos_e2);
        auto posicoes_e2 = remove_aloc_memorizando(*viz, e2, camada_e2);

        // Troca os professores
        std::swap(e1->professor, e2->professor);

        // Tenta reinserir os eventos
        auto ok_e1 = reinsere_alocacoes(*viz, posicoes_e1, e1, camada_e1);
        if (!ok_e1) {
            continue;
        }
        auto ok_e2 = reinsere_alocacoes(*viz, posicoes_e2, e2, camada_e2);
        if (ok_e2) {
            return viz;
        }
    }

    return std::make_unique<Solucao>(sol);
}

std::unique_ptr<Solucao> Resolucao::permute_resources(const Solucao& sol) const
{
    return {};
}

std::unique_ptr<Solucao> Resolucao::kempe_move(const Solucao& sol) const
{
    return {};
}

std::vector<std::pair<int, int>> Resolucao::remove_aloc_memorizando(
    Solucao& sol,
    ProfessorDisciplina* aloc,
    int camada
) const
{
    std::vector<std::pair<int, int>> posicoes_aloc{};

    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            if (sol.horario->at(d, b, camada) == aloc) {
                posicoes_aloc.emplace_back(d, b);
                sol.horario->clearSlot(d, b, camada);
            }
        }
    }

    return posicoes_aloc;
}

bool Resolucao::reinsere_alocacoes(
    Solucao& sol, 
    const std::vector<std::pair<int, int>>& posicoes_aloc, 
    ProfessorDisciplina* aloc, 
    int camada
) const
{
    for (const auto& p : posicoes_aloc) {
        auto ok = sol.horario->insert(p.first, p.second, camada, aloc);
        if (!ok) {
            return false;
        }
    }

    return true;
}

std::pair<int, ProfessorDisciplina*> Resolucao::get_random_notnull_aloc(
    const Solucao& sol
) const
{
    ProfessorDisciplina* aloc{nullptr};
    int pos{};
    while (!aloc) {
        pos = Util::randomBetween(0, sol.horario->matriz.size());
        aloc = sol.horario->at(pos);
    }
    return {pos, aloc};
}

bool Resolucao::is_professor_habilitado(
    const Disciplina& disc, 
    Professor* prof
) const
{
    const auto& capacitados = disc.professoresCapacitados;
    return std::find(begin(capacitados), end(capacitados), prof) != end(capacitados);
}
