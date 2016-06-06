# Compilation
CC_FILES := $(wildcard src/*/*.c)
OBJ_FILES := $(addprefix build/,$(notdir $(CC_FILES:.c=.o)))

CC_FLAGS := -g -Wall -Wextra -Wpedantic -Werror -Wstrict-overflow -fno-strict-aliasing -Wno-missing-field-initializers -O2
LD_FLAGS :=

build/%.o: src/*/%.c
	${CC} $(CC_FLAGS) -c -o $@ $<
%: build/%.o
	${CC} $(LD_FLAGS) -o $@ $^


BINARIES := addcontent addpeer lookupcontent peer removecontent removepeer
all: $(BINARIES)

addcontent: build/addcontent.o
addpeer: build/addpeer.o
lookupcontent: build/lookupcontent.o build/socket.o
peer: build/peer.o build/address.o build/socket.o
removecontent: build/removecontent.o
removepeer: build/removepeer.o


style:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*/*.c
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i src/*/*.h


zip: clean a1-358s16.zip
a1-358s16.zip: src README Makefile
	zip -r a1-358s16.zip src README Makefile


clean:
	rm -f build/*.o
	rm -f $(BINARIES)
	rm -f a1-358s16.zip
