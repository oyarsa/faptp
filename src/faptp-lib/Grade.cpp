#include <iostream>
#include <hash_map.h>

#include <faptp-lib/Grade.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Semana.h>

Grade::Grade(int pBlocosTamanho, AlunoPerfil* pAlunoPerfil, Horario* pHorario,
             const std::vector<Disciplina*>& pDisciplinasCurso,
             const hash_map<std::string, int>& pDiscToIndex)
    : Representacao(pBlocosTamanho, 1)
      , aluno(pAlunoPerfil)
      , horario(pHorario)
      , professorDisciplinas()
      , problemas()
      , professorDisciplinaTemp(nullptr)
      , disciplinasAdicionadas()
      , disciplinasCurso(pDisciplinasCurso)
      , discToIndex(pDiscToIndex)
      , fo(-1)
{
    disciplinasAdicionadas.reserve(dias_semana_util * blocosTamanho);
}

Grade::Grade(const Grade& outro)
    : Representacao(outro)
      , aluno(outro.aluno)
      , horario(outro.horario)
      , professorDisciplinas(outro.professorDisciplinas)
      , problemas(outro.problemas)
      , professorDisciplinaTemp(nullptr)
      , disciplinasAdicionadas(outro.disciplinasAdicionadas)
      , disciplinasCurso(outro.disciplinasCurso)
      , discToIndex(outro.discToIndex)
      , fo(outro.fo) {}

Disciplina* Grade::getDisciplina(const std::string& pNomeDisciplina)
{
    return disciplinasCurso[discToIndex.at(pNomeDisciplina)];
}

Grade::~Grade()
{
    professorDisciplinaTemp = nullptr;
}

bool Grade::hasPeriodoMinimo(const Disciplina* const pDisciplina) const
{
    return aluno->getPeriodoNum() >= pDisciplina->periodoMinimoNum();
}

bool
Grade::discRepetida(const Disciplina* pDisciplina)
{
  const auto found = std::find_if(
    begin(disciplinasAdicionadas), end(disciplinasAdicionadas),
    [pDisciplina](auto d) {
      return d == pDisciplina;
    });

  if (found != end(disciplinasAdicionadas)) {
    return true;
  }

  return !aluno->isRestante(pDisciplina->getId());
}

bool
Grade::hasCoRequisitos(const Disciplina* const pDisciplina)
{
  const auto& corequisitos = pDisciplina->coRequisitos;
  const auto& cursadas = aluno->cursadas;

  for (const auto& coreq : corequisitos) {
    const auto& equivalentes = getDisciplina(coreq)->equivalentes;

    const auto cursou = std::find_first_of(
      begin(equivalentes), end(equivalentes),
      begin(cursadas), end(cursadas)) != end(equivalentes);

    if (cursou) {
      continue;
    }

    const auto cursando = std::find_first_of(
      begin(equivalentes), end(equivalentes),
      begin(disciplinasAdicionadas), end(disciplinasAdicionadas),
      [](auto a, auto b) {
        return a == b->nome;
      }
    ) != end(equivalentes);

    if (!cursando) {
      return false;
    }
  }

  return true;
}

bool
Grade::havePreRequisitos(const Disciplina* const pDisciplina)
{
  const auto& pre_requisitos = pDisciplina->preRequisitos;
  const auto& aprovadas = aluno->aprovadas;

  // Percorre a lista de disciplinas que sao pre-requisitos da atual
  for (const auto& prereq : pre_requisitos) {
    const auto& equivalentes = getDisciplina(prereq)->equivalentes;
    // Se não foi encontrado, retorna falso
    const auto passou = std::find_first_of(
      begin(equivalentes), end(equivalentes),
      begin(aprovadas), end(aprovadas)) != end(equivalentes);
    if (!passou) {
      return false;
    }
  }

  return true;
}

bool Grade::checkCollision(const Disciplina* pDisciplina, int pCamada)
{
    // Percorre a grade do aluno inteira procurando slots da disciplina atual
    // e verificando se já estão ocupados por alguma outra
    for (auto i = 0; i < dias_semana_util; i++) {
        for (auto j = 0; j < blocosTamanho; j++) {
            const auto currPosHorario = getPosition(i, j, pCamada);
            const auto currPosGrade = getPosition(i, j, 0);
            const auto currPd = horario->matriz[currPosHorario];

            if (!currPd || currPd->disciplina != pDisciplina)
                continue;

            // Se o slot não for nulo é porque já tem alguém lá. Logo,
            // há uma colisão
            if (matriz[currPosGrade]) {
                return false;
            }

        }
    }

    return true;
}

bool Grade::isViable(const Disciplina* pDisciplina, int pCamada)
{
    return pDisciplina->ofertada &&
           !discRepetida(pDisciplina) &&
           havePreRequisitos(pDisciplina) &&
           checkCollision(pDisciplina, pCamada) &&
           hasPeriodoMinimo(pDisciplina);
}

