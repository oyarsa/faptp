#include <algorithm>

#include "Disciplina.h"

Disciplina::Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo) {
    UUID uuid;
    std::string pId;

    pId = uuid.GenerateUuid();

    init(pNome, pCargaHoraria, pPeriodo, pCurso, pId, pTurma, pCapacidade, pPeriodoMinimo);
}

Disciplina::Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo) {
    init(pNome, pCargaHoraria, pPeriodo, pCurso, pId, pTurma, pCapacidade, pPeriodoMinimo);
}

void Disciplina::init(std::string pNome, int pCargaHoraria, std::string pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade, std::string pPeriodoMinimo) {
    id = pId;
    cargaHoraria = pCargaHoraria;
    periodo = pPeriodo;
    periodoMinimo = pPeriodoMinimo;
    curso = pCurso;
    turma = pTurma;
    capacidade = pCapacidade;

    //aulasSemana = std::ceil(((cargaHoraria / SEMANA_MES / MES_SEMESTRE) * HORA_MINUTO) / MINUTO_ALUA);
    aulasSemana = cargaHoraria;

    alocados = 0;
    ofertada = true;
    setNome(pNome);
}

Disciplina::~Disciplina() {
}

std::string Disciplina::getId() const {
    return id;
}

std::string Disciplina::getNome() {
    return nome;
}

void Disciplina::setNome(std::string pNome) {
    nome = pNome;
}

std::string Disciplina::getPeriodo() {
    return periodo;
}

void Disciplina::setPeriodo(std::string pPeriodo) {
    periodo = pPeriodo;
}

std::string Disciplina::getCurso() {
    return curso;
}

void Disciplina::setCurso(std::string pCurso) {
    curso = pCurso;
}

int Disciplina::getCargaHoraria() const {
    return cargaHoraria;
}

void Disciplina::setCargaHoraria(int pCargaHoraria) {
    cargaHoraria = pCargaHoraria;
}

int Disciplina::getAulasSemana() {
    return aulasSemana;
}

int Disciplina::getCreditos() {
    return getAulasSemana();
}

void Disciplina::addPreRequisito(std::string pDisciplina) {
    preRequisitos.push_back(pDisciplina);
}

bool Disciplina::isPreRequisito(std::string pDisciplina) {
    return find(preRequisitos.begin(), preRequisitos.end(), pDisciplina) != preRequisitos.end();
}

void Disciplina::addProfessorCapacitado(Professor *professor) {
    professoresCapacitados.insert(
            std::lower_bound(
                begin(professoresCapacitados),
                end(professoresCapacitados), 
                professor, 
                [](Professor *a, Professor *b) {
                    return a->getNumDisponibilidade() < b->getNumDisponibilidade();
                })
            , professor
            );
}