#!/usr/bin/env python3

import sys

print(*[sys.argv[1] + "/" + \
    path.replace(".cpp", sys.argv[2]).replace(".c", sys.argv[2]) \
    for path in sys.argv[3:]])
