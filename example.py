import numpy as np

def main():
	a=np.random.rand(1024*1024*500);
	print(a.sum())


#entry point, keep main thread for signal handling
import signal, sys
import threading, traceback

def signal_stacktrace(signum,frame):
    stacktrace = ""
    for th in threading.enumerate():
        stacktrace += str(th)
        stacktrace += "".join(traceback.format_stack(sys._current_frames()[th.ident]))
    print(stacktrace, file=sys.stderr)
if __name__ == '__main__':
    signal.signal(signal.SIGUSR1, signal_stacktrace)
    t = threading.Thread(target=main)
    t.start()
    t.join()
