#include "ground.hpp"

void Ground::createVertex() {

  m_vertices = {
                Vertex{.position = glm::vec3(-3.0f, 0.0f, +3.0f)},
                Vertex{.position = glm::vec3(-3.0f, 0.0f, -3.0f)},
                Vertex{.position = glm::vec3(+3.0f, 0.0f, -3.0f)},
                Vertex{.position = glm::vec3(+3.0f, 0.0f, +3.0f)}};
  m_indices = {
                2, 1, 0,
                3, 2, 0
  };

  m_hasNormals = false;
  if (!m_hasNormals) {
    computeNormals();
  }
}

void Ground::render(Camera camera, const Light light) {
  abcg::glUseProgram(m_program);
  abcg::glBindVertexArray(m_VAO);
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
  auto const materialModeLoc{abcg::glGetUniformLocation(m_program, "materialMode")};


  // Draw a grid of 2N+1 x 2N+1 tiles on the xz plane, centered around the
  // origin
 auto const N{10};
   for (auto const z : iter::range(-N, N + 1)) {
     for (auto const x : iter::range(-N, N + 1)) {
      // Set model matrix as a translation matrix
      // glm::mat4 model{1.0f};
      setPosition(glm::vec3(x*6.0f, 0.0f, z*6.0f));
      abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
      abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);
      abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
      

      // Set color (checkerboard pattern)
      auto const gray{(z + x) % 2 == 0 ? 1.0f : 2.0f};
      abcg::glUniform4f(colorLoc, 0.2, gray *0.4, 0.2, 1.0f);
          //Normal
      auto const modelViewMatrix{glm::mat3(camera.getViewMatrix() * m_modelMatrix)};
      auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
      abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
      //Light
      abcg::glUniform4fv(lightDirLoc, 1, &light.m_lightDir.x);
      abcg::glUniform4fv(IaLoc, 1, &light.m_Ia.x);
      abcg::glUniform4fv(IdLoc, 1, &light.m_Id.x);
      abcg::glUniform4fv(IsLoc, 1, &light.m_Is.x);
      //Material
      abcg::glUniform4fv(KaLoc, 1, &m_material.m_Ka.x);
      abcg::glUniform4fv(KdLoc, 1, &m_material.m_Kd.x);
      abcg::glUniform4fv(KsLoc, 1, &m_material.m_Ks.x);
      abcg::glUniform1f(shininessLoc, m_material.m_shininess);
      abcg::glUniform1i(materialModeLoc, m_materialMode);
      abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
     }
  }

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}
