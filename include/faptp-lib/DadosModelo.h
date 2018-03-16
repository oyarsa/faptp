#pragma once

#include <string>
#include <tsl/robin_map.h>
#include <vector>

class Resolucao;
class Disciplina;
class Professor;

class DadosModelo
{
public:
  explicit DadosModelo(const Resolucao& res);

  std::size_t num_dias() const;
  std::size_t num_horarios() const;
  std::size_t num_professores() const;
  std::size_t num_disciplinas() const;
  std::size_t num_camadas() const;
  const std::vector<std::string>& periodos() const;
  const std::vector<std::string>& disciplinas() const;
  const std::vector<std::string>& professores() const;
  const std::vector<double>& pesos_constraints() const;
  const std::vector<std::vector<char>>& habilitado() const;
  const std::vector<std::vector<char>>& pertence_periodo() const;
  const std::vector<std::vector<std::vector<char>>>& disponivel() const;
  const std::vector<char>& dificil() const;
  const std::vector<std::vector<char>>& preferencia() const;
  const std::vector<int>& aulas_desejadas() const;
  const std::vector<int>& horas_contrato() const;
  const std::vector<int>& carga_horaria() const;
  const std::vector<char>& oferecida() const;
  const std::vector<int>& aulas_geminadas() const;

private:
  void carregar_pesos(const tsl::robin_map<std::string, double>& pesos);

  void carregar_habilitacoes(
    const std::vector<Disciplina*>& disciplinas, 
    const tsl::robin_map<std::string, Professor*>& professores);

  void carregar_periodos(const std::vector<Disciplina*>& disciplinas);

  static tsl::robin_map<std::string, std::size_t> 
  atribuir_camadas( const std::vector<Disciplina*>& disciplinas);

  void carregar_disponibilidade(
    const tsl::robin_map<std::string, Professor*>& professores);
  
  void carregar_dificeis(const std::vector<Disciplina*>& disciplinas);

  void carregar_preferencias(
    const std::vector<Disciplina*>& disciplinas,
    const tsl::robin_map<std::string, Professor*>& professores);

  std::size_t num_dias_;
  std::size_t num_horarios_;
  std::size_t num_professores_;
  std::size_t num_disciplinas_;
  std::size_t num_camadas_;
  std::vector<std::string> periodos_;
  std::vector<std::string> disciplinas_;
  std::vector<std::string> professores_;
  std::vector<double> pesos_constraints_;
  std::vector<std::vector<char>> habilitado_;
  std::vector<std::vector<char>> pertence_periodo_;
  std::vector<std::vector<std::vector<char>>> disponivel_;
  std::vector<char> dificil_;
  std::vector<std::vector<char>> preferencia_;
  std::vector<int> aulas_desejadas_;
  std::vector<int> horas_contrato_;
  std::vector<int> carga_horaria_;
  std::vector<char> oferecida_;
  std::vector<int> aulas_geminadas_;

};
