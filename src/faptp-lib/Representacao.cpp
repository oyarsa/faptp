#include <faptp-lib/Representacao.h>

#include <algorithm>
#include <cmath>

#include <faptp-lib/Algorithms.h>
#include <faptp-lib/Semana.h>

Representacao::Representacao(int pBlocosTamanho, int pCamadasTamanho)
{
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;

    initMatriz();
}

Representacao::Representacao(const Representacao& outro)
    : blocosTamanho(outro.blocosTamanho)
      , camadasTamanho(outro.camadasTamanho)
      , size(outro.size)
      , blocos(outro.blocos)
      , camadas(outro.camadas)
      , matriz(outro.matriz) {}

Representacao& Representacao::operator=(const Representacao& outro)
{
    blocosTamanho = outro.blocosTamanho;
    camadasTamanho = outro.camadasTamanho;
    size = outro.size;
    blocos = outro.blocos;
    camadas = outro.camadas;
    matriz = outro.matriz;

    return *this;
}

Representacao::~Representacao() {}

void Representacao::initMatriz()
{
    size = (camadasTamanho * blocosTamanho * dias_semana_util);

    matriz.resize(size, nullptr);
}

ProfessorDisciplina* Representacao::at(int pDia, int pBloco, int pCamada) const
{
    return at(getPosition(pDia, pBloco, pCamada));
}

ProfessorDisciplina* Representacao::at(int position) const
{
    return matriz[position];
}

const std::vector<ProfessorDisciplina*>& Representacao::getMatriz() const
{
    return matriz;
}

bool Representacao::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina)
{
    return insert(pDia, pBloco, pCamada, pProfessorDisciplina, false);
}

bool Representacao::insert(int dia, int bloco, int camada, ProfessorDisciplina* pd, bool force)
{
  (void)force;
  int position = getPosition(dia, bloco, camada);
  matriz[position] = pd;
  return true;
}

void Representacao::get3DMatrix(std::size_t pLinear, int triDimensional[3])
{
    Util::get3DMatrix(pLinear, triDimensional, dias_semana_util, blocosTamanho, camadasTamanho);
}

std::tuple<int, int, int> Representacao::getCoords(std::size_t pLinear) const
{
    int coord[3];
    Util::get3DMatrix(pLinear, coord, dias_semana_util, blocosTamanho, camadasTamanho);

    return std::make_tuple(coord[1], coord[0], coord[2]);
}

std::vector<ProfessorDisciplina*>::iterator Representacao::getFirstDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, Disciplina* pDisciplina)
{
    std::vector<ProfessorDisciplina*>::iterator xIter = find_if(iter, iterEnd, HorarioFindDisciplina(pDisciplina));

    return xIter;
}

int Representacao::getFirstDisciplina(Disciplina* pDisciplina)
{
    return getFirstDisciplina(pDisciplina, matriz);
}

int Representacao::getFirstDisciplina(Disciplina* pDisciplina, std::vector<ProfessorDisciplina*> pMatriz)
{
    int x;
    std::vector<ProfessorDisciplina*>::iterator mIter = pMatriz.begin();
    std::vector<ProfessorDisciplina*>::iterator mIterEnd = pMatriz.end();

    std::vector<ProfessorDisciplina*>::iterator xIter = getFirstDisciplina(mIter, mIterEnd, pDisciplina);
    x = getPositionDisciplina(mIter, mIterEnd, xIter);

    return x;
}

std::vector<int> Representacao::getAllEmpty(int camada)
{
    int position = 0;
    std::vector<int> empties;

    for (int d = 0; d < dias_semana_util; d++) {
        for (int b = 0; b < blocosTamanho; b++) {
            position = getPosition(d, b, camada);
            if (matriz[position] == nullptr) {
                empties.push_back(position);
            }
        }
    }

    return empties;
}

void
Representacao::clearDisciplina(const ProfessorDisciplina* pProfessorDisciplina, 
                               int camada)
{
  for (auto d = 0; d < dias_semana_util; d++) {
    for (auto b = 0; b < blocosTamanho; b++) {
      const auto position = getPosition(d, b, camada);
      if (matriz[position] == pProfessorDisciplina) {
        matriz[position] = nullptr;
      }
    }
  }
}

void Representacao::clearSlot(int pDia, int pBloco, int pCamada)
{
    auto position = getPosition(pDia, pBloco, pCamada);

    matriz[position] = nullptr;
}

int Representacao::getPositionDisciplina(std::vector<ProfessorDisciplina*>::iterator iter, std::vector<ProfessorDisciplina*>::iterator iterEnd, std::vector<ProfessorDisciplina*>::iterator iterFound)
{
    int x = -1;

    if (iterFound != iterEnd) {
        x = static_cast<int>(iterFound - iter);
    }

    return x;
}


int Representacao::getBlocosTamanho() const
{
    return blocosTamanho;
}
