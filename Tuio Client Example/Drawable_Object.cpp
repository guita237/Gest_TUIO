#include "Drawable_Object.h"
#include "Drawable_Object.h"

Drawable_Object::Drawable_Object(float x_pos, float y_pos, float box_bound_Up_Left_x, float box_bound_Up_Left_y, float box_bound_Down_Right_x, float box_bound_Down_Right_y, std::string Shape)
{
	this->x_pos = x_pos;
	this->y_pos = y_pos;

	this->box_bound_Down_Right_x = box_bound_Down_Right_x + x_pos;
	this->box_bound_Up_Left_x = box_bound_Up_Left_x + x_pos;
	this->box_bound_Down_Right_y = box_bound_Down_Right_y + y_pos;
	this->box_bound_Up_Left_y = box_bound_Up_Left_y + y_pos;



	this->length = std::abs(this->box_bound_Down_Right_x - this->box_bound_Up_Left_x);
	this->height = std::abs(this->box_bound_Down_Right_y - this->box_bound_Up_Left_y);
	this->rotation = 0.0;
	this->selected = false;


	this->Shape = Shape;
}

void Drawable_Object::select()
{
	this->selected = true;
}
void Drawable_Object::deselect()
{
	this->selected = false;
	this->scaleDistance = 0.0;
}

void Drawable_Object::translate(float x_pos, float y_pos)
{
	scaleDistance = 0.0;

	this->box_bound_Up_Left_x = x_pos - (this->length / 2.0);
	this->box_bound_Up_Left_y = y_pos - (this->height / 2.0);
	this->box_bound_Down_Right_x = x_pos + (this->length / 2.0);
	this->box_bound_Down_Right_y = y_pos + (this->height / 2.0);

	this->x_pos = x_pos;
	this->y_pos = y_pos;
}

void Drawable_Object::scale(float pX, float pY, float pX_2, float pY_2)
{
	float dx = pX - pX_2;
	float dy = pY - pY_2;
	float distance = sqrt(pow(dx, 2) + pow(dy, 2));
	if (scaleDistance == 0.0)
	{
		scaleDistance = distance;
		scale_height = height;
		scale_length = length;
	}
	scaleFactor = distance / scaleDistance;
	length = scale_length * scaleFactor * scaleFactor;
	height = scale_height * scaleFactor * scaleFactor;

	box_bound_Up_Left_x = (x_pos - length * 0.5);
	box_bound_Up_Left_y = (y_pos - length * 0.5);
	box_bound_Down_Right_x = (x_pos + length * 0.5);
	box_bound_Down_Right_y = (y_pos + length * 0.5);

}

void Drawable_Object::rotate(float angle)
{

	//system("CLS");

	rotation = angle;


}

bool Drawable_Object::collision_detection(float finger_x, float finger_y)
{
	float x = 0.0;
	float y = 0.0;
	float x2 = 0.0;
	float y2 = 0.0;

	finger_x = finger_x + 20.0;
	finger_y = finger_y + 20.0;



	x = ((finger_x)-(this->box_bound_Up_Left_x + 20.0));
	y = (finger_y - (this->box_bound_Up_Left_y + 20.0));

	x2 = (finger_x - (this->box_bound_Down_Right_x + 20.0));
	y2 = (finger_y - (this->box_bound_Down_Right_y + 20.0));
	//system("CLS");
	//std::cout << finger_y - 20.0;

	if (this->Shape == "Triangle")
	{
		if (x > 0 && x2 < 0 && y > 0 && y2 < 0)
		{
			return true;
		}
	}
	if (this->Shape == "Rectangle")
	{
		if (x > 0 && x2 < 0 && y > 0 && y2 < 0)
		{
			return true;
		}
		//if (x < 1.0 && x > 0.0 && y > 0.0 && y < 1.0) return true;
	}

	return false;
}

int Drawable_Object::compare(const Drawable_Object* cmp) {
	if (this->box_bound_Down_Right_x != cmp->box_bound_Down_Right_x) return 0;
	else if (this->box_bound_Down_Right_y != cmp->box_bound_Down_Right_y) return 0;
	else if (this->box_bound_Up_Left_x != cmp->box_bound_Up_Left_x) return 0;
	else if (this->box_bound_Up_Left_y != cmp->box_bound_Up_Left_y) return 0;
	else return 1;
}
