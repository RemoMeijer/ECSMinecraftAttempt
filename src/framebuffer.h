#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class Framebuffer {
public:
    // Constructor creates the framebuffer and its attachments
    Framebuffer(int width, int height);
    // Destructor cleans up the resources
    ~Framebuffer();

    // Make this framebuffer the active render target
    void bind();
    // Make the default window framebuffer the active render target
    void unbind();

    // Get the ID of the color texture to use in the post-processing shader
    unsigned int getColorTexture();
    unsigned int getDepthTexture();

private:
    unsigned int m_FBO; // Framebuffer Object ID
    unsigned int m_ColorTexture; // Color attachment texture ID
    unsigned int m_RBO; // Renderbuffer Object for depth/stencil
    unsigned int m_DepthTexture;
};

#endif