#ifndef POSE_H
#define POSE_H

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "my_math.h"

// Calculate pose landmarks -> rotation
//

// function set_pose_origin()
void set_pose_origin(std::vector<glm::vec3>& landmarks) {
    landmarks[34] = (landmarks[11] + landmarks[12]) / 2.f; // shoulder center location
    landmarks[33] = (landmarks[23] + landmarks[24]) / 2.f; // hip center location

    // offset
    glm::vec3 offset = landmarks[33];
    for (int i=0; i<landmarks.size(); i++) {
        landmarks[i] = landmarks[i] - offset;
    }
    landmarks[35] = offset;
}

/// Calculates shoulder rotation.
/// TODO: Check if results match expectations.
void shoulder_rotation(std::vector<glm::vec3>& landmarks, std::vector<glm::quat> &pose_rotations) {
    // As the torso rotation usually is used to rotate the rig,
    // the torso rotation got to be substracted from the hip rotation
    // let shoulder_rot = Quaternion::rotate_towards((landmarks[12]-data[34]).normalize(), Vector3::Z, Vector3::Y);
    auto shoulder_rot = rotate_towards(glm::normalize(landmarks[12]-landmarks[34]), VEC3_Z, VEC3_Y);
    // Quaternion::from_vec_to_track_quat((landmarks[12] - landmarks[34]).normalize().neg(), 2, 1); // rotation from center to right shoulder
    // let hip_rot = Quaternion::rotate_towards((landmarks[24] - landmarks[33]).normalize(), Vector3::Z, Vector3::Y); // rotation from center to right hip
    auto hip_rot = rotate_towards(glm::normalize(landmarks[24] - landmarks[33]), VEC3_Z, VEC3_Y);
    // let hip_rot = Quaternion::from_vec_to_track_quat((landmarks[24] - landmarks[33]).normalize().neg(), 2, 1); // rotation from center to right hip
    // pose_rotations[34] = shoulder_rot - hip_rot;
    pose_rotations[34] = shoulder_rot - hip_rot;
}

glm::vec3 plan_from_vec(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
    return glm::cross((v1 - v0), (v2 - v0));
}

glm::vec3 neg_from_vec(glm::vec3 v0) {
    return glm::vec3(-v0.x, -v0.y, -v0.z);
}

// https://arrowinmyknee.com/2021/02/10/how-to-get-rotation-from-two-vectors/
glm::quat from_two_vecs(glm::vec3 u, glm::vec3 v) {
    float cos_theta = glm::dot(glm::normalize(u), glm::normalize(v));
    // float angle = acos(cos_theta);
    // glm::vec3 w = glm::normalize(glm::cross(u, v));
    // return from_axis_angle(w, angle);
    float half_cos = sqrt(0.5f * (1.f + cos_theta));
    float half_sin = sqrt(0.5f * (1.f - cos_theta));
    glm::vec3 w = glm::normalize(glm::cross(u, v));
    return glm::quat(half_cos,
                     half_sin * w.x,
                     half_sin * w.y,
                     half_sin * w.z);
}

void torso_rotation(std::vector<glm::vec3>& landmarks, std::vector<glm::quat> &pose_rotations) {
    // left hip, right hip, shoulder center
    glm::vec3 normal = plan_from_vec(landmarks[23], landmarks[24], landmarks[34]);
    glm::vec3 tangent = landmarks[24] - landmarks[33]; // right hip, center hip
    glm::vec3 binormal = landmarks[34] - landmarks[33]; // hip center, shoulder center
    pose_rotations[33] = from_rotation_axes(
        glm::normalize(tangent),
        glm::normalize(normal),
        glm::normalize(binormal)
    );
}

