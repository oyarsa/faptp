#ifndef RESOLUCAO_H
#define RESOLUCAO_H

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <vector>

#include <time.h>

#include "../template/Algorithms.h"

#include "Professor.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"
#include "AlunoPerfil.h"
#include "Solucao.h"
#include "Horario.h"

#include "Util.h"
#include "includes/json/json.h"

#include "Files.h"

#define RESOLUCAO_GERAR_GRADE_TIPO_GULOSO          1
#define RESOLUCAO_GERAR_GRADE_TIPO_GRASP           2
#define RESOLUCAO_GERAR_GRADE_TIPO_COMBINATORIO    3

#define RESOLUCAO_GRASP_TEMPO_CONSTRUCAO_FATOR_DEFAULT    .5
#define RESOLUCAO_GRASP_ITERACAO_VIZINHOS_DEDAULT         5

#define RESOLUCAO_GRASP_VIZINHOS_ALEATORIOS   1
#define RESOLUCAO_GRASP_VIZINHOS_CRESCENTE    2

class Resolucao {
public:
    Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
    virtual ~Resolucao();

    void start();
    void start(bool input);

    Solucao* gerarHorarioAG();

    double gerarGrade();
    
    void showResult();

    /*
     Parâmetros da execução da solução
     */
    // Horário população inicial
    int horarioPopulacaoInicial;
    int horarioProfessorColisaoMax;

    // Horário torneio
    double horarioTorneioPares;
    double horarioTorneioPopulacao;
    
    // Horário cruzamento
    int horarioCruzamentoFilhos;
    int horarioCruzamentoDias;
    double horarioCruzamentoCamadas;
    int horarioCruzamentoTentativasMax;
    
    // Horário mutação
    double horarioMutacaoProbabilidade;
    int horarioMutacaoTentativas;

    // Grade tipo de construção
    int gradeTipoConstrucao;
    double gradeAlfa;

    // Grade GRASP
    int gradeGraspVizinhanca;
    int gradeGraspVizinhos;
    double gradeGraspTempoConstrucao;
private:
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    std::map<std::string, Professor*> professores;

    std::map<std::string, int> disciplinasIndex;
    std::vector<Disciplina*> disciplinas;

    std::map< std::string, std::vector<Disciplina*> > periodoXdisciplina;

    std::map<std::string, AlunoPerfil*> alunoPerfis;

    std::map<std::string, ProfessorDisciplina*> professorDisciplinas;

    Solucao* solucao;

    Json::Value jsonRoot;

    void init(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
    void initDefault();

    void carregarDados();

    void carregarDadosProfessores();
    void carregarDadosDisciplinas();

    void carregarAlunoPerfis();

    void carregarDadosProfessorDisciplinas();

    void carregarSolucao();

    std::vector<Disciplina*> ordenarDisciplinas();
    std::vector<Disciplina*> ordenarDisciplinas(std::vector<Disciplina*> pDisciplina);
    void atualizarDisciplinasIndex();

    std::vector<Solucao*> gerarHorarioAGPopulacaoInicial();
    std::vector<Solucao*> gerarHorarioAGTorneioPar(std::vector<Solucao*> solucoesPopulacao);
    Solucao* gerarHorarioAGTorneio(std::vector<Solucao*> solucoesPopulacao);
    std::vector<Solucao*> gerarHorarioAGCruzamentoAleatorio(Solucao *solucaoPai1, Solucao *solucaoPai2);
    bool gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);
    bool gerarHorarioAGCruzamentoAleatorioReparo(Solucao *&solucaoFilho, int diaG, int blocoG, int camadaG);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao);
    void gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*> &populacao, int populacaoMax);
    std::vector<Solucao*> gerarHorarioAGMutacao(std::vector<Solucao*> filhos);
    Solucao* gerarHorarioAGMutacao(Solucao* pSolucao);

    double gerarGradeTipoGuloso();

    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep, int deep, int current);
    Grade* gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, std::vector<Disciplina*> disciplinasRestantes, int maxDeep);
    double gerarGradeTipoCombinacaoConstrutiva();

    Solucao* gerarGradeTipoGraspConstrucao(Solucao *pSolucao);

    Grade* gerarGradeTipoGraspConstrucao(Grade* pGrade);
    Grade* gerarGradeTipoGraspConstrucao(Grade* pGrade, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar);
    Solucao* gerarGradeTipoGraspRefinamentoAleatorio(Solucao *pSolucao);
    Solucao* gerarGradeTipoGraspRefinamentoCrescente(Solucao *pSolucao);
    double gerarGradeTipoGrasp();
    double gerarGradeTipoGrasp(Solucao *&pSolucao, bool printResult);
    std::vector<Disciplina*>::iterator getLimiteIntervaloGrasp(std::vector<Disciplina*> pApRestante);
    int getIntervaloAlfaGrasp(std::vector<Disciplina*> pApRestante);
    
    void showResult(Solucao *pSolucao);
};

#endif /* RESOLUCAO_H */