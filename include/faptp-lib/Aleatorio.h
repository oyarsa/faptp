#ifndef ALEATORIO_H
#define ALEATORIO_H

#include <cstdint>

namespace aleatorio
{
int randomInt();
uint32_t randomUInt();
void initRandom(int numThreads);
}

#endif // ALEATORIO_H


