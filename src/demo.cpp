#include "demo.hpp"

#include <imgui.h>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct MemoryRange {
    GLintptr offset;
    GLsizeiptr size;
};

void Scene::scrollCallback(GLFWwindow* w, double delta, glm::dvec2 offset) {
    if (offset.y != 0.0)
        camera.zoom(static_cast<float>(-offset.y * delta));
    m_view_changed = true;
}

void Scene::frameBufferSizeCallback(GLFWwindow* w, double, glm::ivec2 size) {
    camera.screen_size = size;
    m_proj_chaged = true;
}

Scene::Terrain::Terrain() {
   world_data_texture->setWrapMode(GL::Texture::WrapAxis::S, GL::Texture::WrapMode::ClampToBorder);
   world_data_texture->setWrapMode(GL::Texture::WrapAxis::T, GL::Texture::WrapMode::ClampToBorder);

   const glm::vec4 border_color;
   glTextureParameterfv(world_data_texture->id(), GL_TEXTURE_BORDER_COLOR, glm::value_ptr(border_color));

   shape.mode = GL_TRIANGLES;

   constexpr GLint world_data_tex_unit = 0;
   constexpr GLint blend_tex_unit = 1;

   glBindTextureUnit(world_data_tex_unit, world_data_texture->id());
   glBindTextureUnit(blend_tex_unit, heightmap_texture->id());

   glProgramUniform1i(texture_program->id(), loc_texture, world_data_tex_unit);
   glProgramUniform1i(compute_program->id(), loc_computeWorldData, world_data_tex_unit);
   glProgramUniform1i(blend_program->id(), loc_blendTexture, blend_tex_unit);
}

void Scene::Terrain::generateMesh(glm::uvec2 num_work_groups) {
    shape.index_type = GL_UNSIGNED_INT;
    shape.index_offset = 0;

    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program->id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    const glm::uvec2 num_compute_threads = num_work_groups * glm::uvec2(work_group_size);

    vertex_count = num_compute_threads.x * num_compute_threads.y;
    shape.index_count = (num_compute_threads.x - 1) * (num_compute_threads.y - 1) * 6;

    constexpr GLsizeiptr a_position_alignment = sizeof(glm::vec4);
    constexpr GLsizeiptr a_normal_alignment = sizeof(glm::vec4);
    constexpr GLsizeiptr a_uv_alignment = sizeof(glm::vec2);

    constexpr GLsizeiptr index_alignment = sizeof(unsigned int);

    MemoryRange a_position_mem {0,  a_position_alignment * vertex_count};
    MemoryRange a_normal_mem {a_position_mem.offset + a_position_mem.size, a_normal_alignment * vertex_count};
    MemoryRange a_uv_mem {a_normal_mem.offset + a_normal_mem.size, a_uv_alignment * vertex_count};
    MemoryRange vertex_mem {0, a_position_mem.size + a_normal_mem.size + a_uv_mem.size};

    vertex_buffer->allocate(vertex_mem.size, GL::Buffer::Usage::StaticDraw);

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer->id(), a_position_mem.offset, a_position_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, vertex_buffer->id(), a_normal_mem.offset, a_normal_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, vertex_buffer->id(), a_uv_mem.offset, a_uv_mem.size);

    MemoryRange index_mem {0, index_alignment * shape.index_count};
    element_buffer->allocate(index_mem.size, GL::Buffer::Usage::StaticDraw);

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, element_buffer->id(), index_mem.offset, index_mem.size);

    compute_program->useProgram();
    glDispatchCompute(num_work_groups.x, num_work_groups.y, 1);

    auto vao = shape.vertex_array.handle();

    constexpr int a_pos_loc = 0;
    constexpr int a_normal_loc = 1;
    constexpr int a_uv_loc = 2;

    {
        GL::Buffer buffers[] {vertex_buffer.handle(), vertex_buffer.handle(), vertex_buffer.handle()};
        GLintptr offsets[] {a_position_mem.offset, a_normal_mem.offset, a_uv_mem.offset};
        GLsizei strides[] {a_position_alignment, a_normal_alignment, a_uv_alignment};

        vao.bindVertexBuffers(0, 3, buffers, offsets, strides);
    }

    vao.attribBinding(a_pos_loc, 0);
    vao.attribFormat(a_pos_loc, 3, GL_FLOAT, false, 0);
    vao.enableAttrib(a_pos_loc);

    vao.attribBinding(a_normal_loc, 1);
    vao.attribFormat(a_normal_loc, 3, GL_FLOAT, false, 0);
    vao.enableAttrib(a_normal_loc);

    vao.attribBinding(a_uv_loc, 2);
    vao.attribFormat(a_uv_loc, 2, GL_FLOAT, false, 0);
    vao.enableAttrib(a_uv_loc);

    vao.bindElementBuffer(element_buffer);
}

