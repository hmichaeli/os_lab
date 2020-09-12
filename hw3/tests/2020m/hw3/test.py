#!/usr/bin/python

import os
import time
import pyPolicy
import time

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

def test1():
    pid = os.getpid()
    print("pid: ",pid)
    date_time = time.localtime()
    print("date and time:",date_time)	
    pyPolicy.set_policy(pid, 1, 10)
    date_time = time.localtime()
    print("date and time:",date_time)	

def test2():
    pid = os.getpid()
    print("[parent] pid: ", pid)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
        print("[child] pid: ", pid)
    # In child, spin for 2-3 seconds before exiting
        for i in range(20):
            date_time = time.localtime()
            print("[child] date and time:",date_time)
            for k in xrange(12000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        date_time = time.localtime()
        os.system('dmesg')
        time.sleep(1)
        print("\n\n\n")
        print("[parent] set child policy date and time:",date_time)
        pyPolicy.set_policy(cpid, 1, 5)
        
        for i in range(4):
            os.system('dmesg')
            print("\n\n\n")
            time.sleep(2)
        
        print("[parent] wait")
        os.wait()
        print("[parent] done")
    	# print("[parent] wait:",date_time)
        # time.sleep(30)
    	# print("[parent] kill child:",date_time)
        # os.kill(cpid, 9)
    	# print("[parent] done:",date_time)


def test3():
    pid = os.getpid()
    log("[parent] pid: ", pid)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
        log("[child] pid: ", pid)
    # In child, spin for 2-3 seconds before exiting
        for i in range(20):
            log("[child]")
            for k in xrange(12000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        log("[parent] set child policy date and time:")
        pyPolicy.set_policy(cpid, 2, 5)
        os.system('dmesg')
        time.sleep(2)
        os.system('dmesg')
        time.sleep(2)
        os.system('dmesg')
        time.sleep(2)
    	# print("[parent] wait:",date_time)
        time.sleep(20)
    	log("[parent] kill child:")
        os.kill(cpid, 9)
    	log("[parent] done:")

if __name__ == "__main__":
    os.system('dmesg -c > /dev/null')
#    test1()
    test2()
   # test_sleep()


