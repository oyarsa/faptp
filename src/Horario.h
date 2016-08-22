#ifndef HORARIO_H
#define HORARIO_H

#include <unordered_map>
#include "Representacao.h"

class Horario : public Representacao
{
    friend class Resolucao;
    friend class Grade;
public:
    Horario(int pBlocosTamanho, int pCamadasTamanho);
    Horario(const Horario& outro);
    Horario& operator=(const Horario& outro);
    virtual ~Horario();

    bool colisaoProfessorAlocado(int pDia, int pBloco, std::string professorId);

    bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) override;
    bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) override;
    void clearSlot(int pDia, int pBloco, int pCamada) override;
    void clearCamada(int camada);

    std::size_t getHash();
private:
    std::unordered_map<std::string, int> discCamada;
    std::unordered_map<std::string, int> creditos_alocados_;
    std::size_t hash_;
};

#endif /* HORARIO_H */

