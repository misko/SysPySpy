# SysPySpy
System Call Python Spy is intended as a small raw example of 
how to intercept system calls (specifically *alloc) and dump
a python stack from the top level caller. 

<img src="https://user-images.githubusercontent.com/1278972/217698983-193c0896-5bda-4236-9f6b-4804dde28bd4.png" width="250" height="250" />


Quick start,

`make`

`./preload ./syspyspy.so python example.py`

`./preload ./syspyspy_signal.so python example.py`

alternatively, 

`LD_PRELOAD=./syspyspy.so python example.py`

`LD_PRELOAD=./syspyspy_signal.so python example.py`

An example output

```% ./preload ./syspyspy_signal.so python example.py 
Loading SysPySpy! PID:8711 PPID:6163
SysPySpy: malloc(4194304000) 3.91G
<_MainThread(MainThread, started 140309272430400)>  File "/home/michaeldzamba/SysPySpy/example.py", line 21, in <module>
    t.start()
  File "/fn/lib/python3.10/threading.py", line 940, in start
    self._started.wait()
  File "/fn/lib/python3.10/threading.py", line 607, in wait
    signaled = self._cond.wait(timeout)
  File "/fn/lib/python3.10/threading.py", line 320, in wait
    waiter.acquire()
  File "/home/michaeldzamba/SysPySpy/example.py", line 16, in signal_stacktrace
    stacktrace += "".join(traceback.format_stack(sys._current_frames()[th.ident]))
<Thread(Thread-1 (main), started 140307908876032)>  File "/fn/lib/python3.10/threading.py", line 973, in _bootstrap
    self._bootstrap_inner()
  File "/fn/lib/python3.10/threading.py", line 1016, in _bootstrap_inner
    self.run()
  File "/fn/lib/python3.10/threading.py", line 953, in run
    self._target(*self._args, **self._kwargs)
  File "/home/michaeldzamba/SysPySpy/example.py", line 4, in main
    a=np.random.rand(1024*1024*500);

262138641.5010468
```

To add a hook for your own system call of interest, modify `syspyspy.c`
