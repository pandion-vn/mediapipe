#include "triangle_renderer.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/port/status.h"
#include "mediapipe/framework/port/status_macros.h"
#include "mediapipe/framework/port/statusor.h"
#include "mediapipe/gpu/gl_base.h"
#include "mediapipe/gpu/shader_util.h"

namespace {

class Texture {
public:
    static absl::StatusOr<std::unique_ptr<Texture>> WrapExternalTexture(
        GLuint handle, GLenum target, int width, int height) {
        RET_CHECK(handle) << "External texture must have a non-null handle!";
        return absl::WrapUnique(new Texture(handle, target, width, height,
                                            /*is_owned*/ false));
    }
    GLuint handle() const { return handle_; }
    GLenum target() const { return target_; }
    int width() const { return width_; }
    int height() const { return height_; }

private:
    Texture(GLuint handle, GLenum target, int width, int height, bool is_owned)
        : handle_(handle), target_(target),
          width_(width), height_(height), is_owned_(is_owned) {}
    
    GLuint handle_;
    GLenum target_;
    int width_;
    int height_;
    bool is_owned_;
};

class RenderTarget {
public:
    static absl::StatusOr<std::unique_ptr<RenderTarget>> Create() {
        GLuint framebuffer_handle;
        glGenFramebuffers(1, &framebuffer_handle);
        RET_CHECK(framebuffer_handle)
            << "Failed to initialize an OpenGL framebuffer!";

        return absl::WrapUnique(new RenderTarget(framebuffer_handle));
    }

    ~RenderTarget() {
        glDeleteFramebuffers(1, &framebuffer_handle_);
        // Renderbuffer handle might have never been created if this render target
        // is destroyed before `SetColorbuffer()` is called for the first time.
        if (renderbuffer_handle_) {
            glDeleteFramebuffers(1, &renderbuffer_handle_);
        }
    }

    absl::Status SetViewport(const Texture& texture) {
        viewport_width_ = texture.width();
        viewport_height_ = texture.height();
        return absl::OkStatus();
    }

    void Bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_handle_);
        glViewport(0, 0, viewport_width_, viewport_height_);
    }

    void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    void Clear() const {
        // LOG(INFO) << "RenderTarget.Clear() " << framebuffer_handle_;
        // Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClearDepthf(1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        // Unbind();
        glFlush();
    }


private:
    explicit RenderTarget(GLuint framebuffer_handle)
        : framebuffer_handle_(framebuffer_handle),
            renderbuffer_handle_(0),
            viewport_width_(-1),
            viewport_height_(-1) {}
    GLuint framebuffer_handle_;
    GLuint renderbuffer_handle_;
    int viewport_width_;
    int viewport_height_;
};

class Renderer {
public:
    enum class RenderMode { OPAQUE, OVERDRAW, OCCLUSION };
    static absl::StatusOr<std::unique_ptr<Renderer>> Create() {
        static const GLint kAttrLocation[NUM_ATTRIBUTES] = {
            ATTRIB_VERTEX,
            ATTRIB_TEXTURE_POSITION,
        };
        static const GLchar* kAttrName[NUM_ATTRIBUTES] = {
            "position",
            "tex_coord",
        };    
        static const GLchar* kVertSrc = R"(
            attribute vec4 position;
            attribute vec4 tex_coord;
            varying vec2 v_tex_coord;

            void main() {
                v_tex_coord = tex_coord.xy;
                gl_Position = position;
            }
        )";

        static const GLchar* kFragSrc = R"(
            precision mediump float;

            varying vec2 v_tex_coord;
            uniform sampler2D texture;

