#ifndef FRAMEBUFFER_TARGET_H
#define FRAMEBUFFER_TARGET_H

class FrameBufferTarget {
public:
    static absl::StatusOr<std::unique_ptr<FrameBufferTarget>> Create() {
        GLuint framebuffer_handle;
        glGenFramebuffers(1, &framebuffer_handle);
        RET_CHECK(framebuffer_handle)
            << "Failed to initialize an OpenGL framebuffer!";

        return absl::WrapUnique(new FrameBufferTarget(framebuffer_handle));
    }

    ~FrameBufferTarget() {
        glDeleteFramebuffers(1, &framebuffer_handle_);
        // Renderbuffer handle might have never been created if this render target
        // is destroyed before `SetColorbuffer()` is called for the first time.
        if (renderbuffer_handle_) {
            glDeleteFramebuffers(1, &renderbuffer_handle_);
        }
    }

    absl::Status SetColorbuffer(int src_width, int src_height, GLenum target, GLuint handle) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_handle_);
        glViewport(0, 0, src_width, src_height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(target, handle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, handle, /*level*/ 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, target, handle, /*level*/ 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, target, handle, /*level*/ 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, target, handle, /*level*/ 0);
        glBindTexture(target, 0);

        // If the existing depth buffer has different dimensions, delete it.
        if (renderbuffer_handle_ &&
            (viewport_width_ != src_width || viewport_height_ != src_height)) {
            glDeleteRenderbuffers(1, &renderbuffer_handle_);
            renderbuffer_handle_ = 0;
        }

        // If there is no depth buffer, create one.
        if (!renderbuffer_handle_) {
            glGenRenderbuffers(1, &renderbuffer_handle_);
            RET_CHECK(renderbuffer_handle_)
                << "Failed to initialize an OpenGL renderbuffer!";
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_handle_);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, src_width, src_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_handle_);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
        viewport_width_ = src_width; 
        viewport_height_ = src_height;

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            LOG(ERROR) << "Incomplete framebuffer with status: " << status;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glFlush();

        return absl::OkStatus();
    }

    void Bind(bool is_depth = true) const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_handle_);
        glViewport(0, 0, viewport_width_, viewport_height_);
        if (is_depth) {
            glEnable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
        } else {
            glDisable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }
    }

    void Unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    void Clear() const {
        Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClearDepthf(1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        Unbind();
        glFlush();
    }

private:
    explicit FrameBufferTarget(GLuint framebuffer_handle)
                : framebuffer_handle_(framebuffer_handle),
                  renderbuffer_handle_(0),
                  viewport_width_(-1),
                  viewport_height_(-1) {}

    GLuint framebuffer_handle_;
    GLuint renderbuffer_handle_;
    int viewport_width_;
    int viewport_height_;
};

#endif