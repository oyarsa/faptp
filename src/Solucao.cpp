#include "Solucao.h"

Solucao::Solucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho) {
    blocosTamanho = pBlocosTamanho;
    camadasTamanho = pCamadasTamanho;
    perfisTamanho = pPerfisTamanho;

    init();
}

void Solucao::init() {
    horario = new Horario(blocosTamanho, camadasTamanho);

    gradesLength = 0;
}

Solucao::~Solucao() {
    puts("entrou dtoRRr");
    if (horario) {
        puts("vai horario");
        delete horario;
        puts("ue");
        horario = nullptr;
    }
    puts("saiu horario");
    int i = 0;
    for (auto& par : grades) {
        if (par.second) {
            printf("dtor %d\n", i++);
            delete par.second;
            par.second = nullptr;
            printf("saiu dtori\n");
        }
    }
    puts("saiu dtor");
}

void Solucao::insertGrade(Grade* grade) {
    grades[grade->alunoPerfil->id] = grade;
    gradesLength++;
}

double Solucao::getObjectiveFunction() {
    return std::accumulate(begin(grades), end(grades), 0.0,
            [](const double& acc, const std::pair<std::string, Grade*>& par) {
                return acc + par.second->getObjectiveFunction();
            });
}

Solucao* Solucao::clone() const {
    Solucao* s = new Solucao(*this);

    std::map<std::string, Grade*>::iterator gIter = s->grades.begin();
    std::map<std::string, Grade*>::iterator gIterEnd = s->grades.end();

    for (; gIter != gIterEnd; ++gIter) {
        s->grades[gIter->first] = gIter->second->clone();
    }

    return s;
}
