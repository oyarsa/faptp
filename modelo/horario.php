<?php

const dias = ['Segunda', 'Terça', 'Quarta', 'Quinta', 'Sexta', 'Sábado'];
const num_dias = 6;
const num_horarios = 4;

function print_head()
{
?>

<html>
<head>
  <meta charset="utf-8" />
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bulma/0.2.3/css/bulma.min.css" />
  <style>
    table td, table th {
        text-align: center !important;
    }
  </style>
</head>

<body>
    <section class="content column is-two-thirds is-offset-2">

<?php
}

function print_footer()
{
?>

    </section>
</body>
</html>

<?php
}

function show_table($nome, $matriz) {
?>

<h1 class="has-text-centered title is-2"><?=$nome?></h1>
<table class="table is-bordered is-narrow">
<thead>
    <tr>
        <?php foreach (dias as $dia): ?>
            <th><?=$dia?></th>
        <?php endforeach ?>
    </tr>
</thead>
<tbody>
    <?php foreach ($matriz as $horarios): ?>
        <tr>
            <?php foreach ($horarios as $e): ?>
                <td>
                    <p><b><?=$e['disciplina']?></b></p>
                    <p><i><?=$e['professor']?></i></p>
                </td>
            <?php endforeach ?>
        </tr>
    <?php endforeach ?>
</tbody>
</table>

<?php
}

function empty_matrix($rows, $cols) {
    $mat = [];
    for ($i = 0; $i < $rows; $i++) {
        for ($j = 0; $j < $cols; $j++) {
               $mat[$i][$j] = [
                "professor" => '--',
                "disciplina" => '--'
            ];
        }
    }
    return $mat;
}

if ($argc !== 3) {
    echo 'Argumentos inválidos' . PHP_EOF;
    die();
}

$infile = $argv[1];
$outfile = $argv[2];

$json = file_get_contents($infile);
$horarios = json_decode($json, true);

$periodos = [];

foreach ($horarios["periodos"] as $periodo) {
    $nome = $periodo['nome'];
    $matriz = empty_matrix(num_horarios, num_dias);

    foreach ($periodo['eventos'] as $evento) {
        $h = $evento['horario'];
        $d = $evento['dia'];
        $matriz[$h][$d] = [
            "professor" => $evento['professor'],
            "disciplina" => $evento['disciplina']
        ];
    }
    $periodos[$nome] = $matriz;
}

ob_start();

print_head();
foreach ($periodos as $nome => $matriz) {
    show_table($nome, $matriz);
}
print_footer();

$resultado = ob_get_contents();
ob_end_clean();

file_put_contents($outfile, $resultado);
