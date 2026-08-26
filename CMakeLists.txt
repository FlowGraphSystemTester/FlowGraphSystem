cmake_minimum_required(VERSION 3.12)
project(KonwersjaGrafow)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main src/main.cpp src/Graph.cpp src/PrimePathGen.cpp src/PrimePathEdgeGen.cpp src/ConverterDAG.cpp src/FlowNetwork.cpp src/ResidualGraph.cpp include/GraphUtils.h)

target_include_directories(main PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_executable(test
        include/GraphUtils.h
        src/Graph.cpp
        src/PrimePathGen.cpp
        src/PrimePathEdgeGen.cpp
        src/ConverterDAG.cpp
        src/ResidualGraph.cpp
        src/FlowNetwork.cpp

        test/include/boost/json.hpp
        test/src/PP_to_G2.cpp
        test/src/visualization.cpp
        test/src/testMain.cpp
        test/src/G1_to_PP.cpp
        test/src/G2_to_G3.cpp

        #test/src/uj.hpp
)

add_executable(article
        include/GraphUtils.h
        src/Graph.cpp
        src/PrimePathGen.cpp
        src/PrimePathEdgeGen.cpp
        src/ConverterDAG.cpp
        src/ResidualGraph.cpp
        src/FlowNetwork.cpp

        test/include/boost/json.hpp
        test/include/visualization.h
        test/include/PP_to_G2.h
        test/src/PP_to_G2.cpp
        test/src/visualization.cpp
        test/src/suiteHelper.hpp
        test/src/articleMain.cpp
        test/include/G1_to_PP.h
        test/src/G1_to_PP.cpp
        test/include/G2_to_G3.h
        test/src/G2_to_G3.cpp

        #test/src/uj.hpp
)

target_include_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/test/include")
target_link_directories(test PUBLIC "${CMAKE_SOURCE_DIR}/test/lib/graphviz")
target_link_libraries(test PUBLIC gvc cgraph cdt)

target_include_directories(article PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(article PUBLIC "${CMAKE_SOURCE_DIR}/test/include")
target_link_directories(article PUBLIC "${CMAKE_SOURCE_DIR}/test/lib/graphviz")
target_link_libraries(article PUBLIC gvc cgraph cdt)

add_custom_command(TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test/bin"
        "$<TARGET_FILE_DIR:test>"
)

add_custom_command(TARGET article POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/test/bin"
        "$<TARGET_FILE_DIR:article>"
)