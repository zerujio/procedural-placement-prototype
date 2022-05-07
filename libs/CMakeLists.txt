# glad
add_subdirectory(glad)

#glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW docs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW tests")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW examples")
set(GLFW_INSTALL OFF CACHE BOOL "Install GLFW")
add_subdirectory(glfw)

#glm
add_subdirectory(glm)
