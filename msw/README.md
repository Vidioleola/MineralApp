
# Compile on Microsoft Windows

## Prerequisites

I use Visual Studio, so first download and install it.

Then you need to install `wxWidgets` and `sqlite3` libraries.

I found it easiest to install these two via `vcpkg`. So, open the Windows Power
Shell and download and install `vcpkg`:

(if you don't have `git`, install that too from its website)

```
git clone https://github.com/Microsoft/vcpkg
```

and then:

```
cd vcpkg
.\bootstrap-vcpkg.bat
```

Then install wxWidgets:

```
vcpkg install wxwidgets
```

and sqlite3:

```
vcpkg install sqlite3
```

Finally, make the two libraries available to Visual Studio:

```
vcpkg integrate install
```

## Compile MineralApp

Open Visual studio and open the `mineralapp.sln` from the `msw` directory.

Then `Build->Build Solution`.

When done, you can find the `MineralApp.exe` binary (with some needed dll libraries)
inside the `Release` directory.

To build the installer, first install NSIS, then right-click on
`mineralapp.nsi` and click `Compile NSIS script`. This will create
`mineralapp-install.exe`.

