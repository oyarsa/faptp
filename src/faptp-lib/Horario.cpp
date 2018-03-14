#include <sstream>

#include <faptp-lib/Horario.h>
#include <faptp-lib/Semana.h>


Horario::Horario(int pBlocosTamanho, int pCamadasTamanho)
  : Representacao(pBlocosTamanho, pCamadasTamanho), 
    disc_camada_(Disciplina::max_hash()),
    creditos_alocados_disc_(Disciplina::max_hash()),
    creditos_alocados_prof_(Professor::max_hash()),
    hash_(0) {}

Horario::Horario(const Horario& outro)
    : Representacao(outro), disc_camada_(outro.disc_camada_),
      creditos_alocados_disc_(outro.creditos_alocados_disc_), 
      creditos_alocados_prof_(outro.creditos_alocados_prof_),
      hash_(0) {}

Horario& Horario::operator=(const Horario& outro)
{
    Representacao::operator=(outro);
    creditos_alocados_disc_ = outro.creditos_alocados_disc_;
    creditos_alocados_prof_ = outro.creditos_alocados_prof_;
    hash_ = 0;
    disc_camada_ = outro.disc_camada_;
    return *this;
}

bool Horario::colisaoProfessorAlocado(int pDia, int pBloco, const Professor& professor) const
{
    const auto creditos = creditos_alocados_prof_[professor.id_hash()];
    if (!professor.isDiaDisponivel(pDia, pBloco)
        || creditos >= professor.credito_maximo()) {
        return true;
    }

    for (auto i = 0; i < camadasTamanho; i++) {
        const auto pd = at(pDia, pBloco, i);
        if (pd && pd->professor->id_hash() == professor.id_hash()) {
            return true;
        }
    }

    return false;
}

bool Horario::insert(int pDia, int pBloco, int pCamada, ProfessorDisciplina* pProfessorDisciplina)
{
    if (!pProfessorDisciplina) {
        return true;
    }
    return insert(pDia, pBloco, pCamada, pProfessorDisciplina, false);
}


bool Horario::isViable(int dia, int bloco, int camada, ProfessorDisciplina* pd) const
{
    if (!pd) {
        return true;
    }

    const auto pos = getPosition(dia, bloco, camada);
    const auto disc = pd->disciplina;

    const auto creditos = creditos_alocados_disc_[disc->id_hash()];
    if (creditos >= disc->cargaHoraria) {
        return false;
    }

    if (!matriz[pos]) {
        const auto isProfAloc = colisaoProfessorAlocado(dia, bloco, *pd->professor);
        if (!isProfAloc) {
            return true;
        }
    }

    return false;
}

bool Horario::insert(int dia, int bloco, int camada, ProfessorDisciplina* pd, bool force)
{
    const auto position = getPosition(dia, bloco, camada);
    const auto disc = pd->disciplina;
    const auto prof = pd->getProfessor();

    if (creditos_alocados_disc_[disc->id_hash()] >= disc->cargaHoraria) {
        return false;
    }

    if (!matriz[position] || force) {
        const auto professorAlocado = colisaoProfessorAlocado(dia, bloco, *prof);
        if (!professorAlocado || force) {
            creditos_alocados_disc_[disc->id_hash()]++;
            creditos_alocados_prof_[prof->id_hash()]++;
            return Representacao::insert(dia, bloco, camada, pd, force);
        }
    }
    return false;
}

void Horario::clearSlot(int pDia, int pBloco, int pCamada)
{
    auto pos = getPosition(pDia, pBloco, pCamada);
    auto profdisc = matriz[pos];
    if (!profdisc) {
        return;
    }
    creditos_alocados_disc_[profdisc->disciplina->id_hash()]--;
    creditos_alocados_prof_[profdisc->professor->id_hash()]--;
    Representacao::clearSlot(pDia, pBloco, pCamada);
}

void Horario::clearCamada(int camada)
{
    for (auto d = 0; d < dias_semana_util; d++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            clearSlot(d, b, camada);
        }
    }
}

std::size_t Horario::getHash()
{
    if (hash_ != 0) {
        return hash_;
    }
    auto oss = std::ostringstream {};

    for (const auto& pd : matriz) {
        if (pd) {
            oss << pd->disciplina->id << pd->professor->id;
        } else {
            oss << "00";
        }
    }
    hash_ = Util::hash_string(oss.str());
    return hash_;
}

