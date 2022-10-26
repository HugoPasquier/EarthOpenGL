include(GenerateExportHeader)
include(ExternalProject)
include(WriteCompilerDetectionHeader)

# Set policy if policy is available
function(set_policy POL VAL)

    if(POLICY ${POL})
        cmake_policy(SET ${POL} ${VAL})
    endif()

endfunction(set_policy)

# Set policies
set_policy(CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
set_policy(CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
set_policy(CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
set_policy(CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.

# Installation paths
if(UNIX AND SYSTEM_DIR_INSTALL)
    # Install into the system (/usr/bin or /usr/local/bin)
    set(INSTALL_ROOT      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_CMAKE     "share/${project}/cmake") # /usr/[local]/share/<project>
    set(INSTALL_DATA      "share/${project}")       # /usr/[local]/share/<project>
    set(INSTALL_BIN       "bin")                    # /usr/[local]/bin
    set(INSTALL_SHARED    "lib")                    # /usr/[local]/lib
    set(INSTALL_LIB       "lib")                    # /usr/[local]/lib
    set(INSTALL_INCLUDE   "include")                # /usr/[local]/include
    set(INSTALL_DOC       "share/doc/${project}")   # /usr/[local]/share/doc/<project>
    set(INSTALL_SHORTCUTS "share/applications")     # /usr/[local]/share/applications
    set(INSTALL_ICONS     "share/pixmaps")          # /usr/[local]/share/pixmaps
    set(INSTALL_INIT      "/etc/init")              # /etc/init (upstart init scripts)
else()
    # Install into local directory
    set(INSTALL_ROOT      ".")                      # ./
    set(INSTALL_CMAKE     "cmake")                  # ./cmake
    set(INSTALL_DATA      ".")                      # ./bin
    set(INSTALL_BIN       ".")                      # ./bin
    set(INSTALL_SHARED    "lib")                    # ./lib
    set(INSTALL_LIB       "lib")                    # ./lib
    set(INSTALL_INCLUDE   "include")                # ./include
    set(INSTALL_DOC       "doc")                    # ./doc
    set(INSTALL_SHORTCUTS "misc")                   # ./misc
    set(INSTALL_ICONS     "misc")                   # ./misc
    set(INSTALL_INIT      "misc")                   # ./misc
endif()


# 
# Platform and architecture setup
# 

# Get upper case system name
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

# Determine architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X64 ON)
endif()


# 
# Project options
# 

set(DEFAULT_PROJECT_OPTIONS
    DEBUG_POSTFIX             "d"
    CXX_STANDARD              11
    LINKER_LANGUAGE           "CXX"
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET     "hidden"
)


# 
# Include directories
# 

set(DEFAULT_INCLUDE_DIRECTORIES)


# 
# Libraries
# 

set(DEFAULT_LIBRARIES)


# 
# Compile definitions
# 

set(DEFAULT_COMPILE_DEFINITIONS
    SYSTEM_${SYSTEM_NAME_UPPER}
)

# MSVC compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(DEFAULT_COMPILE_DEFINITIONS ${DEFAULT_COMPILE_DEFINITIONS}
        _SCL_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the Standard C++ Library
        _CRT_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the CRT Library
    )
endif ()


# 
# Compile options
# 

set(DEFAULT_COMPILE_OPTIONS)

# MSVC compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        /MP           # -> build with multiple processes
        /W4           # -> warning level 4
        # /WX         # -> treat warnings as errors
        /Zm114        # -> Memory size for precompiled headers

        /wd4251       # -> disable warning: 'identifier': class 'type' needs to have dll-interface to be used by clients of class 'type2'
        /wd4592       # -> disable warning: 'identifier': symbol will be dynamically initialized (implementation limitation)
        /wd4127       # -> disable warning: conditional expression is constant (caused by Qt)

        #$<$<CONFIG:Debug>:
        #/RTCc         # -> value is assigned to a smaller data type and results in a data loss
        #>

        $<$<CONFIG:Release>: 
        /Gw           # -> whole program global optimization
        /GS-          # -> buffer security check: no 
        /GL           # -> whole program optimization: enable link-time code generation (disables Zi)
        /GF           # -> enable string pooling
        >
    )
endif ()

# GCC and Clang compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        -Wall
        -Wextra
        -Wunused

        -Wreorder
        -Wignored-qualifiers
        -Wmissing-braces
        -Wreturn-type
        -Wswitch
        -Wswitch-default
        -Wuninitialized
        -Wmissing-field-initializers
        
        $<$<CXX_COMPILER_ID:GNU>:
            -Wmaybe-uninitialized
            
            $<$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.8>:
                -Wpedantic
                
                -Wreturn-local-addr
            >
        >
        
        $<$<CXX_COMPILER_ID:Clang>:
            -Wpedantic
            
            -Wreturn-stack-address
        >
        
        $<$<PLATFORM_ID:Darwin>:
            -pthread
        >
    )
endif ()


# 
# Linker options
# 

set(DEFAULT_LINKER_OPTIONS)

# Use pthreads on mingw and linux
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(DEFAULT_LINKER_OPTIONS
        -pthread
    )
endif()

# 
# External dependencies
# 

find_package(OpenGL REQUIRED)

# 
# Library name and options
# 

# Target name
set(target glbinding)

# Exit here if required dependencies are not met
message(STATUS "Lib ${target}")

# Set API export file and macro
string(MAKE_C_IDENTIFIER ${target} target_id)
string(TOUPPER ${target_id} target_id)
set(feature_file         "include/${target}/${target}_features.h")
set(export_file          "include/${target}/${target}_export.h")
set(template_export_file "include/${target}/${target}_api.h")
set(export_macro         "${target_id}_API")


# 
# Sources
# 

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/include/${target}")
set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(headers
    ${include_path}/nogl.h

    ${include_path}/gl/bitfield.h
    ${include_path}/gl/boolean.h
    ${include_path}/gl/enum.h
    ${include_path}/gl/extension.h
    ${include_path}/gl/functions.h
    ${include_path}/gl/types.h
    ${include_path}/gl/values.h
    
    ${include_path}/glbinding.h

    ${include_path}/glbinding.h
    ${include_path}/AbstractFunction.h
    ${include_path}/CallbackMask.h
    ${include_path}/Function.h
    ${include_path}/FunctionCall.h
    ${include_path}/Binding.h
    ${include_path}/ProcAddress.h
    ${include_path}/Value.h
    ${include_path}/Version.h
    ${include_path}/Version.inl
    ${include_path}/SharedBitfield.h

    # KHR binding
    ${include_path}/AbstractFunction.h
    ${include_path}/AbstractState.h
    ${include_path}/AbstractValue.h
    ${include_path}/Boolean8.h
    ${include_path}/Boolean8.inl
    ${include_path}/CallbackMask.h
    ${include_path}/CallbackMask.inl
    ${include_path}/ContextHandle.h
    ${include_path}/Function.h
    ${include_path}/Function.inl
    ${include_path}/FunctionCall.h
    ${include_path}/ProcAddress.h
    ${include_path}/SharedBitfield.h
    ${include_path}/SharedBitfield.inl
    ${include_path}/State.h
    ${include_path}/Value.h
    ${include_path}/Value.inl
    ${include_path}/Version.h
    ${include_path}/Version.inl
)

# add featured headers

file(GLOB featured_includes ${include_path}/gl*/*.h)
list(APPEND headers ${featured_includes})

set(sources
    ${source_path}/glbinding.cpp
    ${source_path}/Binding.cpp
    ${source_path}/Binding_list.cpp

    ${source_path}/glbinding.cpp
    
    ${source_path}/gl/functions-patches.cpp

    ${source_path}/Binding_pch.h
    
    # KHR binding
    ${source_path}/AbstractFunction.cpp
    ${source_path}/AbstractState.cpp
    ${source_path}/AbstractValue.cpp
    ${source_path}/FunctionCall.cpp
    ${source_path}/State.cpp
)


# use splitted function and binding sources on windows compilers (e.g., mingw, msvc) and clang
# also use them for GCC for reduced project setup complexity

file(GLOB splitted_binding_sources ${source_path}/Binding_objects_*.cpp)
file(GLOB splitted_functions_sources ${source_path}/gl/functions_*.cpp)

list(APPEND sources 
    ${splitted_binding_sources}
    ${splitted_functions_sources}
)

if(MSVC_IDE)
    # on msvc use private (non-api) per file precompiled headers on those grouped sources

    list(APPEND sources
        ${source_path}/Binding_pch.cpp)
endif()


# Define function "source_group_by_path with three mandatory arguments (PARENT_PATH, REGEX, GROUP, ...)
# to group source files in folders (e.g. for MSVC solutions).
#
# Example:
# source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}/src" "\\\\.h$|\\\\.inl$|\\\\.cpp$|\\\\.c$|\\\\.ui$|\\\\.qrc$" "Source Files" ${sources})
function(source_group_by_path PARENT_PATH REGEX GROUP)

    foreach (FILENAME ${ARGN})
        
        get_filename_component(FILEPATH "${FILENAME}" REALPATH)
        file(RELATIVE_PATH FILEPATH ${PARENT_PATH} ${FILEPATH})
        get_filename_component(FILEPATH "${FILEPATH}" DIRECTORY)

        string(REPLACE "/" "\\" FILEPATH "${FILEPATH}")

    source_group("${GROUP}\\${FILEPATH}" REGULAR_EXPRESSION "${REGEX}" FILES ${FILENAME})

    endforeach()

endfunction(source_group_by_path)

# Creates an export header similar to generate_export_header, but for templates.
# The main difference is that for MSVC, templates must not get exported.
# When the file ${export_file} is included in source code, the macro ${target_id}_TEMPLATE_API
# may get used to define public visibility for templates on GCC and Clang platforms.
function(generate_template_export_header target target_id export_file)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
        configure_file(${PROJECT_SOURCE_DIR}/ext/glbinding/codegeneration/template_msvc_api.h.in ${CMAKE_CURRENT_BINARY_DIR}/${export_file})
    else()
        configure_file(${PROJECT_SOURCE_DIR}/ext/glbinding/codegeneration/template_api.h.in ${CMAKE_CURRENT_BINARY_DIR}/${export_file})
    endif()
endfunction()

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.inl$" 
    ${header_group} ${headers})
source_group_by_path(${source_path}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.inl$" 
    ${source_group} ${sources})


# 
# Create library
# 

# since we use stl and stl is intended to use exceptions, exceptions should not be disabled
#if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    # workaround for removing default flags 
    # string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) 
#endif ()


# Build library
add_library(${target}
    ${sources}
    ${headers}
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Export library for downstream projects
export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)

# Create API export header
generate_export_header(${target}
    EXPORT_FILE_NAME  ${export_file}
    EXPORT_MACRO_NAME ${export_macro}
)

generate_template_export_header(${target}
    ${target_id}
    ${template_export_file}
)

# Create feature detection header
if (WriterCompilerDetectionHeaderFound)
    write_compiler_detection_header(
        FILE ${feature_file}
        PREFIX ${target_id}
        COMPILERS AppleClang Clang GNU MSVC
        FEATURES
            cxx_thread_local
            cxx_constexpr
            cxx_attribute_deprecated
            cxx_noexcept
        VERSION 3.2
    )
else()
    file(
        COPY ${PROJECT_SOURCE_DIR}/ext/glbinding/codegeneration/${target}_features.h
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/include/${target}
        USE_SOURCE_PERMISSIONS
    )
endif()


# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
    VERSION ${META_VERSION}
    SOVERSION ${META_VERSION_MAJOR}
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${PROJECT_BINARY_DIR}/source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include

    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_LIBRARIES}
    ${OPENGL_LIBRARIES}

    INTERFACE
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    # since we use stl and stl is intended to use exceptions, exceptions should not be disabled
    # furthermore, this flag is not officially supported
    #$<$<CXX_COMPILER_ID:MSVC>:_HAS_EXCEPTIONS=0> 

    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${target_id}_STATIC_DEFINE>
    ${DEFAULT_COMPILE_DEFINITIONS}

    INTERFACE
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}

    INTERFACE
)


#
# Precompiled Header Configuration
#

if (MSVC_IDE)
    # on msvc use private (non-api) per file precompiled headers on those grouped sources

    set_source_files_properties(${source_path}/Binding_pch.cpp PROPERTIES COMPILE_FLAGS /Yc"Binding_pch.h")
    # set_source_files_properties(${source_path}/gl/functions_pch.cpp PROPERTIES COMPILE_FLAGS /Yc"functions_pch.h")

    file(GLOB binding_pch_sources ${source_path}/Binding_objects_*.cpp)
    list(APPEND binding_pch_sources ${source_path}/Binding_list.cpp)

    file(GLOB functions_pch_sources ${source_path}/gl/functions_*.cpp)

    set_source_files_properties(${binding_pch_sources} PROPERTIES COMPILE_FLAGS /Yu"Binding_pch.h")
    set_source_files_properties(${functions_pch_sources} PROPERTIES COMPILE_FLAGS /Yu"../Binding_pch.h")
endif()


#
# Target Health
#

# perform_health_checks(
#     ${target}
#     ${sources}
#     ${headers}
# )


# 
# Deployment
# 

# Library
install(TARGETS ${target}
    EXPORT  "${target}-export"            COMPONENT dev
    RUNTIME DESTINATION ${INSTALL_BIN}    COMPONENT runtime
    LIBRARY DESTINATION ${INSTALL_SHARED} COMPONENT runtime
    ARCHIVE DESTINATION ${INSTALL_LIB}    COMPONENT dev
)

# Header files
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# Generated header files
install(DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# CMake config
install(EXPORT ${target}-export
    NAMESPACE   ${META_PROJECT_NAME}::
    DESTINATION ${INSTALL_CMAKE}/${target}
    COMPONENT   dev
)
