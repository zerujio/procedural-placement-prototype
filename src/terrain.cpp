#include "terrain.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

Terrain::Terrain() {
    glProgramUniform4fv(blend_program->id(), loc_color0, 1, glm::value_ptr(color0));
    glProgramUniform4fv(blend_program->id(), loc_color1, 1, glm::value_ptr(color1));
}

void Terrain::setCameraPosition(const glm::vec3 &pos) const {
    glProgramUniform3fv(texture_program->id(), Terrain::loc_viewPosition, 1, glm::value_ptr(pos));
    glProgramUniform3fv(blend_program->id(), Terrain::loc_viewPosition, 1, glm::value_ptr(pos));
}

void Terrain::setViewMatrix(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(texture_program->id(), Terrain::loc_view, 1, false, glm::value_ptr(matrix));
    glProgramUniformMatrix4fv(blend_program->id(), Terrain::loc_view, 1, false, glm::value_ptr(matrix));
}

void Terrain::setProjMatrix(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(texture_program->id(), Terrain::loc_proj, 1, false, glm::value_ptr(matrix));
    glProgramUniformMatrix4fv(blend_program->id(), Terrain::loc_proj, 1, false, glm::value_ptr(matrix));
}

void Terrain::update(GLFWwindow *window, const Camera &camera, double delta) {
    vertex_array->bind();

    ImGui::Text("Terreno");

    if (ImGui::InputInt2("NumWorkGroups", glm::value_ptr(num_work_groups))) {
        num_work_groups = glm::max(num_work_groups, {1, 1});
    }
    if (ImGui::Button("Regenerar malla")) {
        generateMesh();
    }

    ImGui::Checkbox("Mostrar WorldData", &show_world_data);
    if (show_world_data)
        texture_program->useProgram();
    else {
        blend_program->useProgram();

        if (ImGui::ColorEdit4("Color0", glm::value_ptr(color0)))
            glUniform4fv(loc_color0, 1, glm::value_ptr(color0));

        if (ImGui::ColorEdit4("Color1", glm::value_ptr(color1)))
            glUniform4fv(loc_color1, 1, glm::value_ptr(color1));
    }

    ImGui::Checkbox("Mostrar sólo vértices", &show_vertices_only);
    if (show_vertices_only)
        glDrawArrays(GL_POINTS, 0, vertex_count);
    else
        glDrawElements(mode, index_count, index_type, reinterpret_cast<const void *>(index_offset));
}

void Terrain::generateMesh() {
    index_offset = 0;

    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program->id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    const glm::uvec2 num_compute_threads = num_work_groups * glm::ivec2(work_group_size);

    vertex_count = num_compute_threads.x * num_compute_threads.y;
    index_count = (num_compute_threads.x - 1) * (num_compute_threads.y - 1) * 6;

    constexpr GLsizeiptr a_position_alignment = sizeof(glm::vec4);
    constexpr GLsizeiptr a_normal_alignment = sizeof(glm::vec4);
    constexpr GLsizeiptr a_uv_alignment = sizeof(glm::vec2);

    constexpr GLsizeiptr index_alignment = sizeof(unsigned int);

    struct MemoryRange {
        GLintptr offset;
        GLsizeiptr size;
    };

    MemoryRange a_position_mem {0,  a_position_alignment * vertex_count};
    MemoryRange a_normal_mem {a_position_mem.offset + a_position_mem.size, a_normal_alignment * vertex_count};
    MemoryRange a_uv_mem {a_normal_mem.offset + a_normal_mem.size, a_uv_alignment * vertex_count};
    MemoryRange vertex_mem {0, a_position_mem.size + a_normal_mem.size + a_uv_mem.size};

    vertex_buffer->allocate(vertex_mem.size, GL::Buffer::Usage::StaticDraw);

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, vertex_buffer->id(), a_position_mem.offset, a_position_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, vertex_buffer->id(), a_normal_mem.offset, a_normal_mem.size);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, vertex_buffer->id(), a_uv_mem.offset, a_uv_mem.size);

    MemoryRange index_mem {0, index_alignment * index_count};
    element_buffer->allocate(index_mem.size, GL::Buffer::Usage::StaticDraw);

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, element_buffer->id(), index_mem.offset, index_mem.size);

    compute_program->useProgram();
    glDispatchCompute(num_work_groups.x, num_work_groups.y, 1);

    auto vao = vertex_array.handle();

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

void Terrain::setParentTransform(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(texture_program->id(), loc_model, 1, false, glm::value_ptr(matrix));
    glProgramUniformMatrix4fv(blend_program->id(), loc_model, 1, false, glm::value_ptr(matrix));
}

void Terrain::setWorldDataTexUnit(GLint u) const {
    glProgramUniform1i(compute_program->id(), loc_computeWorldData, u);
    glProgramUniform1i(blend_program->id(), loc_blendTexture, u);
    glProgramUniform1i(texture_program->id(), loc_texture, u);
}
