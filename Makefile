ifeq ($(LANG),)
CP = copy
RM = del
else
CP = cp
RM = rm -rf
endif

CXX = g++
CXXFLAG = -std=c++11 -Wall -O3

TLDA: TLDA_run.o TLDA_process.o TLDA_read.o
	$(CXX) $(CXXFLAG) -o TLDA TLDA_run.o TLDA_process.o TLDA_read.o

TLDA_run.o: TLDA_run.cpp TLDA_process.h
	$(CXX) $(CXXFLAG) -c TLDA_run.cpp

TLDA_process.o: TLDA_process.cpp TLDA_process.h TLDA_utl.h
	$(CXX) $(CXXFLAG) -c TLDA_process.cpp

TLDA_read.o: TLDA_read.cpp TLDA_read.h
	$(CXX) $(CXXFLAG) -c TLDA_read.cpp

	echo make done

clean:
	$(RM) *.o TLDA
	echo clean done
