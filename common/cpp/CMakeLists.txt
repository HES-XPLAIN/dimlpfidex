project(common)

FILE(GLOB COMMON_SRC
    src/errorHandler.h
    src/stringI.cpp
    src/stringI.h
    src/dataSet.h
    src/dataSet.cpp
    src/checkFun.cpp
    src/checkFun.h
    src/antecedent.cpp
    src/antecedent.h
    src/rule.cpp
    src/rule.h
    src/parameters.cpp
    src/parameters.h
)

# make variables visible to external CMakeLists.txt
set(COMMON_SRC "${COMMON_SRC}" CACHE INTERNAL "")
add_library(common STATIC ${COMMON_SRC})
