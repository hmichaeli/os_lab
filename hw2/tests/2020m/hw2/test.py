#!/usr/bin/python

import os
import pyTodoTasks
print("imports don")

def test1():
	print("test1:")
	"""simple test to invoke the TODO syscalls."""

        pid = os.getpid()
       	print("pid: ", pid) 
        #
        # Calling system api's.
        #
        print("push")
	pyTodoTasks.push_TODO(pid, "test1")
        print("peek")
	pyTodoTasks.peek_TODO(pid, 100)
        print("POP")
	pyTodoTasks.pop_TODO(pid)
	print("done")
def test2():
	"""Verify simple self TODO task."""
        pid = os.getpid()
        
        #
        # Basic write to 'one self'
        #
        pyTodoTasks.push_TODO(pid, "test2")
        
        #
        # Check peeked todo is as expected
        #
        rx_description, rx_bytes_number = pyTodoTasks.peek_TODO(pid, 100)
        print("rx_description  ", rx_description)
	print("rx_bytes_number  ", rx_bytes_number)
	#assert (rx_description == "test2")
        #assert (rx_bytes_number == len("test2"))
	
if __name__ == "__main__":
    test1()
    test2()


