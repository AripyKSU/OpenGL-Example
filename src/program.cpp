#include "program.h"

//Program 생성
ProgramUPtr Program::Create(
    const std::vector<ShaderPtr>& shaders) {
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

//Shared Pointer로 참조하는 shader들을 받아와서 link하는 과정
bool Program::Link(  
    const std::vector<ShaderPtr>& shaders) {
    m_program = glCreateProgram();

    //iterator for문
    for (auto& shader: shaders)
        glAttachShader(m_program, shader->Get());
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }
    return true;
}

Program::~Program() {
      if (m_program) {
        glDeleteProgram(m_program);
    }
}

void Program::Use() const{
    glUseProgram(m_program);
}