#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class Resolucao;
class Disciplina;
class Professor;

class Modelo
{
public:
  explicit Modelo(const Resolucao& res);
  
private:
  void carregar_pesos(const std::unordered_map<std::string, double>& pesos);

  void carregar_habilitacoes(
    const std::vector<Disciplina*>& disciplinas, 
    const std::unordered_map<std::string, Professor*>& professores);

  void carregar_periodos(const std::vector<Disciplina*>& disciplinas);

  static std::unordered_map<std::string, std::size_t> 
  atribuir_camadas( const std::vector<Disciplina*>& disciplinas);

  void carregar_disponibilidade(
    const std::unordered_map<std::string, Professor*>& professores);
  
  void carregar_dificeis(const std::vector<Disciplina*>& disciplinas);

  void carregar_preferencias(
    const std::vector<Disciplina*>& disciplinas,
    const std::unordered_map<std::string, Professor*>& professores);

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
