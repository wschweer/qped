QT   = $(HOME)/Qt/6.4.0/gcc_64
LD_LIBRARY_PATH=$(QT)/lib
PATH = $(QT)/bin:/bin:/usr/bin



qped:
	cd build; make -j32

clean:
	rm -rf build/*; cd build; cmake ..

t:    qped
	export QT_FATAL_WARNINGS=1; build/qped

d:
	gdb build/qped core

install:
	cp build/qped $(HOME)/bin




