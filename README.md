# faPTP
## Timetabling solver for higher education institutions

This repository implements the algorithms describe in Silva et al. 2016 and Silva and Cunha 2017.
They consist in a Genetic Algorithm for evolving course timetables and an implementation of GRASP
for student timetables, as well as other algorithms from the literature for benchmarking.

### Implemented algorithms
- Genetic Algorithm
  - Initial population: random generation
  - Selection: n-ary tournament
  - Crossover: OX, CX, PMX, and others
  - Mutation: swap
  - Objective function:
      1. Sum of the objective functions from the student timetables
      2. Custom weighted sum of quality parameters
- GRASP (student timetable)
  - Construction: random
  - Local search: removal and random reinsertion
  - Objective function: number of allocated hours per week
- SA-ILS (Fonseca 2013)
  - Simulated Annealing: standard with reheating
  - Iterated Local Search: standard with perturbations and descent phase based in Random Non Descent
- WDJU (Wilke and Killer 2010)
  - Uses rapid descent flow to a good local optimum followed by a perturbation
    with the goal of finding another optimum, repeatedly looking for the best
    solution
- HySST (Kheiri 2014)
  - Hyper-heuristic that traverses the search space through two alternating stages
  - Mutation stage: neighbourhood movements heuristic make small changes to the solution
  - Hill-climbing stage: hill-climbing heuristics that make large changes

### Implemented movements
- Event Swap
- Event Move
- Resource Swap
- Resource Move
- Permute Resoruces
- Kempe Chain
- First Improvement
- Ejection Chain

### Dependencies
- C++14 (tested on Visual C++ 2017 and GCC 5.4)
- Libraries:
  - JsonCpp
  - fmt
  - CPR
  - CxxOpts

We use the Conan package manager. Dependencies are listed on conanfile.txt.

### Compilation
Compilation structres are on the BUILDING.md file.

### Coding guidance
- Code is written in C++14, using a modern C++ style. Refer to https://github.com/isocpp/CppCoreGuidelines.
- Code must compile without warnings with -Wall for GCC and /W4 for Visual Studio.
- Code should not generate error messages on Cpp Core Checker or clang-tidy Checker.

Copyright 2016 Italo Silva, Saulo Campos, Pedro Mázala
