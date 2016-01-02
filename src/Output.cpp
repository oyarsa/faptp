/* 
 * File:   Output.cpp
 * Author: pedromazala
 * 
 * Created on January 1, 2016, 8:20 PM
 */

#include "Output.h"

Output::Output() {
}

Output::Output(const Output& orig) {
}

Output::~Output() {
}

std::string Output::getDir() {
    time_t current_time = time(NULL);
    std::string c_time_string = ctime(&current_time);
    c_time_string.erase(std::remove(c_time_string.begin(), c_time_string.end(), ' '), c_time_string.end());
    c_time_string.erase(std::remove(c_time_string.begin(), c_time_string.end(), '\n'), c_time_string.end());
    std::string dir = "output/" + c_time_string;

    return dir;
}

void Output::write(Solucao *pSolucao) {
    std::string dir = getDir();
    std::string command = "mkdir -p " + dir;

    // Criando o diretório de saída
    int result = system(command.c_str());

    std::ostringstream saida{};
    const std::string diasDaSemana[] = {"Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado", "Domingo"};

    saida << std::nounitbuf;
    saida << "<!DOCTYPE html>\n"
            << "<html align='center' id='nome'>\n"
            << "<style type=\"text/css\"></style>"
            << "<body>\n";

    for (int i = 0; i < pSolucao->camadasTamanho; i++) {
        saida << "<table align='center' class='horarios'>\n";
        
        saida << "<tr>";
        for (int j = 0; j < 7; j++) {
            saida << "<th>" << diasDaSemana[j] << "</th>";
        }
        saida << "</tr>";


        for (int j = 0; j < pSolucao->blocosTamanho; j++) {
            saida << "<tr>";
            for (int k = 0; k < 7; k++) {
                auto pd = pSolucao->horario->at(k, j, i);
                std::string pds = "-";
                if (pd != NULL) {
                    pds = pd->getDisciplina()->getNome();
                }
                saida << "<td>" << pds << "</td>";
            }
            saida << "</tr>";
        }
        saida << "<table align='center' class='horarios'>\n";
    }

    saida << "</body>\n"
            << "</html>\n";

    std::ofstream arquivoSaida(dir + "/horario.html");
    arquivoSaida << std::nounitbuf << saida.str() << std::endl;
    arquivoSaida.close();
}

