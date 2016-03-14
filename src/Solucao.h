#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <unordered_map>

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

	std::size_t getHash();
private:
    Horario *horario;
    std::map<std::string, Grade*> grades;
    
    int id;
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    int gradesLength;
	double fo = -1;

    void init();
};

#endif /* SOLUCAO_H */

