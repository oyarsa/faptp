<?php

$json = file_get_contents('input.json');

file_put_contents('input.json', json_encode(json_decode($json)));
