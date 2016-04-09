#!/usr/bin/env python2.6
#!/usr/bin/env python2.7
# coding=utf-8
from gearman import GearmanClient

gearman_client = GearmanClient(['127.0.0.1:4730'])

gearman_request = gearman_client.submit_job('echo', 'test gearman')

result_data = gearman_request.result
print result_data
