SRCS=${wildcard src/*.cpp}
OBJS=${patsubst src/%.cpp, obj/%.o, ${SRCS}}
TESTSRCS=${wildcard tests/*.cpp}
TESTBINS=${patsubst tests/%.cpp, tests/bin/%, ${TESTSRCS}}

TESTGENS=${wildcard tests/*.cxx tests/*/*.cxx}
TESTOBJS=${patsubst tests/%.cxx, tests/obj/%.o, ${TESTGENS}}

INCDIRS=include
INCLUDES=${INCDIRS:%=-I%} -isystem /usr/local/include

LIBDIRS=/usr/local/lib /usr/local/lib/mysql
LIBS=mysqlcppconn mysqlclient thr

CXX=c++
DEBUGFLAGS=-O0 -ferror-limit=3 -DDORM_DB_DEBUG
CXXFLAGS=-g -Wall -std=c++14 -pthread ${INCLUDES} ${DEBUGFLAGS}

all: ${OBJS}

.PHONY: clean
clean:
	-rm -fr obj tests/obj tests/bin
	
.PHONY: clean-tests
clean-tests:
	-rm -fr tests/obj tests/bin

obj/%.d: src/%.cpp include/%.hpp
	@mkdir -p `dirname $@`
	@echo 'Building $@ from $<'
	@echo -n 'obj/' > $@
	@${CXX} ${CXXFLAGS} -MM $< >> $@

ifneq ($(MAKECMDGOALS),clean)
DEPENDS=${patsubst src/%.cpp, obj/%.d, ${SRCS}}
-include ${DEPENDS}
endif

obj/%.o: src/%.cpp include/%.hpp
	@echo 'Compiling $@ from $<'
	@${CXX} ${CXXFLAGS} -c -o $@ $<

.PHONY: tests
tests: ${OBJS} ${TESTOBJS} ${TESTBINS}

tests/bin/%: tests/%.cpp ${OBJS} ${TESTOBJS}
	@echo 'Building test $@ from $<'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -Itests ${LIBDIRS:%=-L%} -o $@ $< ${OBJS} ${TESTOBJS} ${LIBS:%=-l%}

tests/obj/%.o: tests/%.cxx ${OBJS}
	@echo 'Compiling object helper $@ from $<'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -Itests -c -o $@ $<
