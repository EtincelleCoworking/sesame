<?php

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;

require_once __DIR__.'/../vendor/autoload.php';
require_once __DIR__.'/../app/MyApplication.php';

$app = new MyApplication();
$app['debug'] = true;

$app->post('/api/log/fingerprint', function(Request $request) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('log - when: [%s], kind: [%s], fingerprint_id: [%s], result: [%s]',
        $request->get('when'),
        $request->get('kind'),
        $request->get('fingerprint_id'),
        $request->get('result')
    ));
    return new Response('OK', 200);
});

$app->post('/api/log/intercom', function(Request $request) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('log - when: [%s], numpresses: [%s], result: [%s]',
        $request->get('when'),
        $request->get('numpresses'),
        $request->get('result')
    ));
    return new Response('OK', 200);
});

$app->post('/api/user', function(Request $request) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('createUser - id: [%s], email: [%s]', $request->get('id'), $request->get('email')));
    return new Response('OK', 200);
});

$app->post('/api/user/{user_id}/fingerprint', function($user_id, Request $request) use($app) {
    /** @var MyApplication $app */
    $app->log(sprintf('createFingerprint - user_id: [%s], id: [%s], image: [%s]',
        $user_id,
        $request->get('id'),
        $request->get('image')));
    return new Response('OK', 200);
});

$app->get('/', function() use($app) {
    return new Response('Etincelle Coworking - Sesame', 200);
});

$app->run();
