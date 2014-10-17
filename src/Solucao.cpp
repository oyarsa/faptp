#include "Solucao.h"

Solucao::Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
  blocosTamanho = pBlocosTamanho;
  camadasTamanho = pCamadasTamanho;
  perfisTamanho = pPerfisTamanho;

  init();
}

void Solucao::init() {
  horario = new Horario(blocosTamanho, camadasTamanho);
  grades.resize(perfisTamanho);

  gradesLength = 0;
}

Solucao::~Solucao() {
}

void Solucao::insertGrade(Grade* grade) {
  insertGrade(grade, gradesLength);
  gradesLength++;
}

void Solucao::insertGrade(Grade* grade, int position) {
  grades[position] = grade;
}

double Solucao::getObjectiveFunction() {
  double fo = 0;

  for (int i = 0; i < grades.size(); i++) {
    fo += grades[i]->getObjectiveFunction();
  }

  return fo;
}