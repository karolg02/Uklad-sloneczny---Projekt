#pragma once
#include <glad/glad.h>
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Orbit.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include "Texture.h"

class Planet {
public:
	const int n_wierzcholkow = 61;
	GLfloat* vertices;
	int n_vertices;
	GLuint* indices;
	int n_indices;

	glm::vec3 position;
	glm::mat4 model;

	Texture texture;

	VAO VAO_;
	VBO VBO_;
	EBO EBO_;

	double size;
	double color[3] = { 0. };
	double center_x, center_y;
	double speed; //liniowa pr�dko�� - styczna do okregu
	double radius_x; //odlegosc od srodka - poziomo (w przypadku elips radius_x != radius_y)
	double radius_y; //odlegosc od srodka - pionowo (w przypadku elips radius_x != radius_y)

	// (double rozmiar_planety, double predkosc_ruchu_planety, double promien_okregu_x, double promien_okregu_y, double color_r, double color_g, double color_b, double srodek_okregu_ruchu_x = 0., double srodek_okregu_ruchu_y = 0.);
	Planet(const char* texture_name, double rozmiar_planety, double predkosc_ruchu_planety, double promien_okregu_x, double promien_okregu_y, double color_r, double color_g, double color_b, double srodek_okregu_ruchu_x = 0., double srodek_okregu_ruchu_y = 0.);
	~Planet();
	
	void update(double time);
	void updateCenterByOtherPlanet(double time, double centerToFollow_x, double centerToFollow_y);
};	