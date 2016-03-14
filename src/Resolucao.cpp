#include <fstream>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <chrono>

#include <modelo-grade/arquivos.h>
#include <modelo-grade/modelo_solver.h>

#include "parametros.h"
#include "Resolucao.h"
#include "Algorithms.h"
#include "Util.h"
#include "Semana.h"
#include "Aleatorio.h"
#include "Output.h"
#include <iomanip>
#include <set>

Resolucao::Resolucao(const Configuracao& c)
	: horarioPopulacaoInicial(c.popInicial_)
	  , horarioTorneioPares(c.numTorneioPares_)
	  , horarioTorneioPopulacao(c.numTorneioPop_)
	  , horarioCruzamentoPorcentagem(c.porcentCruz_)
	  , horarioTipoCruzamento(c.tipoCruz_)
	  , horarioTipoMutacao(c.tipoMut_)
	  , horarioIteracao(c.numIter_)
	  , horarioMutacaoProbabilidade(c.mutProb_)
	  , horarioMutacaoTentativas(c.mutTentativas_)
	  , gradeTipoConstrucao(c.tipoConstr_)
	  , gradeAlfa(c.graspAlfa_)
	  , gradeGraspVizinhanca(c.tipoVizinhanca_)
	  , gradeGraspVizinhos(c.numVizinhos_)
	  , gradeGraspTempoConstrucao(c.graspTempo_)
	  , porcentagemSolucoesAleatorias(c.porcSolucoesAleatorias_)
	  , blocosTamanho(c.blocoTam_)
	  , camadasTamanho(c.camadaTam_)
	  , perfisTamanho(c.perfilTam_)
	  , arquivoEntrada(c.filename_)
	  , professores()
	  , disciplinas()
	  , periodoXdisciplina()
	  , alunoPerfis()
	  , professorDisciplinas()
	  , solucao(nullptr)
	  , jsonRoot()
	  , curso(nullptr)
	  , alunos()
	  , tempoLimiteModelo(c.tempoLimMod_)
	  , numMaximoIteracoesSemEvolucaoAG(c.numMaxIterSemEvoAG_)
	  , numMaximoIteracoesSemEvolucaoGRASP(c.numMaxIterSemEvoGRASP_)
{
	carregarDados();
}

Resolucao::Resolucao(int pBlocosTamanho, int pCamadasTamanho, int pPerfisTamanho,
                     Configuracao::TipoGrade pTipoConstrucao, std::string pArquivoEntrada)
	: gradeTipoConstrucao(pTipoConstrucao)
	  , blocosTamanho(pBlocosTamanho)
	  , camadasTamanho(pCamadasTamanho)
	  , perfisTamanho(pPerfisTamanho)
	  , arquivoEntrada(pArquivoEntrada)
	  , curso(nullptr)
	  , alunos()
{
	carregarDados();
}

Resolucao::~Resolucao()
{
	delete solucao;

	while (!disciplinas.empty()) {
		delete disciplinas.back();
		disciplinas.pop_back();
	}

	for (auto& par : professores) {
		delete par.second;
	}

	for (auto& par : alunoPerfis) {
		delete par.second;
	}

	for (auto& par : professorDisciplinas) {
		delete par.second;
	}
}

fagoc::Curso& Resolucao::getCurso()
{
	return *curso;
}

const std::vector<fagoc::Aluno>& Resolucao::getAlunos() const
{
	return alunos;
}

std::string Resolucao::getLog() const
{
	return log.str();
}

void Resolucao::carregarDados()
{
	std::ifstream myfile(arquivoEntrada);

	if (myfile.is_open()) {
		myfile >> jsonRoot;
		myfile.close();

		carregarDadosDisciplinas();
		carregarDadosProfessores();
		carregarAlunoPerfis();

		if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
			auto p = fagoc::ler_json(arquivoEntrada);
			curso.reset(new fagoc::Curso(std::move(p.first)));
			alunos = move(p.second);
		}
	} else {
		std::cerr << "We had a problem reading file (" << arquivoEntrada << ")\n";
		throw 1;
	}

	for (auto i = 0; i < 3; i++) {
		contadorIguaisCruz[i] = contadorMelhoresCruz[i] = 0;
		tempoTotalCruz[i] = 0;
	}

	contadorIguaisMut[0] = contadorIguaisMut[1] = 0;
	contadorMelhoresMut[0] = contadorMelhoresMut[1] = 0;
	tempoTotalMut[0] = tempoTotalMut[1] = 0;
	tempoTotalElit = tempoTotalSelec = 0;

}

void Resolucao::carregarDadosProfessores()
{
	const auto& jsonProfessores = jsonRoot["professores"];

	for (auto i = 0u; i < jsonProfessores.size(); i++) {
		std::string id = jsonProfessores[i]["id"].asString();
		std::string nome = jsonProfessores[i]["nome"].asString();

		professores[id] = new Professor(nome, id);
		auto& diasDisponiveis = professores[id]->diasDisponiveis;
		auto& numDisp = professores[id]->numDisponibilidade;

		if (jsonProfessores[i].isMember("creditoMaximo") == 1) {
			professores[id]->setCreditoMaximo(jsonProfessores[i]["creditoMaximo"].asInt());
		}

		if (jsonProfessores[i].isMember("disponibilidade") == 1) {
			const auto& disponibilidade = jsonProfessores[i]["disponibilidade"];
			diasDisponiveis.resize(disponibilidade.size());
			for (auto i = 0u; i < disponibilidade.size(); i++) {
				diasDisponiveis[i].resize(disponibilidade[i].size());
				for (auto j = 0u; j < disponibilidade[i].size(); j++) {
					diasDisponiveis[i][j] = disponibilidade[i][j].asBool();
				}

				numDisp += accumulate(begin(diasDisponiveis[i]), end(diasDisponiveis[i]), 0);
			}
		}

		const auto& competencias = jsonProfessores[i]["competencias"];
		for (auto j = 0u; j < competencias.size(); j++) {

			std::vector<Disciplina*>::iterator it;
			std::string disciplinaId = competencias[j].asString();

			it = find_if(disciplinas.begin(), disciplinas.end(),
			             DisciplinaFindDisciplinaId(disciplinaId));
			if (it != disciplinas.end()) {
				(*it)->addProfessorCapacitado(professores[id]);
			}
		}
	}
}

void Resolucao::carregarDadosDisciplinas()
{
	const auto& jsonDisciplinas = jsonRoot["disciplinas"];

	for (auto i = 0u; i < jsonDisciplinas.size(); i++) {
		const auto id = jsonDisciplinas[i]["id"].asString();
		const auto nome = jsonDisciplinas[i]["nome"].asString();
		const auto curso = jsonDisciplinas[i]["curso"].asString();
		const auto capacidade = jsonDisciplinas[i]["capacidade"].asInt();

		const auto cargahoraria = jsonDisciplinas[i]["carga"].asInt();
		const auto turma = jsonDisciplinas[i]["turma"].asString();

		const auto periodo = jsonDisciplinas[i]["periodo"].asString();
		const auto periodoMinimo = jsonDisciplinas[i]["periodominimo"].asString();

		Disciplina* disciplina = new Disciplina(nome, cargahoraria, periodo, curso, id, turma, capacidade, periodoMinimo);
		disciplina->ofertada = jsonDisciplinas[i]["ofertada"].asBool();

		const auto& corequisitos = jsonDisciplinas[i]["corequisitos"];
		for (auto j = 0u; j < corequisitos.size(); j++) {
			disciplina->coRequisitos.insert(corequisitos[j].asString());
		}

		const auto& prerequisitos = jsonDisciplinas[i]["prerequisitos"];
		for (auto j = 0u; j < prerequisitos.size(); j++) {
			disciplina->preRequisitos.insert(prerequisitos[j].asString());
		}

		const auto& equivalentes = jsonDisciplinas[i]["equivalentes"];
		for (auto j = 0u; j < equivalentes.size(); j++) {
			disciplina->equivalentes.insert(equivalentes[j].asString());
		}
		disciplina->equivalentes.insert(nome);

		disciplinas.push_back(disciplina);
		if (disciplina->ofertada) {
			periodoXdisciplina[curso + periodo].push_back(disciplina);
		}
	}

	disciplinas = ordenarDisciplinas();
}

void Resolucao::carregarDadosProfessorDisciplinas()
{
	const auto& jsonProfessorDisciplinas = jsonRoot["professordisciplinas"];

	for (auto i = 0u; i < jsonProfessorDisciplinas.size(); i++) {
		const auto id = jsonProfessorDisciplinas[i]["id"].asString();
		const auto professor = jsonProfessorDisciplinas[i]["professor"].asString();
		const auto disciplina = jsonProfessorDisciplinas[i]["disciplina"].asString();

		ProfessorDisciplina* professorDisciplina = new ProfessorDisciplina(professores[professor], disciplinas[disciplinasIndex[disciplina]], id);

		/**
		 * TODO: recuperar peso do v�nculo do professor com a disciplina do arquivo de entrada
		 */
		double competenciaPeso = 1.0;
		professorDisciplina->professor->addCompetencia(disciplina, competenciaPeso);

		professorDisciplinas[id] = professorDisciplina;
	}
}

void Resolucao::carregarAlunoPerfis()
{
	const auto& jsonAlunoPerfis = jsonRoot["alunoperfis"];

	for (auto i = 0u; i < jsonAlunoPerfis.size(); i++) {
		const auto id = jsonAlunoPerfis[i]["id"].asString();
		const auto peso = jsonAlunoPerfis[i]["peso"].asDouble();
		const auto turma = jsonAlunoPerfis[i]["turma"].asString();
		const auto periodo = jsonAlunoPerfis[i]["periodo"].asString();

		AlunoPerfil* alunoPerfil = new AlunoPerfil(peso, id, turma, periodo);

		const auto& jsonRestantes = jsonAlunoPerfis[i]["restantes"];
		auto worstFit = 99;
		auto bestFit = 0;
		for (auto j = 0u; j < jsonRestantes.size(); j++) {
			Disciplina* disciplina = disciplinas[disciplinasIndex[jsonRestantes[j].asString()]];
			bestFit = std::max(bestFit, disciplina->cargaHoraria);
			worstFit = std::min(worstFit, disciplina->cargaHoraria);
			alunoPerfil->addRestante(disciplina->id);
		}

		const auto& jsonCursadas = jsonAlunoPerfis[i]["cursadas"];
		for (auto j = 0u; j < jsonCursadas.size(); j++) {
			std::string cursada = jsonCursadas[j].asString();
			alunoPerfil->addCursada(cursada);
		}

		std::vector<Disciplina*> aprovadas;
		const auto& restantes = alunoPerfil->restante;
		std::remove_copy_if(begin(disciplinas), end(disciplinas), back_inserter(aprovadas),
							[&restantes](Disciplina* d) {
			return find_if(begin(restantes), end(restantes), [&d](const std::string& x) {
				return x == d->id;
			}) != end(restantes);
		});

		auto& aprovadasNomes = alunoPerfil->aprovadas;

		for (const auto& aprovada : aprovadas) {
			aprovadasNomes.insert(aprovada->id);
		}

		alunoPerfis[id] = alunoPerfil;
	}
}

double Resolucao::start()
{
	return start(true);
}

double Resolucao::start(bool input)
{
	if (input) {
		carregarSolucao();
		return gerarGrade();
	}

	return gerarHorarioAG()->getObjectiveFunction();
}

