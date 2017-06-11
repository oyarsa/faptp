# Instruções de compilação

## Windows 10 (Visual Studio 2017)
Provavelmente funciona com Visual Studio 2015 Update 3 mudando o gerador do CMake,
mas não foi testado.

Pré-requisito: Python (com pip) e Cmake (2.8+)

    > pip install conan
    > git clone https://github.com/oyarsa/faptp.git
    > cd faptp
    > mkdir _build
    > cd _build
    > conan install --build=missing ..
    > cmake -G "Visual Studio 15"
    > cmake --build . --config Release

## Ubuntu 16.04 (GCC 5.4.2)

    # apt update
    # apt install pip build-essential cmake
    $ pip install conan
    $ git clone https://github.com/oyarsa/faptp.git
    $ cd faptp
    $ mkdir .build && cd .build
    $ conan install -s compiler=gcc -s compiler.libcxx=libstdc++11 --build=missing ..
    $ cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    $ cmake --build .
