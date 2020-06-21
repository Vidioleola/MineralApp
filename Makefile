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
all: mineralapp
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
mineralapp: src/app.o src/addmodframe.o src/mainframe.o src/utils.o src/addtodb.o src/csv.o 
	$(CXX) src/app.o src/mainframe.o src/addmodframe.o src/utils.o src/addtodb.o src/csv.o $(LDFLAGS) -o mineralapp
src/app.o: src/app.cpp src/mainframe.h src/addmodframe.h
	$(CXX) -c src/app.cpp -o src/app.o $(CXXFLAGS)
src/mainframe.o: src/mainframe.cpp src/mainframe.h src/addmodframe.h
	$(CXX) -c src/mainframe.cpp -o src/mainframe.o $(CXXFLAGS)
src/addmodframe.o: src/addmodframe.cpp src/mainframe.h src/addmodframe.h src/utils.h
	$(CXX) -c src/addmodframe.cpp -o src/addmodframe.o $(CXXFLAGS)
src/utils.o: src/utils.cpp src/utils.h
	$(CXX) -c src/utils.cpp -o src/utils.o $(CXXFLAGS)
src/addtodb.o: src/addtodb.cpp
	$(CXX) -c src/addtodb.cpp -o src/addtodb.o $(CXXFLAGS)
src/csv.o: src/csv.cpp
	$(CXX) -c src/csv.cpp -o src/csv.o $(CXXFLAGS)

# Linux specific install instructions
ifeq ($(UNAME), Linux)
.PHONY: install
install: mineralapp
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/
	cp mineralapp $(DESTDIR)$(PREFIX)/bin/mineralapp
	cp mineralapp.desktop $(DESTDIR)$(PREFIX)/share/applications/mineralapp.desktop
	cp icon/mineralapp-48.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/48x48/apps/mineralapp.png
	cp icon/mineralapp-512.png $(DESTDIR)$(PREFIX)/share/icons/hicolor/512x512/apps/mineralapp.png
	cp icon/mineralapp-hires.svg $(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/mineralapp.svg
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

# MacOS specific install instructions
ifeq ($(UNAME), Darwin)
MineralApp.app: mineralapp
	mkdir -p MineralApp.app/Contents
	mkdir -p MineralApp.app/Contents/MacOS
	mkdir -p MineralApp.app/Contents/Resources
	cp Info.plist MineralApp.app/Contents/Info.plist
	sed -i -e 's/VERSION/$(VERSION)/g' MineralApp.app/Contents/Info.plist
	/bin/echo "APPL????" >MineralApp.app/Contents/PkgInfo
	cp mineralapp MineralApp.app/Contents/MacOS/mineralapp
	cp icon/mineralapp.icns MineralApp.app/Contents/Resources/
MineralApp.dmg: MineralApp.app
	rm -f MineralApp.dmg
	rm -rf tmpdir
	mkdir -p tmpdir
	ln -sf /Applications/ tmpdir/
	mv MineralApp.app tmpdir/
	hdiutil create -fs HFS+ -srcfolder tmpdir/ -volname MineralApp MineralApp.dmg
	mv tmpdir/MineralApp.app .
	rm -rf tmpdir
.PHONY: install
install: MineralApp.app
	rm -rf /Applications/MineralApp.app
	cp -r MineralApp.app /Applications/MineralApp.app
.PHONY: uninstall
uninstall:
	rm -rf /Applications/MineralApp.app
endif

# Clean build
.PHONY: clean
clean:
	rm -rf mineralapp MineralApp.app src/app.o src/addmodframe.o src/mainframe.o src/utils.o src/addtodb.o src/csv.o

