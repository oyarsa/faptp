#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <unordered_map>

#include "Horario.h"
#include "Grade.h"

class Solucao
{
    friend class Resolucao;
    friend class Output;
public:
    Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho);
    Solucao(const Solucao& outro);
    Solucao& operator=(const Solucao& outro);
    virtual ~Solucao();

    void insertGrade(Grade* grade);

    double getFO();

    std::size_t getHash();
    std::unordered_map<int, std::string> camada_periodo;
private:
    Horario* horario;
    std::unordered_map<std::string, Grade*> grades;

    int id;
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    int gradesLength;
    double fo = -1;

    void init();
};

#endif /* SOLUCAO_H */

