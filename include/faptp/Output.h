/*
 * File:   Output.h
 * Author: pedromazala
 *
 * Created on January 1, 2016, 8:20 PM
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>

#include <faptp/Solucao.h>

class Output
{
public:
    static void writeHtml(Solucao* solucao, const std::string& savePath);
    static void writeJson(const Solucao& solucao, const std::string& outfile);
};

#endif /* OUTPUT_H */

