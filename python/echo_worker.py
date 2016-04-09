#!/usr/bin/env python2.6
# coding=utf-8
import os
import gearman
import math

class MyGearmanWorker(gearman.GearmanWorker):
    def on_job_execute(self, current_job):
        print "Job started"
        print "===================\n"
        return super(MyGearmanWorker, self).on_job_execute(current_job)


def task_callback(gearman_worker, gearman_job):
    print gearman_job.data
    print "-----------\n"
    return gearman_job.data
    

my_worker = MyGearmanWorker(['127.0.0.1:4730'])
my_worker.register_task("echo", task_callback)
my_worker.work()
