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

    Disciplina* getDisciplina() const;
    Professor* getProfessor() const;
private:
    Professor* professor;
    Disciplina* disciplina;
};


inline Disciplina* ProfessorDisciplina::getDisciplina() const
{
    return disciplina;
}

inline Professor* ProfessorDisciplina::getProfessor() const
{
    return professor;
}



#endif /* PROFESSORDISCIPLINA_H */

