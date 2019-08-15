########################################################################
### Configuration:  ensure CSPICETOP points to CSPICE installation

CSPICETOP=$(HOME)/cspice

### End configuration
########################################################################

### Compile and build flags

CPPFLAGS=-I$(CSPICETOP)/include
CXXFLAGS=-O0 -g
LDLIBS=$(CSPICETOP)/lib/cspice.a

### Default target:  build ge executable

all: ge

### Dependencies for ge executable
ge:: ge.cpp csppice.h

### Retrieve kernels; run ge executable with Meta-Kernel ismws.tm
run: ge
	@./retrieve_kernels.bash
	./ge ismws.tm

### Cleanup executable
clean:
	$(RM) ge

### Cleanup executable and kernels
distclean: clean
	[ -L kernels ] && $(RM) -v kernels || $(RM) -Rv kernels
