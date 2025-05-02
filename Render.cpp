#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

#include "Render.h"
#include "Camera.h"
#include "Shader.h"

#include <iostream>

Render::Render()
	: position(glm::vec3()), scale(glm::vec3(1, 1, 1)), color(glm::vec3(1.0f, 1.0f, 1.0f)),rotation(0.f)
{
}

Render::~Render()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void Render::Rendering(Camera* camera, Shader* shader, float aspect, glm::vec3 m_position)
{
	const static glm::vec3 up(0, 1, 0);

	glm::mat4 identity_translate(1.0);
	glm::mat4 identity_scale(1.0);
	glm::mat4 identity_rotation(1.0);
	glm::mat4 model = glm::translate(identity_translate, m_position) * glm::scale(identity_scale, scale) * glm::rotate(identity_rotation, rotation, up);
	glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), aspect, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();

	shader->SetMat4("model", model);
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("color", color);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_element_size, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Render::Draw(std::vector<glm::vec3>& vert, std::vector<unsigned>& indices)
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), &vert[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	m_element_size = indices.size() * sizeof(unsigned);
}

bool Render::LoadObj(const char * path)
{
	float x_max = 0,x_min = 0,y_max = 0,y_min = 0,z_max = 0,z_min = 0,max_abs = 0;
	glm::vec3 mid(0,0,0);

	FILE* file;
	fopen_s(&file, path, "r");

	if (file == NULL)
	{
		std::cout << "Cannot open the file\n";
		return false;
	}
	while (true)
	{
		char lineHeader[128];

		int result = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader) / sizeof(char));
		if (result == EOF)
		{
			if (max_abs < glm::abs(x_max - x_min))
				max_abs = glm::abs(x_max - x_min);
			if (max_abs < glm::abs(y_max - y_min))
				max_abs = glm::abs(y_max - y_min);
			if (max_abs < glm::abs(z_max - z_min))
				max_abs = glm::abs(z_max - z_min);

			mid = glm::vec3((x_max + x_min) / (2 * max_abs),(y_max + y_min) / (2 * max_abs), (z_max + z_min) / (2 * max_abs));

			for (unsigned i = 0; i < out_vertices.size(); ++i)
			{
				out_vertices[i].x = out_vertices[i].x / (0.5f * max_abs);
				out_vertices[i].y = out_vertices[i].y / (0.5f * max_abs);
				out_vertices[i].z = out_vertices[i].z / (0.5f * max_abs);
				out_vertices[i] = out_vertices[i] - (2.f * mid);
			}
			break;
		}
		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
		
			if (x_min < vertex.x)
				x_min = vertex.x;
			if (y_min < vertex.y)
				y_min = vertex.y;
			if (z_min < vertex.z)
				z_min = vertex.z;

			if (x_max > vertex.x)
				x_max = vertex.x;
			if (y_max > vertex.y)
				y_max = vertex.y;
			if (z_max > vertex.z)
				z_max = vertex.z;

			out_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3];
			int matches = fscanf_s(file, "%d %d %d\n", &vertexIndex[0],
				&vertexIndex[1], &vertexIndex[2]);
			if (matches != 3)
			{
				std::cout << "File can't be read\n";
				return false;
			}
			vertex_indices.push_back(vertexIndex[0] - 1);
			vertex_indices.push_back(vertexIndex[1] - 1);
			vertex_indices.push_back(vertexIndex[2] - 1);
		}
	}
	return true;
}



