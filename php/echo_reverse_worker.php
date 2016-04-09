<?php
    $worker= new GearmanWorker();
    $worker->addServer('127.0.0.1', 4730);
    $worker->addFunction('echo', 'echo_reverse_function');
    
    while ($worker->work());
    
    
    function echo_reverse_function($job)
    {
        echo $job->workload(); echo "\n";
        //exit(0);  // 测试gearmand开启持久化后，worker没有处理完就挂掉，该任务的状态（仍会被gearmand重新调度）
        return strrev($job->workload());    // 反转接收到的字符串后再返回
    }
?>
