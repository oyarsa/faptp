#ifndef PROFESSOR_H
#define    PROFESSOR_H

#include <string>
#include <vector>
#include <map>

#include <atomic>
#include <tsl/robin_map.h>

class Professor
{
    friend class Representacao;
    friend class Grade;
    friend class Horario;
    friend class Solucao;
    friend class Resolucao;
public:
    Professor(const std::string& pNome);
    Professor(const std::string& pNome, const std::string& pId);

    const std::string& getId() const;

    const std::string& getNome() const;

    int getNumDisponibilidade() const;
    void setNome(const std::string& pNome);

    void setCreditoMaximo(int pCreditoMaximo);

    void setDiaDisponivel(int dia, int bloco);
    void unsetDiaDisponivel(int dia, int bloco);
    bool isDiaDisponivel(int dia, int bloco) const;

    void addCompetencia(const std::string& pDisciplina, double pPeso);
    bool haveCompetencia(const std::string& pDisciplina) const;

    int preferenciaAulas() const;
    void setPreferenciaAulas(int num);

    bool isDiscPreferencia(std::size_t disc) const;
    void addDiscPreferencia(std::size_t disc);

    int credito_maximo() const;

    std::size_t id_hash() const;

    static std::size_t max_hash();
private:
    static std::atomic_size_t Next_code;

    std::string id;

    std::string nome;
    int creditoMaximo;

    std::vector<std::vector<bool>> diasDisponiveis;
    int numDisponibilidade;

    std::map<std::string, double> competencias;
    std::vector<char> preferenciasDisciplina;
    int preferenciaNumAulas;

    std::size_t idHash;

    void init(const std::string& pNome, const std::string& pId);

  static bool isDiaValido(int dia);
    void setDiaDisponivel(int dia, int bloco, bool value);
};

inline std::size_t
Professor::id_hash() const
{
  return idHash;
}

inline std::size_t
Professor::max_hash()
{
  return Next_code + 1;
}

#endif /* PROFESSOR_H */

