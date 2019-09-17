#!/bin/bash

# Create a pdf version of the manual.

# Bash and python syntax are the same for a simple variable!
source mineralapp/version.py

pandoc \
    -s README.md \
    -s INSTALL.md \
    -s USAGE.md \
    -V documentclass=article \
    -V classoption:fleqn \
    -V geometry:margin=1.5in \
    -V colorlinks=true \
    --number-sections \
    -M title:"MineralApp v$version" \
    -M author:"Simone Conti" \
    -M date:"`date +"%b %e, %Y"`" \
    -M link-citations:true \
    -o mineralapp/manual.pdf

ln -sf mineralapp/manual.pdf .

