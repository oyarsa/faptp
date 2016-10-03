# faptp

## Geração de matrizes de horário para instituições de ensino superior privadas
faptp é o projeto que implementa os algoritmos descritos em Silva et al., 2016.
Atualmente consiste em um Algoritmo Genético para a evolução das matrizes de horário
e uma implementação do GRASP para geração das grades de disciplinas dos estudantes.
No futuro serão implementados algoritmos da literatura a título de comparação.

## Algortimos implementados
- Algoritmo genético
  - População inicial: geração aleatória
  - Seleção: torneio n-ário
  - Cruzamento: OX, CX, PMX, entre outros
  - Mutação: swap
  - FO: soma das FOs das grades geradas a partir da solução
- GRASP (resolução das grades)
  - Construção: aleatória
  - Busca local: remoção e reinserção aleatórios
  - FO: número de horas-aula semanais
- SA-ILS (Fonseca 2013)
  - SA: comum com reheating
  - ILS: comum com perturbações e descent phase baseado em Random Non Descent
- WDJU (Wilke e Killer 2010)
  - Utiliza fluxo de rapid descent a um ótimo local seguido de perturbação
    para encontrar um outro ótimo, buscando encontrar o ótimo local
- HySST (Kheiri 2014)
  - Hyper-heurística que navega o espaço de busca através de dois estágios alternados
  - Estágio de mutação: heurísticas de movimentação de vizinhanca para mutação da solução
  - Estágio de hill-climbing: heurísticas de hill climbing para realizar movimentos
    grandes na solução.

## Movimentos implementados
- Event Swap
- Event Move
- Resource Swap
- Resource Move
- Permute Resoruces
- Kempe Chain
- First Improvement
- Ejection Chain
  
## Dependências
- C++14 (testado com Visual C++ 2015 Update 2)
- Guidelines Support Library (testado com a implementação da Microsoft, não incluso no repositório)
- JsonCpp (incluso no repositório em src/includes)
- Se o modelo matemático for utilizado (configurar com MODELO = True no CMakeLists.txt), uma instalação
  do CPLEX é necessária

## Orientações
- O código é escrito em C++14. O estilo utilizado é o C++ Moderno. Refira-se à https://github.com/isocpp/CppCoreGuidelines.
- O código deve compilar sem warnings no -Wall do GCC e no /W4 do Visual Studio.
- O código não deve gerar mensagens de erro na ferramenta Cpp Core Checker, ou no Checker do clang-tidy.
  - Exceções: o arquivo UUID.cpp, que utiliza técnicas que as Guidelines proibem, teve suas warnings suprimidas
    - A supressão de warnings deve ser de comum acordo
    - Os arquivos da biblioteca JsonCpp também disparam warnings. Elas devem ser ignoradas.
- O projeto inclui muitas instâncias que violam as Guidelines. Modificações para conformar essas
  instâncias são bem-vindas. Commits que incluam códigos que não sigam as Guidelines estão sujeitos a discussão.

Copyright 2016 Italo Silva, Saulo Campos, Pedro Mázala  
