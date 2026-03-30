#include <iostream>
#include <process.h>
#include <math.h>

#include "TuioClient.h"
#include "TuioListener.h"

#include "GeometricRecognizer.h"

#include <GL/glut.h>

#include "Drawable_Object.h"

using namespace TUIO;

TUIO::TuioClient* tuioClient; // global tuioClient for testing
DollarRecognizer::GeometricRecognizer* $1 = new DollarRecognizer::GeometricRecognizer();


bool translate = false;
bool rotation = false;
float object_rotation = 0.0;
float original_angle = 0.0;


float i_x1 = 0.0;
float i_y1 = 0.0;
float i_x2 = 0.0;
float i_y2 = 0.0;

float n;
float rotation_y_base = 0.0;
bool nothing_selected = true;

Drawable_Object* object_one = new Drawable_Object(0.5, 0.5, 0.25, 0.25, 0.5, 0.5, "Rectangle");
Drawable_Object* object_two = new Drawable_Object(-0.2, -0.2, 0.35, 0.35, 0.7, 0.7, "Triangle");
Drawable_Object* the_selected = NULL;
std::list<Drawable_Object*> object_List = { object_one, object_two };
std::list<TuioCursor*> cursorList;

class Client : public TuioListener {
	void Client::addTuioObject(TuioObject* tobj) {};
	void Client::updateTuioObject(TuioObject* tobj) {};
	void Client::removeTuioObject(TuioObject* tobj) {};

	void Client::addTuioCursor(TuioCursor* tcur)
	{
		cursorList.push_back(tcur);
		std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};
	void Client::updateTuioCursor(TuioCursor* tcur) {};

	void Client::removeTuioCursor(TuioCursor* tcur)
	{
		
		// calculate discrete gestures with $1 recognizer
		std::list<TuioPoint> path = tcur->getPath();

		DollarRecognizer::Path2D path_$1;
		DollarRecognizer::Point2D point_$1;

		for (std::list<TuioPoint>::iterator pathIter = path.begin(); pathIter != path.end(); ++pathIter)
		{
			// convert path to something the $1 recognizer understands
			point_$1.x = pathIter->getX();
			point_$1.y = pathIter->getY();

			path_$1.push_back(point_$1);
		}

		DollarRecognizer::RecognitionResult result = $1->recognize(path_$1);

		std::cout << result.name << ", gesture score: " << result.score << "\n";


		//New objects through gesture
		if (cursorList.size() == 1 && nothing_selected == true) {
			std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin();
			float pX = ((*cursorListIter)->getX() - 0.5) * 2.0;
			float pY = ((*cursorListIter)->getY() - 0.5) * 2.0 * -1.0;
			if (result.name == "Rectangle") {
				Drawable_Object* newRect = new Drawable_Object(pX - 0.35, pY - 0.35, 0.25, 0.25, 0.5, 0.5, "Rectangle");
				object_List.push_back(newRect);
			}
			if (result.name == "Triangle") {
				Drawable_Object* newTri = new Drawable_Object(pX - 0.55, pY - 0.55, 0.35, 0.35, 0.7, 0.7, "Triangle");
				object_List.push_back(newTri);
			}
		}

		//Delete object through gesture
		//Keep first finger on object to delete. Draw same shape with other finger
		if (cursorList.size() == 2 && nothing_selected == false) {
			if (the_selected != NULL) {
				if (the_selected->Shape == "Rectangle" && result.name == "Rectangle") {
					for (auto cursorListIter = object_List.begin(); cursorListIter != object_List.end(); ++cursorListIter)
					{
						if ((*cursorListIter)->compare(the_selected)) {
							the_selected = NULL;
							object_List.erase(cursorListIter);
							break;
						}
					}
				}
			}
			if (the_selected != NULL) {
				if (the_selected->Shape == "Triangle" && result.name == "Triangle") {
					for (auto cursorListIter = object_List.begin(); cursorListIter != object_List.end(); ++cursorListIter)
					{
						if ((*cursorListIter)->compare(the_selected)) {
							the_selected = NULL;
							object_List.erase(cursorListIter);
							break;
						}
					}
				}
			}
		}


	};

