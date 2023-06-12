project(dimlp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

FILE(GLOB COMMON_SRC
    src/checkFun.cpp
    src/checkFun.h
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
    src/writeErr.cpp
    src/writeErr.h
)

FILE(GLOB DIMLP_TRN_SRC
    src/DimlpTrnFct.cpp
    src/DimlpTrnFct.h
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
    src/DimlpPredFct.cpp
    src/DimlpPredFct.h
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
    src/DimlpClsFct.cpp
    src/DimlpClsFct.h
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
    src/DimlpBTFct.cpp
    src/DimlpBTFct.h
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
    src/DensClsFct.cpp
    src/DensClsFct.h
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
    src/DimlpRulFct.cpp
    src/DimlpRulFct.h
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

FILE(GLOB CROSS_VALID_SRC
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
    src/DimlpTrnFct.cpp
    src/DimlpTrnFct.h
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
    ../../fidex/cpp/src/fidexFct.cpp
    ../../fidex/cpp/src/fidexFct.h
    ../../fidex/cpp/src/hyperspace.cpp
    ../../fidex/cpp/src/hyperspace.h
    ../../fidexGlo/cpp/src/fidexGloRulesFct.cpp
    ../../fidexGlo/cpp/src/fidexGloRulesFct.h
    ../../fidexGlo/cpp/src/getRulesFun.cpp
    ../../fidexGlo/cpp/src/getRulesFun.h
    ../../fidexGlo/cpp/src/fidexAlgo.cpp
    ../../fidexGlo/cpp/src/fidexAlgo.h
    ../../fidexGlo/cpp/src/hyperbox.cpp
    ../../fidexGlo/cpp/src/hyperbox.h
    ../../fidexGlo/cpp/src/hyperspace.cpp
    ../../fidexGlo/cpp/src/hyperspace.h
    ../../fidexGlo/cpp/src/fidexGloStatsFct.cpp
    ../../fidexGlo/cpp/src/fidexGloStatsFct.h
    ../../fidexCommon/cpp/src/errorHandler.h
    ../../fidexCommon/cpp/src/dataSet.h
    ../../fidexCommon/cpp/src/dataSet.cpp
    ../../fidexCommon/cpp/src/checkFun.h
    ../../fidexCommon/cpp/src/checkFun.cpp
    ../../fidexCommon/cpp/src/hyperbox.cpp
    ../../fidexCommon/cpp/src/hyperbox.h
    ../../fidexCommon/cpp/src/hyperLocus.cpp
    ../../fidexCommon/cpp/src/hyperLocus.h
)

FILE(GLOB FCT_SRC
    src/DimlpPredFct.cpp
    src/DimlpPredFct.h
    src/DimlpClsFct.cpp
    src/DimlpClsFct.h
    src/DimlpRulFct.cpp
    src/DimlpRulFct.h
    src/DimlpTrnFct.cpp
    src/DimlpTrnFct.h
    src/DimlpBTFct.cpp
    src/DimlpBTFct.h
    src/DensClsFct.cpp
    src/DensClsFct.h
)

# make variables visible to external CMakeLists.txt
set(COMMON_SRC "${COMMON_SRC}" CACHE INTERNAL "")
set(DIMLP_TRN_SRC "${DIMLP_TRN_SRC}" CACHE INTERNAL "")
set(DIMLP_PRED_SRC "${DIMLP_PRED_SRC}" CACHE INTERNAL "")
set(DIMLP_CLS_SRC "${DIMLP_CLS_SRC}" CACHE INTERNAL "")
set(DIMLP_BT_SRC "${DIMLP_BT_SRC}" CACHE INTERNAL "")
set(DENS_CLS_SRC "${DENS_CLS_SRC}" CACHE INTERNAL "")
set(DIMLP_RUL_SRC "${DIMLP_RUL_SRC}" CACHE INTERNAL "")
set(CROSS_VALID_SRC "${CROSS_VALID_SRC}" CACHE INTERNAL "")
set(FCT_SRC "${FCT_SRC}" CACHE INTERNAL "")

# build executables
add_executable(DimlpTrn ${COMMON_SRC} ${DIMLP_TRN_SRC} src/DimlpTrn.cpp)
add_executable(DimlpPred ${COMMON_SRC} ${DIMLP_PRED_SRC} src/DimlpPred.cpp)
add_executable(DimlpCls ${COMMON_SRC} ${DIMLP_CLS_SRC} src/DimlpCls.cpp)
add_executable(DimlpBT ${COMMON_SRC} ${DIMLP_BT_SRC} src/DimlpBT.cpp)
add_executable(DensCls ${COMMON_SRC} ${DENS_CLS_SRC} src/DensCls.cpp)
add_executable(DimlpRul ${COMMON_SRC} ${DIMLP_RUL_SRC} src/DimlpRul.cpp)
add_executable(CrossValid ${COMMON_SRC} ${CROSS_VALID_SRC} src/CrossValid.cpp)

set_target_properties(
    DimlpTrn PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    DimlpPred PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    DimlpCls PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    DimlpBT PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    DensCls PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    DimlpRul PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(
    CrossValid PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