void Resolucao::carregarSolucao()
{
	const auto& jsonHorario = jsonRoot["horario"];

	carregarDadosProfessorDisciplinas();

	Solucao* solucaoLeitura = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);
	int bloco, dia, camada;

	for (auto i = 0u; i < jsonHorario.size(); i++) {
		bloco = jsonHorario[i]["horario"].asInt();
		dia = jsonHorario[i]["semana"].asInt();
		camada = jsonHorario[i]["camada"].asInt();

		ProfessorDisciplina* professorDisciplina = professorDisciplinas[jsonHorario[i]["professordisciplina"].asString()];
		if (verbose)
			std::cout << "D:" << dia << " - B:" << bloco << " - C:" << camada << " - PDSP:" << professorDisciplina->disciplina->nome << "  - P:";
		solucaoLeitura->horario->insert(dia, bloco, camada, professorDisciplina);
	}

	if (verbose)
		std::cout << "-----------------------------------------" << std::endl;
	solucao = solucaoLeitura;
}

Disciplina* Resolucao::getDisciplinaByName(const std::string& nomeDisc)
{
	return disciplinas[disciplinasIndex[nomeDisc]];
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas()
{
	disciplinas = ordenarDisciplinas(disciplinas);

	atualizarDisciplinasIndex();

	return disciplinas;
}

std::vector<Disciplina*> Resolucao::ordenarDisciplinas(std::vector<Disciplina*> pDisciplinas)
{
	std::vector<Disciplina*>::iterator dIter = pDisciplinas.begin();
	std::vector<Disciplina*>::iterator dIterEnd = pDisciplinas.end();

	sort(dIter, dIterEnd, DisciplinaCargaHorariaDesc());

	return pDisciplinas;
}

void Resolucao::atualizarDisciplinasIndex()
{
	Disciplina* disciplina;

	std::vector<Disciplina*>::iterator dIter = disciplinas.begin();
	std::vector<Disciplina*>::iterator dIterEnd = disciplinas.end();

	disciplinasIndex.clear();
	for (int i = 0; dIter != dIterEnd; ++dIter , i++) {
		disciplina = *dIter;
		disciplinasIndex[disciplina->id] = i;
	}
}

std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamento(const std::vector<Solucao*>& parVencedor)
{
	std::vector<Solucao*> filhos;
	switch (horarioTipoCruzamento) {
	case Configuracao::TipoCruzamento::construtivo_reparo: {
		auto filhos1 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[0], parVencedor[1]);
		auto filhos2 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[1], parVencedor[0]);
		filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
		filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
		break;
	}

	case Configuracao::TipoCruzamento::simples: {
		auto filhos1 = gerarHorarioAGCruzamentoSimples(parVencedor[0], parVencedor[1]);
		filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
		break;
	}

	case Configuracao::TipoCruzamento::substitui_bloco: {
		auto filhos1 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[0], parVencedor[1]);
		auto filhos2 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[1], parVencedor[0]);
		filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
		filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
		break;
	}

	}

	return filhos;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoExper(const std::vector<Solucao*>& parVencedor, Configuracao::TipoCruzamento tipoCruz)
{
	std::vector<Solucao*> filhos;
	auto idx = 0;

	switch (tipoCruz) {
	case Configuracao::TipoCruzamento::construtivo_reparo:
	{
		auto filhos1 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[0], parVencedor[1]);
		auto filhos2 = gerarHorarioAGCruzamentoConstrutivoReparo(parVencedor[1], parVencedor[0]);
		filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
		filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
		idx = 0;
		break;
	}

	case Configuracao::TipoCruzamento::simples:
	{
		auto filhos1 = gerarHorarioAGCruzamentoSimples(parVencedor[0], parVencedor[1]);
		filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
		idx = 1;
		break;
	}

	case Configuracao::TipoCruzamento::substitui_bloco:
	{
		auto filhos1 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[0], parVencedor[1]);
		auto filhos2 = gerarHorarioAGCruzamentoSubstBloco(parVencedor[1], parVencedor[0]);
		filhos.insert(filhos.end(), filhos1.begin(), filhos1.end());
		filhos.insert(filhos.end(), filhos2.begin(), filhos2.end());
		idx = 2;
		break;
	}

	}

	for (auto& filho : filhos) {
		auto fofilho = filho->getObjectiveFunction();
		auto fopai1 = parVencedor[0]->getObjectiveFunction();
		auto fopai2 = parVencedor[1]->getObjectiveFunction();

		if (fofilho > fopai1 || fofilho > fopai2) {
			contadorMelhoresCruz[idx]++;
		} else if (fofilho == fopai1 || fofilho == fopai2) {
			contadorIguaisCruz[idx]++;
		}
	}

	return filhos;
}

Solucao* Resolucao::gerarHorarioAG()
{
	tempoInicio = std::chrono::steady_clock::now();
	const auto numCruz = std::max(1, int(horarioPopulacaoInicial 
										 * horarioCruzamentoPorcentagem));
	const auto numSolucoesAleatorias = int(porcentagemSolucoesAleatorias 
										   * horarioPopulacaoInicial);

	auto populacao = gerarHorarioAGPopulacaoInicial2();
	foAlvo = populacao[0]->getObjectiveFunction();
	iteracaoAlvo = -1;
	tempoAlvo = Util::chronoDiff(std::chrono::system_clock::now(), tempoInicio);

	//for (auto i = 0; i < horarioIteracao; i++) {
	for (auto i = 0; i - iteracaoAlvo <= numMaximoIteracoesSemEvolucaoAG; i++) {
		ultimaIteracao = i;
		logPopulacao(populacao, i);

		// Insere novas soluções aleatórias na população
		auto solucoesAleatorias = gerarSolucoesAleatorias(numSolucoesAleatorias);
		Util::insert_sorted(populacao, begin(solucoesAleatorias), 
							end(solucoesAleatorias), SolucaoComparaMaior());

		// Aplica os operadores de cruzamento
		gerarHorarioAGEvoluiPopulacao(populacao, numCruz);
		// Aplica mutação
		gerarHorarioAGEfetuaMutacao(populacao);
		// Aplica o elitismo, deletando as que sobraram
		gerarHorarioAGSobrevivenciaElitismo(populacao);
		// Verifica se a nova população possui uma solução melhor que a anterior
		gerarHorarioAGVerificaEvolucao(populacao, i);
	}

	// Captura a melhor solução da população, deletando o resto
	gerarHorarioAGSobrevivenciaElitismo(populacao, 1);
	auto solucaoAG = populacao[0];

	// Se as grades foram resolvidas pelo modelo, suas matrizes estão vazias,
	// o que causa problemas com a escrita do html. 
	if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
		reinsereGrades(solucaoAG);
	}

	solucao = solucaoAG;

	return solucaoAG;
}

Solucao* Resolucao::gerarHorarioAG2()
{
	tempoInicio = std::chrono::steady_clock::now();

	auto populacao = gerarHorarioAGPopulacaoInicial2();
	foAlvo = populacao[0]->getObjectiveFunction();
	iteracaoAlvo = -1;
	tempoAlvo = Util::chronoDiff(Util::now(), tempoInicio);

	//for (auto i = 0; i < horarioIteracao; i++) {
	for (auto i = 0; i - iteracaoAlvo <= numMaximoIteracoesSemEvolucaoAG; i++) {
		ultimaIteracao = i;
		logPopulacao(populacao, i);

		auto selecbegin = Util::now();
		auto pais = gerarHorarioAGTorneioPar(populacao);
		auto totalselc = Util::chronoDiff(Util::now(), selecbegin);
		log << "Tempo selecao: " << totalselc << "ms\n";
		tempoTotalSelec += totalselc;
		
		// Aplica os operadores de cruzamento

		auto simplesbegin = Util::now();
		gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::simples, pais);
		auto simplestotal = Util::chronoDiff(Util::now(), simplesbegin);
		log << "Tempo C2: " << simplestotal << "ms\n";
		tempoTotalCruz[1] += simplestotal;

		auto subblocobegin = Util::now();
		gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::substitui_bloco, pais);
		auto subblocototal = Util::chronoDiff(Util::now(), subblocobegin);
		log << "Tempo C3: " << subblocototal << "ms\n";
		tempoTotalCruz[2] += subblocototal;

		auto constrbegin = Util::now();
		gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::construtivo_reparo, pais);
		auto constrtotal = Util::chronoDiff(Util::now(), constrbegin);
		log << "Tempo C1: " << constrtotal << "ms\n";
		tempoTotalCruz[0] += constrtotal;

		// Aplica mutação

		auto disctempo = Util::now();
		gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substiui_disciplina);
		auto disctotal = Util::chronoDiff(Util::now(), disctempo);
		log << "Tempo M1: " << disctotal << "ms\n";
		tempoTotalMut[0] += disctotal;

		auto profbegin = Util::now();
		gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substitui_professor);
		auto proftotal = Util::chronoDiff(Util::now(), profbegin);
		log << "Tempo M2: " << proftotal << "ms\n";
		tempoTotalMut[1] += proftotal;

		// Aplica o elitismo, deletando as que sobraram
		auto elitbegin = Util::now();
		gerarHorarioAGSobrevivenciaElitismo(populacao);
		auto elittotal = Util::chronoDiff(Util::now(), elitbegin);
		log << "Tempo Elitismo: " << elittotal << "ms\n";
		tempoTotalElit += elittotal;

		// Verifica se a nova população possui uma solução melhor que a anterior
		gerarHorarioAGVerificaEvolucao(populacao, i);
	}

	// Captura a melhor solução da população, deletando o resto
	gerarHorarioAGSobrevivenciaElitismo(populacao, 1);
	auto solucaoAG = populacao[0];

	// Se as grades foram resolvidas pelo modelo, suas matrizes estão vazias,
	// o que causa problemas com a escrita do html. 
	if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
		reinsereGrades(solucaoAG);
	}

	solucao = solucaoAG;

	return solucaoAG;
}

