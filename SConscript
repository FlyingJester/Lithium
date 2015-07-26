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

if os.getenv('AR', 'none') != 'none':
    print "using archiver ", os.environ.get('AR')
    lithium_environment.Replace(AR=os.environ.get('AR'))

if os.getenv('RANLIB', 'none') != 'none':
    print "using archive indexer ", os.environ.get('RANLIB')
    lithium_environment.Replace(RANLIB=os.environ.get('RANLIB'))

if sys.platform.startswith("win"):
    lithium_environment.Append(
        CCFLAGS = " /O2 /W4 ")
    lithium_environment.Append(
        CCFLAGS = " /Za ",
        CXXFLAGS = " /EHsc ",
        CPPPATH = ["../"])
else:
    lithium_environment.Append(
        CCFLAGS = " -g -ffast-math -Wall -pedantic -Werror ",
        CFLAGS = " -Wextra -ansi -O3 ", 
        CXXFLAGS = " -Wunused-parameter -fno-exceptions -fno-rtti -std=c++98 -O2 ")

lithium = lithium_environment.StaticLibrary("lithium", ["lithium.cpp", "strtoll.c"])

Return("lithium")
