# Compilation
CXX_FILES := $(wildcard src/*/*.cpp)
OBJ_FILES := $(addprefix build/,$(notdir $(CXX_FILES:.cpp=.o)))

CXX_FLAGS := -O2 -std=c++11
CXX_FLAGS += -g -Wall -Wextra -Wpedantic -Werror -Wstrict-overflow -fno-strict-aliasing -Wno-missing-field-initializers
LD_FLAGS := -O2

build/%.o: src/*/%.cpp
	${CXX} $(CXX_FLAGS) -c -o $@ $<
%: build/%.o
	${CXX} $(LD_FLAGS) -o $@ $^
	make peer  # I know, but the test script is stupid.


COMMANDS := addcontent addpeer lookupcontent removecontent removepeer
BINARIES := $(COMMANDS) debug peer
all: $(BINARIES)

addcontent: build/addcontent.o build/socket.o
addpeer: build/addpeer.o
debug: build/debug.o build/socket.o
lookupcontent: build/lookupcontent.o build/socket.o
peer: build/peer.o build/address.o build/commands.o build/communication.o build/content.o build/nodes.o build/socket.o build/state.o
removecontent: build/removecontent.o build/socket.o
removepeer: build/removepeer.o build/socket.o

style:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*/*.cpp
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*/*.h

.PHONY: test
test: zip
	test/runner.sh

zip: clean a1-358s16.zip
a1-358s16.zip: src README Makefile
	zip -r a1-358s16.zip build src README Makefile


clean:
	rm -f build/*.o
	rm -rf test/work
	rm -f $(BINARIES)
	rm -f a1-358s16.zip
