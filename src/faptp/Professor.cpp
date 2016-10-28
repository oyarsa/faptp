#include <faptp/Professor.h>
#include <faptp/Semana.h>

Professor::Professor(std::string pNome) : creditoMaximo(), numDisponibilidade()
{
    std::string pId;

    pId = UUID::GenerateUuid();

    init(pNome, pId);
}

Professor::Professor(std::string pNome, std::string pId) : creditoMaximo(), numDisponibilidade()
{
    init(pNome, pId);
}

void Professor::init(std::string pNome, std::string pId)
{
    setNome(pNome);
    id = pId;
}

std::string Professor::getId() const
{
    return id;
}

int Professor::getNumDisponibilidade() const
{
    return numDisponibilidade;
}

std::string Professor::getNome() const
{
    return nome;
}

void Professor::setNome(std::string pNome)
{
    nome = pNome;
}

bool Professor::isDiaValido(unsigned int dia)
{
    return dia < dias_semana_total;
}

void Professor::setCreditoMaximo(int pCreditoMaximo)
{
    creditoMaximo = pCreditoMaximo;
}

void Professor::setDiaDisponivel(unsigned int dia, int bloco, bool value)
{
    if (isDiaValido(dia)) {
        diasDisponiveis[dia][bloco] = value;
    }
}

void Professor::setDiaDisponivel(unsigned int dia, int bloco)
{
    setDiaDisponivel(dia, bloco, true);
}

void Professor::unsetDiaDisponivel(unsigned int dia, int bloco)
{
    setDiaDisponivel(dia, bloco, false);
}

bool Professor::isDiaDisponivel(unsigned int dia, int bloco) const
{
    if (isDiaValido(dia)) {
        return diasDisponiveis[dia][bloco];
    }

    return false;
}

void Professor::addCompetencia(std::string pDisciplina, double pPeso)
{
    competencias[pDisciplina] = pPeso;
}

bool Professor::haveCompetencia(std::string pDisciplina) const
{
    return competencias.count(pDisciplina) > 0;
}

int Professor::preferenciaAulas() const
{
    return 24;
}

int Professor::credito_maximo() const
{
    return creditoMaximo;
}
