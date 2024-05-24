#define _USE_MATH_DEFINES 
#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>
using namespace std;

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "ShaderClass.h"
#include "Planet.h"
#include "Orbit.h"
#include "Camera.h"
#include "Texture.h"

const double targetFrameTime = 1.0 / 1000.0;

// Globalny mno¿nik czasu, aby spowolniæ symulacjê
const double timeScale = 0.001;

void drawIt(Planet &obiekt, double time, GLuint shaderProgramID) {
	obiekt.update(time);
	obiekt.VAO_.Bind();
	obiekt.texture.Bind();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(obiekt.model));
	glDrawElements(GL_TRIANGLES, obiekt.n_indices, GL_UNSIGNED_INT, 0);
	obiekt.texture.Unbind();
	obiekt.VAO_.Unbind();
}
void drawIt(Planet& obiekt, double time, GLuint shaderProgramID, Planet& obiektWokolKtoregoKrazy) {
	obiekt.updateCenterByOtherPlanet(time, obiektWokolKtoregoKrazy.position.x, obiektWokolKtoregoKrazy.position.z);
	obiekt.VAO_.Bind();
	obiekt.texture.Bind();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(obiekt.model));
	glDrawElements(GL_TRIANGLES, obiekt.n_indices, GL_UNSIGNED_INT, 0);
	obiekt.texture.Unbind();
	obiekt.VAO_.Unbind();
}
void drawIt(Orbit& obiekt, double time, GLuint shaderProgramID) {
	obiekt.update(time);
	obiekt.VAO_.Bind();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(obiekt.model));
	glDrawElements(GL_LINES, obiekt.n_indices, GL_UNSIGNED_INT, 0);
	obiekt.VAO_.Unbind();
}
void drawIt(Orbit& obiekt, double time, GLuint shaderProgramID, Planet& obiektWokolKtoregoKrazy) {
	obiekt.updateCenterByOtherPlanet(time, obiektWokolKtoregoKrazy.position.x, obiektWokolKtoregoKrazy.position.z);
	obiekt.VAO_.Bind();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(obiekt.model));
	glDrawElements(GL_LINES, obiekt.n_indices, GL_UNSIGNED_INT, 0);
	obiekt.VAO_.Unbind();
}
void drawSun(Planet& obiekt, Shader lightShader, VAO lightVAO, Camera camera) {
	lightShader.Activate();
	obiekt.texture.Bind();
	camera.Matrix(lightShader, "camMatrix");
	lightVAO.Bind();
	glDrawElements(GL_TRIANGLES, obiekt.n_indices, GL_UNSIGNED_INT, 0);
}
//CAMERA, RUCH KAMERY
float deltaTime = 0., lastFrame = 0.;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
Camera* currentCamera; Camera* cam1; Camera* cam2;
void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // uruchamiane po zmianie rozmiaru okienka
	if (width >= height)
		glViewport(0, 0, width, width);
	else
		glViewport(0, 0, height, height);
}
ostream& operator<<(ostream& stream, const glm::vec3& v)
{
	stream << "(" << v.x << "," << v.y << "," << v.z << ")";
	return stream;
}
int width = 1280, height = 720;
int main() {
	srand(time(NULL));
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Uklad Sloneczny", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	glViewport(0, 0, width, height); // viewport
	glEnable(GL_DEPTH_TEST);
	// Input + Camera
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// Utwórz obiekt Vertex Shader
	//Shader shaderProgram("color_uniform.vert", "default.frag");
	Shader shaderProgram("src/color_uniform.vert", "src/lightning.frag");

	GLuint dxID = glGetUniformLocation(shaderProgram.ID, "dx");
	GLuint dyID = glGetUniformLocation(shaderProgram.ID, "dy");
	
	Planet Mercury("src/Textures/8k_mercury.jpg",0.069849839, 2.306994196, 20.6064265, 17.91523936, 26, 26, 26);
	Orbit mercuryOrbit(Mercury.radius_x, Mercury.radius_y);
	Planet Venus("src/Textures/8k_venus_surface.jpg",0.11001818, 1.972026594, 23.88428091, 23.73729699, 230, 230, 230);
	Orbit venusOrbit(Venus.radius_x,Venus.radius_y);
	Planet Earth("src/Textures/8k_earth_daymap.jpg", 0.112942463, 1.819645875, 26.68401649, 26.38816044, 47, 106, 105);
	Orbit earthOrbit(Earth.radius_x, Earth.radius_y);
	Planet Mars("src/Textures/8k_mars.jpg", 0.082492424, 1.636391694, 31.45597276, 29.4818427, 153, 61, 0);
	Orbit marsOrbit(Mars.radius_x, Mars.radius_y);
	Planet Jupiter("src/Textures/8k_jupiter.jpg", 0.378132252, 1.206464071, 46.7423658, 45.24557103, 176, 127, 53);
	Orbit jupiterOrbit(Jupiter.radius_x, Jupiter.radius_y);
	Planet Saturn("src/Textures/8k_saturn.jpg", 0.347182949, 1.038160767, 57.362621, 55.26047248, 176, 143, 54);
	Orbit saturnOrbit(Saturn.radius_x, Saturn.radius_y);
	Planet Uran("src/Textures/2k_uranus.jpg", 0.226092901, 0.869226987, 72.11247852, 70.05098327, 85, 128, 170);
	Orbit uranusOrbit(Uran.radius_x, Uran.radius_y);
	Planet Neptune("src/Textures/2k_neptune.jpg", 0.222548871, 0.774596669, 82.85278982, 82.24130783, 54, 104, 150);
	Orbit neptuneOrbit(Neptune.radius_x, Neptune.radius_y);

	Planet Sun("src/Textures/8k_sun.jpg", 1, 179830496, 1, 0, 239, 142, 56);

	Planet Ganymede("src/Textures/moon1.jpg", 0.072594766, 4.5, 5.114045609, 5.114045609, 179, 177, 176, Jupiter.center_x, Jupiter.center_y);
	Orbit ganymedeOrbit(5.114045609, 5.114045609, Jupiter.center_x, Jupiter.center_y);
	Planet Titan("src/Textures/moon2.jpg", 0.0717635, 3, 5.313292846, 5.313292846, 224, 194, 133, Saturn.center_x, Saturn.center_y);
	Orbit titanOrbit(5.313292846, 5.313292846, Saturn.center_x, Saturn.center_y);
	Planet Callisto("src/Textures/moon3.jpg", 0.06942622, 4, 6.171005793, 6.171005793, 160, 189, 183, Jupiter.center_x, Jupiter.center_y);
	Orbit callistoOrbit(6.171005793, 6.171005793, Jupiter.center_x, Jupiter.center_y);
	Planet Io("src/Textures/moon4.jpg", 0.06035727, 1, 3.74948141, 3.74948141, 204, 153, 0, Jupiter.center_x, Jupiter.center_y);
	Orbit ioOrbit(3.74948141, 3.74948141, Jupiter.center_x, Jupiter.center_y);
	Planet Moon("src/Textures/8k_moon.jpg", 0.058949131, 10, 3.701436014, 3.567086838, 254, 252, 215, Earth.center_x, Earth.center_y);
	Orbit moonOrbit(3.701436014, 3.567086838, Earth.center_x, Earth.center_y);
	Planet MarsMoon("src/Textures/8k_moon.jpg", 0.038949131, 15, 2.56, 2.56, 254, 252, 215, Earth.center_x, Earth.center_y);

	Orbit phobosOrbit(1.0, 1.0, Mars.radius_x, Mars.radius_y);
	Planet phobos("src/Textures/moon1.jpg", 0.042594766, 10.5, 1.0, 1.0, 179, 177, 176, Mars.center_x, Mars.center_y);
	Orbit deimosOrbit(0.5, 0.5, Mars.radius_x, Mars.radius_y);
	Planet deimos("src/Textures/8k_moon.jpg", 0.022594766, 7, 0.5, 0.5, 179, 177, 176, Mars.center_x, Mars.center_y);

	Orbit tytaniaOrbit(4.5, 4.5, Uran.radius_x, Uran.radius_y);
	Planet tytania("src/Textures/moon1.jpg", 0.032, 10, 4.5, 4.5, 179, 177, 176, Uran.center_x, Uran.center_y);
	Orbit mirandaOrbit(3.5, 3.5, Uran.radius_x, Uran.radius_y);
	Planet miranda("src/Textures/8k_moon.jpg", 0.022594766, 14, 3.5, 3.5, 179, 177, 176, Uran.center_x, Uran.center_y);
	Orbit arielOrbit(3.0, 3.0, Uran.radius_x, Uran.radius_y);
	Planet ariel("src/Textures/moon4.jpg", 0.027, 7, 3.0, 3.0, 179, 177, 176, Uran.center_x, Uran.center_y);
	Orbit oberonOrbit(2.5, 2.5, Uran.radius_x, Uran.radius_y);
	Planet oberon("src/Textures/moon3.jpg", 0.030, 9, 2.5, 2.5, 179, 177, 176, Uran.center_x, Uran.center_y);
	Orbit umrielOrbit(2.0, 2.0, Uran.radius_x, Uran.radius_y);
	Planet umbriel("src/Textures/moon1.jpg", 0.027, 5, 2.0, 2.0, 179, 177, 176, Uran.center_x, Uran.center_y);

	Orbit deathstarOrbit(4.5, 4.5, Earth.center_x, Earth.center_y);
	Planet deathstar("src/Textures/deathstar.jpg", 0.052594766, 7, 4.5, 4.5, 179, 177, 176, Earth.center_x, Earth.center_y);


	// ========= // œwiat³o i cienie

	Shader lightShader("src/light.vert", "src/light.frag");
	VAO lightVAO;
	lightVAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO lightVBO(Sun.vertices, sizeof(GLfloat) * Sun.n_vertices);
	EBO lightEBO(Sun.indices, sizeof(GLuint) * Sun.n_indices);
	// Links VBO attributes such as coordinates and colors to VAO
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);		//linkowanie konkretnych rzeczy z default.vert
	lightVAO.LinkAttrib(lightVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	lightVAO.LinkAttrib(lightVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	lightVAO.LinkAttrib(lightVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightVBO.Unbind();

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	lightPos = Sun.position;
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	// ========= // camera
	Camera camera1(width, height, glm::vec3(-2.2f, 2.2f, 1.0f), glm::vec3(0.6f, -0.6f, -0.5f));
	Camera camera2(width, height, glm::vec3(-2.2f, 2.2f, 1.0f), glm::vec3(0.6f, -0.6f, -0.5f));
	cam1 = &camera1; cam2 = &camera2;
	currentCamera = &camera1;
	// ========= // ustawienia + kolor t³a
	float bg_r = 0., bg_g = 0., bg_b = 0.25;
	glfwSwapInterval(1);

    // Inicjalizacja zmiennych czasu
    double time = 0.0;
    double deltaTime = 0.0;
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		auto frameStartTime = std::chrono::high_resolution_clock::now();

		// Aktualizacja czasu
		auto currentFrameTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = currentFrameTime - lastFrameTime;
		deltaTime = elapsed.count();
		lastFrameTime = currentFrameTime;

		// Aktualizacja czasu symulacji z przeskalowanym delta time
		time += deltaTime * timeScale;

		camera2.Position = Earth.position;
		camera2.Position.y = Earth.size / 5.;
		time += 0.001; //czas - u¿ywane w kolorze t³a, ruchu planet
		// logika koloru t³a
		bg_r = bg_g = cos(time/4.) / 26. ;
		bg_b = sin(time/4.) / 14. + .25;
		glClearColor(bg_r, bg_g, bg_b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyœæ buffor I nadaj mu kolor
		// shader
		shaderProgram.Activate();
		// input + ruch kamery
		currentCamera->Inputs(window); //obs³uga wejœcia do kamery
		currentCamera->updateMatrix(currentCamera->fov, 0.01f, 50.0f);

		shaderProgram.Activate();
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), currentCamera->Position.x, currentCamera->Position.y, currentCamera->Position.z);
		currentCamera->Matrix(shaderProgram, "camMatrix");

		auto frameEndTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> frameDuration = frameEndTime - frameStartTime;

		// Jeœli ramka zakoñczy³a siê zbyt wczeœnie, wstrzymaj
		double sleepTime = targetFrameTime - frameDuration.count();
		if (sleepTime > 0.0) {
			std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
		}

		// PLANETY - orbity
		drawIt(mercuryOrbit, time, shaderProgram.ID);
		drawIt(venusOrbit, time, shaderProgram.ID);
		drawIt(earthOrbit, time, shaderProgram.ID);
		drawIt(marsOrbit, time, shaderProgram.ID);
		drawIt(jupiterOrbit, time, shaderProgram.ID);
		drawIt(saturnOrbit, time, shaderProgram.ID);
		drawIt(uranusOrbit, time, shaderProgram.ID);
		drawIt(neptuneOrbit, time, shaderProgram.ID);
		
		// PLANETY
		drawIt(Mercury, time, shaderProgram.ID);
		drawIt(Venus, time, shaderProgram.ID);
		drawIt(Earth, time, shaderProgram.ID);
		drawIt(Mars, time, shaderProgram.ID);
		drawIt(Jupiter, time, shaderProgram.ID);
		drawIt(Saturn, time, shaderProgram.ID);
		drawIt(Uran, time, shaderProgram.ID);
		drawIt(Neptune, time, shaderProgram.ID);
		
		// KSIÊ¯YCE - orbity
		drawIt(ganymedeOrbit, time, shaderProgram.ID, Jupiter);
		drawIt(titanOrbit, time, shaderProgram.ID, Saturn);
		drawIt(callistoOrbit, time, shaderProgram.ID, Jupiter);
		drawIt(ioOrbit, time, shaderProgram.ID, Jupiter);
		drawIt(moonOrbit, time, shaderProgram.ID, Earth);
		//
		drawIt(phobosOrbit, time, shaderProgram.ID, Mars);
		drawIt(deimosOrbit, time, shaderProgram.ID, Mars);
		//
		drawIt(tytaniaOrbit, time, shaderProgram.ID, Uran);
		drawIt(mirandaOrbit, time, shaderProgram.ID, Uran);
		drawIt(arielOrbit, time, shaderProgram.ID, Uran);
		drawIt(oberonOrbit, time, shaderProgram.ID, Uran);
		drawIt(umrielOrbit, time, shaderProgram.ID, Uran);
		

		// KSIÊ¯YCE
		drawIt(Ganymede, time, shaderProgram.ID, Jupiter);
		drawIt(Titan, time, shaderProgram.ID, Saturn);
		drawIt(Callisto, time, shaderProgram.ID, Jupiter);
		drawIt(Io, time, shaderProgram.ID, Jupiter);
		drawIt(Moon, time, shaderProgram.ID, Earth);
		//
		drawIt(phobos, time, shaderProgram.ID, Mars);
		drawIt(deimos, time, shaderProgram.ID, Mars);
		//
		drawIt(miranda, time, shaderProgram.ID, Uran);
		drawIt(ariel, time, shaderProgram.ID, Uran);
		drawIt(umbriel, time, shaderProgram.ID, Uran);
		drawIt(tytania, time, shaderProgram.ID, Uran);
		drawIt(oberon, time, shaderProgram.ID, Uran);

		drawIt(deathstarOrbit, time, shaderProgram.ID, Earth);
		drawIt(deathstar, time, shaderProgram.ID, Earth);

		// S£OÑCE
		drawSun(Sun, lightShader, lightVAO, *currentCamera);
		// ========= //
		// Odœwie¿ widok
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window); //Delete window before ending the program
	glfwTerminate(); //Terminate GLFW before ending the program 
	return 0;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	currentCamera->fov -= (float)yoffset;
	if (currentCamera->fov < 1.0f)
		currentCamera->fov = 1.0f;
	if (currentCamera->fov > 90.0f)
		currentCamera->fov = 90.0f;
}