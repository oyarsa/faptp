/* 
 * File:   UUID.h
 * Author: pedro
 *
 * Created on 22 de Setembro de 2014, 11:10
 */

#ifndef UUID_H
#define	UUID_H

#include <string>

class UUID {
public:
  UUID() = default;
  std::string GenerateUuid();
};

#endif	/* UUID_H */

