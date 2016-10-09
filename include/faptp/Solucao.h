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
    virtual ~Solucao();

    std::unique_ptr<Solucao> clone() const;
    void insertGrade(Grade* grade);

    void calculaFO();
    int getFO();
    int getFO() const;

    const Horario& getHorario() const;
    Horario& getHorario();

    std::size_t getHash();
    std::unordered_map<int, std::string> camada_periodo;
    std::unordered_map<std::string, ProfessorDisciplina*> alocacoes;
private:
    int id;
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    std::unique_ptr<Horario> horario;
    std::unordered_map<std::string, Grade*> grades;

    int gradesLength;
    int fo = -1;
};

#endif /* SOLUCAO_H */

