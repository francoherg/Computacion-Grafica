#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Debug.hpp"
#include "Shaders.hpp"
#include "Car.hpp"

#define VERSION 20220901.2

// models and settings
bool wireframe = false, play = false, top_view = true;

// extra callbacks (atajos de teclado para cambiar de modo y camara)
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);

// función para sensar joystick o teclado (si no hay joystick) y definir las 
// entradas para el control del auto
std::tuple<float,float,bool> getInput(GLFWwindow *window);

// matrices que definen la camara
glm::mat4 projection_matrix, view_matrix;

// struct para guardar cada "parte" del auto
struct Part {
	std::string name;
	bool show;
	std::vector<Model> models;
};

// función para renderizar cada "parte" del auto
void renderPart(const Car &car, const std::vector<Model> &v_models, const glm::mat4 &matrix) {
	static Shader shader("shaders/phong");
	
	// select a shader
	for(const Model &model : v_models) {
		shader.use();
		
		// matrixes
		if (play) {
			/// @todo: modificar una de estas matrices para mover todo el auto (todas
			///        las partes) a la posición (y orientación) que le corresponde en la pista
			shader.setMatrixes(glm::translate(glm::mat4(1.f),glm::vec3(car.x,0,car.y))*glm::rotate(glm::mat4(1.f),-car.ang,glm::vec3(0,1,0))*matrix,view_matrix,projection_matrix);
		} else {
			glm::mat4 model_matrix = glm::rotate(glm::mat4(1.f),view_angle,glm::vec3{1.f,0.f,0.f}) *
						             glm::rotate(glm::mat4(1.f),model_angle,glm::vec3{0.f,1.f,0.f}) *
			                         matrix;
			shader.setMatrixes(model_matrix,view_matrix,projection_matrix);
		}
		
		// setup light and material
		shader.setLight(glm::vec4{20.f,-20.f,-40.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.35f);
		shader.setMaterial(model.material);
		
		// send geometry
		shader.setBuffers(model.buffers);
		glPolygonMode(GL_FRONT_AND_BACK,(wireframe and (not play))?GL_LINE:GL_FILL);
		model.buffers.draw();
	}
}

// función que renderiza la pista
void RenderTrack() {
	static Model track = Model::loadSingle("track",Model::fDontFit);
	static Shader shader("shaders/texture");
	shader.use();
	shader.setMatrixes(glm::mat4(1.f),view_matrix,projection_matrix);
	shader.setMaterial(track.material);
	shader.setBuffers(track.buffers);
	track.texture.bind();
	static float aniso = -1.0f;
	if (aniso<0) glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso); 
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	track.buffers.draw();
}

// función que actualiza las matrices que definen la cámara
void setViewAndProjectionMatrixes(const Car &car) {
	projection_matrix = glm::perspective( glm::radians(view_fov), float(win_width)/float(win_height), 0.1f, 100.f );
	if (play) {
		if (top_view) {
			/// @todo: modificar el look at para que en esta vista el auto siempre apunte hacia arriba
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			view_matrix = glm::lookAt( pos_auto+glm::vec3{0.f,30.f,0.f}, pos_auto, glm::vec3{cos(car.ang),1.f,sin(car.ang)} );
		} else {
			/// @todo: definir view_matrix de modo que la camara persiga al auto desde "atras"
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			view_matrix = glm::lookAt( pos_auto+glm::vec3{-10*cos(car.ang),5.f,-10*sin(car.ang)}, pos_auto, glm::vec3(0.f,1.f,0.f));
		}
	} else {
		view_matrix = glm::lookAt( glm::vec3{0.f,0.f,3.f}, view_target, glm::vec3{0.f,1.f,0.f} );
	}
}

