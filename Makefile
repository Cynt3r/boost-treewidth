PROGRAM=example_run
TESTS=test_run
CC=g++
CFLAGS=-std=c++14 -Wall -pedantic

all: compile

compile: $(PROGRAM) $(TESTS)

doc:
	doxygen Doxyfile

.SILENT: run help

run: $(PROGRAM)
	@./$(PROGRAM)
	for i in "example/"*.dot; do \
		[ -f "$$i" ] || break ; \
		PNGFILE="$${i%%.*}".png ; \
		dot -Tpng "$$i" -o "$${PNGFILE}" ; \
		if test -f "$$PNGFILE"; then \
			echo "Image $${PNGFILE} created" ; \
		fi ; \
	done

test: $(TESTS)
	@./$(TESTS)

$(PROGRAM): obj/example.o
	$(CC) $(CFLAGS) $^ -o $@

$(TESTS): obj/main_test.o obj/tree_decomposition_test.o obj/nice_tree_decomposition_test.o obj/max_weighted_independent_set_test.o
	$(CC) $(CFLAGS) $^ -o $@

obj/example.o: example/example.cpp | obj
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

obj/main_test.o: test/main_test.cpp | obj
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

obj/tree_decomposition_test.o: test/tree_decomposition_test.cpp | obj
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

obj/nice_tree_decomposition_test.o: test/nice_tree_decomposition_test.cpp | obj
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

obj/max_weighted_independent_set_test.o: test/max_weighted_independent_set_test.cpp | obj
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

obj:
	mkdir obj

.PHONY: clean doc

clean:
	rm -rf $(PROGRAM) $(TESTS) obj 2>/dev/null

help:
	echo '---------------------'
	echo 'Makefile help:'
	echo '---------------------'
	echo 'all:		compile both example and tests'
	echo 'compile:	same as all'
	echo 'doc:		generate documentation'
	echo 'run:		run example'
	echo 'test:		run tests'
	echo 'help:		show this help'
	echo 'clean:		clean binaries and obj/ dir'