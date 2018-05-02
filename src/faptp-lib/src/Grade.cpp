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

Disciplina* Grade::getDisciplina(std::size_t disciplina)
{
    return disciplinasCurso[disciplina];
}

bool Grade::hasPeriodoMinimo(const Disciplina* const pDisciplina) const
{
    return aluno->getPeriodoNum() >= pDisciplina->getPeriodoMinimoNum();
}

bool
Grade::discRepetida(const Disciplina* pDisciplina)
{
  // Se não for restante, o aluno já cursou. Se já cursou, é repetida.
  const auto cursouDisciplina = !aluno->isRestante(pDisciplina->id_hash());
  if (cursouDisciplina) {
    return true;
  }

  // Procura nas adicionadas nessa grade
  const auto found = std::find(disciplinasAdicionadas.begin(),
                               disciplinasAdicionadas.end(), pDisciplina);

  // Se não foi encontrada, não é repetida.
  return found != end(disciplinasAdicionadas);
}

bool
Grade::hasCoRequisitos(const Disciplina* const pDisciplina)
{
  const auto& corequisitos = pDisciplina->coRequisitos;
  const auto& cursadas = aluno->cursadas;

  for (const auto coreq : corequisitos) {
    const auto& equivalentes = getDisciplina(coreq)->equivalentes;

    const auto cursou = Util::have_common_element(equivalentes, cursadas);

    if (cursou) {
      continue;
    }

    const auto cursando = Util::have_common_element(
        equivalentes, disciplinasAdicionadas,
        [](auto a, auto b) {
          return static_cast<int>(a) - static_cast<int>(b->id_hash());
        });

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
    const auto passou = Util::have_common_element(equivalentes, aprovadas);
    if (!passou) {
      return false;
    }
  }

  return true;
}

bool
Grade::checkCollision(const Disciplina* pDisciplina)
{
  // Percorre a grade do aluno inteira procurando slots da disciplina atual
  // e verificando se já estão ocupados por alguma outra

  // Timeslots ocupados pela disciplina
  const auto& slots = horario->getTimeSlotsDisciplina(pDisciplina);

  for (const auto& slot : slots) {
    // Se tem algum PD não nulo no slot, existe uma colisão
    if (at(slot.dia, slot.bloco) != nullptr) {
      return false;
    }
  }

  return true;
}

bool
Grade::isViable(const Disciplina* pDisciplina)
{
    return pDisciplina->ofertada &&
           !discRepetida(pDisciplina) &&
           havePreRequisitos(pDisciplina) &&
           checkCollision(pDisciplina) &&
           hasPeriodoMinimo(pDisciplina);
}

void
Grade::add(Disciplina* pDisciplina)
{
    const auto& slots = horario->getTimeSlotsDisciplina(pDisciplina);

    for (const auto& slot : slots) {
        const auto gradePos = getPosition(slot.dia, slot.bloco);
        matriz[gradePos] =  horario->at(slot.dia, slot.bloco, slot.camada);
    }

    disciplinasAdicionadas.push_back(pDisciplina);
}

bool Grade::insertOld(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar)
{
    return insertOld(pDisciplina, professorDisciplinasIgnorar, false);
}

bool Grade::insertOld(Disciplina* pDisciplina, const std::vector<ProfessorDisciplina*>&, bool force)
{
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

            viavel = isViable(pDisciplina);
            if (viavel) {
              add(pDisciplina);
            }

            first = false;
            ++pdIterFound;
        }
    }

    return viavel;
}

bool Grade::insert(Disciplina* disciplina)
{
    if (!isViable(disciplina)) {
        return false;
    }

    add(disciplina);
    return true;
}

Disciplina* Grade::remove(Disciplina* disciplina)
{
    // Limpa os slots ocupados pela disciplina
    const auto& slots = horario->getTimeSlotsDisciplina(disciplina);
    for (const auto& slot : slots) {
      const auto pos = getPosition(slot.dia, slot.bloco);
      matriz[pos] = nullptr;
    }

    // Remove-a da lista de adicionadas.
    disciplinasAdicionadas.erase(
        std::remove(begin(disciplinasAdicionadas), end(disciplinasAdicionadas), disciplina),
                    end(disciplinasAdicionadas));
    return disciplina;
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

int Grade::getPosition(int dia, int bloco)
{
  return Representacao::getPosition(dia, bloco, 0);
}

