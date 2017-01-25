#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <unordered_map>
#include <boost/optional.hpp>

#include "Horario.h"
#include "Grade.h"
#include "Configuracao.h"

constexpr std::size_t k_num_pesos = 9;
// constexpr std::array<double, 9> k_pesos_padrao{ { 1, 1, 1, 1, 1, 1, 1, 1, 1 } };
constexpr std::array<double, k_num_pesos> k_pesos_padrao{ 
	{ 2, 1.5, 3.5, 4.667, 4, 2.5, 2.333, 3.167, 1.667 } 
};

class Resolucao;

class Solucao
{
    friend class Resolucao;
    friend class Output;
public:
    Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho, 
            const Resolucao& res, Configuracao::TipoFo tipo_fo,
		    const boost::optional<std::array<double, 9>>& pesos = boost::none);
    Solucao(const Solucao& outro);
    virtual ~Solucao();

    std::unique_ptr<Solucao> clone() const;
    void insertGrade(Grade* grade);

    void calculaFO();
    int calculaFOSomaCarga();
    int calculaFOSoftConstraints() const;
    int getFO();
    int getFO() const;

    std::unordered_map<std::string, int> reportarViolacoes() const;

    const Horario& getHorario() const;
    Horario& getHorario();

    std::size_t getHash();
    std::unordered_map<int, std::string> camada_periodo;
    std::unordered_map<std::string, ProfessorDisciplina*> alocacoes;
private:
    int id;
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    std::unique_ptr<Horario> horario;
    std::unordered_map<std::string, Grade*> grades;

    int gradesLength;
    int fo = -1;
    const Resolucao& res;
    Configuracao::TipoFo tipo_fo;
	std::array<double, 9> pesos_;
};

#endif /* SOLUCAO_H */

