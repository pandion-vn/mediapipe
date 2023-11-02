#ifndef KALIDOKIT_H
#define KALIDOKIT_H

#include <math.h>
#define PI 3.14159265
#define TWO_PI PI*2
// #include <algorithm>

enum class Side { RIGHT, LEFT };

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

static std::vector<glm::vec3> DEFAULT_ROTATIONS = {
    glm::vec3(0, 0, 0), // 0. nose
    glm::vec3(0, 0, 0), // 1. left eye inner
    glm::vec3(0, 0, 0), // 2. left eye
    glm::vec3(0, 0, 0), // 3. left eye outer
    glm::vec3(0, 0, 0), // 4. right eye inner
    glm::vec3(0, 0, 0), // 5. right eye
    glm::vec3(0, 0, 0), // 6. right eye outer
    glm::vec3(0, 0, 0), // 7. left ear 
    glm::vec3(0, 0, 0), // 8. right ear
    glm::vec3(0, 0, 0), // 9. mouth left
    glm::vec3(0, 0, 0), // 10. mouth right
    glm::vec3(0, 0, 1.25), // 11. left shoulder
    glm::vec3(0, 0, -1.25), // 12. right shoulder
    glm::vec3(0, 0, 0), // 13. right elbow
    glm::vec3(0, 0, 0), // 14. right elbow
    glm::vec3(0, 0, 0), // 15. left wrist
    glm::vec3(0, 0, 0), // 16. right wrist
    glm::vec3(0, 0, 0), // 17. left pinky
    glm::vec3(0, 0, 0), // 18. right pinky
    glm::vec3(0, 0, 0), // 19. left index
    glm::vec3(0, 0, 0), // 20. right index
    glm::vec3(0, 0, 0), // 21. left thumb
    glm::vec3(0, 0, 0), // 22. right thumb
    glm::vec3(0, 0, 0), // 23. left hip
    glm::vec3(0, 0, 0), // 24. right hip
    glm::vec3(0, 0, 0), // 25. left knee
    glm::vec3(0, 0, 0), // 26. right knee
    glm::vec3(0, 0, 0), // 27. left ankle
    glm::vec3(0, 0, 0), // 28. right ankle
    glm::vec3(0, 0, 0), // 29. left heel
    glm::vec3(0, 0, 0), // 30. right heel
    glm::vec3(0, 0, 0), // 31. left foot index
    glm::vec3(0, 0, 0), // 32. right foot index
};

float normalizeRadians(float radians) {
    if (radians >= PI / 2) {
        radians -= TWO_PI;
    }
    if (radians <= -PI / 2) {
        radians += TWO_PI;
        radians = PI - radians;
    }
    //returns normalized values to -1,1
    return radians / PI;
}

float find2DAngle(float cx, float cy, float ex, float ey) {
    float dy = ey - cy;
    float dx = ex - cx;
    float theta = atan2(dy, dx);
    return theta;
}

float clamp(float val, float min, float max) {
    return fmax(fmin(val, max), min);
};

float angleBetween3DCoords(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    // Calculate vector between points 1 and 2
    auto v1 = (a - b);

    // Calculate vector between points 2 and 3
    auto v2 = (c - b);

    // The dot product of vectors v1 & v2 is a function of the cosine of the
    // angle between them (it's scaled by the product of their magnitudes).
    auto v1norm = glm::normalize(v1);
    auto v2norm = glm::normalize(v2);

    // Calculate the dot products of vectors v1 and v2
    auto dotProducts = glm::dot(v1norm, v2norm);

    // Extract the angle from the dot products
    auto angle = acos(dotProducts);

    // return single angle Normalized to 1
    return normalizeRadians(angle);
}

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float fraction) {
    return ((b - a) * fraction) + a;
}

glm::vec3 findRotation(glm::vec3 a, glm::vec3 b, bool normalize = true) {
    if (normalize) {
        return glm::vec3(
            normalizeRadians(find2DAngle(a.z, a.x, b.z, b.x)),
            normalizeRadians(find2DAngle(a.z, a.y, b.z, b.y)),
            normalizeRadians(find2DAngle(a.x, a.y, b.x, b.y))
        );
    } else {
        return glm::vec3(
            find2DAngle(a.z, a.x, b.z, b.x),
            find2DAngle(a.z, a.y, b.z, b.y),
            find2DAngle(a.x, a.y, b.x, b.y)
        );
    }
}

void rigArm(glm::vec3& upperArm, glm::vec3& lowerArm, glm::vec3& hand, Side side=Side::RIGHT) {
    // Invert modifier based on left vs right side
    int invert = side == Side::RIGHT ? 1 : -1;

    upperArm.z *= -2.3 * invert;
    //Modify UpperArm rotationY  by LowerArm X and Z rotations
    upperArm.y *= PI * invert;
    upperArm.y -= lowerArm.x; //fmax(lowerArm.x);
    upperArm.y -= -invert * fmax(lowerArm.z, 0);
    upperArm.x -= 0.3 * invert;

    lowerArm.z *= -2.14 * invert;
    lowerArm.y *= 2.14 * invert;
    lowerArm.x *= 2.14 * invert;

    //Clamp values to human limits
    upperArm.x = clamp(upperArm.x, -0.5, PI);
    lowerArm.x = clamp(lowerArm.x, -0.3, 0.3);

    hand.y = clamp(hand.z * 2, -0.6, 0.6); //side to side
    hand.z = hand.z * -2.3 * invert; //up down
}