int Horario::contaJanelasDia(int dia, int camada) const
{
    auto janelas = 0;
    auto contando = false;
    auto contador = 0;

    for (auto j = 0; j < blocosTamanho; j++) {
        if (at(dia, j, camada)) {
            if (!contando) {
                contando = true;
            }
            if (contando && contador > 0) {
                janelas += contador;
                contador = 0;
            }
        } else if (contando) {
            contador++;
        }
    }

    return janelas;
}

int Horario::contaJanelasCamada(int camada) const
{
    auto janelas = 0;
    for (auto d = 0; d < dias_semana_util; d++) {
        janelas += contaJanelasDia(d, camada);
    }
    return janelas;
}

int Horario::contaJanelas() const
{
    auto janelas = 0;
    for (auto c = 0; c < camadasTamanho; c++) {
        janelas += contaJanelasCamada(c);
    }
    return janelas;
}

bool Horario::isProfDia(
  std::size_t professor,
  int dia
) const
{
    for (auto c = 0; c < camadasTamanho; c++) {
        for (auto b = 0; b < blocosTamanho; b++) {
            const auto pd = at(dia, b, c);
            if (pd && pd->getProfessor()->id_hash() == professor) {
                return true;
            }
        }
    }
    return false;
}

int Horario::intervalosTrabalhoProf(std::size_t professor) const
{
     auto intervalos = 0;
     auto contando = false;
     auto contador = 0;

     for (auto j = 0; j < dias_semana_util; j++) {
         if (isProfDia(professor, j)) {
             if (!contando) {
                 contando = true;
             }
             if (contando && contador > 0) {
                 intervalos += contador;
                 contador = 0;
             }
         } else if (contando) {
             contador++;
         }
     }

     return intervalos;
}

int Horario::intervalosTrabalho(
    const hash_map<std::string, Professor*>& professores
) const
{
    auto intervalos = 0;
    for (const auto& p : professores) {
        intervalos += intervalosTrabalhoProf(p.second->id_hash());
    }
    return intervalos;
}

bool Horario::isAulaDia(int dia, int camada) const
{
    for (auto b = 0; b < blocosTamanho; b++) {
        if(at(dia, b, camada)) {
            return true;
        }
    }
    return false;
}

int Horario::numDiasAulaCamada(int camada) const
{
    auto num = 0;
    for (auto d = 0; d < dias_semana_util; d++) {
        num += isAulaDia(d, camada);
    }
    return num;
}

int Horario::numDiasAula() const
{
    auto num = 0;
    for (auto c = 0; c < camadasTamanho; c++) {
        num += numDiasAulaCamada(c);
    }
    return num;
}

int Horario::aulasSabadoCamada(int camada) const
{
    const auto sabado = dias_semana_util - 1;
    auto num = 0;
    for (auto b = 0; b < blocosTamanho; b++) {
        if (at(sabado, b, camada)) {
            num++;
        }
    }
    return num;
}

int Horario::aulasSabado() const
{
    auto aulas = 0;
    for (auto c = 0; c < camadasTamanho; c++) {
        aulas += aulasSabadoCamada(c);
    }
    return aulas;
}

int Horario::aulasSeguidasDiscDia(const std::size_t disciplina, int dia) const
{
    auto num = 0;
    const auto camada = disc_camada_[disciplina];

    for (auto b = 0; b < blocosTamanho; b++) {
        const auto pd = at(dia, b, camada);
        if (pd && pd->getDisciplina()->id_hash() == disciplina) {
            num++;
        }
    }

    return std::max(num - 2, 0);
}

int Horario::aulasSeguidasDisc(const std::size_t disciplina) const
{
    auto num = 0;
    for (auto d = 0; d < dias_semana_util; d++) {
        num += aulasSeguidasDiscDia(disciplina, d);
    }
    return num;
}

int Horario::aulasSeguidas(const std::vector<Disciplina*>& disciplinas) const
{
    auto num = 0;
    for (auto& disc : disciplinas) {
        if (disc->ofertada) {
            num += aulasSeguidasDisc(disc->id_hash());
        }
    }

    return num;
}

