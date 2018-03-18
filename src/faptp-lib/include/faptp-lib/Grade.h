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
          const std::vector<Disciplina*>& pDisciplinasCurso,
          const tsl::robin_map<std::string, int>& pDiscToIndex);
    Grade(const Grade& outro);
    Grade& operator=(const Grade&) = delete;
    virtual ~Grade();

    bool insert2(Disciplina* pDisciplina);
    bool insert(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar);
    bool insert(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar, bool force);
    bool insert(Disciplina* pDisciplina);
    Disciplina* remove2(Disciplina* pDisciplina, ProfessorDisciplina* & pProfessorDisciplina);
    Disciplina* remove(Disciplina* pDisciplina);

    double getFO();
    double getFO2();

private:
    AlunoPerfil* aluno;
    Horario* horario;

    std::vector<ProfessorDisciplina*> professorDisciplinas;
    std::vector<std::string> problemas;
    ProfessorDisciplina* professorDisciplinaTemp;
    std::vector<Disciplina*> disciplinasAdicionadas;

    const std::vector<Disciplina*>& disciplinasCurso;
    const tsl::robin_map<std::string, int>& discToIndex;

    //! Guarda a função objetiva dessa grade, vale 0 se ainda não foi definida
    double fo;

    //! Recebe um nome e retorna um ponteiro para uma disciplina
    Disciplina* getDisciplina(const std::string& pNomeDisc);

    bool havePreRequisitos(const Disciplina* const pDisciplina);
    bool hasPeriodoMinimo(const Disciplina* const pDisciplina) const;
    bool hasCoRequisitos(const Disciplina* const pDisciplina);
    bool checkCollision(const Disciplina* pDisciplina, int pCamada);
    bool isViable(const Disciplina* pDisciplina, int pCamada);
    //! Verifica se a disciplina sendo considerada não está na lista de equivalências
    //! de outra que já foi inserida
    bool discRepetida(const Disciplina* pDisciplina);
    void add(Disciplina* pDisciplina, int pCamada);
};

#endif /* GRADE_H */

