cmake_minimum_required(VERSION 3.1)

include_directories(${PROJECT_SOURCE_DIR}/pybind11
		            ${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}
                    )

find_package(PythonLibs 3.6)
include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(pywrapper)