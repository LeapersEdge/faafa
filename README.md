# faafa (fuck around and find aout)

 Collection of some of my new, free time, random projects.

# Build

Linux build:
```
cd
git clone --detph 1 https://github.com/LeapersEdge/faafa/
cd faafa
mkdir build
cd build
cmake ..
make -j$(nproc)
```
Windows build: not supported (tho should still work with this CMakeLists.txt, likely with minor modifications)

- All projects are in `projects` directory
- Binaries will have name of project directory
- No support for building only certain projects (build all or nothing, or just delete all you dont want to build)