	void  Client::refresh(TuioTime frameTime) {};
};

void draw()
{
	float pX = 0.0;
	float pY = 0.0;

	float pX_2 = 0.0;
	float pY_2 = 0.0;

	// get the actual cursor list from the tuioClient
	cursorList = tuioClient->getTuioCursors();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_POINTS);

	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		float pX = ((*cursorListIter)->getX() - 0.5) * 2.0;
		float pY = ((*cursorListIter)->getY() - 0.5) * 2.0 * -1.0;
		glVertex2f(pX, pY); // mirrored y-axis because of camera and coordinates converted to openGL --> glOrtho(-1,1,-1,1,0,1) with (-0.5*2)
	}
	glEnd();

	if (cursorList.size() == 0)
	{
		if (object_List.size() > 0)
		{
			auto the_pointer = object_List.begin();
			for (int i = 0; i < object_List.size(); i++)
			{

				(*the_pointer)->deselect();
				the_pointer++;
			};

		}
		nothing_selected = true;
		the_selected = NULL;
	};


	if (cursorList.size() == 1)
	{
		pX = ((*cursorList.begin())->getX() - 0.5) * 2.0;
		pY = ((*cursorList.begin())->getY() - 0.5) * 2.0 * -1.0;
	}
	if (cursorList.size() == 2)
	{
		pX = ((*cursorList.begin())->getX() - 0.5) * 2.0;
		pY = ((*cursorList.begin())->getY() - 0.5) * 2.0 * -1.0;

		auto second_cursor = cursorList.begin();
		second_cursor++;

		pX_2 = ((*second_cursor)->getX() - 0.5) * 2.0;
		pY_2 = ((*second_cursor)->getY() - 0.5) * 2.0 * -1.0;
	}
	if (object_List.size() > 0)
	{
		auto the_pointer = object_List.begin();
		for (int i = 0; i < object_List.size(); i++)
		{
			if ((*the_pointer)->collision_detection(pX, pY) && nothing_selected == true)
			{
				(*the_pointer)->select();
				the_selected = *the_pointer;
				nothing_selected = false;
			};
			the_pointer++;
		}
	}
	if (cursorList.size() == 1 && (the_selected != NULL)) {

		if (the_selected != NULL)
		{
			the_selected->translate(pX, pY);
			rotation = false;
		}


	}



	if (cursorList.size() == 2 && (the_selected != NULL))
	{
		if (rotation == false)
		{
			rotation = true;

			auto second_pointer = cursorList.begin();
			second_pointer++;

			float pX_1 = ((*cursorList.begin())->getX() - 0.5) * 2;
			float pY_1 = ((*cursorList.begin())->getY() - 0.5) * 2 * -1;

			float pX_2 = ((*(second_pointer))->getX() - 0.5) * 2;
			float pY_2 = ((*(second_pointer))->getY() - 0.5) * 2 * -1;

			// set original angle
			n = (pY_2 - pY_1) / (pX_2 - pX_1);
			rotation_y_base = pY_2 - pY_1;





			original_angle = std::atan(n);
		}

		auto second_pointer = cursorList.begin();
		second_pointer++;
		// calculate new rotation
		float i_x1 = ((*cursorList.begin())->getX() - 0.5) * 2;
		float i_y1 = ((*cursorList.begin())->getY() - 0.5) * 2 * -1;

		float i_x2 = ((*(second_pointer))->getX() - 0.5) * 2;
		float i_y2 = ((*(second_pointer))->getY() - 0.5) * 2 * -1;


		float m = (i_y2 - i_y1) / (i_x2 - i_x1);



		object_rotation = std::atan(m) - original_angle;
		object_rotation = object_rotation * 180 / M_PI;

		if (n * m < 0) { object_rotation = object_rotation + 180; }

		if ((rotation_y_base * (i_y2 - i_y1)) <= 0) { object_rotation = object_rotation + 180; }

		the_selected->rotate(object_rotation);

		the_selected->scale(pX, pY, pX_2, pY_2);

	}

	glEnd();


	if (object_List.size() > 0)
	{
		auto the_end_pointer = object_List.end();
		the_end_pointer--;
		if (the_selected != (*the_end_pointer) && the_selected != NULL)
		{
			object_List.remove(the_selected);
			object_List.push_back(the_selected);
		}

		auto the_pointer = object_List.begin();

		for (int i = 0; i < object_List.size(); i++)
		{
			//glBegin(GL_TRIANGLES);
			// Chose the color based on if selected or not
			if ((*the_pointer)->selected)glColor3f(0.0f, 0.0f, 1.0f);
			if (!(*the_pointer)->selected)glColor3f(1.0f, 0.0f, 0.0f);
			// encapsulate
			glPushMatrix();
			// Rotate
			glTranslatef((*the_pointer)->x_pos, (*the_pointer)->y_pos, 0.0);
			glRotatef((*the_pointer)->rotation, 0.0, 0.0, 1.0);
			glTranslatef((*the_pointer)->x_pos * -1, (*the_pointer)->y_pos * -1, 0.0);


			if ((*the_pointer)->Shape == "Rectangle")
				glRectd
				(
					(*the_pointer)->box_bound_Up_Left_x,
					(*the_pointer)->box_bound_Up_Left_y,
					(*the_pointer)->box_bound_Down_Right_x,
					(*the_pointer)->box_bound_Down_Right_y);
			if ((*the_pointer)->Shape == "Triangle")
			{
				glBegin(GL_TRIANGLES);
				glVertex3f((*the_pointer)->box_bound_Up_Left_x, (*the_pointer)->box_bound_Up_Left_y, 0.0);
				glVertex3f((*the_pointer)->box_bound_Up_Left_x, (*the_pointer)->box_bound_Down_Right_y, 0.0);
				glVertex3f((*the_pointer)->box_bound_Down_Right_x, (*the_pointer)->box_bound_Down_Right_y, 0.0);
				glEnd();
			}

			glPopMatrix();
			//glEnd();
			the_pointer++;
		}
	}
	glBegin(GL_POINTS);

	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		float pX = ((*cursorListIter)->getX() - 0.5) * 2.0;
		float pY = ((*cursorListIter)->getY() - 0.5) * 2.0 * -1.0;
		glVertex2f(pX, pY); // mirrored y-axis because of camera and coordinates converted to openGL --> glOrtho(-1,1,-1,1,0,1) with (-0.5*2)
	}
	glEnd();
	glFinish();
	glutSwapBuffers();
}

void tuioThread(void*)
{
	Client* app = new Client();
	tuioClient = new TUIO::TuioClient();
	tuioClient->addTuioListener(app);
	tuioClient->connect(true);
}

void idle(void)
{
	// this might be needed on some systems, otherwise the draw() function will only be called once
	glutPostRedisplay();
}

void glInit()
{
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(6.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, 520, 520);
	glOrtho(-1, 1, -1, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);


}

int main(int argc, char** argv)
{
	// init $1 gesture recognizer
	$1->loadTemplates();

	// create a second thread for the TUIO listener
	//unsigned threadID;
	//hThread = (HANDLE)_beginthreadex( NULL, 0, &tuioThread, NULL, 0, &threadID );
	HANDLE hThread_TUIO;
	hThread_TUIO = (HANDLE)_beginthread(tuioThread, 0, NULL);

	// GLUT Window Initialization:
	glutInit(&argc, argv);
	glutInitWindowSize(520, 520);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Client");

	// openGL init
	glInit();

	// Register callbacks:
	glutDisplayFunc(draw);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
