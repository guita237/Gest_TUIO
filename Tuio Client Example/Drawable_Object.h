#pragma once
#include <string>
#include <iostream>
#include <process.h>
#include <math.h>

#include "TuioClient.h"
#include "TuioListener.h"

#include "GeometricRecognizer.h"

#include <GL/glut.h>
class Drawable_Object
{
public:
	std::string Shape;
	float x_pos;
	float y_pos;

	float box_bound_Up_Left_x;
	float box_bound_Up_Left_y;
	float box_bound_Down_Right_x;
	float box_bound_Down_Right_y;

	float length;
	float height;

	float scale_length;
	float scale_height;

	float scaleDistance = 0.0;
	float scaleFactor = 1.0;


	float rotation;

	bool selected;


	void select();
	void deselect();
	Drawable_Object(float x_pos, float y_pos, float box_bound_Up_Left_x, float box_bound_Up_Left_y, float box_bound_Down_Right_x, float box_bound_Down_Right_y, std::string Shape);
	void scale(float pX, float pY, float pX_2, float pY_2);
	bool collision_detection(float finger_x, float finger_y);
	void rotate(float angle);
	void translate(float x_pos, float y_pos);
	int compare(const Drawable_Object*);


};

