cmake_minimum_required(VERSION 3.12)
project(KonwersjaGrafow)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(KonwersjaGrafow src/main.cpp src/Graph.cpp src/PrimePathGen.cpp src/PrimePathEdgeGen.cpp src/ConverterDAG.cpp src/FlowNetwork.cpp src/ResidualGraph.cpp
        include/GraphUtils.h
)

target_include_directories(KonwersjaGrafow PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_executable(test
        include/Graph.h
        src/Graph.cpp

        include/PrimePathGen.h
        src/PrimePathGen.cpp

        include/PrimePathEdgeGen.h
        src/PrimePathEdgeGen.cpp

        include/ConverterDAG.h
        src/ConverterDAG.cpp

        include/ResidualEdge.h
        src/ResidualGraph.cpp

        include/FlowNetwork.h
        src/FlowNetwork.cpp

        test/include/boost/json.hpp
        test/include/test.h
        test/include/PP_to_G2.h
        test/src/PP_to_G2.cpp
        test/src/test.cpp
        test/src/main.cpp
        test/src/helper.hpp
        include/GraphUtils.h

        #test/src/uj.hpp
)

target_include_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/test/include")
target_link_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/test/lib/graphviz")
target_link_libraries(test PUBLIC gvc cgraph cdt)

add_custom_command(TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test/bin"
        "$<TARGET_FILE_DIR:test>"
)