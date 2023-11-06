#ifndef MATH_H
#define MATH_H

#include <math.h>

#define PI 3.14159265
#define EPS 1e-4

static const glm::vec3 VEC3_ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
static const glm::vec3 VEC3_ONE = glm::vec3(1.0f, 1.0f, 1.0f);

static const glm::vec3 VEC3_X = glm::vec3(1.0f, 0.0f, 0.0f);
static const glm::vec3 VEC3_Y = glm::vec3(0.0f, 1.0f, 0.0f);
static const glm::vec3 VEC3_Z = glm::vec3(0.0f, 0.0f, 1.0f);

static const glm::vec3 VEC3_NEG_X = glm::vec3(-1.0f, 0.0f, 0.0f);
static const glm::vec3 VEC3_NEG_Y = glm::vec3(0.0f, -1.0f, 0.0f);
static const glm::vec3 VEC3_NEG_Z = glm::vec3(0.0f, 0.0f, -1.0f);

static const glm::quat QUAT_IDENTITY = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

glm::vec3 perpendicular(glm::vec3 vec) {
    if (vec.x != 0.0f) {
        return glm::vec3(-vec.y/vec.x, 1.0f, 0.0f);
    } else if (vec.y != 0.0f) {
        return glm::vec3(0.0f, -vec.z/vec.y, 1.0f);
    } else {
        return glm::vec3(1.0f, 0.0f, -vec.x/vec.z);
    }
}

glm::quat from_axis_angle(glm::vec3 axis, float angle) {
    float half_sin = sin(angle * 0.5);
    float half_cos = cos(angle * 0.5);
    // glm::vec3  = axis * s;
    return glm::quat(half_cos, 
                     axis.x * half_sin,
                     axis.y * half_sin,
                     axis.z * half_sin);
}

// based on opengl rotation tutorial
glm::quat rotation_between(glm::vec3 start, glm::vec3 dest) {
    float cos_theta = glm::dot(start, dest);
    if (cos_theta < -1.0001f) {
        auto perp = glm::normalize(perpendicular(start));
        return from_axis_angle(perp, PI/2.0f);
    }

    auto rot_axis = glm::cross(start, dest);
    float s = sqrt((1.0f+cos_theta)*2.0f);
    float invs = 1.0f / s;
    return glm::quat(
            s*0.5f,
            rot_axis.x*invs,
            rot_axis.y*invs,
            rot_axis.z*invs);
}

glm::quat rotate_towards(glm::vec3 dir, glm::vec3 track, glm::vec3 up) {
    if (glm::length(dir) < 0.0001) {
        return QUAT_IDENTITY;
    }

    auto right = glm::cross(dir, VEC3_Z);
    auto prev_up = glm::cross(right, dir);
    auto q1 = rotation_between(track, dir);
    auto new_up = up * q1;
    auto q2 = rotation_between(glm::normalize(new_up), glm::normalize(prev_up));
    return q2 * q1;
}

/// From the columns of a 3x3 rotation matrix.
/// Based on https://github.com/microsoft/DirectXMath `XM$quaternionRotationMatrix`
glm::quat from_rotation_axes(glm::vec3 x, glm::vec3 y, glm::vec3 z) {
    float m00(x.x), m01(x.y), m02(x.z);
    float m10(y.x), m11(y.y), m12(y.z);
    float m20(z.x), m21(z.y), m22(z.z);

    if (m22 <= 0.0) {
        // x^2 + y^2 >= z^2 + w^2
        float dif10 = m11 - m00;
        float omm22 = 1.0 - m22;
        if (dif10 <= 0.0) {
            // x^2 >= y^2
            float four_xsq = omm22 - dif10;
            float inv4x = 0.5 / sqrt(four_xsq);
            return glm::quat(
                (m12 - m21) * inv4x, // w
                four_xsq * inv4x,
                (m01 + m10) * inv4x,
                (m02 + m20) * inv4x
            );
        } else {
            // y^2 >= x^2
            float four_ysq = omm22 + dif10;
            float inv4y = 0.5 / sqrt(four_ysq);
            return glm::quat(
                (m20 - m02) * inv4y, // w
                (m01 + m10) * inv4y,
                four_ysq * inv4y,
                (m12 + m21) * inv4y
            );
        }
    } else {
        // z^2 + w^2 >= x^2 + y^2
        float sum10 = m11 + m00;
        float opm22 = 1.0 + m22;
        if (sum10 <= 0.0) {
            // z^2 >= w^2
            float four_zsq = opm22 - sum10;
            float inv4z = 0.5 / sqrt(four_zsq);
            return glm::quat(
                (m01 - m10) * inv4z, // w
                (m02 + m20) * inv4z,
                (m12 + m21) * inv4z,
                four_zsq * inv4z
            );
        } else {
            // w^2 >= z^2
            float four_wsq = opm22 + sum10;
            float inv4w = 0.5 / sqrt(four_wsq);
            return glm::quat(
                four_wsq * inv4w, // w
                (m12 - m21) * inv4w,
                (m20 - m02) * inv4w,
                (m01 - m10) * inv4w
            );
        }
    }
}

