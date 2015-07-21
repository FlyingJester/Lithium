
import os
import sys

lithium_environment = Environment(ENV = os.environ)

if os.getenv('CXX', 'none') != 'none':
    print "using CXX ", os.environ.get('CXX')
    lithium_environment.Replace(CXX = os.environ.get('CXX'))

if os.getenv('CC', 'none') != 'none':
    print "using CC ", os.environ.get('CC')
    lithium_environment.Replace(CC = os.environ.get('CC'))

if os.getenv('LINK', 'none') != 'none':
    print "using linker ", os.environ.get('LINK')
    lithium_environment.Replace(LINK = os.environ.get('LINK'))

if sys.platform.startswith("win"):
    lithium_environment.Append(
        CCFLAGS = " /Za /O2 /W4X ",
        CXXFLAGS = " /EHsc ",
        CPPPATH = ["../"])
else:
    lithium_environment.Append(
        CCFLAGS = " -g -ffast-math -Wall -pedantic -Werror ",
        CFLAGS = " -Wextra -ansi -O3 ", 
        CXXFLAGS = " -Wunused-parameter -fno-exceptions -fno-rtti -std=c++98 -O2 ")

lithium = lithium_environment.StaticLibrary("lithium", ["lithium.cpp", "strtoll.c"])

Return("lithium")
