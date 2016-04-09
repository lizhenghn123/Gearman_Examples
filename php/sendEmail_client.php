<?php

    // ... handle form validation, etc
    $email = 'joe@hamburger.com';
    $subject = "Eat @ Joe's!";
    $body = 'They have the best ceaser salad!';
    // ... now the good stuff

    $client = new GearmanClient();
    $client->addServer();
    $result = $client->doBackground("send_email", json_encode(array(
        // whatever details you gathered from the form
        'email' => $email,
        'subject' => $subject,
        'body' => $body
     )));

//    $client= new GearmanClient();
//    $client->addServer('127.0.0.1', 4730);
//    echo $client->doBackground('echo', 'Hello Gearman'), "\n";
//    echo $client->do('echo', 'Hello World!'), "\n";
?>
