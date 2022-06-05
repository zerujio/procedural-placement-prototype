#include "entities.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>


Entities::Entities() {
    glGetUniformfv(shader_program->id(), u_color_loc, glm::value_ptr(point_color));
    glGetUniformfv(shader_program->id(), u_point_size_loc, &point_size);
}

void Entities::setParentTransform(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(shader_program->id(), u_model_loc, 1, false, glm::value_ptr(matrix));
}

void Entities::setViewMatrix(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(shader_program->id(), u_view_loc, 1, false, glm::value_ptr(matrix));
}

void Entities::setProjMatrix(const glm::mat4 &matrix) const {
    glProgramUniformMatrix4fv(shader_program->id(), u_proj_loc, 1, false, glm::value_ptr(matrix));
}

void Entities::setWorldDataTexUnit(GLint unit) const {
    glProgramUniform1i(compute_program->id(), u_world_data_loc, unit);
}

void Entities::update() {
    ImGui::Text("Placement");

    ImGui::InputFloat2("Pos0", glm::value_ptr(pos0));
    ImGui::InputFloat2("Pos1", glm::value_ptr(pos1));
    ImGui::InputInt2("Placement Work Groups", glm::value_ptr(num_work_groups));
    ImGui::Text("WorkGroup: 4 x 4 invocations.");
    if (ImGui::Button("Compute placement")) {
        pos0 = glm::min(pos0, pos1);
        pos1 = glm::max(pos0, pos1);
        num_work_groups = glm::max(num_work_groups, 1);
        generateEntities();
    }

    if (ImGui::InputFloat("Point Size", &point_size)) {
        glProgramUniform1f(shader_program->id(), u_point_size_loc, point_size);
    }

    if (ImGui::ColorEdit4("Point Color", glm::value_ptr(point_color))) {
        glProgramUniform4fv(shader_program->id(), u_color_loc, 1, glm::value_ptr(point_color));
    }

    if (point_count > 0) {
        shader_program->useProgram();
        vao->bind();
        glDrawArrays(GL_POINTS, 0, point_count);
    }
}

void Entities::generateEntities() {
    auto tex_offset = pos0;
    auto tex_scale = pos1 - pos0;
    glProgramUniform2f(compute_program->id(), u_tex_coord_offset_loc, tex_offset.x, tex_offset.y);
    glProgramUniform2f(compute_program->id(), u_tex_coord_scale_loc, tex_scale.x, tex_scale.y);

    glm::ivec3 work_group_size;
    glGetProgramiv(compute_program->id(), GL_COMPUTE_WORK_GROUP_SIZE, glm::value_ptr(work_group_size));
    const glm::uvec2 num_compute_threads = num_work_groups * glm::ivec2(work_group_size);

    int max_points = work_group_size.x * work_group_size.y * num_work_groups.x * num_work_groups.y;

    GLuint initial_count = 0;
    constexpr GLsizei offset = sizeof(glm::vec4);
    constexpr GLsizei stride = sizeof(glm::vec4);
    GLsizei buffer_size = offset + max_points * stride;
    buffer->allocate(buffer_size, GL::Buffer::Usage::StaticDraw);

    buffer->writeData(0, sizeof(initial_count), &initial_count);

    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

    buffer->bind(GL::Buffer::Target::ShaderStorage);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffer->id(), 0, buffer_size);

    compute_program->useProgram();
    glDispatchCompute(num_work_groups.x, num_work_groups.y, 1);

    constexpr int bind_index = 0;
    vao->bindVertexBuffer(bind_index, buffer, offset, stride);
    vao->attribBinding(a_position_loc, bind_index);
    vao->attribFormat(a_position_loc, 3, GL_FLOAT, false, 0);
    vao->enableAttrib(a_position_loc);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //auto data = glMapNamedBuffer(buffer->id(), GL_READ_ONLY);
    //glUnmapNamedBuffer(buffer->id());

    //glNamedBufferSubData(buffer->id(), 0, sizeof(initial_count), &point_count);
    point_count = max_points;
}
