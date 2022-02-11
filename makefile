#just write "make" to build project

TARGET = prog
#target: prerequisites - the rule head
#$@ - means the target
#$^ - means all prerequisites
#$< - means just the first prerequisite

$(TARGET): main.o finder.lib
	g++ $^ -o $@

main.o: main.cpp
	g++ -c $< -o $@

#ar - tool to create, modify, and extract from .lib(library) / Linux -> .a(archives)
#r - replace files existing inside the archive
#c - create a archive if not already existent
#s - create an object-file index into the archive
finder.lib: lib1.o lib2.o
	ar rcs $@ $^

lib1.o: finder_engine.cpp finder_engine.h
	g++ -c -o $@ $<

lib2.o: stdafx.cpp stdafx.h
	g++ -c -o $@ $<

clean:
	rm -f *.o *.a $(TARGET)