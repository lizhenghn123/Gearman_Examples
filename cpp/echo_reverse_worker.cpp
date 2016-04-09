#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <functional>
#include <libgearman/gearman.h>

// typedef void* (gearman_worker_fn)(gearman_job_st *job, void *context, size_t *result_size, gearman_return_t *ret_ptr);
void* echo(gearman_job_st *job, void *context, size_t *result_size, gearman_return_t *ret_ptr)
{
    int workloadsize;

    /* contect is unused */
    context = context;
    workloadsize = gearman_job_workload_size(job); //how big is the workload?

    //Copy the workload into a usable string
    char workload[workloadsize];
    strncpy(workload, (const char *)gearman_job_workload(job), workloadsize);
    workload[workloadsize] = '\0';

    //Do your work here
    printf("worker[echo] get job : [%s]\n", workload);

    //if not passing any result information back at the moment, set result_size = 0, and return 0
    char buffer[2048];
    strncpy(buffer, workload, 2048);
    *result_size = strlen(buffer);

    // Should do some checks to monitor for fails all we're doing here is just printing the workload
    *ret_ptr = GEARMAN_SUCCESS;
    sleep(10);
    printf("worker[reverse] process finish!\n");
    return strdup(buffer);
}


void* reverse_work(gearman_job_st *job, void *context, size_t *result_size, gearman_return_t *ret_ptr)
{
    int workloadsize;

    /* contect is unused */
    context = context;
    workloadsize = gearman_job_workload_size(job); //how big is the workload?

    //Copy the workload into a usable string
    char workload[workloadsize];
    strncpy(workload, (const char *)gearman_job_workload(job), workloadsize);
    workload[workloadsize] = '\0';

    //Do your work here
    printf("worker[reverse] get job : [%s]\n", workload);

    // Should do some checks to monitor for fails all we're doing here is just printing the workload
    *ret_ptr = GEARMAN_SUCCESS;

    std::string strs(workload);
    std::reverse(strs.begin(), strs.end());
   
    for (size_t y= 0, x= workloadsize; x; x--, y++)
    {
        if (gearman_failed(gearman_job_send_data(job, &workload[y], 1)))
        {
            printf("Job send data error\n");
            return NULL;                    
        }

        if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workloadsize)))
        {
            printf("Job send status error\n");
            return NULL;                    
        }
        sleep(1);            
    }
    /**
    if (gearman_failed(gearman_job_send_data(job, &*strs.begin(), strs.size())))
    {
        printf("Job send data error\n");
        *ret_ptr = GEARMAN_WORK_ERROR;
        return NULL;
    }
    uint32_t y = 0;
    if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workloadsize)))
    {
        printf("Job send status error\n");
        *ret_ptr = GEARMAN_WORK_ERROR;
        return NULL;                
    }
    **/

    char* result = strdup(strs.c_str());
    *result_size = strs.size();
    printf("worker[reverse] process finish!\n");
    return result;
}

int main(void)
{
    gearman_worker_st worker;

    //Create the worker
    gearman_worker_create(&worker);

    //Connect the worker to the server and chech it worked
    gearman_return_t ret = gearman_worker_add_server(&worker, "localhost", 4730);
    if(gearman_failed(ret))
    {
        return EXIT_FAILURE;
    }

    //register function, (worker, job_queue_name, timeout, function_to_do_work, context)
    ret = gearman_worker_add_function(&worker, "echo", 0, echo, NULL);
    if(gearman_failed(ret))
    {
        return EXIT_FAILURE;
    }

    ret = gearman_worker_add_function(&worker, "reverse", 0, reverse_work, NULL);
    if(gearman_failed(ret))
    {
        return EXIT_FAILURE;
    }

    //Infinite loop to keep watching
    while(1)
    {
        //Does the work
        ret = gearman_worker_work(&worker);
        if(ret != GEARMAN_SUCCESS)
        {
            fprintf(stderr, "worker error: %s\n", gearman_worker_error(&worker));
            gearman_job_free_all(&worker);
            gearman_worker_free(&worker);
            /* sleep on error to avoid cpu intensive infinite loops */
            sleep(5);
        }
    }
    return EXIT_SUCCESS;
}
