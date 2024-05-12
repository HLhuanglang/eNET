#!/usr/bin/env bash

# You can customize the clang-format path by setting the CLANG_FORMAT environment variable
CLANG_FORMAT=${CLANG_FORMAT:-clang-format}

find easy_net examples raw_examples test -name *.h -o -name *.cpp -exec dos2unix {} \;
find easy_net examples raw_examples test \( -name *.h -o -name *.cpp \) -not -name http_parser.h -not -name httplib.h | xargs $CLANG_FORMAT -i -style=file