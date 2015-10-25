#include "Aleatorio.h"

Aleatorio::Aleatorio()
: dist {
  0, MAX_RANDOM
},
seed{static_cast<TipoSeed> (
  TipoRelogio::now().time_since_epoch().count())},
gerador{seed}
{
}

std::mt19937 Aleatorio::geradorAleatorio() {
  return gerador;
}

int Aleatorio::randomInt() {
  return dist(gerador);
}
