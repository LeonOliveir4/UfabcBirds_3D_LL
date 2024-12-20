#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_

#include "abcgOpenGL.hpp"


struct Material {
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f};
  float m_shininess{25.0f};
};

#endif