Solucao* Resolucao::gerarHorarioAG3()
{
	tempoInicio = std::chrono::steady_clock::now();

	auto populacao = gerarHorarioAGPopulacaoInicial2();
	foAlvo = populacao[0]->getObjectiveFunction();
	iteracaoAlvo = -1;
	tempoAlvo = Util::chronoDiff(Util::now(), tempoInicio);

	//for (auto i = 0; i < horarioIteracao; i++) {
	for (auto i = 0; i - iteracaoAlvo <= numMaximoIteracoesSemEvolucaoAG; i++) {
		ultimaIteracao = i;
		logPopulacao(populacao, i);

		// Aplica os operadores de cruzamento
		auto selecbegin = Util::now();
		auto pais = gerarHorarioAGTorneioPar(populacao);
		auto totalselc = Util::chronoDiff(Util::now(), selecbegin);
		log << "Tempo selecao: " << totalselc << "ms\n";
		tempoTotalSelec += totalselc;

		auto simplesbegin = Util::now();
		gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::simples, pais);
		auto simplestotal = Util::chronoDiff(Util::now(), simplesbegin);
		log << "Tempo C2: " << simplestotal << "ms\n";
		tempoTotalCruz[1] += simplestotal;

		selecbegin = Util::now();
		pais = gerarHorarioAGTorneioPar(populacao);
		totalselc = Util::chronoDiff(Util::now(), selecbegin);
		log << "Tempo selecao: " << totalselc << "ms\n";
		tempoTotalSelec += totalselc;

		auto subblocobegin = Util::now();
		gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::substitui_bloco, pais);
		auto subblocototal = Util::chronoDiff(Util::now(), subblocobegin);
		log << "Tempo C3: " << subblocototal << "ms\n";
		tempoTotalCruz[2] += subblocototal;

		selecbegin = Util::now();
		pais = gerarHorarioAGTorneioPar(populacao);
		totalselc = Util::chronoDiff(Util::now(), selecbegin);
		log << "Tempo selecao: " << totalselc << "ms\n";
		tempoTotalSelec += totalselc;

		auto constrbegin = Util::now();
		gerarHorarioAGEvoluiPopulacaoExper(populacao, Configuracao::TipoCruzamento::construtivo_reparo, pais);
		auto constrtotal = Util::chronoDiff(Util::now(), constrbegin);
		log << "Tempo C1: " << constrtotal << "ms\n";
		tempoTotalCruz[0] += constrtotal;

		// Aplica mutação

		auto disctempo = Util::now();
		gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substiui_disciplina);
		auto disctotal = Util::chronoDiff(Util::now(), disctempo);
		log << "Tempo M1: " << disctotal << "ms\n";
		tempoTotalMut[0] += disctotal;

		auto profbegin = Util::now();
		gerarHorarioAGEfetuaMutacaoExper(populacao, Configuracao::TipoMutacao::substitui_professor);
		auto proftotal = Util::chronoDiff(Util::now(), profbegin);
		log << "Tempo M2: " << proftotal << "ms\n";
		tempoTotalMut[1] += proftotal;

		// Aplica o elitismo, deletando as que sobraram

		auto elitbegin = Util::now();
		gerarHorarioAGSobrevivenciaElitismo(populacao);
		auto elittotal = Util::chronoDiff(Util::now(), elitbegin);
		log << "Tempo Elitismo: " << elittotal << "ms\n";
		tempoTotalElit += elittotal;

		// Verifica se a nova população possui uma solução melhor que a anterior
		gerarHorarioAGVerificaEvolucao(populacao, i);
	}

	// Captura a melhor solução da população, deletando o resto
	gerarHorarioAGSobrevivenciaElitismo(populacao, 1);
	auto solucaoAG = populacao[0];

	// Se as grades foram resolvidas pelo modelo, suas matrizes estão vazias,
	// o que causa problemas com a escrita do html. 
	if (gradeTipoConstrucao == Configuracao::TipoGrade::modelo) {
		reinsereGrades(solucaoAG);
	}

	solucao = solucaoAG;

	return solucaoAG;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGPopulacaoInicial()
{
	std::vector<Solucao*> solucoesAG;

	Disciplina* disciplinaAleatoria;
	Professor* professorSelecionado;

	std::string dId;
	std::string pId;
	std::string pdId;

	int randInt;
	bool inserted = false;
	bool professorPossuiCreditos;

	std::map<std::string, int> creditosUtilizadosProfessor;
	std::map<std::string, int> colisaoProfessor;

	/**
	 * TODO: ordenar os professores com menos blocos dispon�veis para dar aula
	 * Priorizar esses professores para que eles tenham as aulas montadas
	 * antes dos outros professores
	 */
	while (static_cast<int>(solucoesAG.size()) != horarioPopulacaoInicial) {
		Solucao* solucaoLocal = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);
		int i = 0;

		creditosUtilizadosProfessor.clear();
		colisaoProfessor.clear();
		for (const auto& par : periodoXdisciplina) {
			auto disciplinas = par.second;
			std::map<std::string, int> creditosUtilizadosDisciplina;
			std::map<std::string, ProfessorDisciplina*> disciplinaXprofessorDisciplina;
			int dia = 0, bloco = 0;

			bool finding = false;

			std::map<std::string, int> infactiveis;

			colisaoProfessor.clear();

			while (disciplinas.size() != 0) {
				randInt = 0;

				/**
				 * TODO: implementar aleatoriedade uniforme
				 */
				switch (0) {
				case 0:
					randInt = aleatorio::randomInt() % disciplinas.size();
					break;
				case 1:
					break;
				}

				disciplinaAleatoria = disciplinas[randInt];
				dId = disciplinaAleatoria->getId();

				// Se a disciplina ainda n�o tem professor alocado
				if (!disciplinaXprofessorDisciplina[dId]) {

					do {
						randInt = aleatorio::randomInt() % disciplinaAleatoria->professoresCapacitados.size();

						professorSelecionado = disciplinaAleatoria->professoresCapacitados[randInt];
						pId = professorSelecionado->getId();

						professorPossuiCreditos = (professorSelecionado->creditoMaximo != 0 
												   && professorSelecionado->creditoMaximo 
												      < (creditosUtilizadosProfessor[pId] + disciplinaAleatoria->getCreditos()));
					} while (professorPossuiCreditos);

					int profNum = randInt; 

					creditosUtilizadosProfessor[pId] += disciplinaAleatoria->getCreditos();

					pdId = "pr" + pId + "di" + dId;
					if (!professorDisciplinas[pdId]) {
						professorDisciplinas[pdId] = new ProfessorDisciplina(disciplinaAleatoria->professoresCapacitados[profNum], disciplinaAleatoria, pdId);
					}

					disciplinaXprofessorDisciplina[dId] = professorDisciplinas[pdId];
				}

				if (dia > (SEMANA - 1)) {
					finding = true;
				}

				if (finding) {
					inserted = false;
					std::vector<int> empties = solucaoLocal->horario->getAllEmpty(i);
					int positions[3];

					for (std::vector<int>::iterator it = empties.begin(); it != empties.end(); ++it) {

						solucaoLocal->horario->get3DMatrix((*it), positions);

						dia = positions[1];
						bloco = positions[0];

						inserted = solucaoLocal->horario->insert(dia, bloco, i, disciplinaXprofessorDisciplina[dId]);
						if (inserted) {
							break;
						}

						//std::cout << i << " " << par.first << " (" << (*it) << " - " << dia << " - " << bloco << ") [" << disciplinaAleatoria->nome << "]" << std::endl;
					}

					infactiveis[dId]++;

					//std::cerr << infactiveis[dId] << ") Deu merda! " << par.first << " [" << disciplinaAleatoria->nome << "]" << std::endl;
					if (infactiveis[dId] > 4) {

						solucaoLocal->horario->clearDisciplina(disciplinaXprofessorDisciplina[dId], i);

						disciplinas.erase(find_if(disciplinas.begin()
						                          , disciplinas.end()
						                          , DisciplinaFindDisciplina(disciplinaAleatoria)));

						continue;
					}
				} else {

					inserted = solucaoLocal->horario->insert(dia, bloco, i, disciplinaXprofessorDisciplina[dId]);
				}

				if (inserted) {

					bloco++;
					creditosUtilizadosDisciplina[dId]++;
					if (!finding && ((bloco % 2) == 1 && disciplinaAleatoria->getCreditos() > creditosUtilizadosDisciplina[dId])) {
						inserted = solucaoLocal->horario->insert(dia, bloco, i, disciplinaXprofessorDisciplina[dId]);
						if (inserted) {
							bloco++;
							creditosUtilizadosDisciplina[dId]++;
						}
					}
				} else {
					colisaoProfessor[dId]++;

					if (colisaoProfessor[dId] == horarioProfessorColisaoMax) {

						if (bloco % 2 == 1) {
							bloco += 1;
						} else {
							bloco += 2;
						}

						colisaoProfessor[dId] = 0;
					}
				}

				if (bloco >= blocosTamanho) {
					bloco = 0;
					dia++;
				}

				//std::cout << "\n" << disciplinaAleatoria->getCreditos() << " == " << creditosUtilizadosDisciplina[dId] << std::endl;
				if (disciplinaAleatoria->getCreditos() == creditosUtilizadosDisciplina[dId]) {

					disciplinas.erase(find_if(disciplinas.begin()
					                          , disciplinas.end()
					                          , DisciplinaFindDisciplina(disciplinaAleatoria)));
				}
			}

			i++;
		}

		gerarGrade(solucaoLocal);
		//gerarGradeTipoGrasp(solucaoLocal, false);
		//solucoesAG.push_back(solucaoLocal);
		Util::insert_sorted(solucoesAG, solucaoLocal, SolucaoComparaMaior());
	}

	return solucoesAG;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGTorneioPar(std::vector<Solucao*> solucoesPopulacao)
{
	auto pai1 = gerarHorarioAGTorneio2(solucoesPopulacao);
	auto pai2 = gerarHorarioAGTorneio2(solucoesPopulacao);

	return {pai1, pai2};
}

Solucao* Resolucao::gerarHorarioAGTorneio(std::vector<Solucao*> solucoesPopulacao) const
{
	std::vector<Solucao*> torneioCandidatos;
	Solucao* vencedor = nullptr;
	double vencedorFO = 0;
	double randomFO;
	int randInt;

	auto populacaoTorneioMax = size_t(horarioTorneioPopulacao) * solucoesPopulacao.size();

	while (torneioCandidatos.size() <= populacaoTorneioMax && solucoesPopulacao.size() != 0) {
		randInt = aleatorio::randomInt() % solucoesPopulacao.size();

		randomFO = solucoesPopulacao[randInt]->getObjectiveFunction();
		if (vencedorFO < randomFO) {
			vencedor = solucoesPopulacao[randInt];
			vencedorFO = randomFO;
		}

		// Remove elemento sorteado
		solucoesPopulacao.erase(remove(solucoesPopulacao.begin(), solucoesPopulacao.end(), solucoesPopulacao[randInt]), solucoesPopulacao.end());
	}

	return vencedor;
}

Solucao* Resolucao::gerarHorarioAGTorneio2(std::vector<Solucao*> solucoesPopulacao)
{
	auto primeiro = aleatorio::randomInt() % solucoesPopulacao.size();
	auto foPrimeiro = solucoesPopulacao[primeiro]->getObjectiveFunction();

	auto segundo = aleatorio::randomInt() % solucoesPopulacao.size();
	auto foSegundo = solucoesPopulacao[segundo]->getObjectiveFunction();

	if (foPrimeiro > foSegundo) {
		return solucoesPopulacao[primeiro];
	} else {
		return solucoesPopulacao[segundo];
	}
}

