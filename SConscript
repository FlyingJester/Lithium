
import os
import sys

lithium_environment = Environment(ENV = os.environ)

lithium_environment.Append(
    CCFLAGS = " -g -ffast-math -Wall -pedantic -Werror ",
    CFLAGS = " -Wextra -ansi -O3 ", 
    CXXFLAGS = " -Wunused-parameter -fno-exceptions -fno-rtti -std=c++98 -O2 ")

lithium = lithium_environment.StaticLibrary("lithium", ["lithium.cpp", "strtoll.c"])

Return("lithium")
