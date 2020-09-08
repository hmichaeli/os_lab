hwgrader
========
A package for writing test for linux_kernel course. This package
contains several modules that simplify writing tests.

To install the package cd into the root folder of the package and
enter:

 python setup.py install

Compiling the custom kernel:
----------------------------
The package includes a script to help in building the custom kernel.
The script is called 'make_changed.py'. It is useful as it copies
some modified kernel files that are necessary for logging of memory
allocation and release. It can also make a backup copy of the custom
kernel. To use it enter:

    > make_changed.py <path to your submission files>

Using the `-r` flag will reset the custom kernel files using the backup
copy:

    > make_changed.py -r 

Using the `-s` flag will skip copying the custom kernel which can
save you time:

    > make_changed.py -s <path to your submission files>

tests:
--------
 python tests/2020s/hw2/test.py


[1]: http://docs.python.org/release/2.2.1/lib/module-unittest.html
