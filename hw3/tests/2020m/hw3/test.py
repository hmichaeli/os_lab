#!/usr/bin/python

import os
import time
import pyPolicy
import time

def debug_dmesg(repeats=5, interval=2):
    for i in range(repeats):
        os.system('dmesg')
        print("\n\n\n")
        time.sleep(interval)

def log(*args):
    pid = os.getpid()    
    date_time = time.localtime()
    print(pid,date_time[3:6], args)


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

def test0():
    pass

def test1():
    ''' put self to sleep '''
    pid = os.getpid()
    print("pid: ",pid)
    date_time = time.localtime()
    print("date and time:",date_time)	
    t1 = date_time[5]
    pyPolicy.set_policy(pid, 1, 10)
    date_time = time.localtime()
    print("date and time:",date_time)
    t2 = date_time[5]
    sleep_time = t2 - t1
    if sleep_time == 10:
        print("~~~~ test 1 passed ~~~~ ")
        return 0
    else:
        print("sleep_time: %d, nned to be: %d", sleep_time, 10)
	

def test2():
    ''' put child to sleep '''
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
            for k in xrange(2000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        date_time = time.localtime()
        print("[parent] set child policy date and time:",date_time)
        pyPolicy.set_policy(cpid, 1, 10)
        print("[parent] wait")
        os.wait()
        print("[parent] done")
    	# print("[parent] wait:",date_time)
        # time.sleep(30)
    	# print("[parent] kill child:",date_time)
        # os.kill(cpid, 9)
    	# print("[parent] done:",date_time)


def test3():
    ''' policy 2 child '''
    pid = os.getpid()
    log("[parent] pid: ", pid)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
        log("[child] pid: ", pid)
    # In child, spin for 2-3 seconds before exiting
        for i in range(20):
            log("[child]")
            for k in xrange(2000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        log("[parent] set child policy date and time:")
        pyPolicy.set_policy(cpid, 2, 10)
    	# print("[parent] wait:",date_time)
        log("[parent] wait child:")
       	os.wait()
    	log("[parent] done:")

def test4():
    ''' policy 2 self '''
    pid = os.getpid()
    print("pid: ",pid)
    log("set policy 2 to self")
    pyPolicy.set_policy(pid, 2, 5)
    for i in range(20):
        log("process alive")
        for k in xrange(2000000):
            pass

def test5():
    ''' policy 2 child does to self '''
    pid = os.getpid()
    log("[parent] pid: ", pid)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
        log("[child] pid: ", pid)
        log("[child] set policy 2 to self")
        pyPolicy.set_policy(pid, 2, 5)
        for i in range(20):
            log("[child] process alive")
            for k in xrange(2000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        log("[parent] wait child:")
       	os.wait()
    	log("[parent] done:")

def test6():
    ''' policy 1 to child twice '''
    pid = os.getpid()
    print("[parent] pid: ", pid)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
        print("[child] pid: ", pid)
        for i in range(20):
            date_time = time.localtime()
            log("[child]")
            for k in xrange(2000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        date_time = time.localtime()
        log("[parent] set child policy [1]")
        pyPolicy.set_policy(cpid, 1, 5)
        time.sleep(1)
        log("[parent] set child policy [2]")
        pyPolicy.set_policy(cpid, 1, 5)
        debug_dmesg()
        log("[parent] wait")
        os.wait()
        log("[parent] done")

def test7():
    """Verify rescue from terminate."""
    pid = os.getpid()
    print('Parent PID: %d' % os.getpid())

    cpid = os.fork()
    if (cpid == 0):
        print('Child PID: %d' % os.getpid())
        #
        # In child, spin for 2-3 seconds before exiting
        #
        while True:
            pass

        print('before exit')
        os._exit(0)

    print('before set_policy')
    pyPolicy.set_policy(cpid, 2, 5)
    print('after set_policy')

    time.sleep(1)
    
    print('before set_policy - rescue')
    pyPolicy.set_policy(cpid, 0, 3)
    print('after set_policy - rescue')

    time.sleep(6)

    print('before set_policy - terminate')
    pyPolicy.set_policy(cpid, 2, 3)
    print('after set_policy - terminate')

    print('start_time')
    start_time = int(time.time())
    print('start_time: %d' % (start_time))

    # Wait for child to exit
    print('before wait')
    os.wait()
    print('after wait')

    end_time = int(time.time())
    print('end_time-start_time: %d' % (end_time - start_time))
    assert (end_time - start_time >= 3)
    assert (end_time - start_time < 6)
	
def test8():
    ''' multiple sleeps '''
    pid = os.getpid()
    print("[parent] pid: ", pid)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
		ppid = os.get_ppid()
        print("[child] pid: ", pid)
        while True:
            date_time = time.localtime()
            log("[child] set sleep policy on dad ppid:",ppid)
			pyPolicy.set_policy(ppid,1,2)
            for k in xrange(2000000):
                pass

        os._exit(0)
    else:
        time.sleep(3)
        start_time = time.time()
		for i in range(5):
			log("[parent] set child policy [1]")
			pyPolicy.set_policy(cpid, 1, 5)
			time.sleep(1)
        log("[parent] wait")
        os.wait()
        log("[parent] done")
		end_time = time.time()
		test_time = int(end_time-start_time)
		
		print('[parent] kill child, pid: ',pid)
		pyPolicy.set_policy(pid,2,0)
		print('[parent]done ',pid)
		if  test_time > 5 && test_time < 11:
			result = "passed"
		else
			result = "failed"
			
		print ('intended ~10 second sleep , test time is %d'.format(test_time))
		print ('~~~~test 8 %s ~~~~~'.format(result))
		
def test9():
    ''' set terminate to self, fork to child, child must die, rescue self.'''
    pid = os.getpid()
    print("[parent] pid: ", pid)
	start_time = time.time()
	pyPolicy.set_policy(pid,2,10)
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
		ppid = os.get_ppid()
        print("[child] pid: ", pid)
        for i in range(20):
			myPolicy = pyPolicy.get_policy(pid)
			print("[child] my policy is(policy,val):", myPolicy)
            time.sleep(1)
        os._exit(0)
    else:
		myPolicy = pyPolicy.get_policy(pid)
		print("[parent] child policy is(policy,val):", myPolicy)
		time.sleep(3)
		log("[parent] save myself")
		pyPolicy.set_policy(pid,0,0)
        os.wait()
        log("[parent] done")
		end_time = time.time()
		test_time = int(end_time-start_time)
		
		if  test_time > 8 && test_time < 11:
			result = "passed"
		else
			result = "failed"
			
		print ('intended ~10 second untill child dies , test time is %d'.format(test_time))
		print ('~~~~test 9 %s ~~~~~'.format(result))

def test10():
    ''' set sleep to self, fork to child, set more sleep to child.'''
    pid = os.getpid()
    print("[parent] pid: ", pid)
	start_time = time.time()
	pyPolicy.set_policy(pid,1,5)
	log("[parent] forking to child, he should sleep for 5 sec")
    cpid = os.fork()
    if (cpid == 0):
        pid = os.getpid()
        print("[child] pid: ", pid)
        for i in range(5):
			log("[child]")
            time.sleep(1)
        os._exit(0)
    else:
		myPolicy = pyPolicy.get_policy(pid)
		print("[parent] child policy is(policy,val):", myPolicy)
		time.sleep(2)
		sleep_time = 5
		log("[parent] set child to sleep more time:",sleep_time)
		pyPolicy.set_policy(cpid,1,sleep_time)
        os.wait()
        log("[parent] finished waiting")
		end_time = time.time()
		test_time = int(end_time-start_time)
		
		if  test_time > 20-2 && test_time < 20+2:
			result = "passed"
		else
			result = "failed"
			
		print ('intended ~20 second untill child dies (5parent sleep + 5 child + 5 child + 5 print  , test time is %d'.format(test_time))
		print ('~~~~test 10 %s ~~~~~'.format(result))


def run_all_tests():
    total_score = 0
    TEST_NUM = 7
    for i in range(TEST_NUM):
        print("run test %d", i)
        locals()['test' + str(i)]



if __name__ == "__main__":
    os.system('dmesg -c > /dev/null')
    # test1()
    test2()
    # test3()
    # test5()
    # test6()
   # test_sleep()


