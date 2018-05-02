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
      hash(0),
      periodo_num(calcPeriodoNum())
    {}

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

void AlunoPerfil::addCursada(std::size_t cursada)
{
    cursadas.push_back(cursada);
}

bool AlunoPerfil::isCursada(std::size_t cursada)
{
    return std::find(cursadas.begin(), cursadas.end(), cursada)
        != cursadas.end();
}

void AlunoPerfil::addRestante(Disciplina* pRestante)
{
    restante.push_back(pRestante->id_hash());
    Util::insert_sorted(restanteOrd, pRestante, DisciplinaCargaHorariaDesc {});
}

bool AlunoPerfil::isRestante(std::size_t pRestante)
{
    return std::find(restante.begin(), restante.end(), pRestante)
        != restante.end();
}

int AlunoPerfil::calcPeriodoNum() const
{
    if (periodo == "") {
        return 0;
    }

    auto pos = periodo.find('-');
    if (pos == std::string::npos) {
        return 0;
    }

    return std::stoi(periodo.substr(0, pos));
}

void AlunoPerfil::finalizarConstrucao()
{
    std::sort(cursadas.begin(), cursadas.end());
    std::sort(aprovadas.begin(), aprovadas.end());
    std::sort(restante.begin(), restante.end());
}
