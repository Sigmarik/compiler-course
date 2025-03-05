#!/usr/bin/env python3

from walker import *

import sys

print(*walk(sys.argv, lambda path: ".g4" in path))
