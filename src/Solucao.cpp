#include "Solucao.h"

Solucao::Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
  blocosTamanho = pBlocosTamanho;
  camadasTamanho = pCamadasTamanho;
  perfisTamanho = pPerfisTamanho;

  init();
}

void Solucao::init() {
  horario = new Horario(blocosTamanho, camadasTamanho);

  gradesLength = 0;
}

Solucao::~Solucao() {
}

void Solucao::insertGrade(Grade* grade) {
  grades[grade->alunoPerfil->id] = grade;
  gradesLength++;
}

double Solucao::getObjectiveFunction() {
  std::map<std::string, Grade*>::iterator gIter = grades.begin();
  std::map<std::string, Grade*>::iterator gIterEnd = grades.end();
  
  Grade *grade;

  double fo = 0;

  for (; gIter != gIterEnd; ++gIter) {
    grade = gIter->second;
    
    if (grade != NULL) {
      fo += grade->getObjectiveFunction();
    }
  }

  return fo;
}

Solucao* Solucao::clone() const {
  Solucao* s = new Solucao(*this);
  
  std::map<std::string, Grade*>::iterator gIter = s->grades.begin();
  std::map<std::string, Grade*>::iterator gIterEnd = s->grades.end();

  for (; gIter != gIterEnd; ++gIter) {
    s->grades[gIter->first] = gIter->second->clone();
  }
  
  return s;
}