int Horario::aulasSeguidasDificilDia(int dia, int camada) const
{
    auto num = 0;

    for (auto b = 0; b < blocosTamanho; b++) {
        const auto pd = at(dia, b, camada);
        if (pd && pd->getDisciplina()->isDificil()) {
            num++;
        }
    }

    return std::max(num - 2, 0);
}

int Horario::aulasSeguidasDificilCamada(int camada) const
{
    auto num = 0;
    for (auto d = 0; d < dias_semana_util; d++) {
        num += aulasSeguidasDificilDia(d, camada);
    }
    return num;
}

int Horario::aulasSeguidasDificil() const
{
    auto num = 0;
    for (auto c = 0; c < camadasTamanho; c++) {
        num += aulasSeguidasDificilCamada(c);
    }
    return num;
}

bool Horario::aulaDificilUltimoHorarioDia(int dia, int camada) const
{
    const auto ultimo = blocosTamanho - 2;

    const auto pd1 = at(dia, ultimo, camada);
    const auto pd2 = at(dia, ultimo + 1, camada);

    const auto dif1 = pd1 && pd1->getDisciplina()->isDificil();
    const auto dif2 = pd2 && pd2->getDisciplina()->isDificil();

    return dif1 || dif2;
}

int Horario::aulaDificilUltimoHorarioCamada(int camada) const
{
    auto num = 0;
    for (auto d = 0; d < dias_semana_util; d++) {
        num += aulaDificilUltimoHorarioDia(d, camada);
    }
    return num;
}

int Horario::aulaDificilUltimoHorario() const
{
    auto num = 0;
    for (auto c = 0; c < camadasTamanho; c++) {
        num += aulaDificilUltimoHorarioCamada(c);
    }
    return num;
}

int Horario::preferenciasProfessores() const
{
    auto num = 0;
    const auto matrix_size = camadasTamanho * dias_semana_util * blocosTamanho;

    std::vector<char> percorrido(Disciplina::max_hash());

    for (auto i = 0; i < matrix_size; i++) {
      const auto pd = at(i);
      if (!pd) {
        continue;
      }

      const auto professor = pd->getProfessor();
      const auto disc = pd->getDisciplina()->id_hash();

      if (percorrido[disc]) {
        continue;
      }

      percorrido[disc] = true;
      num += !professor->isDiscPreferencia(disc);
    }
    return num;
}

int Horario::aulasProfessor(const std::size_t professor, const int preferencia) const
{
  const auto matrix_size = camadasTamanho * dias_semana_util * blocosTamanho;
  auto num = 0;

  for (auto i = 0; i < matrix_size; i++) {
    const auto pd = at(i);
    if (pd && pd->getProfessor()->id_hash() == professor) {
      num++;
    }
  }

  const auto excesso = num - preferencia;
  return std::max(excesso, 0);
}

int Horario::aulasProfessores(
    const hash_map<std::string, Professor*>& professores
) const
{
  const auto matrix_size = camadasTamanho * dias_semana_util*blocosTamanho;

  std::vector<int> aulas(Professor::max_hash());

  for (auto i = 0; i < matrix_size; i++) {
    const auto pd = at(i);
    if (pd) {
      aulas[pd->getProfessor()->id_hash()]++;
    }
  }

  std::vector<int> preferencias(Professor::max_hash());

  for (const auto& p : professores) {
    const auto& prof = *p.second;
    preferencias[prof.id_hash()] = prof.preferenciaAulas();
  }

  auto num = 0;

  for (auto i = 0; i < Professor::max_hash(); i++) {
    const auto excesso = aulas[i] - preferencias[i];
    num += std::max(excesso, 0);
  }

  return num;
}

hash_map<std::string, ProfessorDisciplina*>
Horario::getAlocFromDiscNames(int camada) const
{
    const auto camada_inicio = getPosition(0, 0, camada);
    const auto camada_fim = camada_inicio + blocosTamanho * dias_semana_util;

    hash_map<std::string, ProfessorDisciplina*> alocs;

    for (auto i = camada_inicio; i < camada_fim; i++) {
        const auto pd = at(i);
        const auto nome = pd ? pd->getDisciplina()->getId() : "null";
        alocs[nome] = pd;
    }

    return alocs;
}
