#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/time.h>
#include <assert.h>
#include <libgearman/gearman.h>         // gearman include

void initLog();
int  initGearman();
int  sendVoicePath(const std::string& path);
int  sendToGearman(const char* data, size_t size);

gearman_client_st* kGearmanClient = NULL;

std::string kServiceName;

size_t kVoiceTotalCount = 0;
size_t kVoiceFailCount = 0;
size_t kTestCount = 0;
std::vector<std::string> kVoiceFailPaths;

size_t processcount = 0;
struct timeval workerstart;
void* echo(gearman_job_st *job, void *context, size_t *result_size, gearman_return_t *ret_ptr)
{
    context = context;
    processcount++;
    if (processcount==kTestCount)
    {
        struct timeval workerend;
        gettimeofday(&workerend, NULL);
        float c = (workerend.tv_sec - workerstart.tv_sec) * 1000 + (workerend.tv_usec - workerstart.tv_usec) * 1.0 / 1000;

        printf("######   Game Over   ######\n");
        printf("There are pop %ld jobs\n", processcount);
        printf("Pop elapsed %f ms, and qps is %f\n", c, kTestCount * 1.0 / c * 10000);
        exit(0);
    }
    *result_size = 0;
    //printf("%ld\n", processcount);
    *ret_ptr = GEARMAN_SUCCESS;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("uage : %s -[p|c] testcount\n", argv[0]);
        printf("example: %s -p 100\n");
        printf("example: %s -c 100\n");
        return 0;
    }

    kServiceName ="echo";
    assert(!kServiceName.empty());

    int ret = initGearman();
    assert(ret == 0);

    kTestCount = atoi(argv[2]);
    std::string tag(argv[1]);
    if (tag == "-p") //producer
    {
        std::string hello("hello");
        size_t size = hello.size();

        struct timeval start,end;
        gettimeofday(&start, NULL);
        for (size_t i = 0; i < kTestCount; i++)
        {
            sendToGearman(hello.c_str(), size);
        }
        gettimeofday(&end, NULL);
        float c = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) * 1.0 / 1000;
        printf("######   Game Over   ######\n");
        printf("There are push %ld jobs, and %ld is fail!\n", kVoiceTotalCount, kVoiceFailCount);
        printf("Push elapsed %f ms, and qps is %f\n", c, kTestCount * 1.0 / c * 10000);
    }
    else
    {
        gearman_worker_st worker;
        gearman_worker_create(&worker);

        gearman_return_t ret = gearman_worker_add_server(&worker, "localhost", 4730);
        if (gearman_failed(ret))
        {
            return EXIT_FAILURE;
        }

        ret = gearman_worker_add_function(&worker, kServiceName.c_str(), 0, echo, NULL);
        if (gearman_failed(ret))
        {
            return EXIT_FAILURE;
        }

        gettimeofday(&workerstart, NULL);
        //Infinite loop to keep watching
        while (1)
        {
            //Does the work
            ret = gearman_worker_work(&worker);
            if (ret != GEARMAN_SUCCESS)
            {
                fprintf(stderr, "worker error: %s\n", gearman_worker_error(&worker));
                gearman_job_free_all(&worker);
                gearman_worker_free(&worker);
            }
        }
    }

    gearman_client_free(kGearmanClient);
    return 0;
}

int initGearman()
{
    kGearmanClient= gearman_client_create(NULL);
    assert(kGearmanClient);
    gearman_return_t ret = gearman_client_add_server(kGearmanClient, "localhost", 4730);
    if (gearman_failed(ret))
    {
        printf("gearman_client_add_server failed!\n");
        return -1;
    }
    return 0;
}

int  sendVoicePath(const std::string& path)
{
    gearman_job_handle_t job_handle;
    gearman_return_t rc = gearman_client_do_background(kGearmanClient,
                            kServiceName.c_str(),
                            NULL,
                            path.c_str(),
                            path.size(),
                            job_handle);
    if (gearman_success(rc))
    {
        //LOG_DEBUG("return %s", job_handle);
        return 0;
    }

    return -1;
}

int  sendToGearman(const char* data, size_t size)
{
    gearman_job_handle_t job_handle;
    gearman_return_t rc = gearman_client_do_background(kGearmanClient,
        kServiceName.c_str(),
        NULL,
        data,
        size,
        job_handle);
    if (gearman_success(rc))
    {
        //LOG_DEBUG("return %s", job_handle);
        return 0;
    }

    return -1;
}
