cmake_minimum_required(VERSION 3.2)

project(http_server)

link_directories(/home/linux/Storage/bin/lib)

add_executable(${CMAKE_PROJECT_NAME} main.cpp Server.cpp Log.cpp ThreadPool.cpp Buffer.cpp Epoll.cpp Timer.cpp Connection.cpp HttpProcess.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} pthread processor)
