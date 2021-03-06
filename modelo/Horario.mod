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
{int} Horarios_pares = {1 + 2*k | k in 0..(num_horarios-2) div 2};
range J = 1..num_dias;  // dias
range I = 1..num_horarios;  // horários
range Ja = 1..num_horarios-2;
range Jb = 1..num_dias-2;

int num_constraints = 10;
float pi[1..num_constraints] = ...;  // pesos das constraints
int h[P][D] = ...;  // se P pode lecionar D
int H[D][C] = ...;  // se D pertence a C
int A[P][I][J] = ...;  // se P está disponível em (I, J)
int G[D] = ...;  // se D é uma disciplina difícil
int F[P][D] = ...;  // se P possui preferência por D
int Q[P] = ...;  // aulas desejadas por P
int N[P] = ...;  // horas no contrato de P
int K[D] = ...;  // carga horária D
int O[D] = ...;  // se D está sendo oferecida
int B[D] = ...;  // número de aulas geminadas da disciplina D

dvar boolean x[P][D][I][J];  // D agendada no horário (I, J) para o professor P
dvar int+ r[C][I][J];  // se a C tem aula em (I, J)
dvar boolean alfa1[Ja][C][I][J];  // se existe uma janela de tamanho Ja
								  // começando em I no dia J para turma C
dvar int+ alfa[C]; // número de janelas para turma C
dvar boolean w[P][J];  // se o professor P dá aula em J
dvar boolean beta1[Jb][P][J];  // se existe uma janela de tamanho Jb
							   // começando em J para professor P
dvar int+ beta[P];  // número de intervalos de trabalho para professor P
dvar boolean g[C][J];  // se a turma C possui aula em J
dvar int+ gama[C];  // número de dias de aula de C
dvar int+ delta[C];  // número de aulas que C tem aos sábados
dvar int+ epsilon[D][J];  // número de aulas seguidas de D em J
dvar int+ teta[J][C];  // número de aulas dificeis em J para C
dvar boolean capa[C][J];  // se existe uma aula difícil no último horário de C em J
dvar int+ lambda[P];  // número de disciplinas atribuídas a P que não são de sua preferências
dvar int+ mi[P];  // número de aulas que excedem a preferência de P
dvar boolean Lec[P][D];  // professor P leciona disciplina D
dvar boolean gem[P][D][I][J]; // indica se uma aula geminada começa em i,j
//dvar int+ penalidades[1..num_constraints];

minimize
	//sum (i in 1..num_constraints) (pi[i] * penalidades[i]);
	pi[1] * (sum (c in C) alfa[c]) + // janelas
  pi[2] * (sum (p in P) beta[p]) + // intervalo de trabalho
  pi[3] * (sum (c in C) gama[c]) + // horário compacto
  pi[4] * (sum (c in C) delta[c]) + // aulas aos sábados
  pi[5] * (sum (d in D, j in J) epsilon[d][j]) + // aulas seguidas
  pi[6] * (sum (j in J, c in C) teta[j][c]) + // aulas difíceis seguidas
  pi[7] * (sum (j in J, c in C) capa[c][j]) + // aulas difíceis no último horário
  pi[8] * (sum (p in P) lambda[p]) + // preferência do professor (disciplinas)
  pi[9] * (sum (p in P) mi[p]); // preferências do professor (número de aulas)


