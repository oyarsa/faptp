#ifndef PROFESSORDISCIPLINA_H
#define    PROFESSORDISCIPLINA_H

#include <string>

#include <faptp-lib/Professor.h>
#include <faptp-lib/Disciplina.h>

class ProfessorDisciplina
{
    friend class Representacao;
    friend class Grade;
    friend class Horario;
    friend class Resolucao;
public:
    ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina);
    ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina,
                        const std::string& pId);

    Disciplina* getDisciplina() const;
    Professor* getProfessor() const;
private:
    std::string id;

    Professor* professor;
    Disciplina* disciplina;
};

#endif /* PROFESSORDISCIPLINA_H */

