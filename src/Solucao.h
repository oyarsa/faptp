#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <string>
#include <map>

#include "Horario.h"
#include "Grade.h"

class Solucao {
    friend class Resolucao;
    friend class Output;
public:
    Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
    Solucao(const Solucao& outro);
    Solucao& operator=(const Solucao& outro);
    virtual ~Solucao();

    void insertGrade(Grade* grade);

    double getObjectiveFunction();

    bool operator<( Solucao *&s2);
private:
    Horario *horario;
    map<string, Grade*> grades;
    
    int id;
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    int gradesLength;

    void init();
};

#endif /* SOLUCAO_H */

