#include <iostream>
#include <tsl/robin_map.h>

#include <faptp-lib/Grade.h>
#include <faptp-lib/Resolucao.h>
#include <faptp-lib/Constantes.h>

Grade::Grade(int pBlocosTamanho, AlunoPerfil* pAlunoPerfil, Horario* pHorario,
             const std::vector<Disciplina*>& pDisciplinasCurso)
    : Representacao(pBlocosTamanho, 1)
      , aluno(pAlunoPerfil)
      , horario(pHorario)
      , disciplinasAdicionadas()
      , disciplinasCurso(pDisciplinasCurso)
      , fo(-1)
{
    disciplinasAdicionadas.reserve(size / 2);
}

Grade::Grade(const Grade& outro)
    : Representacao(outro)
      , aluno(outro.aluno)
      , horario(outro.horario)
      , disciplinasAdicionadas(outro.disciplinasAdicionadas)
      , disciplinasCurso(outro.disciplinasCurso)
      , fo(outro.fo) {}

Disciplina* Grade::getDisciplina(std::size_t disciplina)
{
    return disciplinasCurso[disciplina];
}

Grade::~Grade()
{}

bool Grade::hasPeriodoMinimo(const Disciplina* const pDisciplina) const
{
    return aluno->getPeriodoNum() >= pDisciplina->getPeriodoMinimoNum();
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

  return !aluno->isRestante(pDisciplina->id_hash());
}

bool
Grade::hasCoRequisitos(const Disciplina* const pDisciplina)
{
  const auto& corequisitos = pDisciplina->coRequisitos;
  const auto& cursadas = aluno->cursadas;

  for (const auto coreq : corequisitos) {
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
        return a == b->id_hash();
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

  // Timeslots ocupados pela disciplina
  const auto& slots = horario->getTimeSlotsDisciplina(pDisciplina);

  for (const auto& slot : slots) {
    // Se tem algum PD não nulo no slot, existe uma colisão
    if (at(slot.dia, slot.bloco)) {
      return false;
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

bool Grade::insertOld(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar)
{
    return insertOld(pDisciplina, professorDisciplinasIgnorar, false);
}

bool Grade::insertOld(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>&, bool force)
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

            get3DMatrix(x, triDimensional);
            camada = horario->disc_camada_[pDisciplina->id_hash()];

            viavel = isViable(pDisciplina, camada);
            if (viavel) {
                add(pDisciplina, camada);
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
    // Limpa os slots ocupados pela disciplina
    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            const auto pos = getPosition(d, b, 0);
            const auto currPd = matriz[pos];
            if (currPd && currPd->disciplina == pDisciplina) {
                matriz[pos] = nullptr;
            }
        }
    }
    // Remove-a da lista de adicionadas.
    disciplinasAdicionadas.erase(
        std::remove(begin(disciplinasAdicionadas), end(disciplinasAdicionadas), pDisciplina),
                    end(disciplinasAdicionadas));
    return pDisciplina;
}

Disciplina* Grade::removeOld(Disciplina* pDisciplina, ProfessorDisciplina*& pProfessorDisciplina)
{
    std::vector<Disciplina*>::iterator found;
    Disciplina* rDisciplina = NULL;

    int i = 0;
    int x = getFirstDisciplina(pDisciplina, matriz);

    pProfessorDisciplina = NULL;

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

ProfessorDisciplina*
Grade::at(int dia, int bloco)
{
  return Representacao::at(dia, bloco, 0);
}