// Based on https://github.com/dfelinto/blender from_track_quat mathutil
// glm::quat from_vec_to_track_quat(glm::vec3 vec, int axis, int upflag) {
//     // let mut nor: [f32; 3] = [0.0; 3];
//     // let mut tvec: [f32; 3] = vec.to_array();
//     // let mut co: f32;
//     glm::vec3 tvec = vec;
//     glm::vec3 nor(0.0, 0.0, 0.0);
//     float co;

//     // assert!(axis != upflag);
//     // assert!(axis <= 5);
//     // assert!(upflag <= 2);

//     float len = glm::lenght(vec);
//     if (len == 0.0f) {
//         return QUAT_IDENTITY;
//     }

//     // rotate to axis
//     if (axis > 2) {
//         axis -= 3;
//     } else {
//         tvec[0] *= -1.0;
//         tvec[1] *= -1.0;
//         tvec[2] *= -1.0;
//     }

//     // x-axis
//     if (axis == 0) {
//         nor[0] = 0.0;
//         nor[1] = -tvec[2];
//         nor[2] = tvec[1];

//         if (std::abs(tvec[1]) + std::abs(tvec[2])) < EPS {
//             nor[1] = 1.0;
//         }
//         co = tvec[0];
//     }
//     // y-axis
//     else if (axis == 1) {
//         nor[0] = tvec[2];
//         nor[1] = 0.0;
//         nor[2] = -tvec[0];

//         if (std::abs(tvec[0]) + std::abs(tvec[2])) < EPS {
//             nor[2] = 1.0;
//         }
//         co = tvec[1];
//     }
//     // z-axis
//     else {
//         nor[0] = -tvec[1];
//         nor[1] = tvec[0];
//         nor[2] = 0.0;

//         if (std::abs(tvec[0]) + std::abs(tvec[1]) < EPS {
//             nor[0] = 1.0;
//         }
//         co = tvec[2];
//     }
//     co /= len;

//     // saacos
//     if (co <= -1.0) {
//         co = PI;
//     } else if (co >= 1.0) {
//         co = 0.0;
//     } else {
//         co = std::acos(co);
//     }

//     // q from angle
//     glm::quat q = from_axis_angle(glm::normalize(nor), co);
//     if (axis != upflag) {
//         // let angle: f32;
//         float angle;
//         let mat = q.quat_to_rotation_matrix();
//         let fp = mat.z;
//         float fp;

//         if (axis == 0) {
//             if (upflag == 1) {
//                 angle = 0.5 * fp.z.atan2(fp.y);
//             } else {
//                 angle = -0.5 * fp.y.atan2(fp.z);
//             }
//         } else if axis == 1 {
//             if upflag == 0 {
//                 angle = -0.5 * fp.z.atan2(fp.x);
//             } else {
//                 angle = 0.5 * fp.x.atan2(fp.z);
//             }
//         } else {
//             if upflag == 0 {
//                 angle = 0.5 * -fp.y.atan2(-fp.x);
//             } else {
//                 angle = -0.5 * -fp.x.atan2(-fp.y);
//             }
//         }

//         let si = angle.sin() / len;
//         let mut q2 = Quaternion::new(tvec[0] * si, tvec[1] * si, tvec[2] * si, angle.cos());
//         q2 *= q;
//         return q2
//     }
//     return q;
// }

#endif