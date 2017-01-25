SRCS=${wildcard src/*.cpp}
OBJS=${patsubst src/%.cpp, obj/%.o, ${SRCS}}
TESTSRCS=${wildcard tests/*.cpp}
TESTOBJS=${patsubst tests/%.cpp, tests/obj/%, ${TESTSRCS}}

INCDIRS=include
INCLUDES=${INCDIRS:%=-I%} -isystem /usr/local/include

LIBDIRS=/usr/local/lib /usr/local/lib/mysql
LIBS=mysqlcppconn mysqlclient thr

CXX=c++
DEBUGFLAGS=-O0 -ferror-limit=3 -DDORM_DB_DEBUG
CXXFLAGS=-g -Wall -std=c++11 -pthread ${INCLUDES} ${DEBUGFLAGS}

all: ${OBJS}

.PHONY: clean
clean:
	-rm -fr obj tests/obj
	
.PHONY: clean-tests
clean-tests:
	-rm -fr tests/obj

obj/%.d: src/%.cpp include/%.hpp
	@mkdir -p obj
	@echo 'Building $@ from $<'
	@echo -n 'obj/' > $@
	@${CXX} ${CXXFLAGS} -MM $< >> $@

ifneq ($(MAKECMDGOALS),clean)
DEPENDS=${patsubst src/%.cpp, obj/%.d, ${SRCS}}
-include ${DEPENDS}
endif

obj/%.o: src/%.cpp include/%.hpp
	@echo 'Compiling $@ from $<'
	${CXX} ${CXXFLAGS} -c -o $@ $<

.PHONY: tests
tests: ${OBJS} ${TESTOBJS}

tests/obj/%: tests/%.cpp ${OBJS}
	@echo 'Building test $@ from $<'
	@mkdir -p tests/obj
	${CXX} ${CXXFLAGS} ${LIBDIRS:%=-L%} -o $@ $< ${OBJS} ${LIBS:%=-l%}
	