#ifndef LIGHT_HPP_
#define LIGHT_HPP_

#include "abcgOpenGL.hpp"


struct Light {
    glm::vec4 m_lightDir = glm::normalize(glm::vec4(0.0f, -50.0f, 0.0f, 0.0f));
    glm::vec4 m_Ia{0.4f, 0.37f, 0.35f, 1.0f}; // Luz ambiente com tom quente leve
    glm::vec4 m_Id{1.0f, 0.95f, 0.85f, 1.0f}; // Luz difusa levemente amarelada
    glm::vec4 m_Is{1.0f, 1.0f, 0.9f, 1.0f};   // Luz especular quase neutra
};

#endif


