# SysPySpy
System Call Python Spy is intended as a small raw example of 
how to intercept system calls (specifically *alloc) and dump
a python stack from the top level caller. 

Quick start,

`make`

`./preload ./syspyspy.so python example.py`

`./preload ./syspyspy_signal.so python example.py`

alternatively, 

`LD_PRELOAD=./syspyspy.so python example.py`

`LD_PRELOAD=./syspyspy_signal.so python example.py`


To add a hook for your own system call of interest, modify `syspyspy.c`
