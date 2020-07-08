
version="2.0.12"

rm -rf mineralapp_${version}_amd64
mkdir mineralapp_${version}_amd64
cd mineralapp_${version}_amd64

mkdir -m 0755 usr/
mkdir -m 0755 usr/bin/
cp ../mineralapp usr/bin/
strip --strip-unneeded --remove-section=.comment --remove-section=.note usr/bin/mineralapp
chmod 0755 usr/bin/mineralapp

mkdir -m 0755 usr/share/
mkdir -m 0755 usr/share/applications/
mkdir -m 0755 usr/share/icons/
mkdir -m 0755 usr/share/icons/hicolor/
mkdir -m 0755 usr/share/icons/hicolor/48x48/
mkdir -m 0755 usr/share/icons/hicolor/48x48/apps/
mkdir -m 0755 usr/share/icons/hicolor/512x512/
mkdir -m 0755 usr/share/icons/hicolor/512x512/apps/
mkdir -m 0755 usr/share/icons/hicolor/scalable/
mkdir -m 0755 usr/share/icons/hicolor/scalable/apps/
cp ../mineralapp.desktop usr/share/applications/mineralapp.desktop
cp ../icon/mineralapp-48.png usr/share/icons/hicolor/48x48/apps/mineralapp.png
cp ../icon/mineralapp-512.png usr/share/icons/hicolor/512x512/apps/mineralapp.png
cp ../icon/mineralapp-hires.svg usr/share/icons/hicolor/scalable/apps/mineralapp.svg

chmod 0644 usr/share/applications/mineralapp.desktop
chmod 0644 usr/share/icons/hicolor/48x48/apps/mineralapp.png
chmod 0644 usr/share/icons/hicolor/512x512/apps/mineralapp.png
chmod 0644 usr/share/icons/hicolor/scalable/apps/mineralapp.svg

mkdir -m 0755 usr/share/doc/
mkdir -m 0755 usr/share/doc/mineralapp/
cp ../LICENSE usr/share/doc/mineralapp/copyright
chmod 0644 usr/share/doc/mineralapp/copyright

mkdir DEBIAN

cat <<EOF >DEBIAN/control
Package: mineralapp
Version: $version
Section: science
Architecture: amd64
Priority: optional
Depends: libc6,libsqlite3-0,libwxgtk3.0-gtk3-0v5
Maintainer: Simone Conti <simonecnt@gmail.com>
Description: Simple application to catalog your mineral collection.
 MineralApp is a small and simple application to create a database of
 your minerals. You can add your mineral collection, storing any details
 you are interested in, helping you (hopefully...) to keep your mineral
 collection well organized!
EOF

cd ../

fakeroot dpkg-deb --build mineralapp_${version}_amd64
lintian mineralapp_${version}_amd64.deb

