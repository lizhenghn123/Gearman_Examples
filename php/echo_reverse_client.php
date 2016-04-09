<?php
    $client= new GearmanClient();
    $client->addServer('127.0.0.1', 4730);
    echo $client->doBackground('echo', 'Hello Gearman'), "\n";
    echo $client->do('echo', 'Hello World!'), "\n";
?>
