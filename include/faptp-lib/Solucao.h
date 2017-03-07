#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <unordered_map>
#include <array>
#include <memory>

#include <optional>

#include "Horario.h"
#include "Grade.h"
#include "Configuracao.h"

constexpr std::size_t num_pesos = 9;
// constexpr std::array<double, 9> pesos_padrao{ { 1, 1, 1, 1, 1, 1, 1, 1, 1 } };
constexpr std::array<double, num_pesos> pesos_padrao{ 
	{ 2, 1.5, 3.5, 4.667, 4, 2.5, 2.333, 3.167, 1.667 } 
};

class Resolucao;

class Solucao
{
    friend class Resolucao;
    friend class Output;
public:
	using FO_t = double;
    Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho, 
            const Resolucao& res, Configuracao::TipoFo tipo_fo,
		    const std::optional<std::array<double, 9>>& pesos = std::nullopt);
    Solucao(const Solucao& outro);
    virtual ~Solucao();

    std::unique_ptr<Solucao> clone() const;
    void insertGrade(Grade* grade);

    void calculaFO();
    FO_t calculaFOSomaCarga();
    FO_t calculaFOSoftConstraints() const;
    FO_t getFO();
    FO_t getFO() const;

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
	std::optional<FO_t> fo;
    const Resolucao& res;
    Configuracao::TipoFo tipo_fo;
	std::array<double, num_pesos> pesos_;
};

#endif /* SOLUCAO_H */

