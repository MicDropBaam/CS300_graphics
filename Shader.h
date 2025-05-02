/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <Shader.h>
Purpose: <To provide shader informations>
Language: <c++>
Platform: <Visual studio, window>
Project: <minji.kim, class CS300, Assignment 2, minji.kim_CS300_2>
Author: <Seraphina Kim, minji.kim, minji.kim>
Creation date: <October. 21st. 2018>
End Header --------------------------------------------------------*/

#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>

class Shader 
{

public:

	enum Type {NORMAL, VERTEX, FACE, 
			PHONG_LIGHTING, PHONG_SHADING, BLINN_SHADING};

	void CreateShader(const char* vert_path, const char* frag_path, const char* geo_path);
	void Use() const;

	void SetBool(const std::string &name, bool value) const;
	void SetuInt(const std::string &name, unsigned value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetVec3(const std::string &name, const glm::vec3 &value) const;
	
	void SetVec4(const std::string &name, const glm::vec4 &value) const;
	void SetVec4(const std::string &name, float x, float y, float z, float w) const;
	void SetMat4(const std::string &name, const glm::mat4 &mat) const;
	Shader(bool isGeoUse, Type type);
	~Shader();

	GLuint m_programId, m_vertexId, m_fragmentId, m_geometryId, m_bli;
	int	m_infoLogLength;
	GLint m_result;

	bool isGeoUse;

	Type m_norm;
	
};