subject to {

	// Disponibilidade do professor
	forall (i in I, j in J, p in P)
	  sum (d in D) x[p][d][i][j] <= A[p][i][j];

	// Conflito de aulas em um período
	forall (i in I, j in J, c in C)
	  sum (p in P, d in D) (x[p][d][i][j] * H[d][c]) <= 1;

	// Capacitação do professor
	forall (p in P, d in D, i in I, j in J)
		x[p][d][i][j] <= h[p][d];

  // Impedem que uma disciplina tenha mais de um professor associado
  forall (p in P, d in D, i in I, j in J)
	  Lec[p][d] >= x[p][d][i][j];

	forall (p in P, d in D)
	  Lec[p][d] <= sum (i in I, j in J) x[p][d][i][j];

	forall (d in D)
	   sum (p in P) Lec[p][d] == O[d];

	// Contrato do professor
	forall (p in P)
	  sum(d in D, i in I, j in J) (x[p][d][i][j] * h[p][d]) <= N[p];

	// Carga horária das disciplinas
	forall (d in D)
	  sum (p in P, i in I, j in J) x[p][d][i][j] == K[d] * O[d];

  // Aulas geminadas
  forall (p in P, d in D, i in Horarios_pares, j in J)
		gem[p][d][i][j] <= x[p][d][i][j];

  forall (p in P, d in D, i in Horarios_pares, j in J)
    gem[p][d][i][j] <= x[p][d][i+1][j];

  forall (p in P, d in D, i in Horarios_pares, j in J)
    gem[p][d][i][j] >= x[p][d][i][j] + x[p][d][i+1][j] - 1;

  forall (p in P, d in D, i in Horarios_pares, j in J)
	  gem[p][d][i+1][j] == 0;

  forall (d in D)
	  sum (p in P, i in I, j in J) gem[p][d][i][j] == B[d];

	// Janelas de disciplinas
	forall (c in C, i in I, j in J)
	  r[c][i][j] == (sum (p in P, d in D) x[p][d][i][j] * H[d][c]);

	forall (c in C, i in I, j in J, k in 1..num_horarios-1-i)
	  r[c][i][j] - (sum (l in 1..k) r[c][i+l][j]) + r[c][i+k+1][j] - alfa1[k][c][i][j] <= 1;

	forall (c in C)
	  alfa[c] == sum (k in Ja, i in I, j in J) alfa1[k][c][i][j];

	// Intervalo de trabalho dos professores
	forall (p in P, d in D, i in I, j in J)
		w[p][j] >= x[p][d][i][j];

	forall (p in P, j in J)
	  w[p][j] <= sum (i in I, d in D) x[p][d][i][j];

	forall (p in P, j in J, k in 1..num_dias-1-j)
		w[p][j] - (sum (l in 1..k) w[p][j+l]) + w[p][j+k+1] - beta1[k][p][j] <= 1;

	forall (p in P)
	  beta[p] == sum (k in Jb, j in J) beta1[k][p][j];

	// Horário compacto
	forall (c in C, i in I, j in J)
		g[c][j] >= r[c][i][j];

	forall (c in C, j in J)
	  g[c][j] <= sum (i in I) r[c][i][j];

	forall (c in C)
	  gama[c] == sum (j in J) g[c][j];

	// Aulas aos sábados
	forall (c in C)
	  delta[c] == sum (p in P, d in D, i in I) x[p][d][i][num_dias] * H[d][c];

	// Aulas seguidas
	forall (d in D, j in J)
	  (sum (p in P, i in I) x[p][d][i][j]) - epsilon[d][j] <= 2;

	// Aulas seguidas de nível difícil
	forall (c in C, j in J)
	  sum (i in I, d in D, p in P) x[p][d][i][j] * G[d] * H[d][c] - teta[j][c] <= 2;

	// Aulas de nível difícil no último horário
	forall (c in C, j in J)
	  capa[c][j] == sum(p in P, d in D, i in (num_horarios - 1)..num_horarios)
	  								x[p][d][i][j] * G[d] * H[d][c];

	// Preferência do professor (disciplinas)
	//forall (p in P, i in I, j in J)
	  //lambda[p] == sum (d in D) x[p][d][i][j] * (1 - F[p][d]);

  // Isso talvez não funcione
	forall (p in P)
	  lambda[p] == sum (d in D, i in I, j in J) x[p][d][i][j] * (1 - F[p][d]);

	// Preferência do professor (número de aulas)
	forall (p in P)
	  mi[p] >= sum (d in D, i in I, j in J) x[p][d][i][j] - Q[p];

	/*penalidades[1] == (sum (c in C) alfa[c]); // janelas
	penalidades[2] == (sum (p in P) beta[p]); // intervalo de trabalho
	penalidades[3] == (sum (c in C) gama[c]); // horário compacto
	penalidades[4] == (sum (c in C) delta[c]); // aulas aos sábados
	penalidades[5] == (sum (d in D, j in J) epsilon[d][j]);  // aulas seguidas
	penalidades[6] == (sum (j in J, c in C) teta[j][c]);  // aulas difíceis seguidas
	penalidades[7] == (sum (j in J, c in C) capa[c][j]); // aulas difíceis no último horário
	penalidades[8] == (sum (p in P) lambda[p]); // preferência do professor (disciplinas)
	penalidades[9] == (sum (p in P) mi[p]); // preferências do professor (número de aulas)
  */
}

execute {
	var csv = "";

	csv += "periodo,dia,horario,disciplina,professor\n";

	for (c in C) {
		for (d in D) {
			if (H[d][c] == 0) {
				continue;
			}

			for (p in P) {
				for (i in I) {
					for (j in J) {
						if (x[p][d][i][j] == 0) {
							continue;
						}

						csv += c + "," + (j-1) + "," + (i-1) + "," + d + "," + p + "\n";
					}
				}
			}
		}
	}

	var f = new IloOplOutputFile("result.csv");
	f.writeln(csv);

	/*writeln('Beta');
	writeln(beta1);
	writeln('W');
	writeln(w);*/

	/*writeln('Janelas ' + penalidades[1]);
	writeln('Intevalo de trabalho ' + penalidades[2]);
	writeln('Horário compacto ' + penalidades[3]);
	writeln('Aulas aos sábados ' + penalidades[4]);
	writeln('Aulas seguidas ' + penalidades[5]);
	writeln('Aulas difiíceis seguidas ' + penalidades[6]);
	writeln('Aulas difíceis no último horário ' + penalidades[7]);
	writeln('Preferência do professor (disciplinas) ' + penalidades[8]);
	writeln('Preferência do professor (disciplinas) ' + penalidades[9]);*/
}
