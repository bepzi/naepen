#!/usr/bin/env bash

find Source/ -regex '.*\.\(cpp\|hpp\|cc\|cxx\|h\|c\)' -exec clang-format -style=file -i {} \;