Scene::Axes::Axes() {
    const glm::vec3 vertices[] {
            {0, 0, 0}, {1, 0, 0},
            {1, 0, 0}, {1, 0, 0},
            {0, 0, 0}, {0, 1, 0},
            {0, 1, 0}, {0, 1, 0},
            {0, 0, 0}, {0, 0, 1},
            {0, 0, 1}, {0, 0, 1}
    };

    buffer->initialize(sizeof(vertices), vertices, GL::Buffer::Usage::StaticDraw);

    GL::VertexArray vao = shape.vertex_array.handle();

    GLsizei stride = 2 * sizeof(glm::vec3);
    GLuint buffer_binding_index = 0;
    vao.bindVertexBuffer(0, buffer, buffer_binding_index, stride);

    using AttribLocation = GLint;
    AttribLocation a_pos = program->getAttribLocation("a_position");
    AttribLocation a_color = program->getAttribLocation("a_color");

    vao.attribBinding(a_pos, buffer_binding_index);
    vao.attribFormat(a_pos, 3, GL_FLOAT, false, 0);
    vao.enableAttrib(a_pos);

    vao.attribBinding(a_color, buffer_binding_index);
    vao.attribFormat(a_color, 3, GL_FLOAT, false, stride / 2);
    vao.enableAttrib(a_color);

    shape.mode = GL_LINES;
    shape.first_index = 0;
    shape.index_count = 6;
}

Scene::Scene() {
    terrain.generateMesh({16, 16});

    auto terrain_transform = glm::scale(glm::mat4(1.0f), {10, 0.52, 7.62});
    terrain_transform = glm::translate(terrain_transform, {-.5, 0, -.5});

    glProgramUniformMatrix4fv(terrain.texture_program->id(), Terrain::loc_model, 1, false,
                              glm::value_ptr(terrain_transform));
    glProgramUniformMatrix4fv(terrain.texture_program->id(), Terrain::loc_view, 1, false,
                              glm::value_ptr(camera.viewMatrix()));
    glProgramUniformMatrix4fv(terrain.texture_program->id(), Terrain::loc_proj, 1, false,
                              glm::value_ptr(camera.projMatrix()));
    glProgramUniform3fv(terrain.texture_program->id(), Terrain::loc_viewPosition, 1, glm::value_ptr(camera.eye()));

    glProgramUniformMatrix4fv(terrain.blend_program->id(), Terrain::loc_model, 1, false,
                              glm::value_ptr(terrain_transform));
    glProgramUniformMatrix4fv(terrain.blend_program->id(), Terrain::loc_view, 1, false,
                              glm::value_ptr(camera.viewMatrix()));
    glProgramUniformMatrix4fv(terrain.blend_program->id(), Terrain::loc_proj, 1, false,
                              glm::value_ptr(camera.projMatrix()));
    glProgramUniform3fv(terrain.blend_program->id(), Terrain::loc_viewPosition, 1, glm::value_ptr(camera.eye()));

    glProgramUniform4fv(terrain.blend_program->id(), terrain.loc_color0, 1, glm::value_ptr(terrain.color0));
    glProgramUniform4fv(terrain.blend_program->id(), terrain.loc_color1, 1, glm::value_ptr(terrain.color1));

    glProgramUniformMatrix4fv(axes.program->id(), 0, 1, false, glm::value_ptr(camera.viewMatrix()));
    glProgramUniformMatrix4fv(axes.program->id(), 1, 1, false, glm::value_ptr(camera.projMatrix()));
}

