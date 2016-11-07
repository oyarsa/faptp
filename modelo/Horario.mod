/*********************************************
 * OPL 12.6.3.0 Model
 * Author: Italo Silva
 * Creation Date: 04/11/2016 at 14:34:09
 *********************************************/

// Conjuntos
{string} C = ...;  // periodos
{string} D = ...;  // disciplinas
{string} P = ...;  // professores
int num_dias = ...;
int num_horarios = ...;
range J = 1..num_dias;  // dias
range I = 1..num_horarios;  // horários
range Ja = 1..num_horarios-2;
range Jb = 1..num_dias-2;

int num_constraints = 9;
int pi[1..num_constraints] = ...;  // pesos das constraints
int h[P][D] = ...;  // se P pode lecionar D
int H[D][C] = ...;  // se D pertence a C
int A[P][I][J] = ...;  // se P está disponível em (I, J)
int G[D] = ...;  // se D é uma disciplina difícil
int F[P][D] = ...;  // se P possui preferência por D
int Q[P] = ...;  // aulas desejadas por P
int N[P] = ...;  // horas no contrato de P
int K[D] = ...;  // carga horária D
int O[D] = ...;  // se D está sendo oferecida

dvar boolean x[D][I][J];  // D agendada no horário (I, J)
dvar boolean L[P][D];  // P alocado para D
dvar boolean r[C][I][J];  // se a C tem aula em (I, J)
dvar boolean alfa1[Ja][C][I][J];  // se existe uma janela de tamanho Ja
								  // começando em I no dia J para turma C
dvar int alfa[C]; // número de janelas para turma C
dvar boolean w[P][J];  // se o professor P dá aula em J
dvar boolean beta1[Jb][P][J];  // se existe uma janela de tamanho Jb
							   // começando em J para professor P
dvar int beta[P];  // número de janelas para professor P
dvar boolean g[C][J];  // se a turma C possui aula em J
dvar int gama[C];  // número de dias de aula de C
dvar int delta[C];  // número de aulas que C tem aos sábados
dvar int epsilon[D][J];  // número de aulas seguidas de D em J
dvar int teta[J][C];  // número de aulas dificeis em J para C
dvar boolean capa[C][J];  // se existe uma aula difícil no último horário de C em J
dvar int lambda[P];  // número de disciplinas atribuídas a P que não são de sua preferências
dvar int mi[P];  // número de aulas que excedem a preferência de P

/*
minimize
	pi[1] * (sum (c in C) alfa[c]) +
	pi[2] * (sum (p in P) beta[p]) +
	pi[3] * (sum (c in C) gama[c]) +
	pi[4] * (sum (c in C) delta[c]) +
	pi[5] * (sum (d in D, j in J) epsilon[d][j]) +
	pi[6] * (sum (j in J, c in C) teta[j][c]) +
	pi[7] * (sum (j in J, c in C) capa[c][j]) +
	pi[8] * (sum (p in P) lambda[p]) +
	pi[9] * (sum (p in P) mi[p]);*/


subject to {

	/*// 1
	forall (i in I, j in J, p in P)
	  sum (d in D) x[d][i][j] * L[p][d] <= A[p][i][j];*/

	// 2
	forall (i in I, j in J, c in C)
	  sum (d in D) x[d][i][j] * H[d][c] <= 1;

	// 3
	forall (d in D)
	  sum (p in P) L[p][d] * h[p][d] == O[d];

	// 4
	forall (p in P)
	  sum(d in D, i in I, j in J) x[d][i][j] * h[p][d] <= N[p];

	// 5
	forall (d in D)
	  sum (i in I, j in J) x[d][i][j] == K[d];

	// 6

/*
	// 7
	forall (c in C, i in I, j in J)
	  r[c][i][j] <= (sum (d in D) x[d][i][j] * H[d][c]);

	// 8
	forall (c in C, i in I, j in J, k in 1..num_horarios-1-i)
	  r[c][i][j] - (sum (l in 1..k) r[c][i+l][j]) + r[c][i+k+1][j] - alfa1[k][c][i][j] <= 1;

	// 9
	forall (c in C)
	  alfa[c] == sum (k in Ja, i in I, j in J) alfa1[k][c][i][j];

	// 10
	forall (p in P, j in J)
	  w[p][j] <= sum (i in I, d in D) x[d][i][j] * L[p][d];

	// 11
	forall (p in P, j in J, k in 1..num_dias-1-j)
	   w[p][j] - sum (l in 1..k) w[p][j+l] - w[p][j+k+1] - beta1[k][p][j] <= 1;

	// 12
	forall (p in P)
	  beta[p] == sum (k in Jb, j in J) beta1[k][p][j];

	// 13
	forall (c in C, j in J)
	  g[c][j] == sum (i in I) r[c][i][j];

	// 14
	forall (c in C)
	  gama[c] == sum (j in J) g[c][j];

	// 15
	forall (c in C)
	  delta[c] == sum (d in D, i in I, j in J) x[d][i][j] * H[d][c];

	// 16
	forall (d in D, j in J)
	  (sum (i in I) x[d][i][j]) - epsilon[d][j] <= 2;

	// 17
	forall (c in C, j in J)
	  sum (i in I, d in D) x[d][i][j] * G[d] * H[d][c] - teta[j][c] <= 2;

	// 18
	forall (c in C, j in J)
	  capa[c][j] == sum(d in D, i in num_horarios - 1..num_horarios)
	  								x[d][i][j] * G[d] * H[d][c];

	// 19
	forall (p in P)
	  lambda[p] == sum (d in D) L[p][d] * (1 - F[p][d]);

	// 20
	forall (p in P)
	  mi[p] >= sum (d in D, i in I, j in J) x[d][i][j] * L[p][d] - Q[p];*/
}

execute {
	var csv = "";

	csv += "periodo,dia,horario,disciplina,professor\n";

	for (c in C) {
		for (d in D) {
			if (H[d][c] == 0) {
				continue;
			}

			for (i in I) {
				for (j in J) {
					if (x[d][i][j] == 0) {
						continue;
					}

					for (p in P) {
						if (L[p][d] == 1) {
							csv += c + "," + (j-1) + "," + (i-1) + "," + d + "," + p + "\n";
							break;
						}
					}
				}
			}
		}
	}

	writeln(csv);
	var f = new IloOplOutputFile("result.csv");
	f.writeln(csv);
}
