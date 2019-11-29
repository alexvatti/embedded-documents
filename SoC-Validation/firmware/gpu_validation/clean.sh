#!/bin/sh
find . -name *.o
find . -name *.o | xargs rm -rf
find . -name *depend
find . -name *depend | xargs rm -rf
find . -name *.su
find . -name *.su | xargs rm -rf
