# Compilation
CXX_FILES := $(wildcard src/*/*.cpp)
OBJ_FILES := $(addprefix build/,$(notdir $(CXX_FILES:.cpp=.o)))

CXX_FLAGS := -g -Wall -Wextra -Wpedantic -Werror -Wstrict-overflow -fno-strict-aliasing -Wno-missing-field-initializers -O2 -std=c++11
LD_FLAGS :=

build/%.o: src/*/%.cpp
	${CXX} $(CXX_FLAGS) -c -o $@ $<
%: build/%.o
	${CXX} $(LD_FLAGS) -o $@ $^


BINARIES := addcontent addpeer lookupcontent peer removecontent removepeer
all: $(BINARIES)

addcontent: build/addcontent.o build/socket.o
addpeer: build/addpeer.o
lookupcontent: build/lookupcontent.o build/socket.o
peer: build/peer.o build/address.o build/content.o build/socket.o
removecontent: build/removecontent.o build/socket.o
removepeer: build/removepeer.o build/socket.o

style:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*/*.cpp
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*/*.h


zip: clean a1-358s16.zip
a1-358s16.zip: src README Makefile
	zip -r a1-358s16.zip src README Makefile


clean:
	rm -f build/*.o
	rm -f $(BINARIES)
	rm -f a1-358s16.zip
