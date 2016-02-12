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
    Output() = default;
    
    void write(Solucao *solucao);
    void write(Solucao *solucao, std::string savePath);
private:
    std::string getDir() const;
};

#endif /* OUTPUT_H */

