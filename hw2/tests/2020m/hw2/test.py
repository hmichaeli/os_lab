#!/usr/bin/python

import os
import pyTodoTasks

def test1():
	"""simple test to invoke the TODO syscalls."""

        pid = os.getpid()
        
        #
        # Calling system api's.
        #
        pyTodoTasks.push_TODO(pid, "test1")
        pyTodoTasks.peek_TODO(pid, 100)
        pyTodoTasks.pop_TODO(pid)

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
        assert (rx_description == "test2")
        assert (rx_bytes_number == len("test2"))
	
if __name__ == "__main__":
    test1()
    test2()


