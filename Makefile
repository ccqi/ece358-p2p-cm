all:
	make -C src

style:
	make -C src style

zip: clean a1-358s16.zip
a1-358s16.zip: src README Makefile
	zip -r a1-358s16.zip src README Makefile


clean:
	make -C src clean

	rm -f a1-358s16.zip
