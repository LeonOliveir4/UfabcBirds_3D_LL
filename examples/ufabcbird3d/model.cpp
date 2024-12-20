//Classe padrao para qualquer opbjeto 3d que pode ser renderizado
#include "model.hpp"
#include <iostream>


template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Model::computeNormals() {
  // Clear previous vertex normals
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::vec3(0.0f);
  }

  // Compute face normals
  for (auto const offset : iter::range(0UL, m_indices.size(), 3UL)) {
    // Get face vertices
    auto &a{m_vertices.at(m_indices.at(offset + 0))};
    auto &b{m_vertices.at(m_indices.at(offset + 1))};
    auto &c{m_vertices.at(m_indices.at(offset + 2))};

    // Compute normal
    auto const edge1{b.position - a.position};
    auto const edge2{c.position - b.position};
    auto const normal{glm::cross(edge2, edge1)};

    // Accumulate on vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalize
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}


void Model::createVertex() {
    m_vertices.clear();
    m_indices.clear();

    m_vertices = {//x y z
        Vertex{.position = glm::vec3{-0.5f, -0.5f, -0.5f}},
        Vertex{.position =glm::vec3{-0.5f, 0.5f, -0.5f}},
        Vertex{.position =glm::vec3{-0.5f, 0.5f, 0.5f}},
        Vertex{.position =glm::vec3{-0.5f, -0.5f, 0.5f}},
        Vertex{.position =glm::vec3{0.5f, -0.5f, -0.5f}},
        Vertex{.position =glm::vec3{0.5f, 0.5f, -0.5f}},
        Vertex{.position =glm::vec3{0.5f, 0.5f, 0.5f}},
        Vertex{.position =glm::vec3{0.5f, -0.5f, 0.5f}}
    };

    m_indices = {
        0, 1, 2,//
        0, 2, 3,
        0, 1, 4,//
        1, 4, 5,
        0, 3, 4,//
        3, 4, 7,
        2, 3, 7,//
        2, 6, 7,
        4, 5, 6,//
        4, 6, 7,
        1, 2, 5,//
        2, 5, 6
    };
  m_hasNormals = false;
  if (!m_hasNormals) {
    computeNormals();
  }
}

void Model::createBuffers() {
    //clear buffers
    abcg::glDeleteVertexArrays(1, &m_VAO);
    abcg::glDeleteBuffers(1, &m_VBO);
    abcg::glDeleteBuffers(1, &m_EBO);


    abcg::glGenBuffers(1, &m_VBO);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    abcg::glBufferData(GL_ARRAY_BUFFER,
                        sizeof(m_vertices.at(0)) * m_vertices.size(),
                        m_vertices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glGenBuffers(1, &m_EBO);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        sizeof(m_indices.at(0)) * m_indices.size(),
                        m_indices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //Vertex array definition
    abcg::glGenVertexArrays(1, &m_VAO);
    abcg::glBindVertexArray(m_VAO);
    //Vertex bind
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    //Vertex atributes: Position
    auto const positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
    if (positionAttribute >= 0) {
      abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(Vertex), nullptr);
      abcg::glEnableVertexAttribArray(positionAttribute);
    }
    //Vertex atributes: Normal
    auto const normalAttribute{abcg::glGetAttribLocation(m_program, "inNormal")};
    if (normalAttribute >= 0) {
      abcg::glEnableVertexAttribArray(normalAttribute);
      auto const offset{offsetof(Vertex, normal)};
      abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
    }


        // End of binding
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    abcg::glBindVertexArray(0);
}

void Model::create(GLuint program) {
    m_program = program;
    createVertex();
    createBuffers();
}

void Model::render(const Camera camera, const Light light){//const float *viewMatrix, const float *projMatrix){
    abcg::glUseProgram(m_program);

    auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
    auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
    auto const modelMatrixLoc{abcg::glGetUniformLocation(m_program, "modelMatrix")};
    auto const normalMatrixLoc{abcg::glGetUniformLocation(m_program, "normalMatrix")};
    auto const lightDirLoc{abcg::glGetUniformLocation(m_program, "lightDirWorldSpace")};
    auto const shininessLoc{abcg::glGetUniformLocation(m_program, "shininess")};
    auto const IaLoc{abcg::glGetUniformLocation(m_program, "Ia")};
    auto const IdLoc{abcg::glGetUniformLocation(m_program, "Id")};
    auto const IsLoc{abcg::glGetUniformLocation(m_program, "Is")};
    auto const KaLoc{abcg::glGetUniformLocation(m_program, "Ka")};
    auto const KdLoc{abcg::glGetUniformLocation(m_program, "Kd")};
    auto const KsLoc{abcg::glGetUniformLocation(m_program, "Ks")};
    auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};
    //geometry
    abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
    abcg::glUniform4f(colorLoc, m_color.r,m_color.g, m_color.b, m_color.a);
    //Light and Materials
    //Normal
    auto const modelViewMatrix{glm::mat3(camera.getViewMatrix() * m_modelMatrix)};
    auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
    abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
    //Light
    abcg::glUniform4fv(lightDirLoc, 1, &light.m_lightDir);
    abcg::glUniform4fv(IaLoc, 1, &light.m_Ia.x);
    abcg::glUniform4fv(IdLoc, 1, &light.m_Id.x);
    abcg::glUniform4fv(IsLoc, 1, &light.m_Is.x);
    //Material
    abcg::glUniform4fv(KaLoc, 1, &m_material.m_Ka.x);
    abcg::glUniform4fv(KdLoc, 1, &m_material.m_Kd.x);
    abcg::glUniform4fv(KsLoc, 1, &m_material.m_Ks.x);
    abcg::glUniform1f(shininessLoc, m_material.m_shininess);

    

    abcg::glBindVertexArray(m_VAO);
    abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void Model::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  //auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position) * scaling;
  }
}

void Model::destroy(){
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}