#ifndef PROFESSOR_H
#define    PROFESSOR_H

#include <string>
#include <cstring>
#include <vector>
#include <map>

#include <faptp-lib/UUID.h>
#include <unordered_set>

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

    std::string getId() const;

    std::string getNome() const;

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

    bool isDiscPreferencia(const std::string& disc) const;
    void addDiscPreferencia(std::string disc);

    int credito_maximo() const;
private:
    std::string id;

    std::string nome;
    int creditoMaximo;

    std::vector<std::vector<bool>> diasDisponiveis;
    int numDisponibilidade;

    std::map<std::string, double> competencias;
    std::unordered_set<std::string> preferenciasDisciplina;
    int preferenciaNumAulas;

    void init(const std::string& pNome, const std::string& pId);

  static bool isDiaValido(int dia);
    void setDiaDisponivel(int dia, int bloco, bool value);
};

#endif /* PROFESSOR_H */

