import os
import sys
from setuptools import setup, find_packages
from distutils.core import Extension

setup(
    name='tsearch',
    version='0.0',
    description='UCT and A* algorithms',
    author='Adria Garriga Alonso, Daniel Furelos Blanco',
    author_email='adria.garriga@gmail.com',
    packages=find_packages(),
    ext_modules=[Extension('tsearch.libuct',
                           ['node.cpp', 'strategy.cpp', 'uct.cpp', 'path_cache.cpp'],
                           define_macros=[('NDEBUG', '1')],
                           extra_compile_args=['-O3', '-flto', '-std=c++14'],
                           extra_link_args=['-O3', '-flto'],)])
