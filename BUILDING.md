# Instruções de compilação

## Windows 10 (Visual Studio 2017)
Provavelmente funciona com Visual Studio 2015 Update 3 mudando o gerador do CMake,
mas não foi testado.

Pré-requisito: Vcpkg e Cmake (2.8+)

    > git clone https://github.com/oyarsa/faptp.git
    > cd faptp
    > mkdir _build
    > cd _build..
    > cmake -G "Visual Studio 15 Win64" -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT$\scripts\buildsystems\vcpkg.cmake
    > cmake --build . --config Release

## Ubuntu 16.04 (GCC 5.4)

    # apt update
    # apt install cmake libcurl
    $ git clone https://github.com/oyarsa/faptp.git
    $ cd faptp
    $ mkdir .build && cd .build
    $ cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