// Construtivo-reparo é o operador inicial, que tenta inserir uma disciplina
// de cada vez e reparar a solução se uma restrição for violada
std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoConstrutivoReparo(Solucao* solucaoPai1, Solucao* solucaoPai2)
{
	std::vector<Solucao*> filhos;
	std::vector<ProfessorDisciplina*> matrizBackup;

	auto camadasMax = int(horarioCruzamentoCamadas * camadasTamanho);
	int numFilhos = 0;

	int camadaPeriodo, diaSemana, blocoHorario;
	int posicao, posicaoX;
	bool success;

	ProfessorDisciplina* e;
	ProfessorDisciplina* g;

	Solucao* filho;

	do {

		filho = new Solucao(*solucaoPai1);

		for (int i = 0; i < camadasMax; i++) {

			camadaPeriodo = aleatorio::randomInt() % camadasTamanho;

			for (int j = 0, tentativas = 0; j < horarioCruzamentoDias;) {
				success = false;

				// De segunda a s�bado
				diaSemana = aleatorio::randomInt() % 6;
				blocoHorario = aleatorio::randomInt() % blocosTamanho;

				posicao = filho->horario->getPosition(diaSemana, blocoHorario, camadaPeriodo);

				e = solucaoPai2->horario->at(posicao);
				g = filho->horario->at(posicao);

				// Backup
				matrizBackup = filho->horario->matriz;

				if (e == g) {
					// nada a fazer
					success = true;
				} else if (e == NULL) {
					// Remove G
					posicaoX = filho->horario->getPosition(diaSemana, blocoHorario, camadaPeriodo);
					filho->horario->matriz[posicaoX] = NULL;

					success = gerarHorarioAGCruzamentoAleatorioReparo(filho, diaSemana, blocoHorario, camadaPeriodo);
					if (!success) {
						filho->horario->matriz = matrizBackup;
					}
				} else if (g == NULL) {
					// Recupera primeira ocorrencia da disciplina E
					posicaoX = filho->horario->getFirstDisciplina(e->disciplina);
					// Se o professor da primeira ocorrencia for o mesmo professor da disciplina E
					if (filho->horario->matriz[posicaoX]->professor == e->professor) {
						// Remove primeira ocorrencia de E
						filho->horario->matriz[posicaoX] = NULL;

						success = filho->horario->insert(diaSemana, blocoHorario, camadaPeriodo, e);
						if (!success) {
							filho->horario->matriz = matrizBackup;
						}
					}
				} else {
					// Recupera primeira ocorrencia da disciplina E
					posicaoX = filho->horario->getFirstDisciplina(e->disciplina);
					// Se o professor da primeira ocorrencia for o mesmo professor da disciplina E
					if (filho->horario->matriz[posicaoX]->professor == e->professor) {

						// Remove primeira ocorrencia de E
						filho->horario->matriz[posicaoX] = NULL;

						// insere E
						success = filho->horario->insert(diaSemana, blocoHorario, camadaPeriodo, e);
						if (!success) {
							success = gerarHorarioAGCruzamentoAleatorioReparo(filho, diaSemana, blocoHorario, camadaPeriodo);
							if (!success) {
								filho->horario->matriz = matrizBackup;
							}
						}
					}
				}

				if (success || tentativas >= horarioCruzamentoTentativasMax) {
					j++;
					tentativas = 0;
				}
			}
		}
		gerarGradeTipoGraspClear(filho);
		filhos.push_back(filho);
		numFilhos++;
	} while (numFilhos <= horarioCruzamentoFilhos);


	return filhos;
}

int Resolucao::cruzaCamada(Solucao*& filho, const Solucao* pai, int camada) const
{
	// Se o cruzamento falhar, filho continua o mesmo de antes
	auto fallback = new Solucao(*filho);
	filho->fo = -1;
	filho->horario->hash_ = 0;
	// Apaga camada do filho
	for (auto i = 0; i < SEMANA; i++) {
		for (auto j = 0; j < blocosTamanho; j++) {
			auto pos = filho->horario->getPosition(i, j, camada);
			filho->horario->alocados[pos] = "";
			filho->horario->matriz[pos] = nullptr;
		}
	}

	// Tenta inserir as disciplinas da camada do pai. Se alguma falhar, volta 
	// ao fallback e encerra
	for (auto i = 0; i < SEMANA; i++) {
		for (auto j = 0; j < blocosTamanho; j++) {
			auto pos = filho->horario->getPosition(i, j, camada);
			auto alocPai = pai->horario->matriz[pos];
			if (!alocPai || filho->horario->insert(i, j, camada, alocPai)) {
				continue;
			}

			//puts("Falha");

			if (verbose) {
				std::cout << "Problemas no cruzamento tipo simples\n";
			}

			delete filho;
			filho = fallback;
			return 0;
		}
	}
	//puts("Sucesso");
	delete fallback;
	return 1;
}

// Simples é o novo, que simplesmente tenta substituir todos os perídos 
// do filho pelos do pai a partir de certo ponto de corte. Se durante uma
// operação em uma dessas camadas for violada alguma restrição, aquela camada
// retorna ao original, e a próxima será tentada
std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoSimples(Solucao* solucaoPai1, Solucao* solucaoPai2)
{
	auto camadaCruz = aleatorio::randomInt() % camadasTamanho;
	auto filho1 = new Solucao(*solucaoPai1);
	auto filho2 = new Solucao(*solucaoPai2);
	std::vector<Solucao*> solucoes;
	auto success1 = 0;
	auto success2 = 0;

	success1 += cruzaCamada(filho1, solucaoPai2, camadaCruz);
	success2 += cruzaCamada(filho2, solucaoPai1, camadaCruz);

	if (success1 > 0) {
		gerarGradeTipoGraspClear(filho1);
		solucoes.push_back(filho1);
	} else {
		delete filho1;
	}
	if (success2 > 0) {
		gerarGradeTipoGraspClear(filho2);
		solucoes.push_back(filho2);
	} else {
		delete filho2;
	}

	return solucoes;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGCruzamentoSubstBloco(Solucao* solucaoPai1, Solucao* solucaoPai2)
{
	int dia, bloco, camada;
	std::size_t pos;
	auto filho1 = new Solucao(*solucaoPai1);
	auto filho2 = new Solucao(*solucaoPai2);
	auto& mat1 = filho1->horario->matriz;
	auto& mat2 = filho2->horario->matriz;
	auto fallback1 = new Solucao(*filho1);
	auto fallback2 = new Solucao(*filho2);

	// Escolhe posição de cruzamento. Se estiver no meio um bloco tenta de novo
	do {
		pos = aleatorio::randomInt() % mat1.size();
		int coord[3];
		filho1->horario->get3DMatrix(pos, coord);
		dia = coord[1];
		bloco = coord[0];
		camada = coord[2];
	} while (bloco % 2 == 1);

	struct Gene
	{
		ProfessorDisciplina* disc; // professorDisciplina da célula escolhida
		int                  posGene; // posição original da célula no pai
		int                  posCruz; // posição final da célula no filho
		bool                 disponivel; // registra se a célula já encontrou um compatível

		Gene(ProfessorDisciplina* p, int g, int c, bool d)
			: disc(p), posGene(g), posCruz(c), disponivel(d) {}
	};

	// Insere as disciplinas após o ponto de corte na camada original
	// Modifica então para nulo, para poder inserir as novas alocações no slot
	auto lista1 = std::vector<Gene>{};
	auto lista2 = std::vector<Gene>{};
	for (auto d = dia; d < SEMANA; d++) {
		for (auto h = d == dia ? bloco : 0; h < blocosTamanho; h++) {
			auto i = filho1->horario->getPosition(d, h, camada);
			if (mat1[i]) {
				lista1.emplace_back(mat1[i], i, -1, true);
				mat1[i] = nullptr;
			}
			if (mat2[i]) {
				lista2.emplace_back(mat2[i], i, -1, true);
				mat2[i] = nullptr;
			}
		}
	}
	
	// Procura o primeiro gene compatível com o atual na lista 2, e os marca
	// para troca
	for (auto& gene : lista1) {
		auto it = std::find_if(begin(lista2), end(lista2), [&](Gene& g) {
			return g.disc == gene.disc;
		});
		if (it == end(lista2)) {
			continue;
		}

		gene.posCruz = it->posGene;
		it->posCruz = gene.posGene;
		gene.disponivel = false;
		it->disponivel = false;
	}

	// Separa uma lista em duas: blocos (disciplinas que estão alocadas em blocos)
	// e isoladas (o resto)
	auto sepBlocos = [this](std::vector<Gene>& lista, const std::vector<ProfessorDisciplina*>& mat) {
		auto blocos = std::vector<Gene>{};
		auto isoladas = std::vector<Gene>{};
		for (auto i = 0u; i < lista.size(); i++) {
			auto p = lista[i].posGene;
			if (p < blocosTamanho - 1 && mat[p] == mat[p+1]) {
				blocos.emplace_back(std::move(lista[i]));
			} else {
				isoladas.emplace_back(std::move(lista[i]));
			}
		}

		return std::make_pair(blocos, isoladas);
	};

	// Separa as listas 1 e 2
	auto ret1 = sepBlocos(lista1, mat1);
	auto blocos1 = std::move(ret1.first);
	auto isoladas1 = std::move(ret1.second);
	auto ret2 = sepBlocos(lista2, mat2);
	auto blocos2 = std::move(ret2.first);
	auto isoladas2 = std::move(ret2.second);

	// Separa uma lista entre compatíveis (aqueles que acharam um equivalente)
	// e não compatíveis
	auto sepCompat = [](std::vector<Gene>& lista) {
		auto compat = std::vector<Gene>{};
		auto incompat = std::vector<Gene>{};

		for (auto&& gene : lista) {
			if (gene.disponivel) {
				incompat.emplace_back(std::move(gene));
			} else {
				compat.emplace_back(std::move(gene));
			}
		}

		return std::make_pair(compat, incompat);
	};

	// Separa as isoladas e as blocos da 1 e da 2
	auto retBloco1 = sepCompat(blocos1);
	auto blocos1Comp = retBloco1.first;
	auto blocos1Incomp = retBloco1.second;
	auto retBloco2 = sepCompat(blocos2);
	auto blocos2Comp = retBloco2.first;
	auto blocos2Incomp = retBloco2.second;

	auto retIso1 = sepCompat(isoladas1);
	auto iso1Comp = retIso1.first;
	auto iso1Incomp = retIso1.second;
	auto retIso2 = sepCompat(isoladas2);
	auto iso2Comp = retIso2.first;
	auto iso2Incomp = retIso2.second;

	// Tenta inserir um bloco no horário de acordo com a posição determinada anteriormente
	auto insertBlocoComp = [](Horario* hor, std::vector<Gene>& blocos) {
		for (auto& b : blocos) {
			int coord[3];
			hor->get3DMatrix(b.posCruz, coord);
			if (!hor->insert(coord[1], coord[0], coord[2], b.disc)
				|| !hor->insert(coord[1], coord[0]+1, coord[2], b.disc)) {
				return false;
			}
		}
		return true;
	};

	// Tenta inserir uma isolada no horário de acordo com a posição determinada anteriormente
	auto insertIsoComp = [](Horario* hor, std::vector<Gene>& blocos) {
		for (auto& b : blocos) {
			if (!b.disc) {
				continue;
			}
			int coord[3];
			hor->get3DMatrix(b.posCruz, coord);
			if (!hor->insert(coord[0], coord[1], coord[2], b.disc)) {
				return false;
			}
		}
		return true;
	};

	// Para blocos incompatíveis, tenta-se achar um lugar no horário que caibam os dois juntos
	// Se não for encontrado, são adicionados na lista de isolados
	auto insertBlocoIncom = [&](Horario *hor, std::vector<Gene>& blocos, std::vector<Gene>& iso) {
		for (auto& b : blocos) {
			// Tenta inserir o bloco na matriz
			if (![&](Gene& g) {
				for (auto d = dia; d < SEMANA; d++) {
					for (auto h = d == dia ? bloco : 0; h < blocosTamanho; h++) {
						if (h < blocosTamanho - 1 && !hor->matriz[h] && !hor->matriz[h + 1] &&
							hor->insert(d, h, camada, g.disc)) {
							return true;
						}
					}
				}
				return false;
			// Não conseguindo, insere nas isoladas (duas instancias)
			}(b)) {
				iso.push_back(b);
				iso.emplace_back(std::move(b));
			}
		}
		return true;
	};

	// Insere uma isolada incompatível, procurando uma posição vazia na grade
	auto insertIsoIncom = [&](Horario *hor, std::vector<Gene>& blocos) {
		for (auto& b : blocos) {
			if (![&](Gene& g) {
				for (auto d = dia; d < SEMANA; d++) {
					for (auto h = d == dia ? bloco : 0; h < blocosTamanho; h++) {
						if (!hor->matriz[h] && hor->insert(d, h, camada, g.disc)) {
							return true;
						}
					}
				}
				return false;
			}(b)) {
				return false;
			}
		}
		return true;
	};

	// Verifica se conseguiu terminar o cruzamento. Se não, volta para o fallback
	if (insertBlocoComp(filho1->horario, blocos2Comp) && 
		insertBlocoIncom(filho1->horario, blocos2Incomp, iso2Incomp) &&
		insertIsoComp(filho1->horario, iso2Comp) &&
		insertIsoIncom(filho1->horario, iso2Incomp)) {
		delete fallback1;
	} else {
		delete filho1;
		filho1 = fallback1;
	}

	if (insertBlocoComp(filho2->horario, blocos1Comp) &&
		insertBlocoIncom(filho2->horario, blocos1Incomp, iso2Incomp) &&
		insertIsoComp(filho2->horario, iso1Comp) &&
		insertIsoIncom(filho2->horario, iso1Incomp)) {
		delete fallback2;
	} else {
		delete filho2;
		filho2 = fallback2;
	}

	gerarGradeTipoGraspClear(filho1);
	gerarGradeTipoGraspClear(filho2);
	return {filho1, filho2};
}


bool Resolucao::gerarHorarioAGCruzamentoAleatorioReparoBloco(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG)
{
	bool success = false;
	ProfessorDisciplina* g = solucaoFilho->horario->at(diaG, blocoG, camadaG);

	for (int blocoReparo = 0; blocoReparo < blocosTamanho; blocoReparo++) {
		if (solucaoFilho->horario->at(diaG, blocoReparo, camadaG) == NULL) {
			success = solucaoFilho->horario->insert(diaG, blocoReparo, camadaG, g);
			if (success) {
				return true;
			}
		}
	}

	return false;
}

bool Resolucao::gerarHorarioAGCruzamentoAleatorioReparo(Solucao*& solucaoFilho, int diaG, int blocoG, int camadaG)
{
	if (gerarHorarioAGCruzamentoAleatorioReparoBloco(solucaoFilho, diaG, blocoG, camadaG)) {
		return true;
	}

	for (int diaReparo = 0; diaReparo < 6; diaReparo++) {
		if (diaReparo == diaG) {
			continue;
		}

		if (gerarHorarioAGCruzamentoAleatorioReparoBloco(solucaoFilho, diaReparo, blocoG, camadaG)) {
			return true;
		}
	}

	return false;
}

void Resolucao::gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& populacao)
{
	gerarHorarioAGSobrevivenciaElitismo(populacao, horarioPopulacaoInicial);
}

