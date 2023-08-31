#ifndef MYMEDIAPIPE_TRIANGLE_RENDERER_H_
#define MYMEDIAPIPE_TRIANGLE_RENDERER_H_

#include <memory>
#include <vector>

#include "absl/types/optional.h"

// Encapsulates a stateful face effect renderer.
class TriangleRenderer {
public:
    virtual ~TriangleRenderer() = default;
    virtual absl::Status RenderEffect();
};

absl::StatusOr<std::unique_ptr<TriangleRenderer>> CreateTriangleRenderer();


#endif  // MEDIAPIPE_MODULES_FACE_GEOMETRY_LIBS_EFFECT_RENDERER_H_