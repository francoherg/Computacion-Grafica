#include <forward_list>
#include <iostream>
#include <GL/gl.h>
#include <cmath>
#include "RasterAlgs.hpp"
#include <algorithm>
using namespace std;

void drawSegment(paintPixelFunction paintPixel, glm::vec2 p0, glm::vec2 p1) {

	float dx=p1.x- p0.x;
	float dy=p1.y - p0.y;
	
	if(fabs(dx)>=fabs(dy)){
		if(dx<0){
			swap(p0,p1);
		}
		dx=p1.x- p0.x;
		dy=p1.y - p0.y;
		float x=round(p0.x), y=round(p0.y);
		paintPixel(glm::vec2(x,y)); 
		float m=dy/dx;
		while((++x)<p1.x) {y+=m; paintPixel(glm::vec2(x,round(y)));}
	}
	else{
		if(dy<0){
			swap(p0,p1);
		}
		dx=p1.x - p0.x;
		dy=p1.y - p0.y;
		float x=round(p0.x),y=round(p0.y);
		paintPixel(glm::vec2(x,y)); 
		float m=dx/dy; 
		while((++y)<p1.y) {x+=m; paintPixel(glm::vec2(round(x),y));}
	}
}


void drawCurve(paintPixelFunction paintPixel, curveEvalFunction evalCurve) {
	auto t= 0.f;
	auto dt=0.f;


	while(t<1.f) {  //t<t1

		auto r=evalCurve(t);

		if(fabs(r.d.x)<fabs(r.d.y)){
		   dt=1/fabs(r.d.y);}
		else{
			dt=1/fabs(r.d.x);
		}		
		auto next_p=evalCurve(t+dt).p; //guardo el punto siguiente 
		auto diff=round(next_p) - round(r.p);
		while(fabs(diff.x)>1.f || fabs(diff.y)>1.f){	//si el pixel a pintar se alejo mas de lo que deberia achico dt hasta q sea correcto
			dt=dt*0.5f;
			next_p=evalCurve(t+dt).p;
			diff=round(next_p) - round(r.p);
		}
		t+=dt;
		if(round(r.p)!=round(next_p)){
			paintPixel(round(r.p));
			
		}
	}
		
}






