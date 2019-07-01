#!/bin/bash

# Clean previous run
rm -f MineralApp.dmg
rm -rf build/ dist/

# Make python app
python3 setup.py py2app > py2app.out 2>&1

# Check everything is fine
if [ $? != 0 ]; then
    echo "Something went wrong with py2app. Check py2app.out"
    exit -1
fi

# Make dmg
ln -sf /Applications/ dist/
hdiutil create -fs HFS+ -srcfolder dist/ -volname MineralApp MineralApp.dmg

# Check
if [ $? == 0 ]; then 
    rm -rf build/ dist/ py2app.out
    echo "MineralApp.dmg created successfully!"
else
    echo "Something went wrong with hdiutil :("
fi