// función que rendiriza todo el auto, parte por parte
void renderCar(const Car &car, const std::vector<Part> &parts) {
	const Part &axis = parts[0], &body = parts[1], &wheel = parts[2],
	           &fwing = parts[3], &rwing = parts[4], &helmet = parts[5];
	
	/// @todo: armar la matriz de transformación de cada parte para construir el auto
	
	if (body.show or play) {
		renderPart(car,body.models, glm::translate(glm::mat4( 1.0f ),glm::vec3( 0.f, 0.25f, 0.f )));
	}
	//																		position                   scale
	/*glm::mat4 r1= glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( -0.85f, 0.25f, -0.45f )), 0.2f*(glm::vec3( 1.f, 1.f, 1.f )));
	
	glm::mat4 r2= glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( 0.5f, 0.25f, 0.35f )), 0.2f*(glm::vec3( 1.f, 1.f, 1.f )));
	
	glm::mat4 r3= glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( 0.5f, 0.25f, -0.35f )), 0.2f*(glm::vec3( 1.f, 1.f, 1.f )));
	
	glm::mat4 r4= glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( -0.85f, 0.25f, 0.45f )), 0.2f*(glm::vec3( 1.f, 1.f, 1.f )));*/
	if (wheel.show or play) {
		renderPart(car,wheel.models,glm::rotate(glm::mat4( 0.2f,0,0,0.f,
																	0, 0.2f, 0.f , 0.f,
																	0,	0,	0.2f, 0,
																	-0.85,0.25f,-0.45,1),-car.rang2,glm::vec3{0.f,0.f,1.f}));
		renderPart(car,wheel.models, glm::translate(glm::mat4(1.f),glm::vec3(0.5f, 0.25f, -0.35f))*glm::rotate(glm::mat4(1.f),-car.rang1,glm::vec3{0.f,1.f,0.f})*glm::rotate(glm::mat4(1.f),-car.rang2,glm::vec3{0.f,0.f,1.f})*glm::mat4( 0.2f,0,0,0.f,
											   0, 0.2f, 0.f , 0.f,
											   0,	0,	0.2f, 0,
											   0.f, 0.f, -0.f,1));
		renderPart(car,wheel.models, glm::translate(glm::mat4(1.f),glm::vec3(0.5f, 0.25f, 0.35f))*glm::rotate(glm::mat4(1.f),-car.rang1,glm::vec3{0.f,1.f,0.f})*glm::rotate(glm::mat4(1.f),-car.rang2,glm::vec3{0.f,0.f,1.f})*glm::mat4( 0.2f,0,0,0.f,
											   0, 0.2f, 0.f , 0.f,
											   0,	0,	-0.2f, 0,
											   0.f, 0.f, 0.f,1));
		renderPart(car,wheel.models,(glm::mat4( 0.2f,0,0,0.f,
														   0, 0.2f, 0.f , 0.f,
														   0,	0,	-0.2f, 0,
														   -0.85,0.25f,0.45,1)) *glm::mat4(cos(car.rang2),-sin(car.rang2),0.f,0.f,
																							sin(car.rang2),cos(car.rang2),0.f,0.f,
																							0.f,0.f,1.f,0.f,
																							0.f,0.f,0.f,1.f) );
	}
	
	if (fwing.show or play) {
		renderPart(car,fwing.models, glm::rotate(glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( 0.9f, 0.15f, 0.f )),0.35f*(glm::vec3( 1.f, 1.f, 1.f ))), glm::radians(-90.f), glm::vec3( 0.f, 1.f, 0.f )));
	}
	
	if (rwing.show or play) {
		float scl = 0.30f;
		renderPart(car,rwing.models, glm::rotate(glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( -0.85f, 0.45f, 0.f )),0.30f*(glm::vec3( 1.f, 1.f, 1.f ))), glm::radians(-90.f), glm::vec3( 0.f, 1.f, 0.f )));
	}
	
	if (helmet.show or play) {//																			position 				scale 									rotation
		renderPart(car,helmet.models, glm::rotate(glm::scale(glm::translate(glm::mat4( 1.0f ),glm::vec3( 0.f, 0.35f, 0.f )),0.075f*(glm::vec3( 1.f, 1.f, 1.f ))), glm::radians(-90.f), glm::vec3( 0.f, 1.f, 0.f )));
	}
	
	if (axis.show and (not play)) renderPart(car,axis.models,glm::mat4(1.f));
}

// main: crea la ventana, carga los modelos e implementa el bucle principal
int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo",true);
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.4f,0.4f,0.8f,1.f);
	
	// main loop
	std::vector<Part> parts; parts.reserve(8);
	parts.push_back({"axis",      true,Model::load("axis",      Model::fDontFit)});
	parts.push_back({"body",      true,Model::load("body",      Model::fDontFit)});
	parts.push_back({"wheels",    true,Model::load("wheel",     Model::fDontFit)});
	parts.push_back({"front wing",true,Model::load("front_wing",Model::fDontFit)});
	parts.push_back({"rear wing", true,Model::load("rear_wing", Model::fDontFit)});
	parts.push_back({"driver",    true,Model::load("driver",    Model::fDontFit)});
	
	Car car(+66,-35,1.38);
	Track track("mapa.png",100,100);
	
	FrameTimer ftime;
	double accum_dt = 0.0;
	double lap_time = 0.0;
	double last_lap = 0.0;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		// actualizar las pos del auto y de la camara
		double elapsed_time = ftime.newFrame();
		accum_dt += elapsed_time;
		if (play) lap_time += elapsed_time;
		auto in = getInput(window);
		while (accum_dt>1.0/60.0) { 
			car.Move(track,std::get<0>(in),std::get<1>(in),std::get<2>(in));
			if (track.isFinishLine(car.x,car.y) and lap_time>5) {
				last_lap = lap_time; lap_time = 0.0;
			}
			accum_dt-=1.0/60.0;
			setViewAndProjectionMatrixes(car);
		}
		
		// setear matrices y renderizar
		if (play) RenderTrack();
		renderCar(car,parts);
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			ImGui::Checkbox("Play (P)",&play);
			if (play) {
				ImGui::LabelText("","Lap Time: %f s",lap_time<5 ? last_lap : lap_time);
				ImGui::Checkbox("Top View (T)",&top_view);
			} else {
				ImGui::Checkbox("Wireframe (W)",&wireframe);
				if (ImGui::TreeNode("Parts")) {
					for(Part &p : parts)
						ImGui::Checkbox(p.name.c_str(),&p.show);
					ImGui::TreePop();
				}
			}
			if (ImGui::TreeNode("car")) {
				ImGui::LabelText("","x: %f",car.x);
				ImGui::LabelText("","y: %f",car.y);
				ImGui::LabelText("","ang: %f",car.ang);
				ImGui::LabelText("","rang1: %f",car.rang1);
				ImGui::LabelText("","rang2: %f",car.rang2);
				ImGui::TreePop();
			}
		});
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'W': wireframe = !wireframe; break;
		case 'T': if (!play) play = true; else top_view = !top_view; break;
		case 'P': play = !play; break;
		}
	}
}

std::tuple<float,float,bool> getInput(GLFWwindow *window) {
	float acel = 0.f, dir = 0.f; bool analog = false;
	if (glfwGetKey(window,GLFW_KEY_UP)   ==GLFW_PRESS) acel += 1.f;
	if (glfwGetKey(window,GLFW_KEY_DOWN) ==GLFW_PRESS) acel -= 1.f;
	if (glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) dir += 1.f;
	if (glfwGetKey(window,GLFW_KEY_LEFT) ==GLFW_PRESS) dir -= 1.f;
	
	int count;
	const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
	if (count) { dir = axes[0]; acel = -axes[1]; analog = true; }
	return std::make_tuple(acel,dir,analog);
}
