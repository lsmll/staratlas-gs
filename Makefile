all:
	$(MAKE) -C ./src/
	$(MAKE) -C ./test
	$(MAKE) -C ./udf

.PHONY:clean
clean:
	$(MAKE) -C ./src clean
	$(MAKE) -C ./test clean
	$(MAKE) -C ./udf clean
