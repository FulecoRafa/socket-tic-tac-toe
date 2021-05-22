# socket-tic-tac-toe
Projeto simples de sockets usando C aplicando um jogo da velha online.

## Integrantes
- Rafael Tavares Oliveira [11219071]
- João Pedro Almeida Santos Secundino [10692054]
- Gabriel Santos Souza [11208176]
- Breno Cunha Queiroz [11218991]
- Henrique Hiram Libutti Núñez [11275300]

## Como rodar o programa
No diretório raiz:
Basta rodar o comando `make build`;
Rode o servidor antes dos clientes com o comando `make run.server`;
Rode os cliente com o comado `make run.client` (uma vez para cada cliente).

## Comandos
Todos os comandos usam makefile para rodar.

### build.server
Compila e linka servidor

### build.client
Compila e linka cliente

### build
Compila cliente e servidor

### run.client
Roda cliente

### run.server
Roda servidor

### clean
Limpa todos os compilados

## Estruturação do projeto
Todos os arquivos de código podem estar em qualquer lugar das pastas client e server, dividindo por suas utilidades
Helpers não tem suporte para arquivo .c, apenas .h
Helpers tem macros e outras definições para auxiliar no código

## Especificações de benchmarking
###   OS
Linux pop-os 5.11.0-7614-generic #15~1618626693~20.04~ecb25cd-Ubuntu SMP Thu Apr 22 16:20:32 UTC  x86_64 x86_64 x86_64 GNU/Linux
###   Compilador
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/9/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:hsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.3.0-17ubuntu1~20.04' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-9-HskZEa/gcc-9-9.3.0/debian/tmp-nvptx/usr,hsa --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)