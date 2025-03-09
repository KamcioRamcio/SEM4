/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"


float speed_x = 0;//[radians/s]
float speed_y = 0;//[radians/s]
float speed = 1.14;//[radians/s]

//Error processing callback procedure
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


//Initialization code procedure
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	//************Place any code here that needs to be executed once, at the program start************
}

//Release resources allocated by the program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
	//************Place any code here that needs to be executed once, after the main loop ends************
}
static Models::Sphere planet1(0.2,36,36);
static Models::Sphere sun(0.5,36,36);
static Models::Sphere moon1(0.1,36,36);
//Drawing procedure
void drawScene(GLFWwindow* window, float angle) {
	//************Place any code here that draws something inside the window******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing for proper 3D rendering
	glEnable(GL_DEPTH_TEST);

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);
	glm::mat4 V = glm::lookAt(
		glm::vec3(0.0f, 0.0f, -5.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	spLambert->use();
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));

	// Draw the sun at the center
	glm::mat4 M = glm::mat4(1.0f);
	// Sun rotates on its axis
	M = glm::rotate(M, angle * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(spLambert->u("color"), 1.0f, 0.8f, 0.0f, 1.0f); // Yellow sun
	sun.drawSolid();

	// Draw the planet orbiting the sun
	glm::mat4 M_planet_orbit = glm::mat4(1.0f);
	M_planet_orbit = glm::rotate(M_planet_orbit, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Orbit around sun
	M_planet_orbit = glm::translate(M_planet_orbit, glm::vec3(1.5f, 0.0f, 0.0f)); // Distance from sun

	// Planet rotation on its own axis
	glm::mat4 M_planet = M_planet_orbit;
	M_planet = glm::rotate(M_planet, angle * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Planet's rotation

	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M_planet));
	glUniform4f(spLambert->u("color"), 0.0f, 0.0f, 1.0f, 1.0f); // Blue planet
	planet1.drawSolid();

	// Draw the moon orbiting the planet
	glm::mat4 M_moon = M_planet_orbit; // Start from the planet's orbit position
	M_moon = glm::rotate(M_moon, angle * 3.0f, glm::vec3(0.0f, 1.0f, 0.0f)); // Orbit around planet
	M_moon = glm::translate(M_moon, glm::vec3(0.4f, 0.0f, 0.0f)); // Distance from planet
	M_moon = glm::rotate(M_moon, angle * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f)); // Moon's rotation

	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(M_moon));
	glUniform4f(spLambert->u("color"), 0.8f, 0.8f, 0.8f, 1.0f); // Gray moon
	moon1.drawSolid();

	glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window; //Pointer to object that represents the application window

	glfwSetErrorCallback(error_callback);//Register error processing callback procedure

	if (!glfwInit()) { //Initialize GLFW library
		fprintf(stderr, "Can't initialize GLFW.\n");
		exit(EXIT_FAILURE); 
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it. 

	if (!window) //If no window is opened then close the program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
	glfwSwapInterval(1); //During vsync wait for the first refresh

	GLenum err;
	if ((err=glewInit()) != GLEW_OK) { //Initialize GLEW library
		fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Call initialization procedure

	//Main application loop
	float angle=0;
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window)) {
		angle+=speed*glfwGetTime();
		glfwSetTime(0);
		drawScene(window,angle);
		glfwPollEvents();
	}
	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Delete OpenGL context and the window.
	glfwTerminate(); //Free GLFW resources
	exit(EXIT_SUCCESS);
}
