/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <Render.h>
Purpose: <To Rendering objects>
Language: <c++>
Platform: <Visual studio, window>
Project: <minji.kim, class CS300, Assignment 2, minji.kim_CS300_2>
Author: <Seraphina Kim, minji.kim, minji.kim>
Creation date: <October. 21st. 2018>
End Header --------------------------------------------------------*/
#pragma once
#include "glm/glm.hpp"
#include <vector>
#include <algorithm>

class Camera;
class Shader;

class Render
{
	unsigned m_vao, m_vbo, m_ebo;
	unsigned m_element_size;

public:

	Render();
	~Render();

	void Rendering(Camera* camera, Shader* shader, float aspect, glm::vec3 m_position);
	void Draw(std::vector<glm::vec3>& vert, std::vector<unsigned>& indices);
	bool LoadObj(const char* path);

	glm::vec3 position, scale, color;
	float rotation;

	std::vector<unsigned> vertex_indices;
	std::vector<glm::vec3> out_vertices;

};