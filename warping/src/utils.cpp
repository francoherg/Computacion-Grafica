#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "utils.hpp"
#include "Debug.hpp"
#include <glm/geometric.hpp>
#include <glm/ext/vector_float2.hpp>
using namespace glm;

BoundingBox::BoundingBox(glm::vec3 &p1, glm::vec3 &p2) 
	: pmin({std::min(p1.x,p2.x),std::min(p1.y,p2.y),std::min(p1.z,p2.z)}),
      pmax({std::max(p1.x,p2.x),std::max(p1.y,p2.y),std::max(p1.z,p2.z)}) 
{

}
	
bool BoundingBox::contiene(glm::vec3 &p) const {
	return p.x>=pmin.x && p.x<=pmax.x &&
		p.y>=pmin.y && p.y<=pmax.y &&
		p.z>=pmin.z && p.z<=pmax.z;
}

Pesos calcularPesos(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 &x) {

	glm::vec3 at=cross((x1-x0),(x2-x0));
	//cuando x esta sobre x1, a1 = a total entonces a1/at = 1
	
	glm::vec3 a0= cross( x1-x,x2-x);//areas cuadrilateros
	glm::vec3 a1= cross( x2-x,x0-x);
	glm::vec3 a2= cross( x0-x,x1-x);
	
	float alpha0= (dot(a0,at)/dot(at,at));// proporcion area cuadrilatero formado por (x1-x, x2-x ) sobre el area total del triangulo
	float alpha1= (dot(a1,at)/dot(at,at));
	float alpha2= (dot(a2,at)/dot(at,at));
	//cg_error("debe implementar la funcion calcularPesos (utils.cpp)");
	return {alpha0,alpha1,alpha2};
}