void Resolucao::gerarHorarioAGSobrevivenciaElitismo(std::vector<Solucao*>& populacao, int populacaoMax) const
{
	for (size_t i = populacaoMax; i < populacao.size(); i++) {
		delete populacao[i];
	}

	populacao.resize(populacaoMax);
}

std::vector<Solucao*> Resolucao::gerarHorarioAGMutacao(std::vector<Solucao*>& populacao)
{
	std::vector<Solucao*> genesX;
	Solucao* solucaoTemp;

	double porcentagem;

	// Muta��o dos populacao
	for (auto j = 0u; j < populacao.size(); j++) {
		porcentagem = ((aleatorio::randomInt() % 100) / 100);

		if (porcentagem <= horarioMutacaoProbabilidade) {
			solucaoTemp = gerarHorarioAGMutacao(populacao[j]);

			if (solucaoTemp) {
				gerarGradeTipoGraspClear(solucaoTemp);
				genesX.push_back(solucaoTemp);
			}
		}
	}

	return genesX;
}

std::vector<Solucao*> Resolucao::gerarHorarioAGMutacaoExper(std::vector<Solucao*>& populacao, Configuracao::TipoMutacao tipoMut)
{
	std::vector<Solucao*> genesX;
	Solucao* solucaoTemp = nullptr;

	double porcentagem;

	// Muta��o dos populacao
	for (auto j = 0u; j < populacao.size(); j++) {
		porcentagem = ((aleatorio::randomInt() % 100) / 100);

		if (porcentagem <= horarioMutacaoProbabilidade) {
			switch (tipoMut) {
			case Configuracao::TipoMutacao::substitui_professor:
				solucaoTemp = gerarHorarioAGMutacaoSubstProf(populacao[j]);
				break;
			case Configuracao::TipoMutacao::substiui_disciplina:
				solucaoTemp = gerarHorarioAGMutacaoSubstDisc(populacao[j]);
				break;
			}

			if (solucaoTemp) {
				auto idx = 0;
				switch (tipoMut) {
				case Configuracao::TipoMutacao::substiui_disciplina:
					idx = 0; break;
				case Configuracao::TipoMutacao::substitui_professor:
					idx = 1; break;
				}
				if (solucaoTemp->getObjectiveFunction() 
	                   > populacao[j]->getObjectiveFunction()) {
					contadorMelhoresMut[idx]++;
				} else if (solucaoTemp->getObjectiveFunction()
						      == populacao[j]->getObjectiveFunction()) {
					contadorIguaisMut[idx]++;
				}
				gerarGradeTipoGraspClear(solucaoTemp);
				genesX.push_back(solucaoTemp);
			}
		}
	}

	return genesX;
}

Solucao* Resolucao::gerarHorarioAGMutacaoSubstDisc(Solucao* pSolucao)
{
	Solucao* currentSolucao = new Solucao(*pSolucao);
	bool success = false;

	int camadaX;
	int diaX1, diaX2;
	int blocoX1, blocoX2;
	int x1, x2;

	ProfessorDisciplina *pdX1, *pdX2;
	std::vector<ProfessorDisciplina*> backup;

	for (int i = 0; i < horarioMutacaoTentativas; i++) {
		camadaX = aleatorio::randomInt() % camadasTamanho;

		diaX1 = aleatorio::randomInt() % (SEMANA);
		diaX2 = aleatorio::randomInt() % (SEMANA);

		blocoX1 = aleatorio::randomInt() % blocosTamanho;
		blocoX2 = aleatorio::randomInt() % blocosTamanho;

		x1 = currentSolucao->horario->getPosition(diaX1, blocoX1, camadaX);
		x2 = currentSolucao->horario->getPosition(diaX2, blocoX2, camadaX);

		backup = currentSolucao->horario->matriz;

		pdX1 = currentSolucao->horario->at(x1);
		pdX2 = currentSolucao->horario->at(x2);

		if (pdX1 == pdX2) {
			// Nada a fazer
		} else if (pdX1 == NULL) {
			if (currentSolucao->horario->insert(diaX1, blocoX1, camadaX, pdX2)) {
				currentSolucao->horario->matriz[x2] = NULL;
				success = true;
			}
		} else if (pdX2 == NULL) {
			if (currentSolucao->horario->insert(diaX2, blocoX2, camadaX, pdX1)) {
				currentSolucao->horario->matriz[x1] = NULL;
				success = true;
			}
		} else {
			currentSolucao->horario->matriz[x1] = NULL;
			currentSolucao->horario->matriz[x2] = NULL;

			if (currentSolucao->horario->insert(diaX1, blocoX1, camadaX, pdX2)
				&& currentSolucao->horario->insert(diaX2, blocoX2, camadaX, pdX1)) {
				success = true;
			}
		}
	}

	if (success) {
		return currentSolucao;
	} else {
		delete currentSolucao;
		return nullptr;
	}
}

Solucao* Resolucao::gerarHorarioAGMutacao(Solucao* pSolucao)
{
	switch (horarioTipoMutacao) {
	case Configuracao::TipoMutacao::substiui_disciplina:
		return gerarHorarioAGMutacaoSubstDisc(pSolucao);
	case Configuracao::TipoMutacao::substitui_professor:
		return gerarHorarioAGMutacaoSubstProf(pSolucao);
	}
	return nullptr;
}

double Resolucao::gerarGradeTipoGrasp2(Solucao* solucao)
{
	auto total = 0.0;
	std::unordered_map<long long, Grade*> gradesGeradas;

	for (auto& par : alunoPerfis) {
		auto aluno = par.second;
		Grade* novaGrade = nullptr;
		if (gradesGeradas[aluno->getHash()]) {
			novaGrade = new Grade(*gradesGeradas[aluno->getHash()]);
			novaGrade->alunoPerfil = aluno;
		} else {
			novaGrade = GRASP(aluno, solucao);
			gradesGeradas[aluno->getHash()] = novaGrade;
		}

		total += novaGrade->fo;
		solucao->insertGrade(novaGrade);
	}
	solucao->fo = total;

	return total;
}

double Resolucao::gerarGrade()
{
	return gerarGrade(solucao);
}

double Resolucao::gerarGrade(Solucao*& pSolucao)
{
	switch (gradeTipoConstrucao) {
	case Configuracao::TipoGrade::guloso:
		return gerarGradeTipoGuloso(pSolucao);

	case Configuracao::TipoGrade::grasp:
		//return gerarGradeTipoGrasp(pSolucao);
		return gerarGradeTipoGrasp2(pSolucao);

	case Configuracao::TipoGrade::combinatorio:
		return gerarGradeTipoCombinacaoConstrutiva(pSolucao);

	case Configuracao::TipoGrade::modelo:
		return gerarGradeTipoModelo(pSolucao);
	}

	return 0;
}

double Resolucao::gerarGradeTipoGuloso(Solucao*& pSolucao)
{
	Horario* horario;
	Grade* apGrade;

	std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
	std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();

	horario = pSolucao->horario;

	for (; apIter != apIterEnd; ++apIter) {
		if (verbose)
			std::cout << apIter->first << std::endl;
		auto alunoPerfil = alunoPerfis[apIter->first];

		apGrade = new Grade(blocosTamanho, alunoPerfil, horario, disciplinas, disciplinasIndex);

		auto apRestante = alunoPerfil->restante;
		auto apCursadas = alunoPerfil->cursadas;

		auto dIter = apRestante.begin();
		auto dIterEnd = apRestante.end();

		for (; dIter != dIterEnd; ++dIter) {
			apGrade->insert(getDisciplinaByName(*dIter));
		}

		pSolucao->insertGrade(apGrade);
	}

	return pSolucao->getObjectiveFunction();
}

Grade* Resolucao::gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, const std::unordered_set<std::string>& disciplinasRestantes, 
													  int maxDeep, int deep, std::unordered_set<std::string>::const_iterator current)
{
	Grade* bestGrade = new Grade(*pGrade);
	Grade* currentGrade;

	double bestFO, currentFO;

	bool viavel;

	for (auto it = current; it != std::end(disciplinasRestantes); ++it) {
		currentGrade = new Grade(*pGrade);

		if (verbose)
			std::cout << "Nivel: " << (deep) << " Disciplina: " << *it << ")\n";

		viavel = currentGrade->insert(getDisciplinaByName(*it));
		if (viavel) {

			if (deep != maxDeep) {
				currentGrade = gerarGradeTipoCombinacaoConstrutiva(currentGrade, disciplinasRestantes, maxDeep, deep + 1, ++it);
			}

			bestFO = bestGrade->getObjectiveFunction();
			currentFO = currentGrade->getObjectiveFunction();
			if (bestFO < currentFO) {
				bestGrade = currentGrade;
			}

			if (deep == 0) {
				//std::cout << "----------------------------" << std::endl;
			}
		} else {
			if (verbose)
				std::cout << "[inviavel]" << std::endl;
		}
	}
	if (deep == 0) {
		//std::cout << "############################" << std::endl;
	}

	return bestGrade;
}

