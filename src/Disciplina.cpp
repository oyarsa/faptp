#include <cmath>

#include "Disciplina.h"

Disciplina::Disciplina(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pTurma, int pCapacidade) {
  UUID uuid;
  std::string pId;

  pId = uuid.GenerateUuid();

  init(pNome, pCargaHoraria, pPeriodo, pCurso, pId, pTurma, pCapacidade);
}

Disciplina::Disciplina(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade) {
  init(pNome, pCargaHoraria, pPeriodo, pCurso, pId, pTurma, pCapacidade);
}

void Disciplina::init(std::string pNome, int pCargaHoraria, int pPeriodo, std::string pCurso, std::string pId, std::string pTurma, int pCapacidade) {
  id = pId;
  cargaHoraria = pCargaHoraria;
  periodo = pPeriodo;
  curso = pCurso;
  turma = pTurma;
  capacidade = pCapacidade;

  aulasSemana = std::ceil(((cargaHoraria / SEMANA_MES / MES_SEMESTRE) * HORA_MINUTO) / MINUTO_ALUA);

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

int Disciplina::getPeriodo() {
  return periodo;
}

void Disciplina::setPeriodo(int pPeriodo) {
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

void Disciplina::addPreRequisito(std::string pDisciplina) {
  preRequisitos.push_back(pDisciplina);
}

bool Disciplina::isPreRequisito(std::string pDisciplina) {
  return std::find(preRequisitos.begin(), preRequisitos.end(), pDisciplina) != preRequisitos.end();
}
