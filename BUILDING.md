# Compilation instructions

## Windows 10 (Visual Studio 2017)
Pre-requisite: Vcpkg and Cmake (2.8+)

    > git clone https://github.com/oyarsa/faptp.git
    > cd faptp
    > mkdir _build
    > cd _build..
    > cmake -G "Visual Studio 15 Win64" -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT$\scripts\buildsystems\vcpkg.cmake
    > cmake --build . --config Release

## Ubuntu 17.10 (GCC 7.2)
Seperately install fmtlib.

    # apt update
    # apt install cmake libcurl4-openssl-dev libjsoncpp-dev
    $ git clone https://github.com/oyarsa/faptp.git
    $ cd faptp
    $ mkdir .build && cd .build
    $ cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