void Scene::update(GLFWwindow *w, double delta) {
    ImGui::Begin("Controles");

    ImGui::Text("WASD: desplazamiento");
    ImGui::Text("Click derecho: rotar cámara");
    ImGui::Text("Rueda del ratón: zoom");

    glm::vec2 key_input = {
            glfwGetKey(w, GLFW_KEY_D) - glfwGetKey(w, GLFW_KEY_A),
            glfwGetKey(w, GLFW_KEY_W) - glfwGetKey(w, GLFW_KEY_S)
    };

    if (key_input != glm::vec2(0, 0)) {
        camera.move(-glm::normalize(key_input) * static_cast<float>(delta));
        m_view_changed = true;
    }

    glm::dvec2 curr_cursor_pos;
    glfwGetCursorPos(w, &curr_cursor_pos.x, &curr_cursor_pos.y);
    auto cursor_delta = curr_cursor_pos - m_prev_cursor_pos;
    m_prev_cursor_pos = curr_cursor_pos;

    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT) && cursor_delta != glm::dvec2()) {
        camera.rotate(glm::dvec2(1, -1) * cursor_delta * delta);
        m_view_changed = true;
    }

    if (m_view_changed) {
        auto view_matrix = camera.viewMatrix();
        glProgramUniformMatrix4fv(terrain.texture_program->id(), Terrain::loc_view, 1, false, glm::value_ptr(view_matrix));
        glProgramUniformMatrix4fv(terrain.blend_program->id(), Terrain::loc_view, 1, false, glm::value_ptr(view_matrix));
        glProgramUniformMatrix4fv(axes.program->id(), axes.loc_view, 1, false, glm::value_ptr(view_matrix));

        glProgramUniform3fv(terrain.texture_program->id(), Terrain::loc_viewPosition, 1, glm::value_ptr(camera.eye()));
        glProgramUniform3fv(terrain.blend_program->id(), Terrain::loc_viewPosition, 1, glm::value_ptr(camera.eye()));

        m_view_changed = false;
    }

    if (m_proj_chaged) {
        auto proj_matrix = camera.projMatrix();

        glProgramUniformMatrix4fv(terrain.texture_program->id(), Terrain::loc_proj, 1, false, glm::value_ptr(proj_matrix));
        glProgramUniformMatrix4fv(terrain.blend_program->id(), Terrain::loc_proj, 1, false, glm::value_ptr(proj_matrix));
        glProgramUniformMatrix4fv(axes.program->id(), axes.loc_proj, 1, false, glm::value_ptr(proj_matrix));

        m_proj_chaged = false;
    }

    terrain.shape.vertex_array->bind();

    ImGui::Separator();

    ImGui::Checkbox("Mostrar WorldData", &terrain.show_world_data);
    if (terrain.show_world_data)
        terrain.texture_program->useProgram();
    else {
        terrain.blend_program->useProgram();

        if (ImGui::ColorEdit4("Color0", glm::value_ptr(terrain.color0)))
            glUniform4fv(terrain.loc_color0, 1, glm::value_ptr(terrain.color0));

        if (ImGui::ColorEdit4("Color1", glm::value_ptr(terrain.color1)))
            glUniform4fv(terrain.loc_color1, 1, glm::value_ptr(terrain.color1));
    }

    ImGui::Checkbox("Mostrar sólo vértices", &terrain.show_vertices_only);
    if (terrain.show_vertices_only)
        glDrawArrays(GL_POINTS, 0, terrain.vertex_count);
    else
        glDrawElements(terrain.shape.mode, terrain.shape.index_count, terrain.shape.index_type,
                       reinterpret_cast<const void *>(terrain.shape.index_offset));

    // Axes
    axes.shape.vertex_array->bind();
    axes.program->useProgram();
    glDrawArrays(axes.shape.mode, axes.shape.first_index, axes.shape.index_count);

    ImGui::End();
}

