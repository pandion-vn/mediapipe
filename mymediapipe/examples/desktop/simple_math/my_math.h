#ifndef MATH_H
#define MATH_H

#include <math.h>

#define PI 3.14159265

static const glm::vec3 VEC3_ZERO = glm::vec3(0.0f, 0.0f, 0.0f);
static const glm::vec3 VEC3_ONE = glm::vec3(1.0f, 1.0f, 1.0f);

static const glm::vec3 VEC3_X = glm::vec3(1.0f, 0.0f, 0.0f);
static const glm::vec3 VEC3_Y = glm::vec3(0.0f, 1.0f, 0.0f);
static const glm::vec3 VEC3_Z = glm::vec3(0.0f, 0.0f, 1.0f);

static const glm::vec3 VEC3_NEG_X = glm::vec3(-1.0f, 0.0f, 0.0f);
static const glm::vec3 VEC3_NEG_Y = glm::vec3(0.0f, -1.0f, 0.0f);
static const glm::vec3 VEC3_NEG_Z = glm::vec3(0.0f, 0.0f, -1.0f);

static const glm::quat QUAT_IDENTITY = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);

glm::vec3 perpendicular(glm::vec3 vec) {
    if (vec.x != 0.0f) {
        return glm::vec3(-vec.y/vec.x, 1.0f, 0.0f);
    } else if (vec.y != 0.0f) {
        return glm::vec3(0.0f, -vec.z/vec.y, 1.0f);
    } else {
        return glm::vec3(1.0f, 0.0f, -vec.x/vec.z);
    }
}

glm::quat from_axis_angles(glm::vec3 axis, float angle) {
    float s = sin(angle * 0.5);
    float c = cos(angle * 0.5);
    glm::vec3 v = axis * s;
    return glm::quat(v.x, v.y, v.z, c);
}

// based on opengl rotation tutorial
glm::quat rotation_between(glm::vec3 start, glm::vec3 dest) {
    float cos_theta = glm::dot(start, dest);
    if (cos_theta <= -1.0f+0.0001f) {
        auto perp = glm::normalize(perpendicular(start));
        return from_axis_angles(perp, PI/2.0f);
    }

    auto rot_axis = glm::cross(start, dest);
    float s = sqrt((1.0f+cos_theta)*2.0f);
    float invs = 1.0f / s;
    return glm::quat(
            rot_axis.x*invs,
            rot_axis.y*invs,
            rot_axis.z*invs,
            s*0.5f);
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
                four_xsq * inv4x,
                (m01 + m10) * inv4x,
                (m02 + m20) * inv4x,
                (m12 - m21) * inv4x
            );
        } else {
            // y^2 >= x^2
            float four_ysq = omm22 + dif10;
            float inv4y = 0.5 / sqrt(four_ysq);
            return glm::quat(
                (m01 + m10) * inv4y,
                four_ysq * inv4y,
                (m12 + m21) * inv4y,
                (m20 - m02) * inv4y
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
                (m02 + m20) * inv4z,
                (m12 + m21) * inv4z,
                four_zsq * inv4z,
                (m01 - m10) * inv4z
            );
        } else {
            // w^2 >= z^2
            float four_wsq = opm22 + sum10;
            float inv4w = 0.5 / sqrt(four_wsq);
            return glm::quat(
                (m12 - m21) * inv4w,
                (m20 - m02) * inv4w,
                (m01 - m10) * inv4w,
                four_wsq * inv4w
            );
        }
    }
}

#endif