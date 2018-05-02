#ifndef DISCIPLINA_H
#define    DISCIPLINA_H

#include <string>
#include <vector>
#include <atomic>
#include <tsl/hopscotch_set.h>

#include "Professor.h"

class Disciplina
{
    friend class Representacao;
    friend class Grade;
    friend class Horario;
    friend class Solucao;
    friend class Resolucao;
    friend class Output;

public:
    Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo,
               std::string pCurso, std::string pTurma, int pCapacidade,
               std::string pPeriodoMinimo);

    Disciplina(std::string pNome, int pCargaHoraria, std::string pPeriodo,
               std::string pCurso, std::string pId, std::string pTurma,
               int pCapacidade, std::string pPeriodoMinimo);

    const std::string& getId() const;

    int periodoNum() const;

    int calcPeriodoMinimoNum() const;

    const std::string& getNome() const;
    void setNome(const std::string& pNome);

    const std::string& getPeriodo();
    void setPeriodo(const std::string& pPeriodo);

    const std::string& getCurso();
    void setCurso(const std::string& pCurso);

    int getCargaHoraria() const;
    void setCargaHoraria(int pCargaHoraria);

    int getAulasSemana();
    int getCreditos();

    void addPreRequisito(std::size_t disciplina);
    bool isPreRequisito(std::size_t disciplina) const;

    void addProfessorCapacitado(Professor* professor);

    bool isDificil() const;

    std::size_t id_hash() const;

    static std::size_t max_hash();

    int getPeriodoMinimoNum() const;

private:
    static std::atomic_size_t Next_code;

    std::string id;
    std::string turma;
    std::string nome;
    std::string periodo;
    std::string periodoMinimo;
    std::string curso;
    int cargaHoraria;
    int aulasSemana;
    int capacidade;
    int alocados;
    bool ofertada;
    std::vector<std::size_t> preRequisitos;
    std::vector<std::size_t> coRequisitos;
    std::vector<std::size_t> equivalentes;
    std::vector<Professor*> professoresCapacitados;
    bool dificil;
    std::size_t idHash;
    int periodoMinimoNum;

    void finalizarConstrucao();
};

inline std::size_t Disciplina::id_hash() const
{
  return idHash;
}

inline const std::string& Disciplina::getId() const
{
  return id;
}

inline const std::string& Disciplina::getNome() const
{
  return nome;
}

inline void Disciplina::setNome(const std::string& pNome)
{
  nome = pNome;
}

inline const std::string& Disciplina::getPeriodo()
{
  return periodo;
}

inline void Disciplina::setPeriodo(const std::string& pPeriodo)
{
  periodo = pPeriodo;
}

inline const std::string& Disciplina::getCurso()
{
  return curso;
}

inline void Disciplina::setCurso(const std::string& pCurso)
{
  curso = pCurso;
}

inline int Disciplina::getCargaHoraria() const
{
  return cargaHoraria;
}

inline void Disciplina::setCargaHoraria(int pCargaHoraria)
{
  cargaHoraria = pCargaHoraria;
}

inline int Disciplina::getAulasSemana()
{
  return aulasSemana;
}

inline int Disciplina::getCreditos()
{
  return getAulasSemana();
}

inline void Disciplina::addPreRequisito(std::size_t disciplina)
{
  preRequisitos.push_back(disciplina);
}

inline bool Disciplina::isPreRequisito(std::size_t disciplina) const
{
  return std::find(preRequisitos.begin(), preRequisitos.end(), disciplina)
      != preRequisitos.end();
}

inline std::size_t
Disciplina::max_hash()
{
  return Next_code + 1;
}

inline int
Disciplina::getPeriodoMinimoNum() const
{
  return periodoMinimoNum;
}

#endif /* DISCIPLINA_H */

