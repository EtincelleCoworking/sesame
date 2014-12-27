<?php

class MyApplication extends Silex\Application{
    protected $log_handle;
    protected function openLog(){
        if(null == $this->log_handle){
            $this->log_handle = fopen('records.log', 'a+');
        }
    }

    public function log($content){
        $this->openLog();
        fputs($this->log_handle, $content."\n");
        $this->closeLog();
    }

    protected function closeLog(){
        fclose($this->log_handle);
    }
}