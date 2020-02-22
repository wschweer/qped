


qped:
	cd build; make -j16

t:    qped
	build/qped

d:
	gdb build/qped core



