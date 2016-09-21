#include <iostream>
#include <sstream>

#include "parametros.h"
#include "Horario.h"
#include "Semana.h"

Horario::Horario(int pBlocosTamanho, int pCamadasTamanho) : Representacao(pBlocosTamanho, pCamadasTamanho), hash_(0) {}

Horario::~Horario() {}

Horario::Horario(const Horario& outro)
    : Representacao(outro), discCamada(outro.discCamada),
      creditos_alocados_(outro.creditos_alocados_), hash_(0) {}

Horario& Horario::operator=(const Horario& outro)
{
    Representacao::operator=(outro);
    creditos_alocados_ = outro.creditos_alocados_;
    hash_ = 0;
    discCamada = outro.discCamada;
    return *this;
}

bool Horario::colisaoProfessorAlocado(int pDia, int pBloco, std::string professorId) const
{
    int positionCamada {};

    for (int i = 0; i < camadasTamanho; i++) {

        positionCamada = getPosition(pDia, pBloco, i);
        if (matriz[positionCamada] 
            && matriz[positionCamada]->professor->getId() == professorId) {

            if (verbose) {
                std::cout << "\nColisao[" << professorId << " || "
                        << matriz[positionCamada]->professor->id << "](" << positionCamada
                        << "): " << i << " " << pDia << " " << pBloco
                        << std::endl;
            }
            return true;
        }
    }

    return false;
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina)
{
    if (!pProfessorDisciplina) {
        return true;
    }
    return insert(pDia, pBloco, pCamada, pProfessorDisciplina, false);
}


bool Horario::isViable(int dia, int bloco, int camada, ProfessorDisciplina* pd) const
{
    if (!pd) {
        return true;
    }

    auto pos = getPosition(dia, bloco, camada);
    auto disc = pd->disciplina;

    auto creditos = creditos_alocados_.find(disc->id);
    if (creditos != end(creditos_alocados_) && creditos->second >= disc->cargaHoraria) {
        return false;
    }

    if (!matriz[pos]) {
        auto isProfAloc = colisaoProfessorAlocado(dia, bloco, pd->professor->getId());
        if (!isProfAloc) {
            return true;
        }
    }

    return false;
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force)
{
    int position = getPosition(pDia, pBloco, pCamada);
    bool professorAlocado = false;
    auto disc = pProfessorDisciplina->disciplina;

    if (verbose)
        std::cout << "(" << position << ")" << std::endl;

    if (creditos_alocados_[disc->id] >= disc->cargaHoraria) {
        return false;
    }

    if (!matriz[position] || force) {
        professorAlocado = colisaoProfessorAlocado(pDia, pBloco,
                                                   pProfessorDisciplina->professor->getId());
        if (!professorAlocado || force) {
            creditos_alocados_[disc->id]++;
            return Representacao::insert(pDia, pBloco, pCamada, pProfessorDisciplina, force);
        }
    }
    return false;
}

void Horario::clearSlot(int pDia, int pBloco, int pCamada)
{
    auto pos = getPosition(pDia, pBloco, pCamada);
    auto profdisc = matriz[pos];
    if (!profdisc) {
        return;
    }
    creditos_alocados_[profdisc->disciplina->id]--;
    Representacao::clearSlot(pDia, pBloco, pCamada);
}

void Horario::clearCamada(int camada)
{
    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            clearSlot(d, b, camada);
        }
    }
}

std::size_t Horario::getHash()
{
    if (hash_ != 0) {
        return hash_;
    }
    auto oss = std::ostringstream {};

    for (const auto& pd : matriz) {
        if (pd) {
            oss << pd->disciplina->id + pd->professor->id;
        } else {
            oss << "00";
        }
    }
    hash_ = Util::hash_string(oss.str());
    return hash_;
}

