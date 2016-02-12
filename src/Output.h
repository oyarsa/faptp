/* 
 * File:   Output.h
 * Author: pedromazala
 *
 * Created on January 1, 2016, 8:20 PM
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <fstream>
#include <sstream>

#include "Solucao.h"

class Output {
public:
    Output();
    Output(const Output& orig);
    virtual ~Output();
    
    void write(Solucao *solucao);
    void write(Solucao *solucao, string savePath);
private:
    string getDir();
};

#endif /* OUTPUT_H */

