#!/usr/bin/make -f

CFLAGS ?= -O2 -g -fPIE -fstack-protector-strong -Wformat -Werror=format-security
LDFLAGS ?=  --static

ifeq ($(CROSS),)
	BENCH = bench
	o = o
else
	BENCH = $(CROSS)-bench
	CC := $(CROSS)-gcc
	o = $(CROSS).o
endif

OBJECTS := bench-runner.$(o) \
		   bench-Trie.$(o) \
		   bench-TrieCase.$(o) \
		   bench-GPerf.$(o) \
		   bench-GPerfCase.$(o) \
		   bench-hashes.$(o)

all: $(BENCH)

bench-GPerfCase.c:
	gperf --includes --ignore-case --pic --readonly-tables -N GPerfCase wordlist > bench-GPerfCase.c
bench-GPerf.c:
	gperf --includes  --pic --readonly-tables -N GPerf wordlist > bench-GPerf.c

bench-TrieCase.c bench-TrieCase.h:
	../triehash/triehash.pl --ignore-case  -C bench-TrieCase.c -H bench-TrieCase.h --function-name TrieCase wordlist

bench-Trie.c bench-Trie.h:
	../triehash/triehash.pl -C bench-Trie.c -H bench-Trie.h --function-name Trie wordlist


%.$(o): %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

bench-runner.$(o): CPPFLAGS := -DWORDS="\"Hallo PaCKage $(shell tr '\n' ' ' < wordlist)\"" -DITERS=1000000

$(BENCH): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

run: $(BENCH)
	@if [ -z "$(HOST)" ]; then \
		./$(BENCH); \
	else \
		scp ./$(BENCH) $(HOST):; \
		ssh $(HOST) ./$(BENCH); \
	fi
clean:
	$(RM) -f $(OBJECTS) $(BENCH) \
		*-bench bench*.o bench-GPerfCase.c bench-GPerf.c  \
		bench-TrieCase.c bench-TrieCase.h \
		bench-Trie.c bench-Trie.h
