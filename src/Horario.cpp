#include "includes/parametros.h"
#include "Horario.h"

Horario::Horario(int pBlocosTamanho, int pCamadasTamanho) : Representacao(pBlocosTamanho, pCamadasTamanho) {
}

Horario::~Horario() {
}

Horario::Horario(const Horario& outro) 
    : Representacao(outro) 
{}

/*
 int blocosTamanho;
    int camadasTamanho;

    int size;

    std::vector<std::string> blocos;
    std::vector<std::string> camadas;

    std::vector<ProfessorDisciplina*> matriz;
    std::vector<std::string> alocados;
 */

Horario* Horario::clone() {
    Horario* h = new Horario(blocosTamanho, camadasTamanho);
    
    h->blocos = blocos;
    h->camadas = camadas;
    h->matriz = matriz;
    h->alocados = alocados;
    
    return h;
}


bool Horario::colisaoProfessorAlocado(int pDia, int pBloco, std::string professorId) {
    int positionCamada;

    for (int i = 0; i < camadasTamanho; i++) {

        positionCamada = getPosition(pDia, pBloco, i);
        if (matriz[positionCamada] != NULL && matriz[positionCamada]->professor->getId() == professorId) {

            if (verbose) {
                std::cout << "\nColisao[" << professorId << " || "
                        << alocados[positionCamada] << "](" << positionCamada
                        << "): " << i << " " << pDia << " " << pBloco
                        << std::endl;
            }
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

    if (verbose)
        std::cout << "(" << position << ")" << std::endl;

    if (alocados[position] == "" || force) {
        professorAlocado = colisaoProfessorAlocado(pDia, pBloco,
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