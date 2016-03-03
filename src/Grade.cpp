#include <iostream>
#include <unordered_map>

#include "parametros.h"
#include "Grade.h"
#include "Resolucao.h"
#include "Semana.h"

Grade::Grade(int pBlocosTamanho, AlunoPerfil* pAlunoPerfil, Horario *pHorario,
        std::vector<Disciplina*>& pDisciplinasCurso, std::map<std::string, int>& pDiscToIndex)
: Representacao(pBlocosTamanho, 1)
, alunoPerfil(pAlunoPerfil)
, horario(pHorario)
, professorDisciplinas()
, problemas()
, professorDisciplinaTemp(nullptr)
, disciplinasAdicionadas()
, disciplinasCurso(pDisciplinasCurso)
, discToIndex(pDiscToIndex)
, fo(-1) {
}

Disciplina* Grade::getDisciplina(std::string pNomeDisciplina) {
    return disciplinasCurso[discToIndex[pNomeDisciplina]];
}

Grade::~Grade() {
    professorDisciplinaTemp = nullptr;

}

bool Grade::hasPeriodoMinimo(const Disciplina * const pDisciplina) const
{
    auto success = alunoPerfil->periodo >= pDisciplina->periodoMinimo;

	if (!success)
		std::cout << "---INVALIDO[" << pDisciplina->nome << "] PERIDOMIN\n";

	return success;
}

bool Grade::discRepetida(const Disciplina * const pDisciplina) {
    // Percorre as disciplinas adicionadas e verifica se o nome de pDisciplina
    // se encontra em suas listas de equival�ncias. Se sim, ela � uma disciplina
    // repetida e n�o pode ser inserida
    for (const auto& discAtual : disciplinasAdicionadas) {
        const auto& equivalentes = discAtual->equivalentes;
        if (find(begin(equivalentes), end(equivalentes), pDisciplina->nome)
                != end(equivalentes)) {
			if (verbose)
				std::cout << "---INVIAVEL[" << pDisciplina->nome << "] REPETIDA\n";
            return true;
        }
    }
    // Percorre as disciplinas cursadas do aluno e verifica se o nome de pDisciplina
    // � equivalente a alguma. Se sim, ela � uma disciplina repetida e n�o pode
    // ser inserida
    for (const auto& discAtual : alunoPerfil->cursadas) {
        const auto& equivalentes = getDisciplina(discAtual)->equivalentes;
        if (find(begin(equivalentes), end(equivalentes), pDisciplina->nome)
                != end(equivalentes)) {
			if (verbose)
				std::cout << "---INVIAVEL[" << pDisciplina->nome << "] EQUIV REPETE\n";
            return true;
        }
    }
    return false;
}

bool Grade::hasCoRequisitos(const Disciplina * const pDisciplina) {
    bool viavel = true;

    const auto& pDisciplinaCoRequisitos = pDisciplina->coRequisitos;

    if (pDisciplinaCoRequisitos.size() > 0) {
        const auto& disciplinasCursadas = alunoPerfil->cursadas;

        if (disciplinasCursadas.size() > 0) {
            for (const auto& coRequisito : pDisciplinaCoRequisitos) {
                const auto& equivalentes = getDisciplina(coRequisito)->equivalentes;
                auto possuiPreReq = (find_first_of(equivalentes.begin(), equivalentes.end(),
                        disciplinasCursadas.begin(), disciplinasCursadas.end()) != equivalentes.end())
                        || (std::find_first_of(equivalentes.begin(), equivalentes.end(),
                        disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(),
                        [](const std::string& a, const Disciplina * const b) {
                            return a == b->nome;
                        }) != equivalentes.end());

                if (!possuiPreReq) {
                    viavel = false;
                    break;
                }
            }
        }
    } else {
        viavel = false;
    }

    if (!viavel) {
        if (verbose)
            std::cout << "Nao tem os pre requisitos" << std::endl;
    }

    return viavel;
}

