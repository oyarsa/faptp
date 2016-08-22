#ifndef DISCIPLINA_H
#define    DISCIPLINA_H

#include <string>
#include <vector>
#include <unordered_set>

#include "Professor.h"

class Disciplina
{
    friend class Representacao;
    friend class Grade;
    friend class Horario;
    friend class Solucao;
    friend class Resolucao;
    friend class Output;

public:
    Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo,
               std::string pCurso, std::string pTurma, int pCapacidade,
               std::string pPeriodoMinimo);

    Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo,
               std::string pCurso, std::string pId, std::string pTurma,
               int pCapacidade, std::string pPeriodoMinimo);

    const std::string& getId() const;

    int periodoNum() const;

    int periodoMinimoNum() const;

    const std::string& getNome() const;
    void setNome(const std::string& pNome);

    const std::string& getPeriodo();
    void setPeriodo(const std::string& pPeriodo);

    const std::string& getCurso();
    void setCurso(const std::string& pCurso);

    int getCargaHoraria() const;
    void setCargaHoraria(int pCargaHoraria);

    int getAulasSemana();
    int getCreditos();

    void addPreRequisito(const std::string& pDisciplina);
    bool isPreRequisito(const std::string& pDisciplina);

    void addProfessorCapacitado(Professor* professor);

private:
    std::string id;
    std::string turma;
    std::string nome;
    std::string periodo;
    std::string periodoMinimo;
    std::string curso;
    int cargaHoraria;
    int aulasSemana;
    int capacidade;
    int alocados;
    bool ofertada;
    std::unordered_set<std::string> preRequisitos;
    std::unordered_set<std::string> coRequisitos;
    std::unordered_set<std::string> equivalentes;
    std::vector<Professor*> professoresCapacitados;
};

#endif /* DISCIPLINA_H */

