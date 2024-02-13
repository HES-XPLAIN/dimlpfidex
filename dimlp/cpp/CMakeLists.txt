project(dimlp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB DIMLP_COMMON_SRC
    ../../common/cpp/src/checkFun.cpp
    ../../common/cpp/src/checkFun.h
    ../../common/cpp/src/parameters.cpp
    ../../common/cpp/src/parameters.h
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
    src/stringI.cpp
    src/stringI.h
)

FILE(GLOB DIMLP_TRN_SRC
    src/dimlpTrnFct.cpp
    src/dimlpTrnFct.h
    src/ante.cpp
    src/ante.h
    src/attrName.cpp
    src/attrName.h
    src/bpNN.cpp
    src/bpNN.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/dimlp.cpp
    src/dimlp.h
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
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/rule.cpp
    src/rule.h
    src/rulePro.cpp
    src/rulePro.h
    src/stairObj.cpp
    src/stairObj.h
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

FILE(GLOB DIMLP_CLS_SRC
    src/dimlpClsFct.cpp
    src/dimlpClsFct.h
    src/bpNN.cpp
    src/bpNN.h
    src/dimlp.cpp
    src/dimlp.h
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
    src/stairObj.cpp
    src/stairObj.h
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
    src/bpNN.cpp
    src/bpNN.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/dimlp.cpp
    src/dimlp.h
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
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/rule.cpp
    src/rule.h
    src/rulePro.cpp
    src/rulePro.h
    src/stairObj.cpp
    src/stairObj.h
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
    src/bpNN.cpp
    src/bpNN.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/dimlp.cpp
    src/dimlp.h
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
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/rule.cpp
    src/rule.h
    src/rulePro.cpp
    src/rulePro.h
    src/stairObj.cpp
    src/stairObj.h
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
    src/bpNN.cpp
    src/bpNN.h
    src/cleanRS.cpp
    src/cleanRS.h
    src/dimlp.cpp
    src/dimlp.h
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
    src/oneVarTD.cpp
    src/oneVarTD.h
    src/realHyp.cpp
    src/realHyp.h
    src/realHyp2.cpp
    src/realHyp2.h
    src/rule.cpp
    src/rule.h
    src/rulePro.cpp
    src/rulePro.h
    src/stairObj.cpp
    src/stairObj.h
    src/thresD.cpp
    src/thresD.h
    src/vectWRV.cpp
    src/vectWRV.h
    src/virtHyp.cpp
    src/virtHyp.h
)

FILE(GLOB FCT_SRC
    src/dimlpPredFct.cpp
    src/dimlpPredFct.h
    src/dimlpClsFct.cpp
    src/dimlpClsFct.h
    src/dimlpRulFct.cpp
    src/dimlpRulFct.h
    src/dimlpTrnFct.cpp
    src/dimlpTrnFct.h
    src/dimlpBTFct.cpp
    src/dimlpBTFct.h
    src/densClsFct.cpp
    src/densClsFct.h
)

# make variables visible to external CMakeLists.txt
set(DIMLP_COMMON_SRC "${DIMLP_COMMON_SRC}" CACHE INTERNAL "")
set(DIMLP_TRN_SRC "${DIMLP_TRN_SRC}" CACHE INTERNAL "")
set(DIMLP_PRED_SRC "${DIMLP_PRED_SRC}" CACHE INTERNAL "")
set(DIMLP_CLS_SRC "${DIMLP_CLS_SRC}" CACHE INTERNAL "")
set(DIMLP_BT_SRC "${DIMLP_BT_SRC}" CACHE INTERNAL "")
set(DENS_CLS_SRC "${DENS_CLS_SRC}" CACHE INTERNAL "")
set(DIMLP_RUL_SRC "${DIMLP_RUL_SRC}" CACHE INTERNAL "")
set(FCT_SRC "${FCT_SRC}" CACHE INTERNAL "")

# build executables
add_executable(dimlpTrn ${DIMLP_COMMON_SRC} ${DIMLP_TRN_SRC} src/dimlpTrn.cpp)
add_executable(dimlpPred ${DIMLP_COMMON_SRC} ${DIMLP_PRED_SRC} src/dimlpPred.cpp)
add_executable(dimlpCls ${DIMLP_COMMON_SRC} ${DIMLP_CLS_SRC} src/dimlpCls.cpp)
add_executable(dimlpBT ${DIMLP_COMMON_SRC} ${DIMLP_BT_SRC} src/dimlpBT.cpp)
add_executable(densCls ${DIMLP_COMMON_SRC} ${DENS_CLS_SRC} src/densCls.cpp)
add_executable(dimlpRul ${DIMLP_COMMON_SRC} ${DIMLP_RUL_SRC} src/dimlpRul.cpp)

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