bool Grade::havePreRequisitos(const Disciplina * const pDisciplina) {
    bool viavel = true;

    const auto& pDisciplinaPreRequisitos = pDisciplina->preRequisitos;

    if (pDisciplinaPreRequisitos.size() > 0) {
        const auto& disciplinasAprovadas = alunoPerfil->aprovadas;

        if (disciplinasAprovadas.size() > 0) {
            // Percorre a lista de disciplinas que s�o pre-requisitos da atual
            // e procura alguma disciplina equivalente deste pre-requisito
            // na lista de cursadas do aluno
            for (const auto& preRequisito : pDisciplinaPreRequisitos) {
                const auto& equivalentes = getDisciplina(preRequisito)->equivalentes;
                auto possuiPreReq = find_first_of(equivalentes.begin(), equivalentes.end(),
                        disciplinasAprovadas.begin(), disciplinasAprovadas.end())
                        != equivalentes.end();

                if (!possuiPreReq) {
					if (verbose)
						std::cout << "---INVIAVEL[" << pDisciplina->nome << "] PREREQ [" << preRequisito << "]\n";
                    viavel = false;
                    break;
                }
            }

        } else {
            viavel = false;
        }
    }

    if (!viavel && verbose) {
        std::cout << "Nao tem os pre requisitos" << std::endl;
    }
    return viavel;
}

bool Grade::checkCollision(const Disciplina * const pDisciplina, const int pCamada, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar) {
    bool colisao = false;
    int currentPositionHorario;
    int currentPositionGrade;

    ProfessorDisciplina *currentProfessorDisciplina;

    for (int i = 0; i < SEMANA; i++) {
        for (int j = 0; j < blocosTamanho; j++) {
            currentPositionHorario = getPosition(i, j, pCamada);
            currentPositionGrade = getPosition(i, j, 0);

            currentProfessorDisciplina = horario->matriz[currentPositionHorario];

            if (currentProfessorDisciplina != NULL && currentProfessorDisciplina->disciplina == pDisciplina) {

                if (!professorDisciplinasIgnorar.empty()) {

                    // Se contem currentProfessorDisciplina
                    if (find(professorDisciplinasIgnorar.begin(), professorDisciplinasIgnorar.end(), currentProfessorDisciplina) != professorDisciplinasIgnorar.end()) {
                        colisao = true;
                    }
                }

                // Se ainda n�o marcou colis�o
                if (!colisao) {

                    professorDisciplinaTemp = currentProfessorDisciplina;

                    if (matriz[currentPositionGrade] != NULL) {
                        colisao = true;
                    }
                }

                if (colisao) {
					if (verbose)
						std::cout << "---INVIAVEL[" << pDisciplina->nome << "] COLISAO [" << i << "," << j << "]\n";
                    break;
                }
            }

        }
    }

    return (!colisao);
}

bool Grade::isViable(const Disciplina * const pDisciplina, const int pCamada, const std::vector<ProfessorDisciplina*>& professorDisciplinasIgnorar) {
	if (verbose && !pDisciplina->ofertada)
		std::cout << "---INVIAVEL[" << pDisciplina->nome << "] N OFERT\n";
    return havePreRequisitos(pDisciplina) &&
		checkCollision(pDisciplina, pCamada, professorDisciplinasIgnorar) &&
		!discRepetida(pDisciplina) &&
		hasPeriodoMinimo(pDisciplina) &&
		pDisciplina->ofertada;
}

void Grade::add(Disciplina* pDisciplina, int pCamada) {
    int currentPositionHorario;
    int currentPositionGrade;

    ProfessorDisciplina *currentProfessorDisciplina;

    for (int i = 0; i < SEMANA; i++) {
        for (int j = 0; j < blocosTamanho; j++) {
            currentPositionHorario = getPosition(i, j, pCamada);
            currentPositionGrade = getPosition(i, j, 0);

            currentProfessorDisciplina = horario->matriz[currentPositionHorario];

            if (currentProfessorDisciplina != NULL && currentProfessorDisciplina->disciplina == pDisciplina) {
                if (verbose)
                    std::cout << currentPositionHorario << ", ";

                matriz[currentPositionGrade] = currentProfessorDisciplina;
            }

        }
    }

    //std::lower_bound(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina, DisciplinaCargaHorariaDesc());
    disciplinasAdicionadas.push_back(pDisciplina);
}

bool Grade::insert(Disciplina* pDisciplina) {
    std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

    return insert(pDisciplina, professorDisciplinasIgnorar, false);
}

bool Grade::insert(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar) {
    return insert(pDisciplina, professorDisciplinasIgnorar, false);
}

