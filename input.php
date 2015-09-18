<?php
  $disciplinas = array(
    "disciplinas" => array(
      array("id" => 'A1', "nome" => 'A1', "carga" => 2, "periodo" => '1', "curso" => 'C', "prerequisitos" => array()),
      array("id" => 'B' , "nome" => 'B' , "carga" => 1, "periodo" => '1', "curso" => 'C', "prerequisitos" => array()),
      array("id" => 'C' , "nome" => 'C' , "carga" => 1, "periodo" => '1', "curso" => 'C', "prerequisitos" => array()),
      array("id" => 'D' , "nome" => 'D' , "carga" => 1, "periodo" => '1', "curso" => 'C', "prerequisitos" => array()),
      
      array("id" => 'A2', "nome" => 'A2', "carga" => 2, "periodo" => '2', "curso" => 'C', "prerequisitos" => array('A1')),
      array("id" => 'E' , "nome" => 'E' , "carga" => 2, "periodo" => '2', "curso" => 'C', "prerequisitos" => array()),
      array("id" => 'F' , "nome" => 'F' , "carga" => 2, "periodo" => '3', "curso" => 'C', "prerequisitos" => array()),
    ),
    "professores" => array(
      array("id" => 'P1', "nome" => 'P1', "competencias" => array('A1', 'A2'), "diasdisponiveis" => array()),
      array("id" => 'P2', "nome" => 'P2', "competencias" => array('B', 'E', 'F'), "diasdisponiveis" => array()),
      array("id" => 'P3', "nome" => 'P3', "competencias" => array('C', 'D'), "diasdisponiveis" => array()),
    ),
    "professordisciplinas" => array(
      array("id" => 'A1', "professor" => 'P1', "disciplina" => 'A1'),
      array("id" => 'A2', "professor" => 'P1', "disciplina" => 'A2'),
      
      array("id" => 'B' , "professor" => 'P2', "disciplina" => 'B' ),
      array("id" => 'E' , "professor" => 'P2', "disciplina" => 'E' ),
      array("id" => 'F' , "professor" => 'P2', "disciplina" => 'F' ),
      
      array("id" => 'C' , "professor" => 'P3', "disciplina" => 'C' ),
      array("id" => 'D' , "professor" => 'P3', "disciplina" => 'D' ),
    ),
    "alunoperfis" => array(
      array("id" => 'NOVO', "peso" => 1, "restantes" => array('A1', 'A2', 'B', 'C', 'D', 'E', 'F'), "cursadas" => array()),
      array("id" => 'PAU', "peso" => 1, "restantes" => array('A1', 'A2', 'E', 'F'), "cursadas" => array()),
      array("id" => 'PASSOU', "peso" => 1, "restantes" => array('A2', 'E', 'F'), "cursadas" => array('A1')),
      array("id" => 'GRADE', "peso" => 1, "restantes" => array('A1', 'A2', 'B'), "cursadas" => array()),
      array("id" => 'GRADE_PARCIAL', "peso" => 1, "restantes" => array('A2', 'E'), "cursadas" => array('A2')),
    ),
    "horario" => array(
      /**
       * 0 é segunda
       */
      array("horario" => 0, "semana" => 0, "camada" => 0, "disciplina" => 'A1'),
      array("horario" => 1, "semana" => 0, "camada" => 0, "disciplina" => 'B' ),
      array("horario" => 0, "semana" => 2, "camada" => 0, "disciplina" => 'C' ),
      array("horario" => 1, "semana" => 2, "camada" => 0, "disciplina" => 'A1'),
      array("horario" => 0, "semana" => 4, "camada" => 0, "disciplina" => 'D' ),
      
      array("horario" => 0, "semana" => 0, "camada" => 1, "disciplina" => 'E' ),
      array("horario" => 1, "semana" => 0, "camada" => 1, "disciplina" => 'A2'),
      array("horario" => 0, "semana" => 2, "camada" => 1, "disciplina" => 'E' ),
      array("horario" => 1, "semana" => 2, "camada" => 1, "disciplina" => 'F' ),
      array("horario" => 0, "semana" => 4, "camada" => 1, "disciplina" => 'F' ),
      array("horario" => 1, "semana" => 4, "camada" => 1, "disciplina" => 'A2'),
    ),
  );
  
  System::debug(Json::encode($disciplinas));