void calcArms(std::vector<glm::vec3>& lm, std::vector<glm::vec3>& rotations) {
    auto upperArm_right = findRotation(lm[11], lm[13]);
    auto upperArm_left = findRotation(lm[12], lm[14]);

    upperArm_right.y = angleBetween3DCoords(lm[12], lm[11], lm[13]);
    upperArm_left.y = angleBetween3DCoords(lm[11], lm[12], lm[14]);

    auto lowerArm_right = findRotation(lm[13], lm[15]);
    auto lowerArm_left = findRotation(lm[14], lm[16]);

    lowerArm_right.y = angleBetween3DCoords(lm[11], lm[13], lm[15]);
    lowerArm_left.y = angleBetween3DCoords(lm[12], lm[14], lm[16]);

    lowerArm_left.z = clamp(lowerArm_right.z, -2.14, 0);
    lowerArm_left.z = clamp(lowerArm_left.z, -2.14, 0);

    auto hand_right = findRotation(lm[15], lerp(lm[17], lm[19], 0.5));
    auto hand_left = findRotation(lm[16], lerp(lm[18], lm[20], 0.5));

    rigArm(upperArm_right, lowerArm_right, hand_right, Side::RIGHT);
    rigArm(upperArm_left, lowerArm_left, hand_left, Side::LEFT);


    rotations[11] = upperArm_right;
    rotations[13] = lowerArm_right;
    rotations[15] = hand_right;

    rotations[12] = upperArm_left;
    rotations[14] = lowerArm_left;
    rotations[16] = hand_left;
}

void kalidokitSolve(std::vector<glm::vec3>& lm3d, std::vector<float>& lm3d_visibility, std::vector<glm::vec3>& lm2d, std::vector<glm::vec3>& rotations) {
    calcArms(lm3d, rotations);

    //DETECT OFFSCREEN AND RESET VALUES TO DEFAULTS
    bool rightHandOffscreen = lm3d[15].y > 0.1 || lm3d_visibility[15] < 0.23 || 0.995 < lm2d[15].y;
    bool leftHandOffscreen = lm3d[16].y > 0.1 || lm3d_visibility[16] < 0.23 || 0.995 < lm2d[16].y;

    // const leftFootOffscreen = lm3d[23].y > 0.1 || (lm3d_visibility[23] ?? 0) < 0.63 || Hips.Hips.position.z > -0.4;
    // const rightFootOffscreen = lm3d[24].y > 0.1 || (lm3d_visibility[24] ?? 0) < 0.63 || Hips.Hips.position.z > -0.4;

    // Arms.UpperArm.l = Arms.UpperArm.l.multiply(leftHandOffscreen ? 0 : 1);
    rotations[12] = rotations[12] * (leftHandOffscreen ? 0 : 1);
    // Arms.UpperArm.l.z = leftHandOffscreen ? RestingDefault.Pose.LeftUpperArm.z : Arms.UpperArm.l.z;
    rotations[12].z = leftHandOffscreen ? DEFAULT_ROTATIONS[12].z : rotations[12].z;
    // Arms.UpperArm.r = Arms.UpperArm.r.multiply(rightHandOffscreen ? 0 : 1);
    rotations[11] = rotations[11] * (rightHandOffscreen ? 0 : 1);
    // Arms.UpperArm.r.z = rightHandOffscreen ? RestingDefault.Pose.RightUpperArm.z : Arms.UpperArm.r.z;
    rotations[11].z = rightHandOffscreen ? DEFAULT_ROTATIONS[11].z : rotations[11].z;

    // Arms.LowerArm.l = Arms.LowerArm.l.multiply(leftHandOffscreen ? 0 : 1);
    // Arms.LowerArm.r = Arms.LowerArm.r.multiply(rightHandOffscreen ? 0 : 1);

    // Arms.Hand.l = Arms.Hand.l.multiply(leftHandOffscreen ? 0 : 1);
    // Arms.Hand.r = Arms.Hand.r.multiply(rightHandOffscreen ? 0 : 1);
}

glm::quat quatFromEuler(glm::vec3 v, std::string& order) {
    float c1 = cos( v.x / 2 );
    float c2 = cos( v.y / 2 );
    float c3 = cos( v.z / 2 );

    float s1 = sin( v.x / 2 );
    float s2 = sin( v.y / 2 );
    float s3 = sin( v.z / 2 );

    if (order == "XYZ") {
        return glm::quat(
            c1 * c2 * c3 - s1 * s2 * s3, // w
            s1 * c2 * c3 + c1 * s2 * s3, // x
            c1 * s2 * c3 - s1 * c2 * s3, // y
            c1 * c2 * s3 + s1 * s2 * c3  // z
        );
    }
}

#endif