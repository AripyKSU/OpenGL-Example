#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {
    //정점 좌표 입력
    //x, y, z, r, g, b, s, t
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };
    //element buffer object
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    //VAO 먼저 선언
    m_vertexLayout = VertexLayout::Create();

    //VertexBuffer 생성 후 바인딩
    //vertices를 m_vertexBuffer에 복사
    //형식은 STATIC | DYNAMIC | STREAM  /  DRAW | READ | COPY 의 조합
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 
        vertices, sizeof(float)*32);
    
    //position
    //처음 0은 shader의 location, float값 3개, stride=float 6개, offset 0
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    //color
    //attribute 1, float 3개, stride = float 6개, offset = float[3]부터
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    //texture coordinate
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);


    //index buffer 생성 및 초기화
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 
        indices, sizeof(uint32_t)*6);

    //shader get
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.fs", GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    //program get
    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    /*  
        //simple.fs의 uniform var이름을 통해 loc(위치)를 받아옴
        auto loc = glGetUniformLocation(m_program->Get(), "color");
        m_program->Use();
        glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f); 
    */ 

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    //image load
    auto image = Image::Load("./images/container.jpg");
    if (!image) 
        return false;
    SPDLOG_INFO("image: {}x{}, {} channels",
        image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    //ImageUPtr에서 Image*를 얻기 위해 get() 사용
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./images/awesomeface.png");
    if (!image2) 
        return false;
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);

    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    //type, 점 offset, 점 개수: 현재 VAO에 binding 된 VBO로 부터 받아옴
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}