Grade* Resolucao::gerarGradeTipoCombinacaoConstrutiva(Grade* pGrade, const std::unordered_set<std::string>& disciplinasRestantes, int maxDeep)
{
	return gerarGradeTipoCombinacaoConstrutiva(pGrade, disciplinasRestantes, maxDeep, 0, std::begin(disciplinasRestantes));
}

double Resolucao::gerarGradeTipoCombinacaoConstrutiva(Solucao*& pSolucao)
{
	Horario* horario;
	Grade* apGrade;

	std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
	std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();

	horario = pSolucao->horario;

	for (; apIter != apIterEnd; ++apIter) {
		if (verbose)
			std::cout << "[" << apIter->first << "]" << std::endl;
		auto alunoPerfil = alunoPerfis[apIter->first];

		auto apRestante = alunoPerfil->restante;

		apGrade = gerarGradeTipoCombinacaoConstrutiva(new Grade(blocosTamanho, alunoPerfil, horario,
		                                                        disciplinas, disciplinasIndex), apRestante, apRestante.size());

		pSolucao->insertGrade(apGrade);

		const auto& escolhidas = apGrade->disciplinasAdicionadas;

		for (const auto disc : escolhidas) {
			disc->alocados++;
			if (disc->alocados >= disc->capacidade) {
				disc->ofertada = false;
			}
		}

	}

	return pSolucao->getObjectiveFunction();
}

void Resolucao::gerarGradeTipoGraspConstrucao(Grade* pGrade)
{
	std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

	gerarGradeTipoGraspConstrucao(pGrade, professorDisciplinasIgnorar);
}

void Resolucao::gerarGradeTipoGraspConstrucao(Grade* pGrade, std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar)
{
	auto disponivel = (SEMANA - 2) * camadasTamanho;
	auto alunoPerfil = pGrade->alunoPerfil;
	std::multiset<Disciplina*, DisciplinaCargaHorariaDesc> apRestante;
	//std::transform(begin(alunoPerfil->restante), end(alunoPerfil->restante), 
	//			   std::inserter(apRestante, begin(apRestante)),
	//			   [this](const std::string& nomedisc) {
	//	return getDisciplinaByName(nomedisc);
	//});
	for (const auto& nomedisc : alunoPerfil->restante) {
		auto d = getDisciplinaByName(nomedisc);
		apRestante.insert(d);
	}
	//auto apRestante = alunoPerfil->restante;
	auto dIterStart = apRestante.begin();

	auto adicionados = 0;
	while (apRestante.size() != 0 && dIterStart != apRestante.end() && disponivel != adicionados) {

		auto distancia = getIntervaloAlfaGrasp(apRestante);
		auto rand = Util::randomBetween(0, distancia);
		auto current = apRestante.begin();
		std::advance(current, rand);
		auto disciplina = *current;

		if (pGrade->insert(disciplina, professorDisciplinasIgnorar)) {

			adicionados++;
		}
		apRestante.erase(current);

		dIterStart = apRestante.begin();
	}
}

