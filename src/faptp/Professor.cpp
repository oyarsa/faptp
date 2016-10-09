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

std::string Professor::getId()
{
    return id;
}

int Professor::getNumDisponibilidade() const
{
    return numDisponibilidade;
}

std::string Professor::getNome()
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

bool Professor::isDiaDisponivel(unsigned int dia, int bloco)
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

bool Professor::haveCompetencia(std::string pDisciplina)
{
    return competencias.count(pDisciplina) > 0;
}

