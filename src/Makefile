CC_FLAGS := -g -Wall -Wextra -Wpedantic -Werror -Wstrict-overflow -fno-strict-aliasing -Wno-missing-field-initializers -O2

.SUFFIXES: .c .o
.c.o:
	${CC} $(CC_FLAGS) -c $<


all: addcontent addpeer lookupcontent removecontent removepeer

addcontent: addcontent.o
addpeer: addpeer.o
lookupcontent: lookupcontent.o
removecontent: removecontent.o
removepeer: removepeer.o

style:
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i *.c
	clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, AllowShortFunctionsOnASingleLine: None, KeepEmptyLinesAtTheStartOfBlocks: false}" -i *.h


clean:
	rm -f *.o
	rm -f addcontent addpeer lookupcontent removecontent removepeer
