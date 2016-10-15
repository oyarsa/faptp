/*
 * File:   Output.cpp
 * Author: pedromazala
 *
 * Created on January 1, 2016, 8:20 PM
 */

#include <sstream>
#include <array>
#include <fstream>

#include <faptp/Output.h>
#include <faptp/Semana.h>

void Output::write(Solucao* pSolucao, const std::string& savePath)
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
                auto pd = pSolucao->horario->at(k, j, i);
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
                auto pd = gradeAtual->at(k, j, 0);
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

