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

namespace {

class Texture {
public:
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
    }

    ~RenderTarget() {
    }
};

class Renderer {
public:
    enum class RenderMode { OPAQUE, OVERDRAW, OCCLUSION };
    static absl::StatusOr<std::unique_ptr<Renderer>> Create() {
    }
    ~Renderer() { glDeleteProgram(program_handle_); }

    absl::Status Render(const RenderTarget& render_target, const Texture& texture,
                        RenderMode render_mode) const {
        return absl::OkStatus();
    }
private:
    Renderer(GLuint program_handle, GLint projection_mat_uniform,
           GLint model_mat_uniform, GLint texture_uniform):
           program_handle_(program_handle) {}
    GLuint program_handle_;
};

class TriangleRendererImpl : public TriangleRenderer {
public:
    TriangleRendererImpl(
        std::unique_ptr<RenderTarget> render_target,
        std::unique_ptr<Renderer> renderer):
            render_target_(std::move(render_target)),
            renderer_(std::move(renderer)) {}
    
    absl::Status RenderEffect(
        int frame_width,            //
        int frame_height,           //
        GLenum src_texture_target,  //
        GLuint src_texture_name,    //
        GLenum dst_texture_target,  //
        GLuint dst_texture_name) {
        // Validate input arguments.
        // MP_RETURN_IF_ERROR(ValidateFrameDimensions(frame_width, frame_height))
        //     << "Invalid frame dimensions!";
        RET_CHECK(src_texture_name > 0 && dst_texture_name > 0)
            << "Both source and destination texture names must be non-null!";
        RET_CHECK_NE(src_texture_name, dst_texture_name)
            << "Source and destination texture names must be different!";

        LOG(INFO) << "TriangleRendererImpl.RenderEffect()";

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

    LOG(INFO) << "CreateTriangleRenderer()";
    return result;
}