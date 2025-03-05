#!/usr/bin/env python3

from walker import *

import sys

print(*walk(sys.argv[2:], \
    lambda path: path.endswith(sys.argv[1]) \
))
