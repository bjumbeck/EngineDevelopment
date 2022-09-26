# CMAKE Setup
########################################
cmake_minimum_required(VERSION 3.11...3.20)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(FetchContent)
include(CMakePrintHelpers)

# File's TODO: Move this or find another way to add files. Being at the top provides to much clutter
########################################
set(HEADERS
    source/Application/Application.hpp
    source/Components/Component.hpp
    source/Components/MovementComponent.hpp
    source/Components/RenderableComponent.hpp
    source/Components/SteeringComponent.hpp
    source/Components/TransformableComponent.hpp
    source/Entity/Entity.hpp
    source/Entity/EntityManager.hpp
    source/EventManagement/Events/ComponentEvents.hpp
    source/EventManagement/Events/EntityEvents.hpp
    source/EventManagement/EventManager.hpp
    source/EventManagement/SimpleSignal.hpp
    source/Helpers/MemoryPool.hpp
    source/Math/Trigonometry.hpp
    source/Math/VectorMath.hpp
    source/ResourceManagement/FileLoaders.hpp
    source/ResourceManagement/ResourceCache.hpp
    source/ResourceManagement/ResourceContainers.hpp
    source/ResourceManagement/ResourceHandle.hpp
    source/Systems/MovementSystem.hpp
    source/Systems/RenderSystem.hpp
    source/Systems/System.hpp
    source/Systems/SystemManager.hpp
)

set(SRCS 
    source/main.cpp
    source/Application/Application.cpp
    source/Entity/Entity.cpp
    source/Entity/EntityManager.cpp
    source/EventManagement/EventManager.cpp
    source/Systems/RenderSystem.cpp
    source/Systems/MovementSystem.cpp
    source/ResourceManagement/ResourceHandle.cpp
    source/ResourceManagement/ResourceCache.cpp
    source/ResourceManagement/ResourceContainers.cpp
)

# Project Structure
########################################
project(TestEngine VERSION 1.0 DESCRIPTION "3D Game Engine Library" LANGUAGES CXX)
add_executable(TestEngine ${SRCS} ${HEADERS})

set_target_properties(TestEngine
    PROPERTIES
    VERSION ${PROJECT_VERSION} 
    SOVERSION 0
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/Bin"
    CXX_STANDARD 20
)

set(FETCHCONTENT_BASE_DIR "${PROJECT_SOURCE_DIR}/ThirdParty")

# Dependency Fetching
########################################

# SFML, handles cross platform windowing, audio, graphics (Though moving away from this to OpenGL/DirectX in future), and possibly networking in the future.
FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML.git"
    GIT_TAG        "2.5.1"
)

# ImGUI, handles all GUI components the engine.
FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG        "docking"
)

# SpdLog, Header only fast C++ logging
# TODO: Since this is header only look into possibly altering how we fetch this content, since we really only need the headers and nothing else.
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
)

# Ziplib, allows up to interact with and create zip files. Mainly used in resource management.
FetchContent_Declare(ziplib 
    GIT_REPOSITORY https://github.com/bjumbeck/ziplib-cmake.git
)

FetchContent_MakeAvailable(sfml imgui spdlog ziplib)



# Dependency Linking and Header Setup
########################################
target_link_libraries(TestEngine
    PRIVATE
        sfml-audio
        sfml-graphics
        sfml-system
        sfml-window

        ziplib
)

include_directories(
    "${PROJECT_SOURCE_DIR}/source" 
    "${sfml_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/ThirdParty/spdlog-src/include/"
    "${PROJECT_SOURCE_DIR}/ThirdParty/ziplib-src/"
)

# Post Build 
########################################

# Copy SFML dlls into our build directory (Debug only atm)
add_custom_command(
    TARGET TestEngine POST_BUILD

    COMMAND  ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/ThirdParty/sfml-build/lib/$<CONFIGURATION>/"
        "${PROJECT_SOURCE_DIR}/bin/$<CONFIGURATION>/"
)