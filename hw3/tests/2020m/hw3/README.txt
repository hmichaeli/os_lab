To compile the python extension put your 'policy_api.h' header file in this
folder and type the following command in the terminal:

python setup.py build_ext -b .

If the compilation succeeds a new file will be created: 'pyPolicy.so'.
This extension presents four functions that call your new system calls:
1) get_policy
2) set_policy

You can use this functions in a python script or directly from the python
interpreter, type 'python' in the terminal and then the following commands:

>>>import pyPolicy
>>>import os
>>>pyPolicy.set_policy(os.getpid(), 1,10)
>>>pyPolicy.get_policy(os.getpid())

The syntax of the command can be found by typing the following in the python
interpreter:

>>>import pyPolicy
>>>print pyPolicy.get_policy.__doc__

You can also use the ipython interpreter (you can find the rpm package in the
course website). After running ipython (type 'ipython' in the terminal) do:

[1] import pyPolicy
[2] pyPolicy.get_policy?