bool Grade::insert(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar, bool force) {
    int camada, triDimensional[3];
    int x;

    std::vector<ProfessorDisciplina*>::iterator pdIter = horario->matriz.begin();
    std::vector<ProfessorDisciplina*>::iterator pdIterEnd = horario->matriz.end();
    std::vector<ProfessorDisciplina*>::iterator pdIterFound = horario->matriz.begin();

    bool viavel = false;
    bool first = true;

    if (find(disciplinasAdicionadas.begin(), disciplinasAdicionadas.end(), pDisciplina) == disciplinasAdicionadas.end()) {
        while (((pdIterFound = getFirstDisciplina(pdIterFound, pdIterEnd, pDisciplina)) != pdIterEnd) && (first || !viavel || force)) {
            x = getPositionDisciplina(pdIter, pdIterEnd, pdIterFound);

            professorDisciplinaTemp = NULL;

            get3DMatrix(x, triDimensional);
            camada = triDimensional[2];

            viavel = isViable(pDisciplina, camada, professorDisciplinasIgnorar);
            if (viavel) {
                if (verbose)
                    std::cout << "----ADD: " << pDisciplina->id << "[";
                add(pDisciplina, camada);
                if (verbose)
                    std::cout << "]" << std::endl;
            }
            if (viavel || force) {
                if (professorDisciplinaTemp != NULL) {
                    professorDisciplinas.push_back(professorDisciplinaTemp);
                }
            }
            if (!viavel && force) {
                if (professorDisciplinaTemp != NULL) {
                    problemas.push_back(professorDisciplinaTemp->disciplina->id);
                }
            }

            first = false;
	        ++pdIterFound;
        }
    }

    return viavel;
}

Disciplina* Grade::remove(Disciplina* pDisciplina) {
    ProfessorDisciplina *professorDisciplina;

    return remove(pDisciplina, professorDisciplina);
}

Disciplina* Grade::remove(Disciplina* pDisciplina, ProfessorDisciplina* &pProfessorDisciplina) {
    std::vector<Disciplina*>::iterator found;
    Disciplina* rDisciplina = NULL;

    int i = 0;
    int x = getFirstDisciplina(pDisciplina, matriz);

    pProfessorDisciplina = NULL;

    if (verbose)
        std::cout << "----REM: " << pDisciplina->id << std::endl;

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

double Grade::getObjectiveFunction() {
    ProfessorDisciplina *professorDisciplina;

	if (fo != -1) {
		return fo;
	}

	fo = 0;

    std::unordered_map<std::string, int> discAvaliada;

    int triDimensional[3];

    const auto turmaAluno = alunoPerfil->turma;
    const auto periodoAluno = alunoPerfil->periodo;

    if (problemas.size() > 0) {
        fo = -1;
    } else {
        for (int i = 0; i < size; i++) {
            professorDisciplina = at(i);

            get3DMatrix(i, triDimensional);

            if (professorDisciplina != NULL) {
                fo += ((1) * (alunoPerfil->peso));

                // Se a turma do aluno for a mesma da disciplina e o periodo do aluno
                // for o mesmo da disciplina, ela cumpre uma prefer�ncia do aluno
                const auto& nomeDisc = professorDisciplina->disciplina->nome;
                const auto& turmaDisc = professorDisciplina->disciplina->turma;
                const auto& periodoDisc = professorDisciplina->disciplina->periodo;

                if (turmaAluno == turmaDisc && periodoAluno == periodoDisc 
                    && !discAvaliada[nomeDisc]) {
                    discAvaliada[nomeDisc] = true;
                    fo += 0.1;
                }
            }
        }
    }

    return fo;
}

Grade::Grade(const Grade& outro)
: Representacao(outro)
, alunoPerfil(outro.alunoPerfil)
, horario(outro.horario)
, professorDisciplinas(outro.professorDisciplinas)
, problemas(outro.problemas)
, professorDisciplinaTemp(nullptr)
, disciplinasAdicionadas(outro.disciplinasAdicionadas)
, disciplinasCurso(outro.disciplinasCurso)
, discToIndex(outro.discToIndex)
, fo(outro.fo) {
}

Grade& Grade::operator=(const Grade& outro) {
    Representacao::operator=(outro);
    alunoPerfil = outro.alunoPerfil;
    horario = outro.horario;
    professorDisciplinas = outro.professorDisciplinas;
    problemas = outro.problemas;
    professorDisciplinaTemp = nullptr;
    disciplinasCurso = outro.disciplinasCurso;
    discToIndex = outro.discToIndex;
	fo = outro.fo;
    
    return *this;
}
