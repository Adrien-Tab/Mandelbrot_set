#pragma once

#include <iostream>

namespace GLUtils {

constexpr const char* VertexShader = R"(
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
  gl_Position = vec4(aPos, 1.0);
  TexCoord = aTexCoord;
}
)";

constexpr const char* FragmentShader = R"(
#version 450 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main() {
  FragColor = texture(ourTexture, TexCoord);
}
)";

bool loadShader(GLuint& shader, GLenum shader_type, const GLchar* shader_code) {
  shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &shader_code, NULL);
  glCompileShader(shader);

  GLint compil_status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compil_status);
  if (compil_status == GL_FALSE) {
    GLchar info_log[1024];
    int32_t error_length;
    glGetShaderInfoLog(shader, 1024, &error_length, info_log);
    std::cerr << "Failed to compile shader: " << info_log << '\n';
    glDeleteShader(shader);
    return false;
  }
  return true;
}

bool linkProgram(GLuint* shader_program, const GLuint& vertex_shader,
                 const GLuint& fragment_shader) {
  glAttachShader(*shader_program, vertex_shader);
  glAttachShader(*shader_program, fragment_shader);
  glLinkProgram(*shader_program);

  GLint link_status;
  glGetProgramiv(*shader_program, GL_LINK_STATUS, &link_status);
  if (link_status == GL_FALSE) {
    GLchar info_log[1024];
    glGetProgramInfoLog(*shader_program, 1024, nullptr, info_log);
    std::cerr << "Failed to link shader program: " << info_log << '\n';
    return false;
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return true;
}

}  // namespace GLUtils
