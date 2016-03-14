/*
 * File:   Output.cpp
 * Author: pedromazala
 *
 * Created on January 1, 2016, 8:20 PM
 */

#include <sstream>
#include <fstream>

#include "Output.h"
#include "Semana.h"
#include <ctime>

std::string Output::getDir() {
    return "output/" + timestamp();
}

std::string Output::timestamp()
{
    auto current_time = time(nullptr);
    std::string time_str = ctime(&current_time);
    time_str.erase(remove(time_str.begin(), time_str.end(), ' '), time_str.end());
    time_str.erase(remove(time_str.begin(), time_str.end(), '\n'), time_str.end());
    time_str.erase(remove(time_str.begin(), time_str.end(), ':'), time_str.end());
	return time_str;
}

void Output::write(Solucao *pSolucao) {
    write(pSolucao, getDir());
}

void Output::write(Solucao *pSolucao, const std::string& savePath) {
    // Criando o diretorio de saida
	Util::create_folder(savePath);
    std::stringstream saida{};
    const std::string diasDaSemana[] = {"Segunda", "Terca", "Quarta", "Quinta",
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
        saida << "<table align='center' class='horario'>\n";

        saida << "<tr>";
        for (int j = 0; j < SEMANA; j++) {
            saida << "<th>" << diasDaSemana[j] << "</th>";
        }
        saida << "</tr>";


        for (int j = 0; j < pSolucao->blocosTamanho; j++) {
            saida << "<tr>";
            for (int k = 0; k < SEMANA; k++) {
                auto pd = pSolucao->horario->at(k, j, i);
                if (pd) {
					saida << "<td>"<< pd->getDisciplina()->getNome() << "<br>"
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

        double fo = gradeAtual->getObjectiveFunction();

        if (fo == 0) {
            //continue;
        }

        saida << "<table align='center' class='grade'>\n";

        saida << "<tr><th colspan=\"" << SEMANA << "\">" << gradeAtual->alunoPerfil->id << " (" << fo << ")</th></tr>\n";

        saida << "<tr>";
        for (int j = 0; j < SEMANA; j++) {
            saida << "<th>" << diasDaSemana[j] << "</th>";
        }
        saida << "</tr>";


        for (int j = 0; j < pSolucao->blocosTamanho; j++) {
            saida << "<tr>";
            for (int k = 0; k < SEMANA; k++) {
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

	std::ofstream arquivoSaida(savePath + "/horario.html");
    arquivoSaida << std::nounitbuf << saida.str() << std::endl;
    arquivoSaida.close();
}

