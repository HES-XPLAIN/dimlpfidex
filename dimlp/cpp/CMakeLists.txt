project(dimlp)

FILE(GLOB DIMLP_COMMON_SRC
    src/dataSet.cpp
    src/dataSet.h
    src/errFunct.cpp
    src/errFunct.h
    src/misc.cpp
    src/misc.h
    src/randFun.cpp
    src/randFun.h
    src/standAct.cpp
    src/standAct.h
    src/bpNN.cpp
    src/bpNN.h
    src/dimlp.cpp
    src/dimlp.h
    src/stairObj.cpp
    src/stairObj.h
    src/layer.cpp
    src/layer.h
    src/layerD2.cpp
    src/layerD2.h
    src/layerD3.cpp
    src/layerD3.h
    src/layerD4.cpp
    src/layerD4.h
    src/layerDmp.cpp
    src/layerDmp.h
    src/layerFdp.cpp
    src/layerFdp.h
)

FILE(GLOB DIMLP_TRN_SRC
    src/dimlpTrnFct.cpp
    src/dimlpTrnFct.h
    src/ante.cpp
    src/ante.h
    src/attrName.cpp
    src/attrName.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/dimlpCommonFun.cpp
    src/dimlpCommonFun.h
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/dimlpRule.cpp
    src/dimlpRule.h
    src/rulePro.cpp
    src/rulePro.h
    src/thresD.cpp
    src/thresD.h
    src/vectWRV.cpp
    src/vectWRV.h
    src/virtHyp.cpp
    src/virtHyp.h
)

FILE(GLOB DIMLP_PRED_SRC
    src/dimlpPredFct.cpp
    src/dimlpPredFct.h
    src/dimlpCommonFun.cpp
    src/dimlpCommonFun.h
)

FILE(GLOB DIMLP_CLS_SRC
    src/dimlpClsFct.cpp
    src/dimlpClsFct.h
    src/dimlpCommonFun.cpp
    src/dimlpCommonFun.h
)

FILE(GLOB DIMLP_BT_SRC
    src/dimlpBTFct.cpp
    src/dimlpBTFct.h
    src/ante.cpp
    src/ante.h
    src/attrName.cpp
    src/attrName.h
    src/bagDimlp.cpp
    src/bagDimlp.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/dimlpRule.cpp
    src/dimlpRule.h
    src/rulePro.cpp
    src/rulePro.h
    src/thresD.cpp
    src/thresD.h
    src/vectWRV.cpp
    src/vectWRV.h
    src/virtHyp.cpp
    src/virtHyp.h
)

FILE(GLOB DENS_CLS_SRC
    src/densClsFct.cpp
    src/densClsFct.h
    src/ante.cpp
    src/ante.h
    src/attrName.cpp
    src/attrName.h
    src/bagDimlp.cpp
    src/bagDimlp.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/dimlpRule.cpp
    src/dimlpRule.h
    src/rulePro.cpp
    src/rulePro.h
    src/thresD.cpp
    src/thresD.h
    src/vectWRV.cpp
    src/vectWRV.h
    src/virtHyp.cpp
    src/virtHyp.h
)

FILE(GLOB DIMLP_RUL_SRC
    src/dimlpRulFct.cpp
    src/dimlpRulFct.h
    src/ante.cpp
    src/ante.h
    src/attrName.cpp
    src/attrName.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/dimlpRule.cpp
    src/dimlpRule.h
    src/rulePro.cpp
    src/rulePro.h
    src/thresD.cpp
    src/thresD.h
    src/vectWRV.cpp
    src/vectWRV.h
    src/virtHyp.cpp
    src/virtHyp.h
)

# make variables visible to external CMakeLists.txt
set(DIMLP_COMMON_SRC "${DIMLP_COMMON_SRC}" CACHE INTERNAL "")
set(DIMLP_TRN_SRC "${DIMLP_TRN_SRC}" CACHE INTERNAL "")
set(DIMLP_PRED_SRC "${DIMLP_PRED_SRC}" CACHE INTERNAL "")
set(DIMLP_CLS_SRC "${DIMLP_CLS_SRC}" CACHE INTERNAL "")
set(DIMLP_BT_SRC "${DIMLP_BT_SRC}" CACHE INTERNAL "")
set(DENS_CLS_SRC "${DENS_CLS_SRC}" CACHE INTERNAL "")
set(DIMLP_RUL_SRC "${DIMLP_RUL_SRC}" CACHE INTERNAL "")

add_library(dimlpTrnLib STATIC ${DIMLP_COMMON_SRC} ${DIMLP_TRN_SRC})
add_library(dimlpPredLib STATIC ${DIMLP_COMMON_SRC} ${DIMLP_PRED_SRC})
add_library(dimlpClsLib STATIC ${DIMLP_COMMON_SRC} ${DIMLP_CLS_SRC})
add_library(dimlpBTLib STATIC ${DIMLP_COMMON_SRC} ${DIMLP_BT_SRC})
add_library(densClsLib STATIC ${DIMLP_COMMON_SRC} ${DENS_CLS_SRC})
add_library(dimlpRulLib STATIC ${DIMLP_COMMON_SRC} ${DIMLP_RUL_SRC})

# build executables
add_executable(dimlpTrn ${DIMLP_COMMON_SRC} ${DIMLP_TRN_SRC} src/dimlpTrn.cpp)
add_executable(dimlpPred ${DIMLP_COMMON_SRC} ${DIMLP_PRED_SRC} src/dimlpPred.cpp)
add_executable(dimlpCls ${DIMLP_COMMON_SRC} ${DIMLP_CLS_SRC} src/dimlpCls.cpp)
add_executable(dimlpBT ${DIMLP_COMMON_SRC} ${DIMLP_BT_SRC} src/dimlpBT.cpp)
add_executable(densCls ${DIMLP_COMMON_SRC} ${DENS_CLS_SRC} src/densCls.cpp)
add_executable(dimlpRul ${DIMLP_COMMON_SRC} ${DIMLP_RUL_SRC} src/dimlpRul.cpp)
add_dependencies(dimlpTrn common)
add_dependencies(dimlpPred common)
add_dependencies(dimlpCls common)
add_dependencies(dimlpBT common)
add_dependencies(densCls common)
add_dependencies(dimlpRul common)
target_link_libraries(dimlpTrn common)
target_link_libraries(dimlpPred common)
target_link_libraries(dimlpCls common)
target_link_libraries(dimlpBT common)
target_link_libraries(densCls common)
target_link_libraries(dimlpRul common)

target_link_libraries(dimlpTrnLib common)
target_link_libraries(dimlpPredLib common)
target_link_libraries(dimlpClsLib common)
target_link_libraries(dimlpBTLib common)
target_link_libraries(densClsLib common)
target_link_libraries(dimlpRulLib common)

set_target_properties(
    dimlpTrn PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    dimlpPred PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    dimlpCls PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    dimlpBT PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    densCls PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    dimlpRul PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
