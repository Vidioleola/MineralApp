#!/bin/bash

# With Inkscape create:
#   mineralapp.svg
#   mineralapp.png
# png can have any resolution you want, but make sure it is a square image.


# For Linux:

mkdir -p ../share/icons/hicolor/scalable/apps/
mkdir -p ../share/icons/hicolor/48x48/apps/
mkdir -p ../share/icons/hicolor/512x512/apps/

cp mineralapp.svg                       ../share/icons/hicolor/scalable/apps/mineralapp.svg
convert mineralapp.png -resize 48x48    ../share/icons/hicolor/48x48/apps/mineralapp.png
convert mineralapp.png -resize 512x512  ../share/icons/hicolor/512x512/apps/mineralapp.png

# For MacOS:
if [ `uname` == 'Darwin' ]; then
    sips -s format icns ../share/icons/hicolor/512x512/apps/mineralapp.png --out mineralapp.icns
fi

# Not sure I need this...
#convert mineralapp.png -resize 32x32 mineralapp.ico

