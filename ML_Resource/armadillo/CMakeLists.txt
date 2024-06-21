add_library(armadillo 
    wrapper1.cpp
    wrapper2.cpp
)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
set(C_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")

target_include_directories(armadillo PUBLIC "${PROJECT_SOURCE_DIR}/ML_Resource/openblas_x64-windows/include")
target_link_libraries(armadillo PUBLIC "${PROJECT_SOURCE_DIR}/ML_Resource/openblas_x64-windows/lib/openblas.lib")