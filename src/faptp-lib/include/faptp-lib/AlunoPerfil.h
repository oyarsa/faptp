#ifndef ALUNOPERFIL_H
#define    ALUNOPERFIL_H

#include <string>
#include <tsl/robin_map.h>
#include <tsl/hopscotch_set.h>

#include <faptp-lib/Disciplina.h>

class AlunoPerfil
{
    friend class Grade;
    friend class Solucao;
    friend class Resolucao;
    friend class Output;
public:
    AlunoPerfil(double pPeso, const std::string& pId, const std::string& pTurma, const std::string& pPeriodo);

    std::size_t getHash() const;

    double getPeso() const;
    void setPeso(double pPeso);

    void addCursada(std::size_t cursada);
    bool isCursada(std::size_t cursada);

    void addRestante(Disciplina* pRestante);
    bool isRestante(std::size_t restante);

    int getPeriodoNum() const;

    void finalizarConstrucao();

private:
    int calcPeriodoNum() const;
    std::size_t calcHash() const;

    struct DisciplinaCargaHorariaDesc
    {
        bool operator()(const Disciplina* a, const Disciplina* b) const
        {
            return a->getCargaHoraria() > b->getCargaHoraria();
        }
    };

    std::string id;
    std::string turma;
    std::string periodo;
    double peso;
    std::size_t hash;
    std::vector<std::size_t> cursadas;
    std::vector<std::size_t> aprovadas;
    std::vector<std::size_t> restante;
    std::vector<Disciplina*> restanteOrd;
    int periodo_num;
};

inline int
AlunoPerfil::getPeriodoNum() const
{
    return periodo_num;
}

inline std::size_t
AlunoPerfil::getHash() const
{
    return hash;
}


#endif /* ALUNOPERFIL_H */

