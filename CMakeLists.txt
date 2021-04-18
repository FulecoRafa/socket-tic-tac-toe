cmake_minimum_required(VERSION 3.17)
project(socket_tic_tac_toe)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(helpers)

add_executable(socket_tic_tac_toe
        client/Makefile
        client/tictactoe_client.c
        helpers/macros.h
        server/Makefile
        server/tictactoe_server.c
        Makefile
        README.md server/game.c server/game.h server/match.c server/match.h)
