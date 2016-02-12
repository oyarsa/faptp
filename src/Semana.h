#ifndef SEMANA_H
#define	SEMANA_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SEMANA    5

#define SEGUNDA   0
#define TERCA     1
#define QUARTA    2
#define QUINTA    3
#define SEXTA     4
#define SABADO    5
#define DOMINGO   6

#define HORA_MINUTO 60.0
#define MINUTO_ALUA 60.0
#define SEMANA_MES 4.0
#define MES_SEMESTRE 6.0
#define CARGA_HORARIA ((double)(MINUTO_ALUA/HORA_MINUTO))

#ifdef	__cplusplus
}
#endif

#endif	/* SEMANA_H */