            void main() {
                // gl_FragColor = texture2D(texture, v_tex_coord);
                gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
            }
        )";

        GLuint program_handle = 0;
        mediapipe::GlhCreateProgram(kVertSrc, kFragSrc, NUM_ATTRIBUTES,
                     (const GLchar**)&kAttrName[0], kAttrLocation,
                     &program_handle);
        RET_CHECK(program_handle) << "Problem initializing the texture program!";

        LOG(INFO) << "GlhCreateProgram: " << program_handle;
        // GLint projection_mat_uniform =
        //     glGetUniformLocation(program_handle, "projection_mat");
        // GLint model_mat_uniform = glGetUniformLocation(program_handle, "model_mat");
        GLint texture_uniform = glGetUniformLocation(program_handle, "texture");

        // RET_CHECK_NE(projection_mat_uniform, -1)
        //     << "Failed to find `projection_mat` uniform!";
        // RET_CHECK_NE(model_mat_uniform, -1)
        //     << "Failed to find `model_mat` uniform!";
        // RET_CHECK_NE(texture_uniform, -1) << "Failed to find `texture` uniform!";
        return absl::WrapUnique(new Renderer(program_handle, texture_uniform));
    }

    ~Renderer() { glDeleteProgram(program_handle_); }

    absl::Status Render(const RenderTarget& render_target, const Texture& texture,
                        RenderMode render_mode) const {
        glUseProgram(program_handle_);
        // LOG(INFO) << "Renderer.Render()" << program_handle_;
        // Set up the GL state.
        glEnable(GL_BLEND);
        glFrontFace(GL_CCW);
        switch (render_mode) {
        case RenderMode::OPAQUE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            break;

        case RenderMode::OVERDRAW:
            glBlendFunc(GL_ONE, GL_ZERO);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            break;

        case RenderMode::OCCLUSION:
            glBlendFunc(GL_ZERO, GL_ONE);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            break;
        }

        // render_target.Bind();
        // LOG(INFO) << "Renderer.Render()";

        // Set up textures and uniforms.
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(texture.target(), texture.handle());
        // glUniform1i(texture_uniform_, 1);
        
        GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                                -0.5f, -0.5f, 0.0f,
                                 0.5f, -0.5f, 0.0f };
        // Set the viewport
        // glViewport ( 0, 0, esContext->width, esContext->height );
        // Clear the color buffer
        // glClear ( GL_COLOR_BUFFER_BIT );
        // Use the program object
        // glUseProgram ( userData->programObject );
        // Load the vertex data
        glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
        glEnableVertexAttribArray ( 0 );
        glDrawArrays ( GL_TRIANGLES, 0, 3 );

        // Unbind textures and uniforms.
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(texture.target(), 0);
        
        // render_target.Unbind();
        // Unbind vertex attributes.
        // glDisableVertexAttribArray(ATTRIB_TEXTURE_POSITION);
        // glDisableVertexAttribArray(ATTRIB_VERTEX);
        // Restore the GL state.
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glUseProgram(0);
        glFlush();

        return absl::OkStatus();
    }
private:
    enum { ATTRIB_VERTEX, ATTRIB_TEXTURE_POSITION, NUM_ATTRIBUTES };

    Renderer(GLuint program_handle, GLint texture_uniform):
             program_handle_(program_handle),
             texture_uniform_(texture_uniform) {}
    GLuint program_handle_;
    // GLint projection_mat_uniform_;
    // GLint model_mat_uniform_;
    GLint texture_uniform_;
};

class TriangleRendererImpl : public TriangleRenderer {
public:
    TriangleRendererImpl(
        std::unique_ptr<RenderTarget> render_target,
        std::unique_ptr<Renderer> renderer):
            render_target_(std::move(render_target)),
            renderer_(std::move(renderer)) {}

    absl::Status ValidateFrameDimensions(int frame_width, int frame_height) {
        RET_CHECK_GT(frame_width, 0) << "Frame width must be positive!";
        RET_CHECK_GT(frame_height, 0) << "Frame height must be positive!";

        return absl::OkStatus();
    }
    
    absl::Status RenderEffect(
        int frame_width,            //
        int frame_height,           //
        GLenum src_texture_target,  //
        GLuint src_texture_name,    //
        GLenum dst_texture_target,  //
        GLuint dst_texture_name) {
        // Validate input arguments.
        MP_RETURN_IF_ERROR(ValidateFrameDimensions(frame_width, frame_height))
            << "Invalid frame dimensions!";
        RET_CHECK(src_texture_name > 0 && dst_texture_name > 0)
            << "Both source and destination texture names must be non-null!";
        RET_CHECK_NE(src_texture_name, dst_texture_name)
            << "Source and destination texture names must be different!";

        // Wrap both source and destination textures.
        ASSIGN_OR_RETURN(
            std::unique_ptr<Texture> src_texture,
            Texture::WrapExternalTexture(src_texture_name, src_texture_target,
                                         frame_width, frame_height),
            _ << "Failed to wrap the external source texture");
        ASSIGN_OR_RETURN(
            std::unique_ptr<Texture> dst_texture,
            Texture::WrapExternalTexture(dst_texture_name, dst_texture_target,
                                         frame_width, frame_height),
            _ << "Failed to wrap the external destination texture");

        MP_RETURN_IF_ERROR(render_target_->SetViewport(*dst_texture))
            << "Failed to set the destination texture with viewport!";
        
        render_target_->Clear();
        // Render the source texture on top of the quad mesh (i.e. make a copy)
        // into the render target.
        MP_RETURN_IF_ERROR(renderer_->Render(
            *render_target_, *src_texture, Renderer::RenderMode::OVERDRAW))
            << "Failed to render the source texture on top of the quad mesh!";

        // At this point in the code, the destination texture must contain the
        // correctly renderer effect, so we should just return.
        return absl::OkStatus();
    }
private:
    std::unique_ptr<RenderTarget> render_target_;
    std::unique_ptr<Renderer> renderer_;
};

} // namespace

absl::StatusOr<std::unique_ptr<TriangleRenderer>> CreateTriangleRenderer() {
    ASSIGN_OR_RETURN(std::unique_ptr<RenderTarget> render_target,
                        RenderTarget::Create(),
                        _ << "Failed to create a render target!");

    ASSIGN_OR_RETURN(std::unique_ptr<Renderer> renderer, 
                        Renderer::Create(),
                        _ << "Failed to create a renderer!");

    std::unique_ptr<TriangleRenderer> result =
        absl::make_unique<TriangleRendererImpl>(std::move(render_target), 
                                                std::move(renderer));

    return result;
}