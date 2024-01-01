#include "context.h"
#include "image.h"

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init() {
    //정점 좌표 입력 (VBO)
    //x, y, z, r, g, b, s, t
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };
    //element buffer object (VAO)
    uint32_t indices[] = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    //VAO 먼저 선언
    m_vertexLayout = VertexLayout::Create();

    //VertexBuffer 생성 후 바인딩
    //vertices를 m_vertexBuffer에 복사
    //형식은 STATIC | DYNAMIC | STREAM  /  DRAW | READ | COPY 의 조합
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, 
        vertices, sizeof(float)*120);
    
    //position
    //처음 0은 shader의 location, float값 3개, stride=float 5개, offset 0
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    //color
    //attribute 1, float 3개, stride = float 6개, offset = float[3]부터
    //m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    //texture coordinate
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);


    //index buffer 생성 및 초기화
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, 
        indices, sizeof(uint32_t)*36);

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
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    // x축으로 -55도 회전
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // 종횡비 window_width : window_height (cmake에서 정의), 세로화각 45도의 원근 투영
    auto projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 10.0f);
    //mvp matrix 생성
    auto transform = projection * view * model;
    m_program->SetUniform("transform", transform);

    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //깊이 버퍼 활성화
    glEnable(GL_DEPTH_TEST);

    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    m_program->Use();
    auto projection = glm::perspective(glm::radians(45.0f),
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.01f, 20.0f);
    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));
    for (size_t i = 0; i < cubePositions.size(); i++){
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model,
            glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),
            glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    //type, 점 offset, 점 개수: 현재 VAO에 binding 된 VBO로 부터 받아옴
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}