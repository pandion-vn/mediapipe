// Copyright 2019 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gl_base_calculator.h"

namespace mediapipe {

static constexpr char kImageGpuTag[] = "IMAGE_GPU";

// A calculator that renders a visual effect for multiple faces.
//
// Inputs:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer containing input image.
// Output:
//   IMAGE_GPU (`GpuBuffer`, required):
//     A buffer with a visual effect being rendered for multiple faces.
// static
absl::Status GlBaseCalculator::GetContract(CalculatorContract* cc) {
    MP_RETURN_IF_ERROR(GlCalculatorHelper::UpdateContract(cc))
        << "Failed to update contract for the GPU helper!";

    cc->Inputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    cc->Outputs().Tag(kImageGpuTag).Set<GpuBuffer>();
    // Currently we pass GL context information and other stuff as external
    // inputs, which are handled by the helper.
    return GlCalculatorHelper::UpdateContract(cc);
}

absl::Status GlBaseCalculator::Open(CalculatorContext* cc) {
    // Inform the framework that we always output at the same timestamp
    // as we receive a packet at.
    cc->SetOffset(mediapipe::TimestampDiff(0));

    // Let the helper access the GL context information.
    MP_RETURN_IF_ERROR(gpu_helper_.Open(cc)) 
        << "Failed to open the GPU helper!";
    return RunInGlContext([this, cc]() -> absl::Status {
        ASSIGN_OR_RETURN(framebuffer_target_, FrameBufferTarget::Create(),
                         _ << "Failed to create a framebuffer target!");
        return absl::OkStatus();
    });
}

absl::Status GlBaseCalculator::Process(CalculatorContext* cc) {
    // The `IMAGE_GPU` stream is required to have a non-empty packet. In case
    // this requirement is not met, there's nothing to be processed at the
    // current timestamp.
    if (cc->Inputs().Tag(kImageGpuTag).IsEmpty()) {
        return absl::OkStatus();
    }

    return RunInGlContext([this, cc]() -> absl::Status {
        const auto& input_gpu_buffer = cc->Inputs().Tag(kImageGpuTag).Get<GpuBuffer>();
        if (!initialized_) {
            MP_RETURN_IF_ERROR(GlSetup());
            initialized_ = true;
            animation_start_time_ = cc->InputTimestamp().Seconds();
        }

        GlTexture input_gl_texture = gpu_helper_.CreateSourceTexture(input_gpu_buffer);
        int dst_width = input_gl_texture.width();
        int dst_height = input_gl_texture.height();
        GlTexture output_gl_texture = gpu_helper_.CreateDestinationTexture(dst_width, dst_height);

        // Set the destination texture as the color buffer. Then, clear both the
        // color and the depth buffers for the render target.
        framebuffer_target_->SetColorbuffer(dst_width, dst_height, output_gl_texture.target(), output_gl_texture.name());
        framebuffer_target_->Clear();

        MP_RETURN_IF_ERROR(GlBind());
        // Run core program.
        MP_RETURN_IF_ERROR(GlRender(input_gl_texture, 
                                    output_gl_texture, 
                                    cc->InputTimestamp().Seconds()));

        MP_RETURN_IF_ERROR(GlCleanup());
        
        std::unique_ptr<GpuBuffer> output_gpu_buffer = output_gl_texture.GetFrame<GpuBuffer>();

        cc->Outputs()
            .Tag(kImageGpuTag)
            .AddPacket(mediapipe::Adopt<GpuBuffer>(output_gpu_buffer.release())
                            .At(cc->InputTimestamp()));

        output_gl_texture.Release();
        input_gl_texture.Release();

        return absl::OkStatus();
    });
}

absl::Status GlBaseCalculator::Close(CalculatorContext* cc) {
    return RunInGlContext([this]() -> absl::Status { 
        return GlTeardown(); 
    });
}

GlBaseCalculator::~GlBaseCalculator() {
  RunInGlContext([this] {
    framebuffer_target_.reset();
  });
}

}  // namespace mediapipe
