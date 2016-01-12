#ifndef REPRESENTACAO_H
#define REPRESENTACAO_H

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "../template/Algorithms.h"
#include "Semana.h"

#include "ProfessorDisciplina.h"
#include "Util.h"

class Representacao {
    friend class Resolucao;
public:
    Representacao(int pBlocosTamanho, int pCamadasTamanho);
    Representacao(const Representacao& outro);
    Representacao& operator=(const Representacao& outro);
    virtual ~Representacao();

    int getPosition(int pDia, int pBloco, int pCamada);

    ProfessorDisciplina* at(int pDia, int pBloco, int pCamada);
    ProfessorDisciplina* at(int position);

    virtual bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina);
    virtual bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force);

    void get3DMatrix(int pLinear, int* triDimensional);

    std::vector<ProfessorDisciplina*>::iterator getFirstDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, Disciplina* pDisciplina);

    int getFirstDisciplina(Disciplina* pDisciplina);
    int getFirstDisciplina(Disciplina* pDisciplina, int camada);
    int getFirstDisciplina(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> pMatriz);

    int getPositionDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, std::vector<ProfessorDisciplina*>::iterator iterFound);

    virtual double getObjectiveFunction() = 0;
protected:
    int blocosTamanho;
    int camadasTamanho;

    int size;

    std::vector<std::string> blocos;
    std::vector<std::string> camadas;

    std::vector<ProfessorDisciplina*> matriz;
    std::vector<std::string> alocados;

    void initMatriz();
};

#endif /* REPRESENTACAO_H */

