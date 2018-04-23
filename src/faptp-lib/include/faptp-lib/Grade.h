#ifndef GRADE_H
#define    GRADE_H

#include <string>
#include <vector>

#include "Representacao.h"
#include "Horario.h"
#include "AlunoPerfil.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"

class Grade : public Representacao
{
    friend class Solucao;
    friend class Resolucao;
    friend class Output;
public:
    Grade(int pBlocosTamanho, AlunoPerfil* pAlunoPerfil, Horario* pHorario,
          const std::vector<Disciplina*>& pDisciplinasCurso);
    Grade(const Grade& outro);
    Grade& operator=(const Grade&) = delete;
    virtual ~Grade();

    // Deprecated
    bool insertOld(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
    bool insertOld(
        Disciplina* pDisciplina,
        const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar,
        bool force
    );
    Disciplina* removeOld(Disciplina* pDisciplina, ProfessorDisciplina*& pProfessorDisciplina);
    // End deprecated

    bool insert(Disciplina* pDisciplina);
    Disciplina* remove(Disciplina* pDisciplina);

    double getFO();

    ProfessorDisciplina* at(int dia, int bloco);

private:
    AlunoPerfil* aluno;
    Horario* horario;
    std::vector<Disciplina*> disciplinasAdicionadas;
    const std::vector<Disciplina*>& disciplinasCurso;

    //! Guarda a fun��o objetiva dessa grade, vale 0 se ainda n�o foi definida
    double fo;

    //! Recebe um nome e retorna um ponteiro para uma disciplina
    Disciplina* getDisciplina(std::size_t disciplina);

    bool havePreRequisitos(const Disciplina* const pDisciplina);
    bool hasPeriodoMinimo(const Disciplina* const pDisciplina) const;
    bool hasCoRequisitos(const Disciplina* const pDisciplina);
    bool checkCollision(const Disciplina* pDisciplina);
    bool isViable(const Disciplina* pDisciplina);
    //! Verifica se a disciplina sendo considerada n�o est� na lista de equival�ncias
    //! de outra que j� foi inserida
    bool discRepetida(const Disciplina* pDisciplina);
    void add(Disciplina* pDisciplina);
};

#endif /* GRADE_H */

