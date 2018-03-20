#ifndef ALEATORIO_H
#define ALEATORIO_H

#include <cstdint>

namespace aleatorio
{
int randomInt();
int randomInt(int thread_id);

uint32_t randomUInt();
uint32_t randomUInt(int thread_id);

void initRandom(int numThreads);

}

#endif // ALEATORIO_H


