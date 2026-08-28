#!/usr/bin/env python3
"""Prints an .xnb's header and its type-reader table.

The reader table is the contract between the file and the runtime: it must resolve IN FULL before
a single object is read, so a name CNA does not know fails the whole asset, not just the part that
uses it. Reading it out of the real file is how SAMPLE-048 named two framework gaps in ten minutes.
"""
import sys


def read_7bit(data, i):
    value = 0
    shift = 0
    while True:
        byte = data[i]
        i += 1
        value |= (byte & 0x7F) << shift
        if not byte & 0x80:
            return value, i
        shift += 7


def read_string(data, i):
    length, i = read_7bit(data, i)
    return data[i:i + length].decode("utf-8"), i + length


for path in sys.argv[1:]:
    data = open(path, "rb").read()
    assert data[:3] == b"XNB", f"{path}: not an .xnb"
    platform = chr(data[3])
    version = data[4]
    flags = data[5]
    size = int.from_bytes(data[6:10], "little")
    i = 10
    print(f"=== {path} ===")
    print(f"  platform={platform} version={version} flags=0x{flags:02x} size={size} "
          f"(file {len(data)})")
    if flags & 0x80:
        print("  COMPRESSED -- reader table is not readable without decompressing first")
        continue
    count, i = read_7bit(data, i)
    print(f"  {count} type readers:")
    for n in range(count):
        name, i = read_string(data, i)
        reader_version = int.from_bytes(data[i:i + 4], "little", signed=True)
        i += 4
        print(f"   [{n + 1:2d}] v{reader_version}  {name}")
    shared, i = read_7bit(data, i)
    print(f"  shared resources: {shared}")
