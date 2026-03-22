.PHONY: all clean

all:
	$(MAKE) -C client 
	$(MAKE) -C server 
	$(MAKE) -C client all
	$(MAKE) -C server all
	$(MAKE) -C server/test

clean:
	$(MAKE) -C client clean
	$(MAKE) -C server clean
	$(MAKE) -C server/test clean
	rm -rf *.txt
	rm -rf ../bin