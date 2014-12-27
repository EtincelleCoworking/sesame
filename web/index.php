<?php

use Symfony\Component\HttpFoundation\Response;

require_once __DIR__.'/../vendor/autoload.php';

$app = new MyApplication();

$app->get('/api/log', function($when, $kind, $fingerprint_id, $result) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('log - when: [%s], kind: [%s], fingerprint_id: [%s], result: [%s]', $when, $kind, $fingerprint_id, $result));
    return new Response('OK', 200);
});

$app->post('/api/user', function($id, $email) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('createUser - id: [%s], email: [%s]', $id, $email));
    return new Response('OK', 200);
});

$app->post('/api/user/{user_id}/fingerprint', function($user_id, $id, $image) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('createFingerprint - user_id: [%s], id: [%s], email: [%s]', $user_id, $id, $image));
    return new Response('OK', 200);
});

$app->run();