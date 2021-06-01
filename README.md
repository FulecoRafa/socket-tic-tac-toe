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

## Principais verificações de falha de conexão e de transmissão das informações

### Falha de conexão

#### Cliente
No lado do cliente, é realizada a verificação de falha na criação do socket, na sua vinculação (bind) e na conexão com o servidor.
#### Servidor
No lado do servidor, todas as conexões de clientes são aceitas na função match_making. Nela, são realizadas verificações de erro nos descritores de arquivos recém criados (via accept) e são enviadas mensagens para o cliente informando que a conexão foi aceita. Durante a sua criação, o socket do servidor é testado para que se tenha certeza de que ele foi criado corretamente, vinculado e que esteja ouvindo por solicitações dos clientes.
Assim que são aceitas, as conexões são colocadas numa fila que é utilizada para formar as partidas, unindo jogadores dois a dois. Não foi implementada uma solução para quando um jogador finaliza a conexão enquanto está no lobby.
### Transmissão de informações
Durante uma partida, mensagens de controle são trocadas entre os dois jogadores e o servidor. Como o protocolo utilizado é o TCP, não foi necessário realizar verificações manuais de confirmação de recebimento de mensagens ou tarefas correlatas. A principal preocupação se manteve em fazer com que os dois jogadores soubessem a todo tempo do estado atual do jogo. Nesse sentido, dois maiores problemas surgiram:

#### Como comunicar ao outro jogador sobre a desistência consciente do seu oponente?
Para resolver este problema, bastou que o próprio servidor interceptasse a mensagem proveniente do jogador desistente e a repassasse para o seu oponente. Ao receber essa mensagem, o processo do jogador que sobrou é finalizado.
#### Como comunicar ao outro jogador sobre a desistência do seu oponente caso ele tenha perdido a conexão ou fechado o jogo de forma abrupta?
Neste caso, nenhuma mensagem é enviada ao servidor e, consequentemente, nada é enviado para o oponente. Infelizmente, este problema não foi resolvido, pois a sua solução envolvia tópicos que fugiam ao escopo deste projeto.

