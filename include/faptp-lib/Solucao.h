#ifndef SOLUCAO_H
#define SOLUCAO_H

#include <unordered_map>
#include <array>
#include <memory>

#include <optional>

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
            const Resolucao& res, Configuracao::TipoFo tipo_fo,
            const std::unordered_map<std::string, double>& pesos = pesos_padrao);
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
    std::unordered_map<std::string, double> pesos_;

    static const std::unordered_map<std::string, double> pesos_padrao;
};

#endif /* SOLUCAO_H */
