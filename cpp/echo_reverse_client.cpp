#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgearman/gearman.h>

// see http://gearman.info/libgearman/gearman_execute.html
int echo(gearman_client_st *client)
{
	const char *echo_value = "test gearman";
    gearman_argument_t value = gearman_argument_make(0, 0, echo_value, strlen(echo_value));

    gearman_task_st *task = gearman_execute(client,
                                            "echo", strlen("echo"),  // function
                                            NULL, 0,  				 // no unique value provided
                                            NULL,
                                            &value, 0);

    if(task == NULL)  // If gearman_execute() can return NULL on error
    {
        fprintf(stderr, "Error: %s\n", gearman_client_error(client));
        gearman_client_free(client);
        return EXIT_FAILURE;
    }

    // Make sure the task was run successfully
    if(gearman_success(gearman_task_return(task)))
    {
        // Make use of value
        gearman_result_st *result = gearman_task_result(task);
        printf("job[echo] result size: %d, str: '%s'\n", (int)gearman_result_size(result), gearman_result_value(result));
    }
    else
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int reverse(gearman_client_st *client)
{
    const char *reverse_value = "test gearman";
    gearman_argument_t value = gearman_argument_make(0, 0, reverse_value, strlen(reverse_value));
    gearman_task_attr_t workload = gearman_task_attr_init_background(GEARMAN_JOB_PRIORITY_NORMAL);

    gearman_task_st *task = gearman_execute(client,
                                            "reverse", strlen("reverse"),  // function
                                            NULL, 0,  					   // no unique value provided
                                            &workload,
                                            &value, 0);

    if(task == NULL)  // If gearman_execute() can return NULL on error
    {
        fprintf(stderr, "Error: %s\n", gearman_client_error(client));
        gearman_client_free(client);
        return EXIT_FAILURE;
    }

    bool is_known = true;
    do
    {
        bool is_running;
        uint32_t numerator;
        uint32_t denominator;
        gearman_return_t ret;

        ret = gearman_client_job_status(client, gearman_task_job_handle(task),
                                        &is_known, &is_running,
                                        &numerator, &denominator);

        printf("is_known: %d, is_running: %d, ret: %d\n", is_known, is_running, gearman_continue(ret));
        printf("numerator: %d, denominator: %d\n", numerator, denominator);

        size_t data_size;
        char *data = (char*)gearman_task_take_data(task, &data_size);
        printf("Task data: %s-%d\n", data, data_size);
        printf("gearman_task_context: %s\n", gearman_task_context(task));
        printf("%d/%d\n", gearman_task_numerator(task), gearman_task_denominator(task));

        // Non-blocking event occurred, try again
        if(gearman_continue(ret))
        {
            printf("worker is doing...\n");
            continue;
        }
        else if(gearman_failed(ret))
        {
            printf("failed %s\n", gearman_client_error(client));
            return EXIT_FAILURE;
            break;
        }
        sleep(1);
    }while(is_known);

    return EXIT_SUCCESS;
}

int main()
{
    gearman_client_st *client = gearman_client_create(NULL);

    gearman_return_t ret = gearman_client_add_server(client, "localhost", 4730);
    if(gearman_failed(ret))
    {
        return EXIT_FAILURE;
    }

    printf("######   run echo test   ######\n");
    int t = echo(client);
    if(t != EXIT_SUCCESS)
    {
        printf("run echo test failure\n");
        return t;
    }

    printf("######   run reverse test   ######\n");
    t = reverse(client);
    if(t != EXIT_SUCCESS)
    {
        printf("run echo test failure\n");
        return t;
    }

    gearman_client_free(client);

    return 0;
}
