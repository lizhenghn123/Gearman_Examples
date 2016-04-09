#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgearman/gearman.h>

//http://gearman.info/libgearman/gearman_client_do_background.html
//http://gearman.info/libgearman/gearman_client_options.html
int main(void)
{
  gearman_client_st *client= gearman_client_create(NULL);

  gearman_return_t ret= gearman_client_add_server(client, "localhost", 0);
  if (gearman_failed(ret))
  {
    return EXIT_FAILURE;
  }

  gearman_job_handle_t job_handle;
  gearman_return_t rc= gearman_client_do_background(client,
                                                    "echo",
                                                    NULL,// unique Optional unique job identifier, or NULL for a new UUID.如果写成唯一值只会提交一个任务（即使重复提交）
                                                    "hello-world",
                                                    strlen("hello-world"),
                                                    job_handle);

  if (gearman_success(rc))
  {
    // Make use of value
    printf("return %s\n", job_handle);
  }

  gearman_client_free(client);

  return 0;
}