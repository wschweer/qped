


qped:
	cd build; make -j16

t:    qped
	export QT_FATAL_WARNINGS=1; build/qped

d:
	gdb build/qped core



