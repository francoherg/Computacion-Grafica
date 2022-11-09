#include <algorithm>
#include <stdexcept>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ObjMesh.hpp"
#include "Shaders.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Model.hpp"

#define VERSION 20221019
#include <cmath>
using namespace std;

std::vector<glm::vec2> generateTextureCoordinatesForBottle(const std::vector<glm::vec3> &v) {
	std::vector<glm::vec2> v_mapeado(v.size());
	float ang, s, t;
	
	for(size_t i=0;i<v.size();i++) { 
		
		ang = atan2(v[i].x,v[i].z);//ang va de 0 a 2pi
		t = (ang/(3.14f));// t va de 0 a 1, de 0.5 a 1.5
		s =0.4+ v[i].y*2.4; //multiplico y para "achicar" el plano y le sumo 0.4 para moverlo mas abajo
		v_mapeado[i] = glm::vec2(t,s);
		
	}
	//error esta en la rasterizacion
	return v_mapeado;
}

std::vector<glm::vec2> generateTextureCoordinatesForLid(const std::vector<glm::vec3> &v) {
	/// @todo: generar el vector de coordenadas de texturas para los vertices de la tapa
	float s, t;
	
	std::vector<glm::vec2> v_mapeado(v.size());
	for(size_t i=0;i<v.size();i++) { 
		//el plano ax+by+cz+d=0 se define mediante coordenadas {a,b,c,d}.
		/* Los primeros tres valores especifican un vector normal al plano y el 
		cuarto es función lineal de la distancia del plano al origen*/
//		auto planoS= glm::vec4(0.f,0.f,1.f,v[i].z);
//		auto planoT= glm::vec4(1.f,0.f,0.f,v[i].x);
//		auto puntoActual =  glm::vec4(v[i].x,v[i].y,v[i].z,5); //w =5
//		s = glm::dot(planoS,puntoActual)+0.5f; //La coordenada x se puede ver como la distancia a un plano S vertical por el origen (plano yz)
//		t =  glm::dot(puntoActual,planoT)+0.5f;//la coordenada y es la distancia a un plano T horizontal (xz);
		float w=5;
		//s(P) = wxas+ wybs+ wzcs+ wd
		s= w*v[i].z +0.5;
		t=w*v[i].x +0.5;
		v_mapeado[i] = glm::vec2 (s,t);
		
	}
	return (v_mapeado);
	
}

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Texturas");
	setCommonCallbacks(window);
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glEnable(GL_BLEND); glad_glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.6f,0.6f,0.8f,1.f);
	Shader shader("shaders/texture");
	
	// load model and assign texture
	auto models = Model::load("among us",Model::fKeepGeometry);
	Model &bottle = models[0], &lid = models[1];
	bottle.buffers.updateTexCoords(generateTextureCoordinatesForBottle(bottle.geometry.positions),true);
	bottle.texture = Texture("models/label.png",true,false);
	lid.buffers.updateTexCoords(generateTextureCoordinatesForLid(lid.geometry.positions),true);
	lid.texture = Texture("models/lid.png",false,false);
	
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		setMatrixes(shader);
		shader.setLight(glm::vec4{2.f,-2.f,-4.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.15f);
		for(Model &mod : models) {
			mod.texture.bind();
			shader.setMaterial(mod.material);
			shader.setBuffers(mod.buffers);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			mod.buffers.draw();
		}
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

