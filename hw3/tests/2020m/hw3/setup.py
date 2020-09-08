#!/usr/bin/python

from distutils.core import setup, Extension

module1 = Extension(
            'pyPolicy',
            define_macros = [('MAJOR_VERSION', '1'),
                             ('MINOR_VERSION', '0')],
            sources = ['py_policy.c']
            )

setup(
    name = 'PackageName',
    version = '1.0',
    description = 'system calls wrappers',
    author = 'X',
    author_email = 'X@tx',
    url = 'http://www.ee.technion.ac.il',
    long_description = "System calls wrappers for second assignment in course 046210.",
    ext_modules = [module1]
    )
