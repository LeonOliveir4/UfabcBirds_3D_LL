#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include "abcgOpenGL.hpp"


struct Light {
    glm::vec4 m_lightDir{0.0f, -50.0f, 0.0f, 0.0f};
    glm::vec4 m_Ia{1.0f};
    glm::vec4 m_Id{1.0f};
    glm::vec4 m_Is{1.0f};
};

#endif