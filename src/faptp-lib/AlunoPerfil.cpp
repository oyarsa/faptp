#include <utility>
#include <sstream>

#include <faptp-lib/AlunoPerfil.h>
#include <faptp-lib/Util.h>

AlunoPerfil::AlunoPerfil(double pPeso, const std::string& pId,
                         const std::string& pTurma, const std::string& pPeriodo)
    : id(pId),
      turma(pTurma),
      periodo(pPeriodo),
      peso(pPeso),
      hash(0) {}

double AlunoPerfil::getPeso() const
{
    return peso;
}

long long AlunoPerfil::getHash()
{
    if (hash != 0) {
        return hash;
    }

    std::ostringstream oss;
    oss << turma << periodo;
    for (auto& disc : aprovadas) {
        oss << disc;
    }
    for (auto& disc : cursadas) {
        oss << disc;
    }

    hash = std::hash<std::string> {}(oss.str());
    return hash;
}

void AlunoPerfil::setPeso(double pPeso)
{
    peso = pPeso;
}

void AlunoPerfil::addCursada(const std::string& pCursada)
{
    cursadas.insert(pCursada);
}

bool AlunoPerfil::isCursada(const std::string& pCursada)
{
    return cursadas.find(pCursada) != cursadas.end();
}

void AlunoPerfil::addRestante(Disciplina* pRestante)
{
    restante.insert(pRestante->getId());
    Util::insert_sorted(restanteOrd, pRestante, DisciplinaCargaHorariaDesc {});
}

bool AlunoPerfil::isRestante(const std::string& pRestante)
{
    return restante.find(pRestante) != restante.end();
}

int AlunoPerfil::getPeriodoNum() const
{
    if (periodo == "")
        return 0;

    static auto pos = periodo.find('-');
    static auto per_num = std::stoi(periodo.substr(0, pos));

    return per_num;
}

