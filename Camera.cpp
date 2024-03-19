#include "Camera.hpp"

namespace gps {

//Camera constructor
Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
    //TODO
    this->cameraPosition = cameraPosition;
    this->cameraTarget = cameraTarget;
    this->cameraUpDirection = cameraUp;
    this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
    this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    
    this->pitch = 0.0f;
    this->yaw = -90.0f;
}

//return the view matrix, using the glm::lookAt() function
glm::mat4 Camera::getViewMatrix() {
    //TODO
    return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
}

//update the camera internal parameters following a camera move event
void Camera::move(MOVE_DIRECTION direction, float speed) {
    switch (direction) {
    case MOVE_DIRECTION::MOVE_FORWARD:
        cameraPosition += speed * cameraFrontDirection;
//            this->cameraTarget += speed * this->cameraFrontDirection;
        break;
    case MOVE_DIRECTION::MOVE_BACKWARD:
        cameraPosition -= speed * cameraFrontDirection;
//            this->cameraTarget -= speed * this->cameraFrontDirection;
        break;
    case MOVE_DIRECTION::MOVE_RIGHT:
        cameraPosition += speed * cameraRightDirection;
//            this->cameraTarget += speed * this->cameraRightDirection;
        break;
    case MOVE_DIRECTION::MOVE_LEFT:
        cameraPosition -= speed * cameraRightDirection;
//            this->cameraTarget -= speed * this->cameraRightDirection;
        break;
    default:
        break;
    }
}
    

//update the camera internal parameters following a camera rotate event
//yaw - camera rotation around the y axis
//pitch - camera rotation around the x axis
void Camera::rotate(float pitch, float yaw) {
    //TODO
    
    this->pitch = pitch;
    this->yaw = yaw;
    
    glm::vec3 aux;
    
    aux.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    aux.y = -sin(glm::radians(pitch));
    aux.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    cameraFrontDirection = glm::normalize(aux);
    cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
}
}
