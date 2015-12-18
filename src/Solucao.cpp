#include "Solucao.h"

Solucao::Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;
    perfisTamanho = pPerfisTamanho;
    id = Aleatorio().randomInt();
    init();
}

Solucao::Solucao(const Solucao& outro) 
    : horario(new Horario(*(outro.horario)))
    , id(Aleatorio().randomInt())
    , blocosTamanho(outro.blocosTamanho)
    , camadasTamanho(outro.camadasTamanho)
    , perfisTamanho(outro.perfisTamanho)
    , gradesLength(outro.gradesLength)
{
    for (const auto& par : outro.grades) {
        grades[par.first] = new Grade(*(par.second));
    }
}


void Solucao::init() {
    horario = new Horario(blocosTamanho, camadasTamanho);

    gradesLength = 0;
}

Solucao::~Solucao() {
    delete horario;
    
    for (auto& par : grades) {
        delete par.second;
    }
}

void Solucao::insertGrade(Grade* grade) {
    auto& alvoInsercao = grades[grade->alunoPerfil->id];
    if (alvoInsercao) {
        delete alvoInsercao;
    }
    alvoInsercao = grade;
    gradesLength++;
}

double Solucao::getObjectiveFunction() {
    return std::accumulate(begin(grades), end(grades), 0.0,
            [](const double& acc, const std::pair<std::string, Grade*>& par) {
                return acc + par.second->getObjectiveFunction();
            });
}
