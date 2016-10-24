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

    bool colisaoProfessorAlocado(int pDia, int pBloco, std::string professorId) const;
    bool isViable(int dia, int bloco, int camada, ProfessorDisciplina* pd) const;

    bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) override;
    bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force) override;
    void clearSlot(int pDia, int pBloco, int pCamada) override;
    void clearCamada(int camada);

    std::size_t getHash();
private:
    std::unordered_map<std::string, int> discCamada;
    std::unordered_map<std::string, int> creditos_alocados_;
    std::size_t hash_;

    int contaJanelasDia(int dia, int camada) const;
    int contaJanelasCamada(int camada) const;
    int contaJanelas() const;

    bool isProfDia(const std::string& professor, int dia) const;
    bool intervalosTrabalhoProf(const std::string& professor) const;
    int intervalosTrabalho(const std::unordered_map<std::string, Professor*>& professores) const;

    bool isAulaDia(int dia, int camada) const;
    int numDiasAulaCamada(int camada) const;
    int numDiasAula() const;

    int aulasSabadoCamada(int camada) const;
    int aulasSabado() const;

    int aulasSeguidasDiscDia(const std::string& disciplina, int dia) const;
    int aulasSeguidasDisc(const std::string& disciplina) const;
    int aulasSeguidas(const std::vector<Disciplina*>& disciplinas) const;

    bool isDiscDificil(const std::string& disciplina) const;

    int aulasSeguidasDificilDia(int dia, int camada) const;
    int aulasSeguidasDificilCamada(int camada) const;
    int aulasSeguidasDificil() const;

    bool aulaDificilUltimoHorarioDia(int dia, int camada) const;
    int aulaDificilUltimoHorarioCamada(int camada) const;
    int aulaDificilUltimoHorario() const;

    bool isProfPref(const std::string& prof, const std::string& disc) const;
    int preferenciasProfessor(const std::string& professor) const;
    int preferenciasProfessores(
        const std::unordered_map<std::string, Professor*>& professores) const;

    int aulasProfessor(const std::string& professor, int preferencia) const;
    int aulasProfessores(
        const std::unordered_map<std::string, Professor*>& professores) const;
};

#endif /* HORARIO_H */

