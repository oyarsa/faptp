#ifndef HORARIO_H
#define HORARIO_H

#include <tsl/robin_map.h>
#include "Representacao.h"

struct TimeSlot
{
  int dia;
  int bloco;
  int camada;

  explicit TimeSlot(const std::tuple<int, int, int>& tuple);
  TimeSlot(int dia, int bloco, int camada);

  std::tuple<int, int, int> toTuple() const;
};

class Horario : public Representacao
{
    friend class Resolucao;
    friend class Grade;
public:
    Horario(int pBlocosTamanho, int pCamadasTamanho);
    Horario(const Horario& outro);
    Horario& operator=(const Horario& outro);
    virtual ~Horario() = default;

    bool colisaoProfessorAlocado(int pDia, int pBloco, const Professor & professor) const;
    bool isViable(int dia, int bloco, int camada, ProfessorDisciplina* pd) const;

    bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina) override;
    bool insert(int dia, int bloco, int camada, ProfessorDisciplina* pd, bool force) override;
    void clearSlot(int pDia, int pBloco, int pCamada) override;
    void clearCamada(int camada);
    tsl::robin_map<std::string, ProfessorDisciplina*> getAlocFromDiscNames(int camada) const;

    std::size_t getHash();

    int contaJanelas() const;
    int intervalosTrabalho() const;
    int numDiasAula() const;
    int aulasSabado() const;
    int aulasSeguidas() const;
    int aulasSeguidasDificil() const;
    int aulaDificilUltimoHorario() const;
    int preferenciasProfessores() const;
    int aulasProfessores(
        const tsl::robin_map<std::string, Professor*>& professores) const;

    const std::vector<TimeSlot>&
    getTimeSlotsDisciplina(const Disciplina* disc) const;
private:
    std::vector<int> disc_camada_;
    std::vector<int> creditos_alocados_disc_;
    std::vector<int> creditos_alocados_prof_;
    std::size_t hash_;
    std::vector<std::vector<TimeSlot>> timeslots_disciplinas_;

    int contaJanelasDia(int dia, int camada) const;
    int contaJanelasCamada(int camada) const;

    int intervalosTrabalhoProf(std::size_t professor,
                               const std::vector<std::vector<uint8_t>>& prof_dia) const;

    bool isAulaDia(int dia, int camada) const;
    int numDiasAulaCamada(int camada) const;

    int aulasSabadoCamada(int camada) const;

    int aulasSeguidasDificilDia(int dia, int camada) const;
    int aulasSeguidasDificilCamada(int camada) const;

    bool aulaDificilUltimoHorarioDia(int dia, int camada) const;
    int aulaDificilUltimoHorarioCamada(int camada) const;
};

#endif /* HORARIO_H */

