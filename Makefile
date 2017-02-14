SRCS=${wildcard src/[[:upper:]]*.cpp}
OBJS=${patsubst src/%.cpp, obj/%.o, ${SRCS}}
TESTSRCS=${wildcard tests/*.cpp}
TESTBINS=${patsubst tests/%.cpp, tests/bin/%, ${TESTSRCS}}

TESTOBJECTS=${wildcard tests/objects/*.hpp tests/objects/*/*.hpp}
TESTGENS=${patsubst tests/objects/%.hpp, tests/obj/%_.cxx, ${TESTOBJECTS}}
TESTOBJS=${patsubst tests/obj/%.cxx, tests/obj/%.o, ${TESTGENS}}

LIBDIRS=/usr/local/lib /usr/local/lib/mysql
LIBS=mysqlcppconn mysqlclient thr stdc++

CXX?=c++
CXXFLAGS+=-pipe -g -Wall -std=c++14 -pthread -fPIC ${INCLUDES}

ifdef DEBUG
CXXFLAGS+=-O0 -ferror-limit=3 -fno-omit-frame-pointer -fsanitize=address -DDORM_DB_DEBUG
endif

INCDIRS=include
CXXFLAGS+=${addprefix -I, ${INCDIRS}} -isystem /usr/local/include

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
	@${CXX} ${CXXFLAGS} -MM $< >> $@ || rm $@

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
	@${CXX} ${CXXFLAGS} -Itests -Itests/obj -Itests/objects ${LIBDIRS:%=-L%} -o $@ $< ${OBJS} ${TESTOBJS} ${LIBS:%=-l%}

tests/obj/%_.cxx: tests/objects/%.hpp bin/generate_object 
	@echo 'Generating object helper $@ from $<'
	@mkdir -p `dirname $@`
	@bin/generate_object -d tests/obj $<

tests/obj/%.o: tests/obj/%.cxx
	@echo 'Compiling object helper $@ from $<'
	@mkdir -p `dirname $@`
	@${CXX} ${CXXFLAGS} -Itests -Itests/obj -Itests/objects -c -o $@ $<

tests/obj/Test_.o: tests/obj/Test/Frog_.cxx tests/obj/Test/Single_.cxx