void limb_rotation(std::vector<glm::vec3>& landmarks, std::vector<glm::quat> &pose_rotations) {

    pose_rotations[23] = rotate_towards(neg_from_vec(glm::normalize(landmarks[23] - landmarks[25])), VEC3_X, VEC3_Z); // left hip
    pose_rotations[25] = rotate_towards(neg_from_vec(glm::normalize(landmarks[25] - landmarks[27])), VEC3_X, VEC3_Z); // left knee
    // pose_rotations[23] = rotate_towards(neg_from_vec(glm::normalize(landmarks[25] - landmarks[23])), VEC3_X, VEC3_Z); // left hip
    // pose_rotations[25] = rotate_towards(neg_from_vec(glm::normalize(landmarks[27] - landmarks[25])), VEC3_X, VEC3_Z); // left knee

    pose_rotations[24] = rotate_towards(neg_from_vec(glm::normalize(landmarks[24] - landmarks[26])), VEC3_X, VEC3_Z); // right hip
    pose_rotations[26] = rotate_towards(neg_from_vec(glm::normalize(landmarks[26] - landmarks[28])), VEC3_X, VEC3_Z); // right knee
    
    // pose_rotations[11] = rotate_towards((glm::normalize(landmarks[11] - landmarks[13])), VEC3_Z, VEC3_X); // left shoulder
    // pose_rotations[13] = rotate_towards((glm::normalize(landmarks[13] - landmarks[15])), VEC3_Z, VEC3_X); // left elbow
    // pose_rotations[15] = rotate_towards((glm::normalize(landmarks[15] - landmarks[19])), VEC3_Z, VEC3_X); // left hand
    pose_rotations[11] = from_two_vecs(landmarks[11], landmarks[13]);
    pose_rotations[13] = from_two_vecs(landmarks[13], landmarks[15]);
    pose_rotations[15] = from_two_vecs(landmarks[15], landmarks[17]);

    // pose_rotations[12] = rotate_towards((glm::normalize(landmarks[12] - landmarks[14])), VEC3_Z, VEC3_X); // right shoulder
    // pose_rotations[14] = rotate_towards((glm::normalize(landmarks[14] - landmarks[16])), VEC3_Z, VEC3_X); // right elbow
    // pose_rotations[16] = rotate_towards((glm::normalize(landmarks[16] - landmarks[20])), VEC3_Z, VEC3_X); // right hand
    pose_rotations[12] = from_two_vecs(landmarks[12], landmarks[14]);
    pose_rotations[14] = from_two_vecs(landmarks[14], landmarks[16]);
    pose_rotations[16] = from_two_vecs(landmarks[16], landmarks[10]);

    // pose_rotations[12] = rotate_towards((glm::normalize(landmarks[14] - landmarks[12])), VEC3_Z, VEC3_X); // right shoulder
    // pose_rotations[14] = rotate_towards((glm::normalize(landmarks[16] - landmarks[14])), VEC3_Z, VEC3_X); // right elbow
    // pose_rotations[16] = rotate_towards((glm::normalize(landmarks[20] - landmarks[16])), VEC3_Z, VEC3_X); // right hand
}

/// Calculates foot rotation.
/// MPs knee, foot_index and heel usually form a triangle.
void foot_rotation(std::vector<glm::vec3>& landmarks, std::vector<glm::quat> &pose_rotations) {
    // left
    glm::vec3 tangent_left = plan_from_vec(landmarks[25], landmarks[27], landmarks[31]);
    glm::vec3 binormal_left = landmarks[25] - landmarks[31];
    glm::vec3 normal_left = landmarks[27] - landmarks[31];
    pose_rotations[27] = from_rotation_axes(
        glm::normalize(tangent_left),
        glm::normalize(normal_left),
        glm::normalize(binormal_left)
    );

    // right
    glm::vec3 tangent_right = plan_from_vec(landmarks[26], landmarks[28], landmarks[32]);
    glm::vec3 binormal_right = landmarks[26] - landmarks[32];
    glm::vec3 normal_right = landmarks[28] - landmarks[32];
    pose_rotations[28] = from_rotation_axes(
        glm::normalize(tangent_right),
        glm::normalize(normal_right),
        glm::normalize(binormal_right)
    );
}

// function calc_rotation_data()
//   shoulder_rotation
//   torso_rotation
//   limb_rotation
//   foot_rotation
void calc_rotation_data(std::vector<glm::vec3>& landmarks, std::vector<glm::quat> &pose_rotations) {
    shoulder_rotation(landmarks, pose_rotations);
    torso_rotation(landmarks, pose_rotations);
    limb_rotation(landmarks, pose_rotations);
    foot_rotation(landmarks, pose_rotations);
}

// function pose(landmarks)
//   from landmark[33] to vector of glm::vec3 -> data
//   set pose origin(data)
//   initial rotation_data [36]
//   calculate rotation data (data, rotation_data)
//   return rotation_data
std::vector<glm::quat> pose_rotation(std::vector<glm::vec3>& landmarks) {
    std::vector<glm::quat> pose_rotations(36);
    landmarks.resize(36);
    set_pose_origin(landmarks);

    calc_rotation_data(landmarks, pose_rotations);
    return pose_rotations;
}

#endif