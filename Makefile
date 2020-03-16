#
## MineralApp
#
#   To compile and install:
#       make && sudo make install
#
# Simone Conti simonecnt@gmail.com
#

# Where to install -- Used in Linux only!
# You can specify at install time the variable DESTDIR to install somewhere else.
# e.g. `make DESTDIR=/tmp/stage install`
# will install in /tmp/stage/usr/
PREFIX = /usr


# Set compiler flags
CXX      = g++
CXXFLAGS = --std=c++17 -Wall -O2
LDFLAGS  = $(CXXFLAGS)
UNAME := $(shell uname -s)
ifeq ($(UNAME), Linux)
CXXFLAGS += `wx-config --cxxflags`
LDFLAGS  += `wx-config --cxxflags --libs std,richtext`
LDFLAGS  += -lsqlite3
all: mineralapp mineralapp-48x48.png mineralapp-512x512.png
endif
ifeq ($(UNAME), Darwin)
CXXFLAGS += `wx-config --cxxflags --static`
CXXFLAGS += -mmacosx-version-min=10.12
LDFLAGS  += `wx-config --cxxflags --libs std,richtext --static`
LDFLAGS  += /usr/local/Cellar/sqlite/3.31.1/lib/libsqlite3.a
all: MineralApp.app
endif

# Set version
include version.in
CXXFLAGS += -DVERSION_MAJOR='"$(VERSION_MAJOR)"' -DVERSION_MINOR='"$(VERSION_MINOR)"' -DVERSION_PATCH='"$(VERSION_PATCH)"' -DVERSION='"$(VERSION)"'

# Compile source code, make mineralapp executable
mineralapp: src/app.o src/addmodframe.o src/mainframe.o src/utils.o
	$(CXX) $(LDFLAGS) src/app.o src/mainframe.o src/addmodframe.o src/utils.o -o mineralapp
src/app.o: src/app.cpp src/mainframe.h src/addmodframe.h
	$(CXX) -c src/app.cpp -o src/app.o $(CXXFLAGS)
src/mainframe.o: src/mainframe.cpp src/mainframe.h src/addmodframe.h
	$(CXX) -c src/mainframe.cpp -o src/mainframe.o $(CXXFLAGS)
src/addmodframe.o: src/addmodframe.cpp src/mainframe.h src/addmodframe.h src/utils.h
	$(CXX) -c src/addmodframe.cpp -o src/addmodframe.o $(CXXFLAGS)
src/utils.o: src/utils.cpp src/utils.h
	$(CXX) -c src/utils.cpp -o src/utils.o $(CXXFLAGS)

### Linux specific install instructions
ifeq ($(UNAME), Linux)
.PHONY: install
install: mineralapp mineralapp-48x48.png mineralapp-512x512.png mineralapp.svg mineralapp.desktop
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/
	cp mineralapp $(DESTDIR)$(PREFIX)/bin/mineralapp
	cp mineralapp.desktop $(DESTDIR)$(PREFIX)/share/applications/mineralapp.desktop
	cp mineralapp-48x48.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/mineralapp.png
	cp mineralapp-512x512.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps/mineralapp.png
	cp mineralapp.svg $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/mineralapp.svg
	-gtk-update-icon-cache /usr/share/icons/hicolor/
.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/mineralapp
	rm -f $(DESTDIR)$(PREFIX)/share/applications/mineralapp.desktop
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/mineralapp.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps/mineralapp.png
	rm -f $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/mineralapp.svg
	-gtk-update-icon-cache /usr/share/icons/hicolor/
endif

### MacOS specific install instructions
ifeq ($(UNAME), Darwin)
MineralApp.app: mineralapp Info.plist mineralapp.icns
	mkdir -p MineralApp.app/Contents
	mkdir -p MineralApp.app/Contents/MacOS
	mkdir -p MineralApp.app/Contents/Resources
	cp Info.plist MineralApp.app/Contents/Info.plist
	sed -i -e 's/VERSION/$(VERSION)/g' MineralApp.app/Contents/Info.plist
	/bin/echo "APPL????" >MineralApp.app/Contents/PkgInfo
	cp mineralapp MineralApp.app/Contents/MacOS/mineralapp
	cp mineralapp.icns MineralApp.app/Contents/Resources/
MineralApp.dmg: MineralApp.app
	rm -f MineralApp.dmg
	rm -rf dist
	mkdir -p dist
	ln -sf /Applications/ dist/
	mv MineralApp.app dist/
	hdiutil create -fs HFS+ -srcfolder dist/ -volname MineralApp MineralApp.dmg
	mv dist/MineralApp.app .
	rm -rf dist
.PHONY: install
install: MineralApp.app
	rm -rf /Applications/MineralApp.app
	cp -r MineralApp.app /Applications/MineralApp.app
.PHONY: uninstall
uninstall:
	rm -rf /Applications/MineralApp.app
endif


### Icon images
# With Inkscape create:
#   mineralapp.svg
#   mineralapp.png
# png can have any resolution you want, but make sure it is a square image.
mineralapp-48x48.png: mineralapp.png
	convert mineralapp.png -resize 48x48 mineralapp-48x48.png
mineralapp-512x512.png: mineralapp.png
	convert mineralapp.png -resize 512x512 mineralapp-512x512.png
# For MacOS:
ifeq ($(UNAME), Darwin)
mineralapp.icns: mineralapp-512x512.png
	sips -s format icns mineralapp-512x512.png --out mineralapp.icns
endif

# Clean build
.PHONY: clean
clean:
	rm -rf mineralapp MineralApp.app src/app.o src/addmodframe.o src/mainframe.o src/utils.o
	rm -f mineralapp-512x512.png mineralapp-48x48.png mineralapp.icns
	rm -rf dist/

