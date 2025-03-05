#!/usr/bin/env python3

import os

def walk(path, filter = (lambda x: True), transformer = (lambda x: x)):
    arr = []

    if type(path) == str:
        for root, dirs, files in os.walk(path):
            for file in files:
                full_path = os.path.join(root, file);
                if not filter(full_path): continue
                arr.append(transformer(full_path))
    else:
        for el_path in path:
            arr += walk(el_path, filter, transformer)
    
    return arr
