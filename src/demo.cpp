#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glfw_utils/window.hpp"
#include "glfw_utils/init.hpp"

#include "gl_utils/gl.hpp"

#include "utils/image.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

void glfwCallback(int error_code, const char *description) {
    std::cerr << "[GLFW ERROR " << error_code << "] " << description << std::endl;
}

GL::Shader loadShader(const std::string &path, GL::ShaderType shader_type) {
    std::fstream file {path};
    auto shader = GL::Shader::create(shader_type);
    std::stringstream sstream;
    sstream << file.rdbuf();
    shader.setSource( sstream.str() );
    shader.compileShader();

    return shader;
}

struct TextureData {
    GL::Texture gl_texture;
    glm::ivec2 dimensions;
};

TextureData loadTexture(const std::string &path) {
    Image image(path);

    using IFormat = GL::Texture::InternalFormat;
    using Format = GL::Texture::Format;
    using Type = GL::Texture::Type;

    const std::pair<IFormat, Format> formats[] {
            {IFormat::R8, Format::Red},
            {IFormat::RG8, Format::RG},
            {IFormat::RGB8, Format::RGB},
            {IFormat::RGBA8, Format::RGBA}
    };

    auto iformat = formats[image.channels() - 1].first;
    auto format = formats[image.channels() - 1].second;

    auto texture = GL::Texture::create();
    auto target = GL::Texture::Target::Tex2D;
    texture.bind(target);
    GL::Texture::texImage2D(target, 0,
                            iformat,
                            image.dimensions().x, image.dimensions().y,
                            format,
                            Type::UByte,
                            image.data());
    GL::Texture::generateMipmap(target);

    return {texture, image.dimensions()};
}

class Camera {

    static constexpr float pi = 3.14159265359f;

public:
    glm::vec2 angular_speed {pi / 2, pi / 4};
    float linear_speed {1.5f};
    float zoom_speed {10.0f};
    float scale_acc_factor {.75f};

    glm::ivec2 screen_size {1024, 768};
    float fov = pi / 4;
    float z_near = 0.01f;
    float z_far = 1000.0f;

    void move(glm::vec2 movement) {
        glm::vec3 forward {std::cos(m_sphere_coord.z), 0., std::sin(m_sphere_coord.z)};
        glm::vec3 right = glm::normalize(glm::cross(forward, {0, 1, 0}));
        m_at += linear_speed * (forward * movement.y + right * movement.x) * scale_acc_factor * m_sphere_coord.x;
        m_eye_offset = sphereToCartesian(m_sphere_coord);
    }

    void rotate(glm::vec2 rotation) {
        m_sphere_coord.z += rotation.x * angular_speed.x;
        m_sphere_coord.y += rotation.y * angular_speed.y;
        m_sphere_coord.y = glm::min(pi - 0.01f, glm::max(0.01f, m_sphere_coord.y));
        m_eye_offset = sphereToCartesian(m_sphere_coord);
    }

    void zoom(float z) {
        m_sphere_coord.x += zoom_speed * z * scale_acc_factor * m_sphere_coord.x;
        m_eye_offset = sphereToCartesian(m_sphere_coord);
    }

    [[nodiscard]]
    glm::mat4 viewMatrix() const {
        return glm::lookAt(m_eye_offset + m_at, m_at, m_up);
    }

    [[nodiscard]]
    glm::mat4 projMatrix() const {
        return glm::perspectiveFov(fov, static_cast<float>(screen_size.x), static_cast<float>(screen_size.y), z_near, z_far);
    }

    [[nodiscard]]
    glm::vec3 at() const {
        return m_at;
    }

    [[nodiscard]]
    glm::vec3 eye() const {
        return m_at + m_eye_offset;
    }

private:

    static glm::vec3 sphereToCartesian(glm::vec3 sphere) {
        return {
                std::cos(sphere.z) * std::sin(sphere.y) * sphere.x,
                std::cos(sphere.y) * sphere.x,
                std::sin(sphere.z) * std::sin(sphere.y) * sphere.x
        };
    }

