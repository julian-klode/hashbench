#!/usr/bin/make -f

CFLAGS ?= -O2 -g -fPIE -fstack-protector-strong -Wformat -Werror=format-security
LDFLAGS ?=  --static

ifeq ($(HOST),)
	DISPLAY = 1
else
	DISPLAY = 0
endif

ifeq ($(CROSS),)
	CROSS = $(shell dpkg-architecture -q DEB_HOST_GNU_TYPE)
	HOST = $(shell hostname)
endif

BENCH = $(CROSS)-bench
JSON = $(shell dpkg-architecture -t $(CROSS) -q DEB_HOST_ARCH)@$(HOST)
CC := $(CROSS)-gcc
o  := $(CROSS).o

OBJECTS := bench-runner.$(o) \
		   bench-Trie.$(o) \
		   bench-TrieCase.$(o) \
		   bench-GPerf.$(o) \
		   bench-GPerfCase.$(o) \
		   bench-hashes.$(o) \
		   bench-re2c.$(o) \
		   bench-re2ci.$(o)

all: $(BENCH)

bench-GPerfCase.c: wordlist
	gperf --includes --ignore-case --pic --readonly-tables -N GPerfCase wordlist > bench-GPerfCase.c
bench-GPerf.c: wordlist
	gperf --includes  --pic --readonly-tables -N GPerf wordlist > bench-GPerf.c

bench-re2c.c: wordlist
	python3 ./list2re2c.py < $< | re2c - > $@

bench-re2ci.c: wordlist
	python3 ./list2re2c.py -i < $< | re2c --case-insensitive - > $@

bench-TrieCase.c bench-TrieCase.h: wordlist ../triehash/triehash.pl
	@echo TH $@
	@../triehash/triehash.pl --ignore-case  -C bench-TrieCase.c -H bench-TrieCase.h --function-name TrieCase wordlist

bench-Trie.c bench-Trie.h: wordlist ../triehash/triehash.pl
	@echo TH $@
	@../triehash/triehash.pl -C bench-Trie.c -H bench-Trie.h --function-name Trie wordlist


%.$(o): %.c
	@echo CC $@
	@$(CC) -c $(CPPFLAGS) $(EXTRA_CPPFLAGS) $(CFLAGS) -o $@ $<

bench-runner.$(o): EXTRA_CPPFLAGS := -DWORDS="\"Hallo PaCKage PackageXXX $(shell tr '\n' ' ' < wordlist)\"" -DITERS=1000000

$(BENCH): $(OBJECTS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

run: $(BENCH)
	@echo EXEC $^
	@if [ "$(HOST)" = "jak-op3" ]; then \
		adb push ./$(BENCH) /data/local/tmp; \
		adb shell env xONLY_TRIE=1 /data/local/tmp/$(BENCH) > $(JSON).json; \
	else \
		rsync -a --partial ./$(BENCH) $(HOST):; \
		ssh $(HOST) env xONLY_TRIE=1 ./$(BENCH) > $(JSON).json; \
	fi
	[ $(DISPLAY) = 0 ] || python3 render-results.py $(JSON).json

clean:
	$(RM) -f $(OBJECTS) $(BENCH) *.json multi.md be \
		*-bench bench*.o bench-GPerfCase.c bench-GPerf.c  \
		bench-TrieCase.c bench-TrieCase.h \
		bench-Trie.c bench-Trie.h \
		bench-re2c.c bench-re2ci.c

%.json: wordlist ../triehash/triehash.pl
	$(MAKE) run CROSS=`dpkg-architecture -a$$(echo "$@" | cut -f1 -d"@") -qDEB_HOST_GNU_TYPE` HOST=`echo "$@" | cut -f2- -d"@" | sed s#.json##`


this: $(shell dpkg-architecture -qDEB_HOST_ARCH)@$(shell hostname).json
	python3 render-results.py $^


multi.md: amd64@jak-x230.json
multi.md: amd64@backup.json
multi.md: arm64@asachi.debian.org.json
multi.md: armhf@asachi.debian.org.json
multi.md: armel@asachi.debian.org.json
multi.md: ppc64el@plummer.debian.org.json
multi.md: mipsel@eller.debian.org.json
#multi: sparc64@notker.debian.net.json

multi: multi.md
	@cat $<
multi.md:
	python3 render-results.py $^ > $@

.PHONY: multi
