/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <main.cpp>
Purpose: <Get all the information from other files and complete the task in here>
Language: <c++>
Platform: <Visual studio, window>
Project: <minji.kim, class CS300, Assignment 2, minji.kim_CS300_2>
Author: <Seraphina Kim, minji.kim, minji.kim>
Creation date: <October. 21st. 2018>
End Header --------------------------------------------------------*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "imgui_master\\imgui.h"
#include "imgui_master\\imgui_impl_glfw.h"
#include "imgui_master\\imgui_impl_opengl3.h"

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Render.h"

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SPHERE 16
#define MIN_SPHERE 1

using namespace glm;

const char* glsl_version = "#version 330";

const float PI = 4.0f*atan(1.0f);
const unsigned  Width = 1024;
const unsigned  Height = 768;

float lastX = Width / 2.0f;
float lastY = Height / 2.0f;
float aspect = float(Width) / float(Height);

float dt, last_Frame = 0.f;
bool firstMouse = true;

bool isObjLoad = false;

bool scenario1 = false;
bool scenario2 = false;
bool scenario3 = false;

bool showVertexNorm = false;
bool showFaceNorm = false;

bool showBlinn = false;
bool showPhong_Lighting = false;
bool showPhong_Shading = true;

bool sphere_rotating = true;
int sphere_min = 0;
int sphere_count = 0;

float count = 0.f;

Camera cam(glm::vec3(0.0f, 1.2f, 7.0f));

