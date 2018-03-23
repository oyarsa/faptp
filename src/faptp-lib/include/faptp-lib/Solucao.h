#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <tsl/robin_map.h>
#include <array>
#include <memory>

#include <optional.hpp>

#include "Horario.h"
#include "Grade.h"
#include "Configuracao.h"

class Resolucao;

class Solucao
{
    friend class Resolucao;
    friend class Output;
public:
	using FO_t = double;
    Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho,
            const Resolucao& res, Configuracao::TipoFo tipo_fo);
    Solucao(const Solucao& outro);
    Solucao& operator=(const Solucao&) = delete;
    virtual ~Solucao();

    std::unique_ptr<Solucao> clone() const;
    void insertGrade(Grade* grade);

    void calculaFO();
    FO_t calculaFOSomaCarga() const;
    FO_t calculaFOSoftConstraints() const;
    FO_t getFO();
    FO_t getFO() const;

    tsl::robin_map<std::string, int> reportarViolacoes() const;

    const Horario& getHorario() const;
    Horario& getHorario();

    std::size_t getHash();
    tsl::robin_map<int, std::string> camada_periodo;
    tsl::robin_map<std::string, ProfessorDisciplina*> alocacoes;
private:
    int id;
    int blocosTamanho;
    int camadasTamanho;
    int perfisTamanho;

    std::unique_ptr<Horario> horario;
    tsl::robin_map<std::string, Grade*> grades;

    int gradesLength;
	std::experimental::optional<FO_t> fo;
    const Resolucao& res;
    Configuracao::TipoFo tipo_fo;

    static const tsl::robin_map<std::string, double> pesos_padrao;
};

#endif /* SOLUCAO_H */
