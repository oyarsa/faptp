<?php

function getJsonHorarioCtrl($where, $group, $order, $hra_tipo = 1, $encode = false, $validate = true, $debug = false) {

  System::import('d', 'academico', 'Horario', 'src', true, '{project.rsc}');

  $horarioDAO = new HorarioDAO(PATH_APP, null);
  $result = $horarioDAO->getDisciplinasHorarioDAO($where, $group, $order, $validate, $debug);

  $semana = array('hra_domingo', 'hra_segunda', 'hra_terca', 'hra_quarta', 'hra_quinta', 'hra_sexta', 'hra_sabado');

  $arr_horas = array();
  if ($hra_tipo == 1) {
    $arr_horas = unserialize(ARRAY_HORAS_NOTURNAS);
  } else if ($hra_tipo == 2) {
    $arr_horas = unserialize(ARRAY_HORAS_DIURNAS);
  } else if ($hra_tipo == 3) {
    $arr_horas = unserialize(ARRAY_HORAS_INTEGRAL);
  }

  $disciplina_horarios = null;
  if (!is_null($result)) {

    $disciplina_horarios = array();

    $periodo = 0;
    while ($row = $horarioDAO->fetchRowHorarioDAO($result)) {

      if ($row['cali_semana']) {
        $line = $row['hrai_hora_inicio'];
        $column = $semana[$row['cali_semana']];
        while (isset($disciplina_horarios[$periodo][$line][$column])) {
          $periodo++;
        }
        $disciplina_horarios[$periodo][$line][$column] = $row;
      }
    }
  }

  return $disciplina_horarios;

  $retorno = array();

  for ($k = 0; $k <= $periodo; $k++) {
    for ($i = 0; $i < count($arr_horas); $i++) {

      $line = $arr_horas[$i];

      $disciplina_horario = array();
      $columns = array();

      if (isset($disciplina_horarios[$periodo][$line])) {
        $disciplina_horario = $disciplina_horarios[$periodo][$line];
      }

      $columns['hra_inicio'] = $line;

      for ($j = 1; $j < count($semana); $j++) {

        $column = $semana[$j];

        if (isset($disciplina_horario[$column]['grdg_codigo'])) {

          $row = $disciplina_horario[$column];

          if ($encode) {
            foreach ($row as $index => $r) {
              $row[$index] = Json::encodeValue($r);
            }
          }

          $text = $row['grdd_sigla'] . ' - ' . $row['dsp_titulo'];

          $columns[$column] = $text;
        } else {
          $columns[$column] = '-';
        }
      }

      $retorno[] = $columns;
    }
  }
  $json = $retorno;

  if ($encode) {
    $json = json_encode($json);
  }

  return $json;
}

System::import('c', 'academico', 'Horario', 'src');
System::import('c', 'academico', 'HorarioItem', 'src');

$horarioCtrl = new HorarioCtrl(PATH_APP);
$horarioItemCtrl = new HorarioItemCtrl(PATH_APP);

$where = "
(
  grdg_codigo IN (358, 10851, 363, 10856, 8405, 8528, 8531, 8635, 361, 10854, 8406, 8628, 8644, 359, 10852, 8410, 8532, 8636, 362, 10855, 360, 10853, 2237, 8401, 8524, 8626, 8642, 2238, 8402, 8525, 8627, 8643, 2240, 8404, 8527, 8530, 8634, 2239, 7587, 2236, 7314, 8408, 8629, 8645, 7313, 7312, 7311, 370, 10902, 7310, 7588, 357, 10810, 11215, 8403, 8526, 8529, 8633, 11217, 8407, 8432, 8535, 11213, 11216, 8415, 11214, 11218, 11785, 11787, 368, 10861, 11789, 351, 10804, 11786, 11788, 11784, 12574, 8414, 12571, 12570, 12572, 12569, 12573, 12713, 12715, 12712, 7589, 12716, 353, 10806, 13316, 8413, 8534, 13317, 7590, 13318, 13320, 13319)
)

AND (hrai_hora_inicio NOT IN (1,2,3,4,5) AND hrai_hora_inicio LIKE '1%:%')
";

$group = "cali_semana, hrai_hora_inicio, hrai_hora_termino, grdg_codigo";

$order_periodo = "(SELECT grdi_cod_GRADE_ITEM_PERIODO FROM TBL_GRADE_ITEM WHERE grdi_codigo = grdd_cod_GRADE_ITEM)";
$order = $order_periodo . ", cali_data, hrai_hora_inicio";

$horarios = getJsonHorarioCtrl($where, $group, $order);

$z = $y = $x = 0;

foreach ($horarios as $periodo => $horario_periodo) {
  $y = 0;
  foreach ($horario_periodo as $bloco => $horarioBloco) {
    $x = 0;
    foreach ($horarioBloco as $semana => $horario_semana) {
      $teste[$z][$y][$x] = $horario_semana['grdd_sigla'] . ' - ' . $horario_semana['dsp_titulo'];
      
      $x++;
    }
    $y++;
  }
  $z++;
}
$horarios = $teste;

System::debug($horarios);