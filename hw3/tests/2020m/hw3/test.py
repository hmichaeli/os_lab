#!/usr/bin/python

import os
import time
import pyPolicy

def test_sleep():
	"""Verify sleep."""
        pid = os.getpid()

        cpid = os.fork()
        if (cpid == 0):
            #
            # In child, spin for 2-3 seconds before exiting
            #
	    for i in range(3):
	        for k in xrange(12000000):
        	    pass

            os._exit(0)

        
        pyPolicy.set_policy(cpid, 1, 10)
	start_time = int(time.time())

        # Wait for child to exit
	os.wait()
 
	end_time = int(time.time())
	assert (end_time-start_time > 10)
        assert (end_time-start_time < 20)
	
if __name__ == "__main__":
    test_sleep()


