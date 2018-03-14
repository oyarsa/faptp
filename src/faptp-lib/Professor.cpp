#include <faptp-lib/Professor.h>
#include <faptp-lib/Semana.h>
#include <faptp-lib/UUID.h>
#include <faptp-lib/ProfessorDisciplina.h>

std::atomic_size_t Professor::Next_code = 0;

Professor::Professor(const std::string& pNome) 
: creditoMaximo(), numDisponibilidade(), 
  preferenciasDisciplina(Disciplina::max_hash())
{
    std::string pId = fagoc::UUID::GenerateUuid();
    init(pNome, pId);
}

Professor::Professor(const std::string& pNome, const std::string& pId) 
: creditoMaximo(), numDisponibilidade(),
  preferenciasDisciplina(Disciplina::max_hash())
{
    init(pNome, pId);
}

void Professor::init(const std::string& pNome, const std::string& pId)
{
    setNome(pNome);
    id = pId;
    idHash = Next_code++;
    preferenciaNumAulas = 24;
}

const std::string& Professor::getId() const
{
    return id;
}

int Professor::getNumDisponibilidade() const
{
    return numDisponibilidade;
}

const std::string& Professor::getNome() const
{
    return nome;
}

void Professor::setNome(const std::string& pNome)
{
    nome = pNome;
}

bool Professor::isDiaValido(int dia)
{
    return dia < dias_semana_total;
}

void Professor::setCreditoMaximo(int pCreditoMaximo)
{
    creditoMaximo = pCreditoMaximo;
}

void Professor::setDiaDisponivel(int dia, int bloco, bool value)
{
    if (isDiaValido(dia)) {
        diasDisponiveis[dia][bloco] = value;
    }
}

void Professor::setDiaDisponivel(int dia, int bloco)
{
    setDiaDisponivel(dia, bloco, true);
}

void Professor::unsetDiaDisponivel(int dia, int bloco)
{
    setDiaDisponivel(dia, bloco, false);
}

bool Professor::isDiaDisponivel(int dia, int bloco) const
{
    if (isDiaValido(dia)) {
        return diasDisponiveis[bloco][dia];
    }
    return false;
}

void Professor::addCompetencia(const std::string& pDisciplina, double pPeso)
{
    competencias[pDisciplina] = pPeso;
}

bool Professor::haveCompetencia(const std::string& pDisciplina) const
{
    return competencias.count(pDisciplina) > 0;
}

int Professor::preferenciaAulas() const
{
    return preferenciaNumAulas;
}

void
Professor::setPreferenciaAulas(int num)
{
  preferenciaNumAulas = num;
}

bool
Professor::isDiscPreferencia(std::size_t disc) const
{
  return preferenciasDisciplina[disc];
}

void
Professor::addDiscPreferencia(std::size_t disc)
{
  preferenciasDisciplina[disc] = true;
}

int Professor::credito_maximo() const
{
    return creditoMaximo;
}
