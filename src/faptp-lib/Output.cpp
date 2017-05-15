/*
 * File:   Output.cpp
 * Author: pedromazala
 *
 * Created on January 1, 2016, 8:20 PM
 */

#include <sstream>
#include <array>
#include <fstream>
#include <json/json.h>
#include <faptp-lib/Output.h>
#include <faptp-lib/Semana.h>

void Output::writeHtml(Solucao* pSolucao, const std::string& savePath)
{
    // Criando o diretorio de saida
    Util::create_folder(savePath);
    std::stringstream saida {};
    const std::array<std::string, dias_semana_total> diasDaSemana {
            "Segunda", "Terca", "Quarta", "Quinta",
            "Sexta", "Sabado", "Domingo"};

    saida << std::nounitbuf;
    saida << "<!DOCTYPE html>\n"
            << "<html>\n"
            << "<head>\n"
            << "<style type=\"text/css\">\n"
            << "  table {\n"
            << "    width: 100%;\n"
            << "    border-collapse: collapse;\n"
            << "    margin: 10px 0 15px 0px;\n"
            << "  }\n"
            << "  table, td, th {\n"
            << "    border: 1px solid black;\n"
            << "  }\n"
            << "</style>\n"
            << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n"
            << "</head>\n"
            << "<body>\n";

    saida << "<hr /> <h3>Horario</h3>\n";
    for (int i = 0; i < pSolucao->camadasTamanho; i++) {
        saida << "<center><h1>" << pSolucao->camada_periodo[i] << "</h1></center>";
        saida << "<table align='center' class='horario'>\n";

        saida << "<tr>";
        for (const auto& dia : diasDaSemana) {
            saida << "<th>" << dia << "</th>";
        }
        saida << "</tr>";


        for (int j = 0; j < pSolucao->blocosTamanho; j++) {
            saida << "<tr>";
            for (int k = 0; k < dias_semana_util; k++) {
                const auto pd = pSolucao->horario->at(k, j, i);
                if (pd) {
                    saida << "<td>" << pd->getDisciplina()->getNome() << "<br>"
                            << "<i>" << pd->getProfessor()->getNome() << "</i>"
                            << "</td>";
                } else {
                    saida << "<td>" << " -- " << "</td>";
                }
            }
            saida << "</tr>";
        }
        saida << "</table>\n";
    }

    saida << "<hr /> <h3>Grades</h3>\n";
    for (const auto& par : pSolucao->grades) {
        const auto gradeAtual = par.second;

        double fo = gradeAtual->getFO();

        saida << "<table align='center' class='grade'>\n";
        saida << "<tr><th colspan=\"" << dias_semana_util << "\">" << "Aluno: "
                << gradeAtual->aluno->id << " FO: (" << fo << ")</th></tr>\n";

        saida << "<tr>";
        for (const auto& dia : diasDaSemana) {
            saida << "<th>" << dia << "</th>";
        }
        saida << "</tr>";


        for (int j = 0; j < pSolucao->blocosTamanho; j++) {
            saida << "<tr>";
            for (int k = 0; k < dias_semana_util; k++) {
                const auto pd = gradeAtual->at(k, j, 0);
                if (pd) {
                    saida << "<td>" << pd->getDisciplina()->getNome() << "<br>"
                            << "<b>" << pd->getDisciplina()->getId() << "</b>"
                            << "</td>";
                } else {
                    saida << "<td> -- </td>";
                }
            }
            saida << "</tr>";
        }

        saida << "</table>\n";
    }

    saida << "</body>\n"
            << "</html>\n";

    std::ofstream arquivoSaida {savePath + "/horario.html"};
    arquivoSaida << std::nounitbuf << saida.str() << "\n";
}

void
Output::writeJson(const Solucao& solucao, const std::string& outfile)
{
  Json::Value raiz;
  Json::Value periodos{ Json::arrayValue };
  auto& horario = solucao.getHorario();

  raiz["fo_grade"] = solucao.calculaFOSomaCarga();
  raiz["fo_preferencias"] = std::abs(solucao.calculaFOSoftConstraints());

  for (auto c = 0; c < solucao.camadasTamanho; c++) {
    Json::Value periodo;
    periodo["nome"] = solucao.camada_periodo.at(c);

    Json::Value eventos{ Json::arrayValue };
    for (auto b = 0; b < solucao.blocosTamanho; b++) {
      for (auto d = 0; d < dias_semana_util; d++) {
        const auto pd = horario.at(d, b, c);
        if (pd) {
          Json::Value e;
          e["professor"] = pd->getProfessor()->getId();
          e["disciplina"] = pd->getDisciplina()->getId();
          e["horario"] = b;
          e["dia"] = d;
          eventos.append(e);
        }
      }
    }
    periodo["eventos"] = eventos;
    periodos.append(periodo);
  }
  raiz["periodos"] = periodos;

  Json::Value alunos{ Json::arrayValue };
  for (const auto& par : solucao.grades) {
    Json::Value aluno;
    const auto grade = par.second;

    aluno["id"] = grade->aluno->id;
    aluno["descricao"] = grade->aluno->id + " - " + grade->aluno->periodo;
    aluno["fo"] = grade->getFO();

    Json::Value horarios{ Json::arrayValue };

    for (auto b = 0; b < solucao.blocosTamanho; b++) {
      for (auto d = 0; d < dias_semana_util; d++) {
        const auto pd = grade->at(d, b, 0);
        if (pd) {
          Json::Value e;
          e["disciplina"] = pd->getDisciplina()->getId();
          e["dia"] = d;
          e["horario"] = b;
          horarios.append(e);
        }
      }
    }

    aluno["grade"] = horarios;
    alunos.append(aluno);
  }
  raiz["alunos"] = alunos;

  std::ofstream out{ outfile };
  out << raiz;
}