    glm::vec3 m_sphere_coord {2.5, pi / 4, 0.};
    glm::vec3 m_at {0.0f};
    glm::vec3 m_up {0., 1., 0.};
    glm::vec3 m_eye_offset {sphereToCartesian(m_sphere_coord)};
};

Camera g_camera {};
float g_frame_delta = 0;

void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        glfwSetInputMode(w, GLFW_CURSOR, action == GLFW_PRESS ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwSetInputMode(w, GLFW_RAW_MOUSE_MOTION, action == GLFW_PRESS);
    }
}

void scrollCallback(GLFWwindow* w, double x_offset, double y_offset) {
    if (y_offset != 0)
        g_camera.zoom(-y_offset * g_frame_delta);
}

void cursorPosCallback(GLFWwindow* w, double x_pos, double y_pos) {
    static glm::vec2 prev_pos = {0., 0.};
    glm::vec2 pos {x_pos, y_pos};
    glm::vec2 delta = pos - prev_pos;
    prev_pos = pos;

    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT))
        g_camera.rotate(glm::vec2(1, -1) * delta * g_frame_delta);
}

void frameBufferSizeCallback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
    g_camera.screen_size = {width, height};
}

int main() {

    glfwSetErrorCallback(glfwCallback);

    GLFW::InitSentry _glfw_init_sentry;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    GLFW::Window window {
        static_cast<unsigned int>(g_camera.screen_size.x),
        static_cast<unsigned int>(g_camera.screen_size.y),
        "Hello world!"
    };

    glfwMakeContextCurrent(window.ptr());
    glfwSetMouseButtonCallback(window.ptr(), mouseButtonCallback);
    glfwSetScrollCallback(window.ptr(), scrollCallback);
    glfwSetCursorPosCallback(window.ptr(), cursorPosCallback);
    glfwSetFramebufferSizeCallback(window.ptr(), frameBufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "OpenGL initialization failed" << std::endl;
        return -1;
    }

    GL::enableDebugMessageCallback();


    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.ptr(), true);
    ImGui_ImplOpenGL3_Init("#version 150");

    // 1 Load shaders
    // 1.1 Compute shader for terrain generation
    GL::ObjectManager<GL::ShaderProgram> compute_program;
    {
        GL::ObjectManager<GL::Shader> compute_shader{loadShader("shaders/heightmap.comp", GL::ShaderType::Compute)};
        compute_program->attachShader(compute_shader.handle());
        compute_program->linkProgram();
    }

    // 1.2 Simple shader for point cloud rendering
    GL::ObjectManager<GL::ShaderProgram> point_program;
    {
        GL::ObjectManager vertex{loadShader("shaders/point.vert", GL::ShaderType::Vertex)};
        GL::ObjectManager fragment{loadShader("shaders/point.frag", GL::ShaderType::Fragment)};

        point_program->attachShader(vertex.handle());
        point_program->attachShader(fragment.handle());
        point_program->linkProgram();
    }

    // 1.3 Shader with lighting
    GL::ObjectManager<GL::ShaderProgram> textured_program;
    {
        GL::ObjectManager vertex_shader{loadShader("shaders/textured.vert", GL::ShaderType::Vertex)};
        GL::ObjectManager fragment_shader{loadShader("shaders/textured.frag", GL::ShaderType::Fragment)};
        textured_program->attachShader(vertex_shader.handle());
        textured_program->attachShader(fragment_shader.handle());
        textured_program->linkProgram();
    }

    // 1.4 Shader with lighting and blended textures
    GL::ObjectManager<GL::ShaderProgram> tex_blend_program;
    {
        GL::ObjectManager vertex_shader {loadShader("shaders/textured.vert", GL::ShaderType::Vertex)};
        GL::ObjectManager fragment_shader {loadShader("shaders/blend_textured.frag", GL::ShaderType::Fragment)};
        tex_blend_program->attachShader(vertex_shader.handle());
        tex_blend_program->attachShader(fragment_shader.handle());
        tex_blend_program->linkProgram();
    }

    // 2 Allocate/initialize GPU memory
    // 2.1 Calculate index/vertex count
    const glm::ivec3 num_work_groups {16, 16, 1};
    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program->id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    const glm::ivec3 num_compute_threads = num_work_groups * work_group_size;

    GLsizei vertex_count = num_compute_threads.x * num_compute_threads.y;
    GLsizei index_count = (num_compute_threads.x - 1) * (num_compute_threads.y - 1) * 6;

    // 2.2 Create buffers
    GL::ObjectManager<GL::Buffer> vertex_buffer;
    vertex_buffer->bind(GL::Buffer::Target::Array);
    GL::Buffer::setData(GL::Buffer::Target::Array,
                        vertex_count * (2 * sizeof(glm::vec4) + sizeof(glm::vec2)),
                        nullptr,
                        GL::Buffer::Usage::StaticDraw);

    GL::ObjectManager<GL::Buffer> index_buffer;
    index_buffer->bind(GL::Buffer::Target::ElementArray);
    GL::Buffer::setData(GL::Buffer::Target::ElementArray,
                        index_count * sizeof(unsigned int),
                        nullptr,
                        GL::Buffer::Usage::StaticDraw);

    // 3 Generate Terrain
    struct GPUMemoryRange {
        GLintptr offset;
        GLsizeiptr size;
    };

    GPUMemoryRange index_mem {0, index_count * static_cast<GLsizei>(sizeof(unsigned int))};

    // vec3's are padded to sizeof(vec4) in SSB0's
    GPUMemoryRange position_mem {0, vertex_count * static_cast<GLsizei>(sizeof(glm::vec4))};
    GPUMemoryRange normal_mem {
        position_mem.size,
        vertex_count * static_cast<GLsizei>(sizeof(glm::vec4))
    };
    GPUMemoryRange tex_coord_mem {
        normal_mem.offset + normal_mem.size,
        vertex_count * static_cast<GLsizei>(sizeof(glm::vec2))
    };

    // 3.1 Bind SSBO
    compute_program->useProgram();

    vertex_buffer->bind(GL::Buffer::Target::ShaderStorage);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, /*biding=*/0, vertex_buffer->id(),
                      position_mem.offset, position_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, vertex_buffer->id(), normal_mem.offset, normal_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, vertex_buffer->id(), tex_coord_mem.offset, tex_coord_mem.size);

    index_buffer->bind(GL::Buffer::Target::ShaderStorage);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, index_buffer->id(), index_mem.offset, index_mem.size);

    // 3.2 Bind world data texture
    GL::Texture::setActive(0);
    auto world_data_texdata = loadTexture("world-data.png");
    GL::ObjectManager<GL::Texture> world_data_texture {world_data_texdata.gl_texture};
    world_data_texture->setWrapMode(GL::Texture::Target::Tex2D, GL::Texture::WrapAxis::R, GL::Texture::WrapMode::ClampToBorder);
    world_data_texture->setWrapMode(GL::Texture::Target::Tex2D, GL::Texture::WrapAxis::S, GL::Texture::WrapMode::ClampToBorder);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec3()));

    // 3.3 Execute compute shader
    glDispatchCompute(num_work_groups.x, num_work_groups.y, num_work_groups.z);

    // 4. Set up VAOs for rendering

    constexpr unsigned int a_position_location {0};
    constexpr unsigned int a_normal_location {1};
    constexpr unsigned int a_texCoord_location {2};

    // 4.1 Terrain
    GL::ObjectManager<GL::VertexArray> terrain_vao;
    terrain_vao->bind();
    vertex_buffer->bind(GL::Buffer::Target::Array);
    index_buffer->bind(GL::Buffer::Target::ElementArray);

    glVertexAttribPointer(a_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                          reinterpret_cast<void*>(position_mem.offset));
    glEnableVertexAttribArray(a_position_location);

    glVertexAttribPointer(a_normal_location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                          reinterpret_cast<void*>(normal_mem.offset));
    glEnableVertexAttribArray(a_normal_location);

    glVertexAttribPointer(a_texCoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                          reinterpret_cast<void*>(tex_coord_mem.offset));
    glEnableVertexAttribArray(a_texCoord_location);

    // 5. Calculate transforms, set uniforms

    constexpr int u_model_loc = 0;
    constexpr int u_view_loc = 1;
    constexpr int u_proj_loc = 2;
    constexpr int u_view_pos_loc = 7;

    auto terrain_transform = glm::scale(glm::mat4(1.0f), {10, 0.52, 7.62});
    terrain_transform = glm::translate(terrain_transform, {-.5, 0, -.5});

    textured_program->useProgram();
    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, glm::value_ptr(terrain_transform));

    point_program->useProgram();
    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, glm::value_ptr(terrain_transform));

    tex_blend_program->useProgram();
    glUniformMatrix4fv(u_model_loc, 1, GL_FALSE, glm::value_ptr(terrain_transform));

    int u_blend_tex_loc = glGetUniformLocation(tex_blend_program->id(), "u_blendTexture");
    //int u_blend_tex_scale_loc = glGetUniformLocation(tex_blend_program->id(), "u_blendTexScale");
    GL::Texture::setActive(1);
    auto heightmap_tex_data = loadTexture("heightmap.png");
    GL::ObjectManager heightmap_texture {heightmap_tex_data.gl_texture};
    glUniform1i(u_blend_tex_loc, 1);

    int u_color0_loc = glGetUniformLocation(tex_blend_program->id(), "u_color0");
    glm::vec4 u_color0 {.9, .9, .9, 1.};

    int u_color1_loc = glGetUniformLocation(tex_blend_program->id(), "u_color1");
    glm::vec4 u_color1 {.25, .3, .12, 1.};

    // 6. Render
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // 7. Debug?
    //auto vertices = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY));
    //glUnmapBuffer(GL_ARRAY_BUFFER);

    bool show_points = false;
    bool show_world_data = false;
    glm::vec4 color = {.7, .7, .7, .5};
    double last = glfwGetTime();

    while (!glfwWindowShouldClose(window.ptr())) {
        glfwPollEvents();

        double now = glfwGetTime();
        g_frame_delta = now - last;
        last = now;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec2 key_input = {
                glfwGetKey(window.ptr(), GLFW_KEY_D) - glfwGetKey(window.ptr(), GLFW_KEY_A),
                glfwGetKey(window.ptr(), GLFW_KEY_W) - glfwGetKey(window.ptr(), GLFW_KEY_S)
        };
        if (key_input != glm::vec2(0, 0))
            g_camera.move(-glm::normalize(key_input) * g_frame_delta);

        terrain_vao->bind();

        if (show_world_data)
            textured_program->useProgram();
        else {
            tex_blend_program->useProgram();
            glUniform4fv(u_color0_loc, 1, glm::value_ptr(u_color0));
            glUniform4fv(u_color1_loc, 1, glm::value_ptr(u_color1));
        }
        glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, glm::value_ptr(g_camera.projMatrix()));
        glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.viewMatrix()));
        glUniform3fv(u_view_pos_loc, 1, glm::value_ptr(g_camera.eye()));

        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, reinterpret_cast<const void *>(index_mem.offset));

        if (show_points) {
            point_program->useProgram();
            glUniformMatrix4fv(u_view_loc, 1, GL_FALSE, glm::value_ptr(g_camera.viewMatrix()));
            glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, glm::value_ptr(g_camera.projMatrix()));
            glUniform4fv(4, 1, glm::value_ptr(color));
            glDrawArrays(GL_POINTS, 0, vertex_count);
        }

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controles");

        ImGui::Text("WASD: desplazamiento");
        ImGui::Text("Click derecho: rotar cámara");
        ImGui::Text("Rueda del ratón: zoom");

        ImGui::Separator();
        ImGui::Checkbox("Mostrar vértices", &show_points);
        ImGui::ColorEdit4("Color", glm::value_ptr(color));

        ImGui::Separator();
        ImGui::Checkbox("Mostrar WorldData", &show_world_data);
        ImGui::ColorEdit4("Color0", glm::value_ptr(u_color0));
        ImGui::ColorEdit4("Color1", glm::value_ptr(u_color1));

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // ImGui

        glfwSwapBuffers(window.ptr());
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
