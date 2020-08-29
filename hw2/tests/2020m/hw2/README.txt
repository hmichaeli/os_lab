To compile the python extension put your 'todo_api.h' header file in this
folder and type the following command in the terminal:

python setup.py build_ext -b .

If the compilation succeeds a new file will be created: 'pyTodoTasks.so'.
This extension presents four functions that call your new system calls:
1) push_TODO
2) peek_TODO
3) pop_TODO

You can use this functions in a python script or directly from the python
interpreter, type 'python' in the terminal and then the following commands:

>>>import pyTodoTasks
>>>import os
>>>pyTodoTasks.push_TODO(os.getpid(), 'feed dog')
>>>pyTodoTasks.peek_TODO(os.getpid(), 100)
feed dog, 5

The syntax of the command can be found by typing the following in the python
interpreter:

>>>import pyTodoTasks
>>>print pyTodoTasks.push_TODO.__doc__

You can also use the ipython interpreter (you can find the rpm package in the
course website). After running ipython (type 'ipython' in the terminal) do:

[1] import pyTodoTasks
[2] pyTodoTasks.push_TODO?
