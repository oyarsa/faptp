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

    bool colisaoProfessorAlocado(int pDia, int pBloco, const Professor & professor) const;
    bool isViable(int dia, int bloco, int camada, ProfessorDisciplina* pd) const;

    bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) override;
    bool insert(int dia, int bloco, int camada, ProfessorDisciplina* pd, bool force) override;
    void clearSlot(int pDia, int pBloco, int pCamada) override;
    void clearCamada(int camada);
    std::unordered_map<std::string, ProfessorDisciplina*> getAlocFromDiscNames(int camada) const;

    std::size_t getHash();

    int contaJanelas() const;
    int intervalosTrabalho(const std::unordered_map<std::string, Professor*>& professores) const;
    int numDiasAula() const;
    int aulasSabado() const;
    int aulasSeguidas(const std::vector<Disciplina*>& disciplinas) const;
    int aulasSeguidasDificil() const;
    int aulaDificilUltimoHorario() const;
    int preferenciasProfessores(
        const std::unordered_map<std::string, Professor*>& professores) const;
    int aulasProfessores(
        const std::unordered_map<std::string, Professor*>& professores) const;
private:
    std::unordered_map<std::string, int> discCamada;
    std::unordered_map<std::string, int> creditos_alocados_disc_;
    std::unordered_map<std::string, int> creditos_alocados_prof_;
    std::size_t hash_;

    int contaJanelasDia(int dia, int camada) const;
    int contaJanelasCamada(int camada) const;

    bool isProfDia(const std::string& professor, int dia) const;
    int intervalosTrabalhoProf(const std::string& professor) const;

    bool isAulaDia(int dia, int camada) const;
    int numDiasAulaCamada(int camada) const;

    int aulasSabadoCamada(int camada) const;

    int aulasSeguidasDiscDia(const std::string& disciplina, int dia) const;
    int aulasSeguidasDisc(const std::string& disciplina) const;

    int aulasSeguidasDificilDia(int dia, int camada) const;
    int aulasSeguidasDificilCamada(int camada) const;

    bool aulaDificilUltimoHorarioDia(int dia, int camada) const;
    int aulaDificilUltimoHorarioCamada(int camada) const;

    int preferenciasProfessor(const Professor& professor) const;

    int aulasProfessor(const std::string& professor, int preferencia) const;
};

#endif /* HORARIO_H */

