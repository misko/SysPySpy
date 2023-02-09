all: preload syspyspy.so

syspyspy.so: syspyspy.c syspyspy.h
	gcc -shared -DDEBUG -fPIC syspyspy.c -o syspyspy.so -ldl -Wall -g

syspyspy_signal.so: syspyspy.c syspyspy.h
	gcc -shared -DDEBUG -fPIC syspyspy.c -o syspyspy.so -ldl -Wall -g -DSYSPYSPY_SIGNAL

preload: preload.c
	gcc -Wall preload.c -o preload 

