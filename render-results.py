#!/usr/bin/env python3
import sys
import os.path
import json
import collections

tables = []
results = collections.OrderedDict()


for filename in sys.argv[1:]:
    table = os.path.splitext(filename)[0].replace("debian.org", "d.o").replace("debian.net", "d.n")
    table = " ".join([table.split("@", 1)[1], "(%s)" % table.split("@", 1)[0]])
    with open(filename) as fobj:
        result = json.load(fobj, object_pairs_hook=collections.OrderedDict)

    for function, values in result["summary"].items():
        if function == "_":
            continue
        try:
            r = results[function]
        except:
            r = results[function] = collections.OrderedDict()

        r[table] = values["bytes_per_ns"]

    tables.append(table)


def print_table(table):
    """Print a nicely formatted github-flavored markdown table"""
    col_lengths = {}

    for line in table:
        for i, col in zip(range(len(line)), line):
            if isinstance(col, float):
                col = line[i] = "%9.2f" % col
            try:
                col_lengths[i] = max(col_lengths[i], len(col))
            except KeyError:
                col_lengths[i] = len(col)

    for line in table:
        for i, col in zip(range(len(line)), line):
            if col == "-":
                line[i] = col_lengths[i] * "-"


    fmt_string = "|".join(("%-{}s" if i == 0 else "%{}s").format(col_lengths[i]) for i in sorted(col_lengths))

    for line in table:
        print(fmt_string % tuple(line))






if 1:
    table = []
    table.append(["arch/function"] + [st for st in tables])
    table.append(["-"] + ["-" for table in tables])

    for function, values in results.items():
        table.append([function] + [values[subtable] for subtable in tables])
    print_table(table)

if 1:
    print()
    print("And transposed:")
    print()
if 1:
    table = []
    table.append(["function/arch"] + [fun for fun in results])
    table.append(["-"] + ["-" for fun in results])

    for subtable in tables:
        table.append([subtable] + [values[subtable] for values in results.values()])


    print_table(table)
