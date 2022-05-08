//
//  Arcball.cpp


#include "Arcball.h"
#include <algorithm>

Arcball::Arcball(int window_width, int window_height, GLfloat roll_speed, bool x_axis, bool y_axis) {
    this->windowWidth = window_width;
    this->windowHeight = window_height;

    this->mouseEvent = 0;
    this->rollSpeed = roll_speed;
    this->angle = 0.0f;
    this->camAxis = glm::vec3(0.0f, 1.0f, 0.0f);

    this->xAxis = x_axis;
    this->yAxis = y_axis;
}

void Arcball::init(int window_width, int window_height, GLfloat roll_speed, bool x_axis, bool y_axis) {
    this->windowWidth = window_width;
    this->windowHeight = window_height;

    this->rollSpeed = roll_speed;
    this->angle = 0.0f;
    this->camAxis = glm::vec3(0.0f, 1.0f, 0.0f);

    this->xAxis = x_axis;
    this->yAxis = y_axis;
}


glm::vec3 Arcball::toScreenCoord(double x, double y) {
    glm::vec3 coord(0.0f);

    if (xAxis)
        coord.x = (2 * x - windowWidth) / windowWidth;

    if (yAxis)
        coord.y = -(2 * y - windowHeight) / windowHeight;

 
    coord.x = glm::clamp(coord.x, -1.0f, 1.0f);
    coord.y = glm::clamp(coord.y, -1.0f, 1.0f);

    float length_squared = coord.x * coord.x + coord.y * coord.y;
    if (length_squared <= 1.0)
        coord.z = sqrt(1.0 - length_squared);
    else
        coord = glm::normalize(coord);

    return coord;
}


void Arcball::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    mouseEvent = (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT);
}

void Arcball::cursorCallback(GLFWwindow* window, double x, double y) {
    if (mouseEvent == 0)
        return;
    else if (mouseEvent == 1) {
      
        prevPos = toScreenCoord(x, y);
        mouseEvent = 2;
        return;
    }

    currPos = toScreenCoord(x, y);

    
    angle = acos(std::min(1.0f, glm::dot(prevPos, currPos)));

    camAxis = glm::cross(prevPos, currPos);
}


glm::mat4 Arcball::createViewRotationMatrix() {
    return glm::rotate(glm::degrees(angle) * rollSpeed, camAxis);
}


glm::mat4 Arcball::createModelRotationMatrix(glm::mat4& view_matrix) {
    glm::vec3 axis = glm::inverse(glm::mat3(view_matrix)) * camAxis;
    return glm::rotate(glm::degrees(angle) * rollSpeed, axis);
}