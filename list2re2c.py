#!/usr/bin/python
import sys

if '-i' in sys.argv:
    prefix="i"
else:
    prefix=""

print("""
#include <stddef.h>
int re2c%s(const char *YYCURSOR, size_t dummy)
{
    const char *YYMARKER;
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;
        end = "\x00";

""" % prefix)

for word, num in zip(sys.stdin, range(999999)):
    print("        \"%s\" end { return %d; }" % (word.strip(), num))

print("        * { return -1; }")
print("""
    */
}""")
