import sys

from walker import *

for path in walk(sys.argv[1]):
    if ".h" not in path and ".cpp" not in path: continue

    file = open(path, 'r')
    content = file.read()
    file.close()

    # content = content.replace( \
    #     "virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.", \
    #     "" \
    # )

    open(path, 'w').write(content)
