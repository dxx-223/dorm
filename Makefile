SRCS=${wildcard src/[[:upper:]]*.cpp}
OBJS=${patsubst src/%.cpp, obj/%.o, ${SRCS}}
TESTSRCS=${wildcard tests/*.cpp}
TESTBINS=${patsubst tests/%.cpp, tests/bin/%, ${TESTSRCS}}

TESTGENS=${wildcard output/*.cxx output/*/*.cxx}
TESTOBJS=${patsubst output/%.cxx, tests/obj/%.o, ${TESTGENS}}

INCDIRS=include
INCLUDES=${INCDIRS:%=-I%} -isystem /usr/local/include

LIBDIRS=/usr/local/lib /usr/local/lib/mysql
LIBS=mysqlcppconn mysqlclient thr stdc++

CXX?=c++
DEBUGFLAGS=-O0 -ferror-limit=3 -DDORM_DB_DEBUG
CXXFLAGS+=-pipe -g -Wall -std=c++14 -pthread -fPIC ${INCLUDES} ${DEBUGFLAGS}

all: lib/libDORM.so bin/generate_object

.PHONY: clean
clean:
	-rm -fr bin obj lib tests/obj tests/bin
	
.PHONY: clean-tests
clean-tests:
	-rm -fr tests/obj tests/bin

obj/%.d: src/%.cpp include/%.hpp
	@echo 'Generating makefile $@ from $<'
	@mkdir -p `dirname $@`
	@echo -n 'obj/' > $@
	@${CXX} ${CXXFLAGS} -MM $< >> $@

ifneq ($(MAKECMDGOALS),clean)
DEPENDS=${patsubst src/%.cpp, obj/%.d, ${SRCS}}
-include ${DEPENDS}
endif

obj/%.o: src/%.cpp include/%.hpp
	@echo 'Compiling $@ from $<'
	@${CXX} ${CXXFLAGS} -c -o $@ $<

bin/%: src/%.cpp
	@echo 'Building $@ from $<'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -o $@ $<

lib/libDORM.so: ${OBJS}
	@echo 'Building shared library $@'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -shared -Wl,-soname,libDORM.so -o $@ $^
	

.PHONY: tests
tests: ${OBJS} ${TESTOBJS} ${TESTBINS}

tests/bin/%: tests/%.cpp ${OBJS} ${TESTOBJS}
	@echo 'Building test $@ from $<'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -Itests -Ioutput ${LIBDIRS:%=-L%} -o $@ $< ${OBJS} ${TESTOBJS} ${LIBS:%=-l%}

tests/obj/%.o: output/%.cxx ${OBJS}
	@echo 'Compiling object helper $@ from $<'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -Itests -Ioutput -c -o $@ $<
