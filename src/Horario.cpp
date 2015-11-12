#include "includes/parametros.h"
#include "Horario.h"

Horario::Horario(int pBlocosTamanho, int pCamadasTamanho) : Representacao(pBlocosTamanho, pCamadasTamanho) {
}

Horario::~Horario() {
}

bool Horario::colisaoProfessorAlocado(int pDia, int pBloco, int pCamada, std::string professorId) {
    int positionCamada;

    for (int i = pCamada; i < camadasTamanho; i++) {

        positionCamada = getPosition(pDia, pBloco, i);
        if (matriz[positionCamada] != NULL && matriz[positionCamada]->professor->getId() == professorId) {

            std::cout << "\nColisao[" << professorId << " || " 
                    << alocados[positionCamada] << "](" << positionCamada 
                    << "): " << i << " " << pDia << " " << pBloco 
                    << std::endl;
            return true;
        }
    }

    return false;
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) {
    return insert(pDia, pBloco, pCamada, pProfessorDisciplina, false);
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) {
    int position = getPosition(pDia, pBloco, pCamada);
    bool professorAlocado = false;

    /**
     * TODO: criar m�todo para verificar as restri��es da aloca��o de uma 
     * disciplina no hor�rio
     */

    if (verbose)
        std::cout << "(" << position << ")" << std::endl;

    if (alocados[position] == "" || force) {

        professorAlocado = colisaoProfessorAlocado(pDia, pBloco, pCamada, 
                pProfessorDisciplina->professor->getId());

        if (!professorAlocado || force) {
            return Representacao::insert(pDia, pBloco, pCamada, pProfessorDisciplina, force);
        }
    }

    return false;
}

double Horario::getObjectiveFunction() {
    return 0;
}