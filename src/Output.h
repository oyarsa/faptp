/* 
 * File:   Output.h
 * Author: pedromazala
 *
 * Created on January 1, 2016, 8:20 PM
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>

#include "Solucao.h"

class Output {
public:
	static std::string timestamp();
    static void write(Solucao *solucao);
	static void write(Solucao *solucao, const std::string& savePath);
private:
    static std::string getDir();
};

#endif /* OUTPUT_H */