void Grade::add(Disciplina* pDisciplina, int pCamada)
{
    for (int i = 0; i < dias_semana_util; i++) {
        for (int j = 0; j < blocosTamanho; j++) {
            auto currPosHorario = getPosition(i, j, pCamada);
            auto currPosGrade = getPosition(i, j, 0);

            auto currPd = horario->matriz[currPosHorario];

            if (currPd && currPd->disciplina == pDisciplina) {
                matriz[currPosGrade] = currPd;
            }

        }
    }

    disciplinasAdicionadas.push_back(pDisciplina);
}

bool Grade::insert2(Disciplina* pDisciplina)
{
    return insert(pDisciplina, {}, false);
}

bool Grade::insert(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar)
{
    return insert(pDisciplina, professorDisciplinasIgnorar, false);
}

bool Grade::insert(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>&, bool force)
{
    int camada {};
    int triDimensional[3] {};
    int x {};

    auto pdIter = horario->matriz.begin();
    auto pdIterEnd = horario->matriz.end();
    auto pdIterFound = horario->matriz.begin();

    bool viavel = false;
    bool first = true;

    if (find(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina) == disciplinasAdicionadas.end()) {
        while (((pdIterFound = getFirstDisciplina(pdIterFound, pdIterEnd, pDisciplina)) != pdIterEnd) && (first || !viavel || force)) {
            x = getPositionDisciplina(pdIter, pdIterEnd, pdIterFound);

            professorDisciplinaTemp = nullptr;

            get3DMatrix(x, triDimensional);
            camada = horario->disc_camada_[pDisciplina->id_hash()];

            viavel = isViable(pDisciplina, camada);
            if (viavel) {
                add(pDisciplina, camada);
            }
            if (viavel || force) {
                if (professorDisciplinaTemp != nullptr) {
                    professorDisciplinas.push_back(professorDisciplinaTemp);
                }
            }
            if (!viavel && force) {
                if (professorDisciplinaTemp != nullptr) {
                    problemas.push_back(professorDisciplinaTemp->disciplina->id);
                }
            }

            first = false;
            ++pdIterFound;
        }
    }

    return viavel;
}

bool Grade::insert(Disciplina* pDisciplina)
{
    const auto camada = horario->disc_camada_[pDisciplina->id_hash()];
    const auto viavel = isViable(pDisciplina, camada);
    if (!viavel) {
        return false;
    }

    add(pDisciplina, camada);
    return true;
}

Disciplina* Grade::remove(Disciplina* pDisciplina)
{
    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            const auto pos = getPosition(d, b, 0);
            const auto currPd = matriz[pos];
            if (currPd && currPd->disciplina == pDisciplina) {
                matriz[pos] = nullptr;
            }
        }
    }
    disciplinasAdicionadas.erase(std::remove(
                                     begin(disciplinasAdicionadas), end(disciplinasAdicionadas), pDisciplina),
                                 end(disciplinasAdicionadas));
    return pDisciplina;
}

Disciplina* Grade::remove2(Disciplina* pDisciplina, ProfessorDisciplina* & pProfessorDisciplina)
{
    std::vector<Disciplina*>::iterator found;
    Disciplina* rDisciplina = NULL;

    int i = 0;
    int x = getFirstDisciplina(pDisciplina, matriz);

    pProfessorDisciplina = NULL;

    problemas.erase(std::remove(problemas.begin(), problemas.end(), pDisciplina->id), problemas.end());

    while (x >= 0) {

        if (pProfessorDisciplina == NULL) {
            pProfessorDisciplina = matriz[x];
        }

        matriz[x] = NULL;
        x = getFirstDisciplina(pDisciplina, matriz);

        i++;
    }
    if (i > 0) {
        found = std::remove(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina);
        if (found != disciplinasAdicionadas.end()) {
            rDisciplina = *(found);
        }
        disciplinasAdicionadas.erase(found, disciplinasAdicionadas.end());
    }

    return rDisciplina;
}

double Grade::getFO2()
{
    if (fo != -1) {
        return fo;
    }
    fo = 0;

    hash_map<std::string, int> discAvaliada;
    const auto turmaAluno = aluno->turma;
    const auto periodoAluno = aluno->periodo;

    if (problemas.size() > 0) {
        fo = -1;
        return fo;
    }

    for (auto i = 0; i < size; i++) {
        auto professorDisciplina = at(i);

        if (professorDisciplina) {
            fo++;

            // Se a turma do aluno for a mesma da disciplina e o periodo do aluno
            // for o mesmo da disciplina, ela cumpre uma prefer�ncia do aluno
            const auto& nomeDisc = professorDisciplina->disciplina->nome;
            const auto& turmaDisc = professorDisciplina->disciplina->turma;
            const auto& periodoDisc = professorDisciplina->disciplina->periodo;

            if (turmaAluno == turmaDisc && periodoAluno == periodoDisc
                && !discAvaliada[nomeDisc]) {
                discAvaliada[nomeDisc] = 1;
                fo += 0.1;
            }
        }
    }

    return fo;
}

double Grade::getFO()
{
    if (fo != -1)
        return fo;
    fo = 0;

    for (const auto& disc : disciplinasAdicionadas) {
        fo += disc->cargaHoraria;
        if (disc->periodo == aluno->periodo && disc->turma == aluno->turma)
            fo += 0.1;
    }

    return fo;
}

