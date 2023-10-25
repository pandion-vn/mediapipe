#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/modules/face_geometry/protos/environment.pb.h"
#include "mediapipe/modules/face_geometry/protos/face_geometry.pb.h"

namespace mediapipe {

namespace {
    constexpr char kMultiFaceGeometryTag[] = "MULTI_FACE_GEOMETRY";
    constexpr char kMultiFaceDetectionTag[] = "DETECTIONS";
    constexpr char kMultiFaceLandmarksTag[] = "LANDMARKS";
    constexpr char kEnvironmentTag[] = "ENVIRONMENT";
}  // namespace

class CommonDebuggerCalculator : public CalculatorBase {
public:
    static absl::Status GetContract(CalculatorContract* cc);
    absl::Status Open(CalculatorContext* cc) override;
    absl::Status Process(CalculatorContext* cc) override;

private:
};

REGISTER_CALCULATOR(CommonDebuggerCalculator);

absl::Status CommonDebuggerCalculator::GetContract(CalculatorContract* cc) {  
    if (cc->Inputs().HasTag(kMultiFaceDetectionTag)) {
        cc->Inputs().Tag(kMultiFaceDetectionTag).Set<std::vector<mediapipe::Detection>>();
    }

    if (cc->Inputs().HasTag(kMultiFaceLandmarksTag)) {
        cc->Inputs().Tag(kMultiFaceLandmarksTag).Set<mediapipe::NormalizedLandmarkList>();
    }
    if (cc->Inputs().HasTag(kMultiFaceGeometryTag)) {
        cc->Inputs().Tag(kMultiFaceGeometryTag).Set<std::vector<face_geometry::FaceGeometry>>();
    }

    if (cc->InputSidePackets().HasTag(kEnvironmentTag)) {
        cc->InputSidePackets().Tag(kEnvironmentTag).Set<face_geometry::Environment>();
    }

    return absl::OkStatus();
}

absl::Status CommonDebuggerCalculator::Open(CalculatorContext* cc) {
    cc->SetOffset(mediapipe::TimestampDiff(0));

    // options_ = cc->Options<::mediapipe::AnglesToDetectionCalculatorOptions>();
    return absl::OkStatus();
}

absl::Status CommonDebuggerCalculator::Process(CalculatorContext* cc) {
    // RET_CHECK(!cc->Inputs().Tag(kDetectionsTag).IsEmpty());

    // std::vector<Detection> detections;
    // detections = cc->Inputs().Tag(kDetectionsTag).Get<std::vector<Detection>>();
    // std::cout << "detections size: " << detections.size() << std::endl;

    // debug here
    if (cc->Inputs().HasTag(kMultiFaceDetectionTag)) {
        const auto& detections = cc->Inputs().Tag(kMultiFaceDetectionTag).Get<std::vector<mediapipe::Detection>>();
        std::cout << "detections size: " << detections.size() << std::endl;
        for (const auto& detection : detections) {
            std::cout << "  format: " << detection.location_data().format() << std::endl;
            std::cout << "  location data (" << detection.location_data().bounding_box().xmin() << ", "
                                             << detection.location_data().bounding_box().ymin() << ", "
                                             << detection.location_data().bounding_box().width() << ", "
                                             << detection.location_data().bounding_box().height() << ")" << std::endl;
            std::cout << "  relative data (" << detection.location_data().relative_bounding_box().xmin() << ", "
                                             << detection.location_data().relative_bounding_box().ymin() << ", "
                                             << detection.location_data().relative_bounding_box().width() << ", "
                                             << detection.location_data().relative_bounding_box().height() << ")" << std::endl;   
            std::cout << "  relative keypoints" << std::endl;
            for (int i = 0; i < detection.location_data().relative_keypoints_size(); i++) {
                const auto& keypoint = detection.location_data().relative_keypoints(i);
                std::cout << "      (x, y) = " << keypoint.x() << ", " << keypoint.y() << std::endl;
            }
        }
    }

    if (cc->Inputs().HasTag(kMultiFaceLandmarksTag)) {
        const auto& landmarks = cc->Inputs().Tag(kMultiFaceLandmarksTag).Get<mediapipe::NormalizedLandmarkList>();
        std::cout << "landmarks size: " << landmarks.landmark_size() << std::endl;
        for (int i = 0; i < landmarks.landmark_size(); i++) {
            std::cout << "x : " << landmarks.landmark(i).x() 
                      << " y : " << landmarks.landmark(i).y() 
                      << " z : " << landmarks.landmark(i).z() << std::endl;
        }
        std::cout << std::endl;
    }

    if (cc->Inputs().HasTag(kMultiFaceGeometryTag)) {
        const auto& multi_face_geometry = cc->Inputs().Tag(kMultiFaceGeometryTag).Get<std::vector<face_geometry::FaceGeometry>>();

        // std::cout << "gender: " << input_agegender.gender() << std::endl;
        // std::cout << "age: " << input_age.age() << std::endl;
        std::cout << "multi_face_geometry size: " << multi_face_geometry.size() << std::endl;
        for (const auto& face_geometry : multi_face_geometry) {
            std::cout << "mesh3d vertex buffer: " << face_geometry.mesh().vertex_buffer(0) << std::endl;
            std::cout << "pose_transform_matrix row: " << face_geometry.pose_transform_matrix().rows() << std::endl;
            std::cout << "pose_transform_matrix cols: " << face_geometry.pose_transform_matrix().cols() << std::endl;
            std::cout << "pose_transform_matrix packed_data: " << face_geometry.pose_transform_matrix().packed_data(0) << std::endl;
        }
        std::cout << std::endl;
    }

    if (cc->InputSidePackets().HasTag(kEnvironmentTag)) {
        const auto& environment = cc->InputSidePackets().Tag(kEnvironmentTag).Get<face_geometry::Environment>();
        std::cout << "environment perspective_camera: " << environment.perspective_camera().vertical_fov_degrees() << std::endl;
    }

    return absl::OkStatus();
}

}