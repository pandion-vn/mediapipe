#ifndef CAMERA_H__
#define CAMERA_H__

enum Camera_Type{ THIRD_PERSON, FREE_FLY };

// Beware, brain-compiled code ahead! 
inline Camera_Type& operator++(Camera_Type& cameraType) { 
    int val = static_cast<int>(cameraType);
    if (val>1)
        val = 0;

    return cameraType = static_cast<Camera_Type>( ++val );
}

inline Camera_Type operator++(Camera_Type& cameraType, int) {
    Camera_Type tmp(cameraType);
    ++cameraType;
    return tmp;
}

// Default camera values
const float YAW        =  -90.0f;
const float PITCH      =  0.0f;
const float SPEED      =  3.0f;
const float SENSITIVTY =  0.25f;
const float ZOOM       =  45.0f;

class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::quat Rotation;
    glm::quat ModelRotation;
    glm::vec3 Offset;
    glm::vec3 Direction;
    // Euler Angles
    GLfloat Yaw;
    GLfloat Pitch;
    // Camera options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom; 
    GLboolean HasMoved;
    Camera_Type CameraType;

public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f),
           GLfloat yaw = YAW, 
           GLfloat pitch = PITCH) :
            Front(glm::vec3(0.0f, 0.0f, -1.0f)),
            MovementSpeed(5.0f),
            MouseSensitivity(0.25f), 
            Zoom(45.0f) {
        this->Position = position;
        this->WorldUp = up;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->CameraType = THIRD_PERSON;
        this->updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() {
      return glm::lookAt(this->Position , this->Position + this->Front, this->WorldUp);
    }

    void SetTarget(glm::vec3 target) {
        if (CameraType == THIRD_PERSON) {
            this->Target = target;
            this->Front = glm::normalize(Target - Position);
        }
    }

    void SetPosition(glm::vec3 position) {
        this->Position = position;
        this->updateCameraVectors();
    }

private:  
    glm::vec3 Target;
    GLfloat deltaTime;
    
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // Calculate the new 
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    
        Direction = front;
      
        this->Rotation =   glm::quat(glm::vec3(0.0f,glm::radians(Yaw),glm::radians(Pitch)));
        this->ModelRotation =  glm::quat(glm::vec3(0.0f,glm::radians(-Yaw),0.0f));
    
        if (CameraType==FREE_FLY)
            this->Front = glm::normalize(front);
    
        // Make sure that when pitch is out of bounds, screen doesn't get flipped 
        //this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }
};
  
#endif