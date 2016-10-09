#ifndef REPRESENTACAO_H
#define REPRESENTACAO_H

#include <string>
#include <vector>
#include <tuple>
#include <faptp/ProfessorDisciplina.h>
#include <faptp/Util.h>
#include <faptp/Semana.h>

class Representacao
{
    friend class Resolucao;
public:
    Representacao(int pBlocosTamanho, int pCamadasTamanho);
    Representacao(const Representacao& outro);
    Representacao& operator=(const Representacao& outro);
    virtual ~Representacao();

    int getPosition(int pDia, int pBloco, int pCamada) const;

    ProfessorDisciplina* at(int pDia, int pBloco, int pCamada) const;
    ProfessorDisciplina* at(int position) const;

    const std::vector<ProfessorDisciplina*>& getMatriz() const;

    virtual bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina);
    virtual bool insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina, bool force);

    // Bloco, dia, camada
    void get3DMatrix(int pLinear, int triDimensional[3]);
    // Bloco, dia, camada
    std::tuple<int, int, int> getCoords(int pLinear) const;

    std::vector<ProfessorDisciplina*>::iterator getFirstDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, Disciplina* pDisciplina);

    int getFirstDisciplina(Disciplina* pDisciplina);
    int getFirstDisciplina(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> pMatriz);

    std::vector<int> getAllEmpty(int camada);
    void clearDisciplina(ProfessorDisciplina* pProfessorDisciplina, int camada);
    virtual void clearSlot(int pDia, int pBloco, int pCamda);

    int getPositionDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, std::vector<ProfessorDisciplina*>::iterator iterFound);

    int getBlocosTamanho() const;

protected:
    int blocosTamanho;
    int camadasTamanho;

    int size;

    std::vector<std::string> blocos;
    std::vector<std::string> camadas;

    std::vector<ProfessorDisciplina*> matriz;

    void initMatriz();
};

inline int Representacao::getPosition(int pDia, int pBloco, int pCamada) const
{
    return pBloco + pDia * blocosTamanho + pCamada * blocosTamanho * dias_semana_util;
}

#endif /* REPRESENTACAO_H */

