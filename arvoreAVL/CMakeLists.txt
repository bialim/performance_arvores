cmake_minimum_required(VERSION 3.20)
project(ArvoreAVL_GUI C CXX)

set(CMAKE_C_STANDARD 11)

# Encontra o pacote PkgConfig, que ajuda a localizar o GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Adiciona os diretórios de inclusão do GTK3
include_directories(${GTK3_INCLUDE_DIRS})

# Cria o executável
add_executable(ArvoreAVL_GUI "interface_grafica.cpp")

# Vincula as bibliotecas do GTK3 ao seu executável
target_link_libraries(ArvoreAVL_GUI ${GTK3_LIBRARIES})