void Resolucao::gerarGradeTipoGraspConstrucao(Solucao* pSolucao)
{
	Horario* horario;
	Grade* apGrade;

	std::map<std::string, AlunoPerfil*>::iterator apIter = alunoPerfis.begin();
	std::map<std::string, AlunoPerfil*>::iterator apIterEnd = alunoPerfis.end();
	AlunoPerfil* alunoPerfil;

	horario = pSolucao->horario;

	for (; apIter != apIterEnd; ++apIter) {

		if (verbose)
			std::cout << apIter->first << std::endl;
		alunoPerfil = alunoPerfis[apIter->first];

		apGrade = new Grade(blocosTamanho, alunoPerfil, horario, disciplinas, disciplinasIndex);

		gerarGradeTipoGraspConstrucao(apGrade);

		pSolucao->insertGrade(apGrade);
	}
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamentoAleatorio(Solucao* pSolucao)
{
	Solucao* bestSolucao = new Solucao(*pSolucao);
	Solucao* currentSolucao;

	std::map<std::string, AlunoPerfil*>::iterator apIter;
	std::map<std::string, AlunoPerfil*>::iterator apIterEnd;
	AlunoPerfil* alunoPerfil;

	Grade* grade;

	std::vector<ProfessorDisciplina*> professorDisciplinasIgnorar;

	int random;
	int disciplinasSize;
	int disciplinasRemoveMax;
	int disciplinasRemoveRand;
	double bestFO, currentFO;

	bestFO = bestSolucao->getObjectiveFunction();

	for (int i = 0; i < gradeGraspVizinhos; i++) {
		currentSolucao = new Solucao(*pSolucao);

		apIter = alunoPerfis.begin();
		apIterEnd = alunoPerfis.end();

		if (verbose)
			std::cout << "------NGH" << i << std::endl;

		for (; apIter != apIterEnd; ++apIter) {
			alunoPerfil = alunoPerfis[apIter->first];

			grade = currentSolucao->grades[alunoPerfil->id];

			disciplinasSize = grade->disciplinasAdicionadas.size();
			disciplinasRemoveMax = int(ceil(disciplinasSize * 1));
			disciplinasRemoveRand = Util::randomBetween(1, disciplinasRemoveMax);

			for (int j = 0; j < disciplinasRemoveRand; j++) {
				ProfessorDisciplina* professorDisciplinaRemovido = NULL;

				disciplinasSize = grade->disciplinasAdicionadas.size();
				random = Util::randomBetween(0, disciplinasSize);
				grade->remove(grade->disciplinasAdicionadas[random], professorDisciplinaRemovido);

				// Se houve uma remo��o
				if (professorDisciplinaRemovido != NULL) {
					professorDisciplinasIgnorar.push_back(professorDisciplinaRemovido);
				}
			}

			gerarGradeTipoGraspConstrucao(grade, professorDisciplinasIgnorar);
		}
		currentFO = currentSolucao->getObjectiveFunction();
		if (verbose)
			std::cout << std::endl << "------NGH" << i << ": L(" << bestFO << ") < C(" << currentFO << ")" << std::endl;

		if (bestFO < currentFO) {
			delete bestSolucao;
			bestSolucao = currentSolucao;
			bestFO = currentFO;

			if (verbose)
				std::cout << "------NGH new best: " << bestFO << std::endl;
			i = 0;
		} else {
			delete currentSolucao;
		}
	}

	return bestSolucao;
}

Solucao* Resolucao::gerarGradeTipoGraspRefinamentoCrescente(Solucao* pSolucao)
{
	Solucao* bestSolucao = new Solucao(*pSolucao);
	Solucao* currentSolucao;

	std::map<std::string, AlunoPerfil*>::iterator apIter;
	std::map<std::string, AlunoPerfil*>::iterator apIterEnd;
	AlunoPerfil* alunoPerfil;

	Disciplina* disciplinaRemovida;
	std::vector<Disciplina*> disciplinasRemovidas;
	std::vector<Disciplina*>::iterator drIter;
	std::vector<Disciplina*>::iterator drIterEnd;

	Grade *bestGrade, *currentGrade;

	int random;

	double bestFO, currentFO;

	apIter = alunoPerfis.begin();
	apIterEnd = alunoPerfis.end();

	for (; apIter != apIterEnd; ++apIter) {

		alunoPerfil = alunoPerfis[apIter->first];
		bestGrade = bestSolucao->grades[alunoPerfil->id];

		for (int i = 0; i < gradeGraspVizinhos; i++) {
			currentSolucao = new Solucao(*pSolucao);
			currentGrade = currentSolucao->grades[alunoPerfil->id];

			if (verbose)
				std::cout << std::endl << "------NGH" << i << std::endl;

			disciplinasRemovidas.clear();
			auto disciplinasRestantes = alunoPerfil->restante;

			for (int j = 0; j < (i + 1); j++) {
				random = Util::randomBetween(0, currentGrade->disciplinasAdicionadas.size());
				if (random == -1) {
					break;
				}
				disciplinaRemovida = currentGrade->remove(currentGrade->disciplinasAdicionadas[random]);
				if (disciplinaRemovida) {
					disciplinasRemovidas.push_back(disciplinaRemovida);
				}
			}

			RemoveDisciplinasNome(disciplinasRestantes, disciplinasRemovidas);
			RemoveDisciplinasNome(disciplinasRestantes, currentGrade->disciplinasAdicionadas);
			gerarGradeTipoCombinacaoConstrutiva(currentGrade, disciplinasRestantes, i);

			bestFO = bestGrade->getObjectiveFunction();
			currentFO = currentGrade->getObjectiveFunction();
			if (verbose)
				std::cout << "------NGH" << i << ": L(" << bestFO << ") < C(" << currentFO << ")" << std::endl;
			if (bestFO < currentFO) {
				delete bestSolucao;
				bestSolucao = currentSolucao;
				bestFO = currentFO;

				if (verbose)
					std::cout << "------NGH new best: " << bestFO << std::endl;
				i = 0;
			} else {
				delete currentSolucao;
			}
		}
	}

	return bestSolucao;
}

double Resolucao::gerarGradeTipoGrasp()
{
	return gerarGradeTipoGrasp(solucao);
}

double Resolucao::gerarGradeTipoGrasp(Solucao*& pSolucao)
{
	Solucao* currentSolucao;
	Solucao* temp;

	double bestFO, currentFO;

	clock_t t0;
	double diff = 0;

	//    int const RESOLUCAO_GRASP_TEMPO_CONSTRUCAO = ceil(gradeGraspTempoConstrucao * alunoPerfis.size());
	double const RESOLUCAO_GRASP_TEMPO_CONSTRUCAO = gradeGraspTempoConstrucao;
	//printf("c: %f\n", RESOLUCAO_GRASP_TEMPO_CONSTRUCAO);

	bestFO = 0;

	if (verbose)
		std::cout << "HORARIO (Solucao) :" << std::endl;

	int iteracoes = 0;
	while (diff <= RESOLUCAO_GRASP_TEMPO_CONSTRUCAO) {
		currentSolucao = new Solucao(*pSolucao);

		t0 = clock();
		gerarGradeTipoGraspConstrucao(currentSolucao);

		if (verbose)
			std::cout << "----FIT: " << currentSolucao->getObjectiveFunction() << std::endl;

		temp = currentSolucao;

		switch (gradeGraspVizinhanca) {
		case Configuracao::TipoVizinhos::aleatorios:
			currentSolucao = gerarGradeTipoGraspRefinamentoAleatorio(currentSolucao);
			break;
		case Configuracao::TipoVizinhos::crescente:
			currentSolucao = gerarGradeTipoGraspRefinamentoCrescente(currentSolucao);
			break;
		}
		if (temp->id != currentSolucao->id)
			delete temp;

		if (verbose)
			std::cout << "----FIT(NGH):" << currentSolucao->getObjectiveFunction() << std::endl;

		diff += Util::timeDiff(clock(), t0);

		currentFO = currentSolucao->getObjectiveFunction();
		if (bestFO < currentFO) {
			delete pSolucao;
			pSolucao = currentSolucao;
			bestFO = currentFO;
			diff = 0;

			if (verbose)
				std::cout << "----NGH is the new best (gerarGradeTipoGrasp)" << std::endl;
		} else {
			delete currentSolucao;
		}
		if (verbose)
			std::cout << "-------------------------------------------------" << std::endl;

		iteracoes++;
	}
	//std::cout << iteracoes << std::endl;

	return pSolucao->getObjectiveFunction();
}

double Resolucao::gerarGradeTipoGraspClear(Solucao*& pSolucao)
{
	for (auto& par : pSolucao->grades) {
		delete par.second;
	}
	pSolucao->grades.clear();
	pSolucao->gradesLength = 0;

	return gerarGrade(pSolucao);
}

int Resolucao::getIntervaloAlfaGrasp(const std::multiset<Disciplina*, DisciplinaCargaHorariaDesc>& apRestante)
{
	auto distancia = 0;
	auto bestFIT = (*begin(apRestante))->cargaHoraria;
	auto worstFIT = (*rbegin(apRestante))->cargaHoraria;
	auto acceptFIT = bestFIT - int(ceil((1 - gradeAlfa) * (bestFIT - worstFIT)));

	for (const auto& disc : apRestante) {
		auto currentFIT = disc->cargaHoraria;
		if (currentFIT < acceptFIT) {
			break;
		}
		distancia++;
	}

	return distancia;
}

void Resolucao::showResult()
{
	showResult(solucao);
}

void Resolucao::showResult(Solucao* pSolucao)
{
	const auto& grades = pSolucao->grades;
	for (const auto& par : grades) {
		const auto gradeAtual = par.second;
		std::cout << gradeAtual->alunoPerfil->id << ":\n";
		const auto& discEscolhidas = gradeAtual->disciplinasAdicionadas;
		for (const auto disc : discEscolhidas) {

			std::cout << disc->nome << " ";
		}
		std::cout << " " << gradeAtual->getObjectiveFunction() << "\n";
	}
	std::cout << "\nFO da solucao: " << pSolucao->getObjectiveFunction() << std::endl;
}

Solucao* Resolucao::getSolucao()
{
	return solucao;
}

void Resolucao::teste()
{
	Solucao* sol = nullptr;
	do {
		sol = gerarSolucaoAleatoria();
	} while (!sol);
	printf("hash: %u\n", sol->getHash());
	auto n = 1;
	auto& now = std::chrono::steady_clock::now;
	// bench grasp
	auto tgrasp = 0ll;
	auto fograsp = 0.0;
	auto maxfograsp = 0.0;
	gradeTipoConstrucao = Configuracao::TipoGrade::grasp;
	for (auto i = 0; i < n; i++) {
		auto t1 = now();
		auto r = gerarGradeTipoGraspClear(sol);

		if (r > maxfograsp) maxfograsp = r;

		fograsp += r;
		auto t2 = now();
		auto t = Util::chronoDiff(t2, t1);
		tgrasp += t;
		std::cout << "Tempo: " << t << "\n";
		std::cout << "Fo: " << r << "\n\n";
	}
	std::cout << "Media tempo:" << tgrasp / (1. * n) << "\n";
	std::cout << "Media FO:   " << fograsp / n << "\n";
	std::cout << "Maior fo:   " << maxfograsp << "\n\n\n";

	auto savePath = Util::join_path({"teste", "fo" + std::to_string(fograsp / n) + "grasp"});
	Output::write(sol, savePath);

	// bench modelo
	gradeTipoConstrucao = Configuracao::TipoGrade::modelo;
	auto t1 = now();
	auto r = gerarGradeTipoGraspClear(sol);
	auto t2 = now();
	auto t = Util::chronoDiff(t2, t1);
	std::cout << "Tempo mod: " << t << "\n";
	std::cout << "FO modelo: " << r << "\n";
	
	reinsereGrades(sol);
	savePath = Util::join_path({"teste", "fo" + std::to_string(r) + "modelo"});
	Output::write(sol, savePath);
	//showResult(sol);
}

double Resolucao::gerarGradeTipoModelo(Solucao* pSolucao)
{
	auto horarioBin = converteHorario(pSolucao);
	auto total = 0.0;
	std::unordered_map<long long, Grade*> gradesGeradas;

	for (const auto& aluno : alunos) {
		auto currAluno = alunoPerfis[aluno.nome()];
		Grade* novaGrade = nullptr;
		if (gradesGeradas[currAluno->getHash()]) {
			novaGrade = new Grade(*gradesGeradas[currAluno->getHash()]);
			novaGrade->alunoPerfil = currAluno;
		} else {
			novaGrade = new Grade(blocosTamanho, currAluno, pSolucao->horario,
								  disciplinas, disciplinasIndex);
			fagoc::Modelo_solver solver(*curso, aluno, horarioBin, tempoLimiteModelo);
			solver.solve();
			const auto& solucao = solver.solucao();
			novaGrade->fo = solucao.funcao_objetivo;

			auto& adicionadas = novaGrade->disciplinasAdicionadas;
			for (const auto& disc : solucao.nomes_disciplinas) {
				adicionadas.push_back(disciplinas[disciplinasIndex[disc]]);
			}
			gradesGeradas[currAluno->getHash()] = novaGrade;
		}

		total += novaGrade->fo;
		pSolucao->insertGrade(novaGrade);
	}
	pSolucao->fo = total;

	return total;
}

std::vector<std::vector<char>> Resolucao::converteHorario(Solucao* pSolucao) const
{
	const auto& matriz = pSolucao->horario->matriz;
	auto numDisciplinas = disciplinas.size();
	auto numHorarios = SEMANA * blocosTamanho;
	std::vector<std::vector<char>> horarioBin(numHorarios, std::vector<char>(numDisciplinas, 0));
	int posicoes[3];

	for (auto i = 0u; i < matriz.size(); i++) {
		if (!matriz[i]) {
			continue;
		}
		auto index = curso->id_to_indice()[matriz[i]->disciplina->id];
		pSolucao->horario->get3DMatrix(i, posicoes);
		auto dia = posicoes[1];
		auto bloco = posicoes[0];
		auto posicao = (dia * blocosTamanho) + bloco;

		horarioBin[posicao][index] = 1;
	}

	return horarioBin;
}

void Resolucao::logExperimentos()
{
	char str[101];
	sprintf(str, "Tempo total selecao: %llu\n", tempoTotalSelec);
	log << str; puts(str);

	sprintf(str, "Tempo total elitismo: %llu\n", tempoTotalElit);
	log << str; puts(str);

	for (auto i = 0; i < 3; i++) {
		sprintf(str, "C%d: tempo total: %llu iguais: %d melhores: %d\n", i+1,
			   tempoTotalCruz[i], contadorIguaisCruz[i], contadorMelhoresCruz[i]);
		log << str; puts(str);
	}
	for (auto i = 0; i < 2; i++) {
		sprintf(str, "M%d: tempo total: %llu iguais: %d melhores: %d\n", i+1,
			   tempoTotalMut[i], contadorIguaisMut[i], contadorMelhoresMut[i]);
		log << str; puts(str);
	}
}

Solucao* Resolucao::gerarHorarioAGMutacaoSubstProf(const Solucao* pSolucao) const
{
	for (auto i = 0; i < horarioMutacaoTentativas; i++) {
		auto mut = new Solucao(*pSolucao);

		// Determina aleatoriamente qual disciplina terá seu professor substituído
		auto idx = aleatorio::randomInt() % mut->horario->matriz.size();
		auto disc = mut->horario->matriz[idx];
		if (!disc || disc->disciplina->professoresCapacitados.size() == 1) {
			delete mut;
			continue;
		}
		// Encontra um professor substituto, que não seja o original
		Professor* prof = nullptr;
		do {
			auto j = aleatorio::randomInt() % disc->disciplina->professoresCapacitados.size();
			prof = disc->disciplina->professoresCapacitados[j];
		} while (prof == disc->professor);
		// Remove todas as ocorrências da alocãção, guardando suas posições
		int coord[3];
		mut->horario->get3DMatrix(idx, coord);
		auto camada = coord[2];
		auto posicoesAloc = std::vector<std::pair<int, int>>{};
		for (auto d = 0; d < SEMANA; d++) {
			for (auto h = 0; h < blocosTamanho; h++) {
				auto linearPos = mut->horario->getPosition(d, h, camada);
				if (mut->horario->matriz[linearPos] == disc) {
					posicoesAloc.emplace_back(d, h);
					mut->horario->matriz[linearPos] = nullptr;
				}
			}
		}
		// Modifica a alocação para o novo professor
		disc->professor = prof;
		// Reinsere com a nova alocação
		for (const auto& pos : posicoesAloc) {
			auto success = mut->horario->insert(pos.first, pos.second, camada, disc);
			if (!success) {
				delete mut;
				continue;
			}
		}

		return mut;
	}
	return nullptr;
}

void Resolucao::logPopulacao(const std::vector<Solucao*>& populacao,
                             int iter)
{
	// Imprime FO e Hash de todas as soluções e determina frequência dos hashes
	std::unordered_map<std::size_t, int> freq;
	printf("iteracao: %d\n", iter);
	log << "iteracao: " << iter << "\n";

	printf("%-8s %s\n", "FO", "Hash");
	log << std::left << std::setw(8) << "FO" << " Hash\n";
	for (auto& individuo : populacao) {
		auto hash = std::to_string(individuo->getHash());
		log << std::left << std::setw(8) << individuo->getObjectiveFunction() << " "
			<< hash << "\n";
		printf("%-8g %s\n", individuo->getObjectiveFunction(), 
						    hash.c_str());
		freq[individuo->getHash()]++;
	}

	printf("\n%-11s %-5s %s\n", "Hash", "Freq", "%");
	log << std::left << std::setw(11) << "Hash" << std::setw(5) << "Freq" << " %\n";

	for (const auto& par : freq) {
		auto porc = par.second * 1. / populacao.size() * 100;
		log << std::left << std::setw(11) << std::left << par.first
			<< std::setw(5) << par.second << porc << "\n";
		printf("%-11u %-5d %g\n", par.first, par.second, porc);
	}

	log << "\n\n";
	printf("\n\n");

}

std::vector<Solucao*> Resolucao::gerarHorarioAGPopulacaoInicial2() 
{
	std::vector<Solucao*> populacaoInical;

	auto solucoes = gerarSolucoesAleatorias(horarioPopulacaoInicial);
	Util::insert_sorted(populacaoInical, begin(solucoes), end(solucoes), SolucaoComparaMaior());

	return populacaoInical;
}

bool Resolucao::gerarCamada(Solucao* solucao, int camada, std::vector<Disciplina*> discs,
							std::unordered_map<std::string, int>& creditos_alocados_prof) 
{
	auto num_discs = discs.size();
	auto num_disc_visitadas = 0u;
	std::vector<char> disc_visitadas(num_discs, false);

	while (num_disc_visitadas < num_discs) {
		int rnd_disc;
		do {
			rnd_disc = aleatorio::randomInt() % num_discs;
		} while (disc_visitadas[rnd_disc]);

		disc_visitadas[rnd_disc] = true;
		num_disc_visitadas++;
		auto disc = discs[rnd_disc];

		auto success = geraProfessorDisciplina(solucao, disc, camada, creditos_alocados_prof);
		if (!success) {
			return false;
		}
	}
	return true;
}

bool Resolucao::geraProfessorDisciplina(Solucao* solucao, Disciplina* disc,
										int camada, std::unordered_map<std::string, int>& creditos_alocados_prof) 
{
	auto& profs = disc->professoresCapacitados;
	auto num_profs = profs.size();
	auto num_prof_visitados = 0u;
	std::vector<char> prof_visitados(num_profs, false);
	auto success = false;

	while (!success && num_prof_visitados < num_profs) {
		int rnd_prof;
		do {
			rnd_prof = aleatorio::randomInt() % num_profs;
		} while (prof_visitados[rnd_prof]);

		prof_visitados[rnd_prof] = true;
		num_prof_visitados++;
		auto prof = profs[rnd_prof];

		if (prof->creditoMaximo != 0 &&
			prof->creditoMaximo < creditos_alocados_prof[prof->nome]
			+ disc->cargaHoraria) {
			continue;
		}
		success = geraAlocacao(solucao, disc, prof, camada);
	}
	return success;
}

bool Resolucao::geraAlocacao(Solucao* solucao, Disciplina* disc, Professor* prof, int camada)
{
	auto pdId = "pr" + prof->id + "di" + disc->id;
	if (!professorDisciplinas[pdId]) {
		professorDisciplinas[pdId] = new ProfessorDisciplina(prof, disc);
	}
	auto pd = professorDisciplinas[pdId];

	auto creditos_alocados_disc = 0;
	auto num_slots = SEMANA * blocosTamanho;
	auto num_slot_visitados = 0;
	std::vector<std::vector<char>> slots_visitados(SEMANA, std::vector<char>(blocosTamanho, false));

	while (creditos_alocados_disc < disc->cargaHoraria && num_slot_visitados < num_slots) {
		auto restante = disc->cargaHoraria - creditos_alocados_disc;
		int bloco, dia;
		auto success_prof = false;

		while (!success_prof && num_slot_visitados < num_slots) {
			do {
				dia = aleatorio::randomInt() % SEMANA;
				// Se restante for maior que um, deve alocar um bloco de duas disciplinas
				// em um horário par. Para isso se gera um número até a metade do número
				// de blocos e multiplica por dois. Ex: 4 blocos, serão gerados números 0 ou 1,
				// que possibilitam o bloco 0 ou 2.
				if (restante > 1) {
					bloco = 2 * (aleatorio::randomInt() % (blocosTamanho / 2));
				} else {
					// Se não for o caso, qualquer bloco serve
					bloco = aleatorio::randomInt() % blocosTamanho;
				}
			} while (slots_visitados[dia][bloco]);

			if (restante == 1) {
				slots_visitados[dia][bloco] = true;
				num_slot_visitados++;
				success_prof = solucao->horario->insert(dia, bloco, camada, pd);
				if (success_prof) {
					creditos_alocados_disc++;
				}
			} else {
				slots_visitados[dia][bloco] = true;
				slots_visitados[dia][bloco + 1] = true;
				num_slot_visitados += 2;
				auto success_bloco1 = solucao->horario->insert(dia, bloco, camada, pd);
				auto success_bloco2 = solucao->horario->insert(dia, bloco + 1, camada, pd);
				success_prof = success_bloco1 && success_bloco2;
				if (success_prof) {
					creditos_alocados_disc += 2;
				} else if (!success_prof) {
					if (success_bloco1) {
						solucao->horario->clearSlot(dia, bloco, camada);
					} 
					if (success_bloco2) {
						solucao->horario->clearSlot(dia, bloco + 1, camada);
					}
				}
			}
		}
	}

	return creditos_alocados_disc == disc->cargaHoraria;
}

Solucao* Resolucao::gerarSolucaoAleatoria() 
{
	auto solucaoRnd = new Solucao(blocosTamanho, camadasTamanho, perfisTamanho);
	std::unordered_map<std::string, int> creditos_alocados_prof;

	auto num_periodos = periodoXdisciplina.size();
	auto num_per_visitados = 0u;
	auto camada = 0;
	std::vector<char> periodos_visitados(num_periodos, false);

	// Percorre todos os períodos aleatoriamente
	while (num_per_visitados < num_periodos) {
		// Escolhe um período. Escolhe outro se já for percorrido.
		int rnd_per;
		do {
			rnd_per = aleatorio::randomInt() % num_periodos;
		} while (periodos_visitados[rnd_per]);

		periodos_visitados[rnd_per] = true;
		num_per_visitados++;

		// Encontra o período referente ao número, copia suas disciplinas
		auto it = periodoXdisciplina.begin();
		std::advance(it, rnd_per);
		auto& discs = it->second;

		auto success = gerarCamada(solucaoRnd, camada, discs, creditos_alocados_prof);
		if (!success) {
			delete solucaoRnd;
			return nullptr;
		}

		camada++;
	}
	return solucaoRnd;
}

std::vector<Solucao*> Resolucao::gerarSolucoesAleatorias(int numSolucoes) 
{
	std::vector<Solucao*> solucoes;
	for (auto i = 0; i < numSolucoes; i++) {
		printf("sol: %d\n", i);
		Solucao* currSolucao = nullptr;
		while (!currSolucao) {
			currSolucao = gerarSolucaoAleatoria();
		}
		gerarGradeTipoGraspClear(currSolucao);
		solucoes.push_back(currSolucao);
	}
	puts("\n");
	return solucoes;
}

void Resolucao::reinsereGrades(Solucao* solucao) const
{
	// Aqui as disciplinas são inseridas na matriz. É necessário copiar as 
	// adicionadas e depois resetá-las para a inserção funcionar.
	for (auto& par : solucao->grades) {
		auto& grade = *par.second;
		std::vector<Disciplina*> disc_add_copy{};
		std::swap(disc_add_copy, grade.disciplinasAdicionadas);
		for (auto& disc : disc_add_copy) {
			grade.insert(disc, {}, true);
		}
	}
}

void Resolucao::gerarHorarioAGEvoluiPopulacao(std::vector<Solucao*>& populacao, 
											  int numCruz)
{
	for (auto j = 0; j < numCruz; j++) {
		auto parVencedor = gerarHorarioAGTorneioPar(populacao);

		auto filhos = gerarHorarioAGCruzamento(parVencedor);

		Util::insert_sorted(populacao, begin(filhos), end(filhos),
							SolucaoComparaMaior());

	}
}

void Resolucao::gerarHorarioAGEvoluiPopulacaoExper(
	std::vector<Solucao*>& populacao, Configuracao::TipoCruzamento tipoCruz,
	std::vector<Solucao*>& pais)
{
	auto filhos = gerarHorarioAGCruzamentoExper(pais, tipoCruz);
	Util::insert_sorted(populacao, begin(filhos), end(filhos),
						SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGEfetuaMutacao(std::vector<Solucao*>& populacao)
{
	auto geneX = gerarHorarioAGMutacao(populacao);
	Util::insert_sorted(populacao, begin(geneX), end(geneX),
						SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGEfetuaMutacaoExper(std::vector<Solucao*>& populacao, 
												 Configuracao::TipoMutacao tipoMut)
{
	
	auto geneX = gerarHorarioAGMutacaoExper(populacao, tipoMut);
	Util::insert_sorted(populacao, begin(geneX), end(geneX),
						SolucaoComparaMaior());
}

void Resolucao::gerarHorarioAGVerificaEvolucao(std::vector<Solucao*>& populacao, 
												  int iteracaoAtual)
{
	if (populacao[0]->getObjectiveFunction() > foAlvo) {
		foAlvo = populacao[0]->getObjectiveFunction();
		hashAlvo = populacao[0]->getHash();
		iteracaoAlvo = iteracaoAtual;
		tempoAlvo = Util::chronoDiff(std::chrono::steady_clock::now(), 
									 tempoInicio);
		puts("Nova melhor solucao!");
		log << "Nova melhor solucao!\n";
		char str[101];
		sprintf(str, "%-8g %u\n", populacao[0]->getObjectiveFunction(),
								  populacao[0]->getHash());
		puts(str); log << str;
	}
}

Grade* Resolucao::gradeAleatoria(AlunoPerfil* alunoPerfil, Solucao* solucao)
{
	auto novaGrade = new Grade(blocosTamanho, alunoPerfil, solucao->horario,
							   disciplinas, disciplinasIndex);
	auto horariosMax = (SEMANA - 2) * blocosTamanho;
	auto adicionados = 0;

	std::multiset<Disciplina*, DisciplinaCargaHorariaDesc> restantes;
	for (const auto& nomedisc : alunoPerfil->restante) {
		restantes.insert(getDisciplinaByName(nomedisc));
	}

	while (!restantes.empty() && adicionados < horariosMax) {
		auto current = getRandomDisc(restantes);

		auto success = novaGrade->insert(*current);
		if (success) {
			adicionados++;
		}
		restantes.erase(current);
	}

	return novaGrade;
}

Grade* Resolucao::vizinhoGrasp(Grade* grade)
{
	auto currGrade = new Grade(*grade);

	const auto& adicionadas = currGrade->disciplinasAdicionadas;
	Disciplina* discremovida = nullptr;

	if (!adicionadas.empty()) {
		auto rand = aleatorio::randomInt() % adicionadas.size();
		discremovida = currGrade->remove(adicionadas[rand]);
	}

	std::multiset<Disciplina*, DisciplinaCargaHorariaDesc> restantes;
	for (const auto& nomedisc : currGrade->alunoPerfil->restante) {
		restantes.insert(getDisciplinaByName(nomedisc));
	}

	while (!restantes.empty()) {
		auto current = getRandomDisc(restantes);
		auto discJaAdicionada = std::find(
			begin(adicionadas), end(adicionadas), *current) != end(adicionadas);

		if (*current == discremovida || discJaAdicionada) {
			restantes.erase(current);
			continue;
		}

		if (currGrade->insert(*current)) {
			break;
		}
		restantes.erase(current);
	}

	return currGrade;
}

void Resolucao::buscaLocal(Grade*& grade)
{
	auto bestGrade = grade;

	for (auto i = 0; i < gradeGraspVizinhos; i++) {
		auto vizinho = vizinhoGrasp(grade);
		if (vizinho->getObjectiveFunction() > bestGrade->getObjectiveFunction()) {
			delete bestGrade;
			bestGrade = vizinho;
		} else {
			delete vizinho;
		}
	}

	grade = bestGrade;
}

Grade* Resolucao::GRASP(AlunoPerfil* alunoPerfil, Solucao* solucao)
{
	auto melhorGrade = gradeAleatoria(alunoPerfil, solucao);
	auto ultimaMelhoriaIter = -1;

	for (auto i = 0; i - ultimaMelhoriaIter < numMaximoIteracoesSemEvolucaoGRASP; i++) {
		auto gradeCandidata = gradeAleatoria(alunoPerfil, solucao);
		buscaLocal(gradeCandidata);

		if (gradeCandidata->getObjectiveFunction() > melhorGrade->getObjectiveFunction()) {
			ultimaMelhoriaIter = i;
			delete melhorGrade;
			melhorGrade = gradeCandidata;
		} else {
			delete gradeCandidata;
		}
	}

	return melhorGrade;
}
