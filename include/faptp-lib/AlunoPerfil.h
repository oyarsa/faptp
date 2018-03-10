#ifndef ALUNOPERFIL_H
#define    ALUNOPERFIL_H

#include <string>
#include <hash_map.h>

#include <faptp-lib/Disciplina.h>

class AlunoPerfil
{
    friend class Grade;
    friend class Solucao;
    friend class Resolucao;
    friend class Output;
public:
    AlunoPerfil(double pPeso, const std::string& pId, const std::string& pTurma, const std::string& pPeriodo);

    long long getHash();

    double getPeso() const;
    void setPeso(double pPeso);

    void addCursada(const std::string& pCursada);
    bool isCursada(const std::string& pCursada);

    void addRestante(Disciplina* pRestante);
    bool isRestante(const std::string& pRestante);

    int getPeriodoNum() const;

private:

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
    long long hash;
    hash_set<std::string> cursadas;
    hash_set<std::string> aprovadas;
    hash_set<std::string> restante;
    std::vector<Disciplina*> restanteOrd;
};

#endif /* ALUNOPERFIL_H */

