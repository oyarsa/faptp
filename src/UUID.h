/* 
 * File:   UUID.h
 * Author: pedro
 *
 * Created on 22 de Setembro de 2014, 11:10
 */

#ifndef UUID_H
#define	UUID_H

#define UUID_LENGTH 40

#include <cstdlib>
#include <iostream>
#include <string>

class UUID {
public:
  UUID();
  UUID(const UUID& orig);
  virtual ~UUID();
  
  std::string GenerateUuid();
};

#endif	/* UUID_H */

