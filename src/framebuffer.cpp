#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const std::vector<TexturePtr>& colorAttachments) {
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachments))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer() {
    if (m_depthStencilBuffer) {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const std::vector<TexturePtr>& colorAttachments) {
    m_colorAttachments = colorAttachments;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    //color texture binding
    for (size_t i = 0; i < m_colorAttachments.size(); i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
            m_colorAttachments[i]->Get(), 0);
    }

    //glDrawBuffer에 여러 장의 texture를 쓴다는 것 전달
    if (m_colorAttachments.size() > 0) {
        std::vector<GLenum> attachments;
        attachments.resize(m_colorAttachments.size());
        for (size_t i = 0; i < m_colorAttachments.size(); i++)
            attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(m_colorAttachments.size(), attachments.data());
    }

    int width = m_colorAttachments[0]->GetWidth();
    int height = m_colorAttachments[0]->GetHeight();

    //RenderBuffer 생성 (depth24_stencil8 형태)
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //frame buffer에 renderbuffer 끼우기
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    //하드웨어에서 프레임버퍼 사용 가능한지 체크
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}