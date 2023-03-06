#PROJS=tmpl sigproc
PROJS=sigproc stitch sm_to_exponent exponent_to_sm find_start_sequences
TMPLSRCS=tmpl.c
TMPLOBJS=${TMPLSRCS:.c=.o}
SIGPROCSRCS=sigproc.c  trace.c
SIGPROCOBJS=${SIGPROCSRCS:.c=.o}

SIGPROCORIGSRCS=sigproc_orig.c  trace.c
SIGPROCORIGOBJS=${SIGPROCSRCS:.c=.o}


CFLAGS=-g -Wall
LDFLAGS=-g 
LDLIBS=


all:${PROJS}


tmpl: ${TMPLOBJS}
	${CC} ${LDFLAGS} -o $@ $< ${LDLIBS}

stitch: stitch_parallel_simd stitch_parallel_simd_any_pos 

stitch_parallel_simd: stitch_parallel_simd.cpp
	$(CXX) -mavx2 -std=c++17 -Ofast stitch_parallel_simd.cpp -o stitch_parallel_simd

stitch_parallel_simd_any_pos: stitch_parallel_simd_any_pos.cpp
	$(CXX) -mavx2 -std=c++17 -Ofast stitch_parallel_simd_any_pos.cpp -o stitch_parallel_simd_any_pos
	
sm_to_exponent: sm_to_exponent.cpp
	$(CXX) sm_to_exponent.cpp -o sm_to_exponent

exponent_to_sm: exponent_to_sm.cpp
	$(CXX) exponent_to_sm.cpp -o exponent_to_sm
	
find_start_sequences: find_start_sequences.cpp
	$(CXX) find_start_sequences.cpp -o find_start_sequences
	
sigproc: ${SIGPROCOBJS} 
	${CC} ${LDFLAGS} -o $@ ${SIGPROCOBJS} ${LDLIBS}
	
orig: ${SIGPROCORIGOBJS} 
	${CC} ${LDFLAGS} -o $@ ${SIGPROCOBJS} ${LDLIBS}


clean: 
	rm -f ${PROJS} ${TMPLOBJS} ${SIGPROCOBJS} stitch_parallel_simd_any_pos stitch_parallel_simd sm_to_exponent find_start_sequences


trace.o: trace.h
sigproc.o: trace.h



