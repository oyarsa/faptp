/* 
 * File:   UUID.cpp
 * Author: pedro
 * 
 * Created on 22 de Setembro de 2014, 11:10
 */

#include <ctime>
#include <cstdlib>
#include <iostream>

#include <gsl.h>

#include "UUID.h"

[[suppress(bounds .1)]]
std::string UUID::GenerateUuid()
{
    const int UUID_LENGTH = 40;
    std::string uuid_code;
    char* guidStr = new char[UUID_LENGTH];
    char* pGuidStr = guidStr;
    int i;

    srand(gsl::narrow_cast<unsigned>(time(nullptr))); /*Randomize based on time.*/

    /*Data1 - 8 characters.*/
    //*pGuidStr++ = '{';  
    for (i = 0; i < 8; i++ , pGuidStr++)
        ((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

    /*Data2 - 4 characters.*/
    *pGuidStr++ = '-';
    for (i = 0; i < 4; i++ , pGuidStr++)
        ((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

    /*Data3 - 4 characters.*/
    *pGuidStr++ = '-';
    for (i = 0; i < 4; i++ , pGuidStr++)
        ((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

    /*Data4 - 4 characters.*/
    *pGuidStr++ = '-';
    for (i = 0; i < 4; i++ , pGuidStr++)
        ((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

    /*Data5 - 12 characters.*/
    *pGuidStr++ = '-';
    for (i = 0; i < 12; i++ , pGuidStr++)
        ((*pGuidStr = (rand() % 16)) < 10) ? *pGuidStr += 48 : *pGuidStr += 55;

    //*pGuidStr++ = '}';    
    *pGuidStr = '\0';

    uuid_code = guidStr;
    delete[] guidStr;

    return uuid_code;
}