struct GLFWwindow;

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
void ProcessInput(GLFWwindow *window);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(Width, Height, "CS300", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGui::StyleColorsDark();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader_vert_norm(true, Shader::VERTEX);
	Shader shader_face_norm(true, Shader::FACE);

	Shader shader(false, Shader::NORMAL);
	Shader phong_lighting_shader(false, Shader::PHONG_LIGHTING);
	Shader phong_shading_shader(false, Shader::PHONG_SHADING);
	Shader blinn_shader(false, Shader::BLINN_SHADING);

	const char* model_name = "models\\sphere_high_poly.obj";

	Render Plane;
	Plane.LoadObj("models\\plane_low_poly.obj");
	Plane.position = { 0, -1.15f, 0 };
	Plane.color = { 0.7f, 0.8f, 0.7f };
	Plane.scale = { -1.5f, 0, -1.5f };
	Plane.Draw(Plane.out_vertices, Plane.vertex_indices);

	Render Object;
	Object.LoadObj(model_name);
	Object.position = { 0, 0, 0 };
	Object.color = { 0.1f, 0.2f, 0.3f };
	Object.scale = { 1.f, 1.f, 1.f };
	Object.Draw(Object.out_vertices, Object.vertex_indices);

	Render Object2[16];
	Object2[0].LoadObj("models\\sphere_high_poly.obj");
	Object2[0].position = { 1, 0, 0 };
	Object2[0].color = { 0.2f, 0.1f, 0.1f };
	Object2[0].scale = { 0.1f, 0.1f, 0.1f };
	Object2[0].Draw(Object2[0].out_vertices, Object2[0].vertex_indices);

	for (int i = 1; i < 16; i++)
	{
		Object2[i].out_vertices = Object2[0].out_vertices;
		Object2[i].vertex_indices = Object2[0].vertex_indices;
		Object2[i].scale = Object2[0].scale;
	}

	for (int i = 1; i < 16; i++)
	{
		Object2[i].Draw(Object2[i].out_vertices, Object2[i].vertex_indices);
	}
	
	glm::vec3 Orbit_init_pos(2 * sinf(1), 0, 2 * cosf(1));

	Render Orbit;
	Orbit.color = { 0.0f, 0.0f, 0.0f };
	Orbit.scale = { 1.0f, 1.0f, 1.0f };
	Orbit.position = { 2.0f, 0.f, 0.f };

	for(unsigned i = 0; i < 16; ++i)
	{
		Object2[i].position.x = 2.0f*sin(PI*i / 8);
		Object2[i].position.z = 2.0f*cos(PI*i / 8);
	}

	Light directionalLight;

	bool isOrbit = false;
	unsigned orbit_vao, orbit_vbo;

	glGenVertexArrays(1, &orbit_vao);
	glGenBuffers(1, &orbit_vbo);

	bool show_demo_window = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		dt = currentFrame - last_Frame;
		last_Frame = currentFrame;

		ProcessInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("IMGUI_FIRST");
			ImGui::Text("Informations");

			if (ImGui::CollapsingHeader("Load_Objects"))
			{
				if (ImGui::Button("bunny_high_poly.obj"))
				{
					model_name = "models\\bunny_high_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("cube_high_poly.obj"))
				{
					model_name = "models\\cube_high_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("cube_low_poly.obj"))
				{
					model_name = "models\\cube_low_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("horse_high_poly.obj"))
				{
					model_name = "models\\horse_high_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("menger_sponge_level_1_high_poly.obj"))
				{
					model_name = "models\\menger_sponge_level_1_high_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("menger_sponge_level_1_low_poly.obj"))
				{
					model_name = "models\\menger_sponge_level_1_low_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("plane_low_poly.obj"))
				{
					model_name = "models\\plane_low_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("sphere_high_poly.obj"))
				{
					model_name = "models\\sphere_high_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("sphere_mid_poly.obj"))
				{
					model_name = "models\\sphere_mid_poly.obj";
					isObjLoad = true;
				}
				if (ImGui::Button("teapot_mid_poly.obj"))
				{
					model_name = "models\\teapot_mid_poly.obj";
					isObjLoad = true;
				}
			}
			if (ImGui::CollapsingHeader("Two Normals"))
			{
				if (ImGui::Button("Show Vertex Normal"))
				{
					if (showVertexNorm == true)
					{
						showVertexNorm = false;
					}
					else
					{
						showVertexNorm = true;
					}	
				}
				if (ImGui::Button("Show Face Normal"))
				{
					if (showFaceNorm == true)
					{
						showFaceNorm = false;
					}
					else
					{
						showFaceNorm = true;
					}
				}
			}	
			if (ImGui::CollapsingHeader("Rotating sphere"))
			{
				ImGui::Checkbox("Rotating", &sphere_rotating);
			}
			if (ImGui::CollapsingHeader("Sphere Light"))
			{
				ImGui::BulletText("Control numbers");
				if (ImGui::Button("  Add more sphere  "))
				{	
					++directionalLight.sphere_max;
					if (directionalLight.sphere_max >= MAX_SPHERE)
					{
						directionalLight.sphere_max = MAX_SPHERE;
					}
					directionalLight.lightColor[directionalLight.sphere_max-1] 
						= directionalLight.lightColor[directionalLight.sphere_max - 1];
				}
				ImGui::SameLine();
				if (ImGui::Button(" Remove sphere "))
				{
					--directionalLight.sphere_max;
					if (directionalLight.sphere_max <= MIN_SPHERE)
					{
						directionalLight.sphere_max = MIN_SPHERE;
					}
					directionalLight.lightColor[directionalLight.sphere_max] =
						directionalLight.lightColor[directionalLight.sphere_max];
				}
				int index = 0;
				for (int i = 0; i < directionalLight.sphere_max; i++)
				{
					std::string name = std::to_string(i);
					const char* pchar = name.c_str();

					if (ImGui::Button(pchar))
					{
						index = i;
						ImGui::SameLine();
					}
				}
			}
			ImGui::End();

			ImGui::Begin("IMGUI_SECOND");
			ImGui::Text("Shaders");

			if (ImGui::CollapsingHeader("Shading and Light"))
			{
				if (ImGui::CollapsingHeader("Blinn and Phong"))
				{
					if (ImGui::Button("Blinn Shading"))
					{
						showBlinn = true;
						showPhong_Lighting = false;
						showPhong_Shading = false;
					}
					if (ImGui::Button("Phong Lighting"))
					{
						showPhong_Lighting = true;
						showPhong_Shading = false;
						showBlinn = false;
					}
					if (ImGui::Button("Phong Shading"))
					{
						showPhong_Shading = true;
						showPhong_Lighting = false;
						showBlinn = false;
					}
				}

				if (ImGui::CollapsingHeader("Light"))
				{
					if (ImGui::Button("Direction Light"))
					{
						directionalLight.isDir = true;
						directionalLight.isPoint = false;
						directionalLight.isSpot = false;
					}
					if (ImGui::Button("Point Light"))
					{
						directionalLight.isDir = false;
						directionalLight.isPoint = true;
						directionalLight.isSpot = false;
					}
					if (ImGui::Button("Spot Light"))
					{
						directionalLight.isDir = false;
						directionalLight.isPoint = false;
						directionalLight.isSpot = true;
					}
				}
			
			}
			
			ImGui::SameLine();
			ImGui::End();

			ImGui::Begin("IMGUI_THIRD");
			ImGui::Text("Scenarios");

			if (ImGui::CollapsingHeader("Scenarios_123"))
			{

			if (ImGui::Button("Scenario_1"))
			{
				scenario2 = false;
				scenario3 = false;

				scenario1 = true;
				if (scenario1 == true)
				{
					directionalLight.sphere_max = MAX_SPHERE;
					for (int i = 0; i < directionalLight.sphere_max; ++i)
					{
						directionalLight.lightColor[i] = glm::vec3(0.3f, 0.1f, 0.3f);
					}
				}
			}
			if (ImGui::Button("Scenario_2"))
			{
				scenario1 = false;
				scenario3 = false;

				scenario2 =true;
				if (scenario2)
				{
					directionalLight.sphere_max = MAX_SPHERE;
					for (int i = 0; i < directionalLight.sphere_max; i++)
					{
						directionalLight.lightColor[i]
							= directionalLight.lightColor2[i];
					}
				}
			}
			if (ImGui::Button("Scenario_3"))
			{
				scenario2 = false;
				scenario1 = false;

				scenario3 = true;
				if (scenario3)
				{
					directionalLight.sphere_max = MAX_SPHERE;
					for (int i = 0; i < directionalLight.sphere_max; ++i)
					{
						directionalLight.lightColor[i] = glm::vec3(0.1f, 0.1f, 0.3f);
					}
				}
			}
		}

			ImGui::SameLine();
			ImGui::End();
		}

		if (isObjLoad == true)
		{
			Object.out_vertices.clear();
			Object.vertex_indices.clear();

			Object.LoadObj(model_name);
			Object.Draw(Object.out_vertices, Object.vertex_indices);
			isObjLoad = false;
		}

		// background color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		if (showPhong_Lighting == true)
		{
			cam.Update(&phong_lighting_shader);
		}
		else if (showPhong_Shading == true)
		{
			cam.Update(&phong_shading_shader);
		}
		else if (showBlinn == true)
		{
			cam.Update(&blinn_shader);
		}

		if (showBlinn == true)
		{
			blinn_shader.Use();
			Plane.Rendering(&cam, &blinn_shader, aspect, Plane.position);
			Object.Rendering(&cam, &blinn_shader, aspect, Object.position);
			directionalLight.Update(&blinn_shader);
		}
		if (showPhong_Lighting == true)
		{
			phong_lighting_shader.Use();
			Plane.Rendering(&cam, &phong_lighting_shader, aspect, Plane.position);
			Object.Rendering(&cam, &phong_lighting_shader, aspect, Object.position);
			directionalLight.Update(&phong_lighting_shader);
		}
		if (showPhong_Shading == true)
		{
			phong_shading_shader.Use();
			Plane.Rendering(&cam, &phong_shading_shader, aspect, Plane.position);
			Object.Rendering(&cam, &phong_shading_shader, aspect, Object.position);
			directionalLight.Update(&phong_shading_shader);
		}

		for (unsigned i = 0; i < 360; ++i)
		{
			float index = (float)i;
			Orbit_init_pos.x = 2.0f*sinf(glm::radians(index + 1));
			Orbit_init_pos.z = 2.0f*cosf(glm::radians(index + 1));

			Orbit.out_vertices.push_back(Orbit_init_pos);
		}
		if (!isOrbit)
		{
			glBindVertexArray(orbit_vao);
			glBindBuffer(GL_ARRAY_BUFFER, orbit_vbo);
			glBufferData(GL_ARRAY_BUFFER, Orbit.out_vertices.size() * sizeof(glm::vec3), &Orbit.out_vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			isOrbit = true;
		}
	
		glBindVertexArray(orbit_vao);
		glDrawArrays(GL_LINES, (GLuint)Orbit.out_vertices[0].x, Orbit.out_vertices.size());
		
		for (sphere_count = 0; sphere_count < directionalLight.sphere_max; sphere_count++)
		{
			directionalLight.position[sphere_count] = Object2[sphere_count].position;
			Object2[sphere_count].color = directionalLight.lightColor[sphere_count];
			Object2[sphere_count].Rendering(&cam, &shader, aspect, Object2[sphere_count].position);
		}
	
		if (sphere_rotating)
		{
			count += (float)0.004f;
			for (sphere_count = 0; sphere_count < directionalLight.sphere_max; ++sphere_count)
			{
				Object2[sphere_count].position.x = 2.0f*(float)sin(count + (PI*sphere_count / 8));
				Object2[sphere_count].position.z = 2.0f*(float)cos(count + (PI*sphere_count / 8));
			}
		}
	
		if (showVertexNorm == true)
		{
			shader_vert_norm.Use();
			Object.Rendering(&cam, &shader_vert_norm, aspect, Object.position);
		}
		if (showFaceNorm == true)
		{
			shader_face_norm.Use();
			Object.Rendering(&cam, &shader_face_norm, aspect, Object.position);
		}

		ImGui::Render();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}


void ProcessInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, dt);
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(width);
	UNREFERENCED_PARAMETER(height);
	glViewport(0, 0, width, height);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	UNREFERENCED_PARAMETER(window);
	UNREFERENCED_PARAMETER(xoffset);
	//UNREFERENCED_PARAMETER(yoffset);

	cam.ProcessMouseScroll(float(yoffset));
}