##Debugging 
#DEBUG = -DNDEBUG
DEBUG = -g

##Profiling 
#PROF = -pg
##Optmisation options
OPT =  #-felide-constructors -fno-implement-inlines -O3
CC = gcc 
CXX = g++
###Select output format: normal, maria or spin
OUTPUT=-DNORMAL
#OUTPUT=-DMARIA
#OUTPUT=-DSPIN

##Location of hash_map and hash_set include files for other compilers than gcc
HASH_MAP_LOC='<ext/hash_map>'
HASH_SET_LOC='<ext/hash_set>'

DEFINES = -DSGI_HASH_MAP -DHASH_MAP_LOC=$(HASH_MAP_LOC) -DHASH_SET_LOC=$(HASH_SET_LOC) $(OUTPUT)  
INCLUDES=-IAutomata -ILTL
CFLAGS =  -Wall -ansi -pedantic
CXXFLAGS = -fno-exceptions -fno-rtti $(CFLAGS) $(DEBUG) $(INCLUDES) $(PROF) $(OPT) $(DEFINES)
TARGET = scheck2

LTLSRC = \
	LTL/Formula.C \
	LTL/TemporalBinOp.C \
	LTL/TemporalUnOp.C \
	LTL/BinOp.C \
	LTL/Atom.C \
	LTL/Const.C \
	LTL/Not.C \
	LTL/FormulaAlgs.C

AUTSRC = \
	Automata/NonDetAut.C \
	Automata/Automaton.C \
	Automata/DetAut.C \
	Automata/PairMap.C \
	Automata/BitVector.C \
	Automata/Pathologic.C \
	Automata/PrintAut.C \
	Automata/Implicant.C

GENSRC = \
	scheck.C

SRCS = $(LTLSRC) $(AUTSRC) $(GENSRC)
OBJS = $(SRCS:.C=.o)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) 

$(OBJS) : %.o: %.C
	$(CXX) -c $(CXXFLAGS) $< -o $@

depend : $(SRCS)
	touch $@ && makedepend -f$@ -Y $(SRCS) 2> /dev/null

.PHONY : clean reallyclean

clean : 
	rm -f $(OBJS) depend depend.bak

reallyclean : clean
	rm -f $(TARGET)

include depend
