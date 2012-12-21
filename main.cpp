

#include <iostream>
#include <GL/glut.h>
#include <vector>
using namespace std;

#include "math3d.h"
#include "Selection.h"
#include "glutils.h"
#include "fileop.h"
#include "InteractiveMatrix.h"

Selection selection;

M3DVector2f left_bottom, right_top;
bool bool_select_area = false;
bool bool_transform = true;
bool bool_ctrl_down = false;

InteractiveMatrix *_rotation = new InteractiveMatrix();
InteractiveMatrix *_translation = new InteractiveMatrix();

int lastPos[2];
float scalar_rotation = 0.05;
float scalar_trans = 0.1;
int gl_button_type, gl_button_state;
float camera_pos[3] = {0, 0, 2};
static int last_color_option = 1;


int viewport[4];
static Points pts;
static vector<int> idx;

char filename[1024];

void init(void) 
{
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel (GL_FLAT);

  open_xyz( filename, pts);
  _rotation->reset();
  _translation->reset();

  // 构建idx
  for(int i = 0; i < pts.pos.size(); i++){
	  idx.push_back(i);
  }

  color_handler(pts);
}


void display(void)
{
   M3DMatrix44f mat_proj, mat_modelview;

   int width = glutGet( GLUT_WINDOW_WIDTH ), height = glutGet( GLUT_WINDOW_HEIGHT );
   
   glViewport (0, 0, (GLsizei) width, (GLsizei) height); 

   glGetIntegerv(GL_VIEWPORT, viewport);

   glClear (GL_COLOR_BUFFER_BIT);

   glPushAttrib(GL_POLYGON_BIT);
   glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();


   
   glPushMatrix();
   glMultMatrixf(_translation->getMatrix());
   glMultMatrixf(_rotation->getMatrix());



  
   gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);
   glColor4f(0.1, 0.4, 0.6, 0.7);

   

   glViewport (0, 0, (GLsizei) width, (GLsizei) height);

   glPushMatrix();
		// 获取模型视图矩阵
		glGetFloatv(GL_MODELVIEW_MATRIX, mat_modelview);
		// 获取投影矩阵
		glGetFloatv(GL_PROJECTION_MATRIX, mat_proj);

		draw_cube(pts.x_min, pts.y_min, pts.z_min, pts.x_max, pts.y_max, pts.z_max);
		draw_with_interleaved(pts.pos.data(), 0, pts.pos.size());

   glPopMatrix();
   glPopAttrib();

   glPopMatrix();

   /************************************************************************/
   /* 构造一个新的环境                                                                     */
   /************************************************************************/
   
   if( bool_select_area ){
	   // 配置
	   selection.set_config( (float*)pts.pos.data(), pts.pos.size(), left_bottom, right_top, mat_modelview, mat_proj, viewport, 6, 3);

	   selection.draw_area();
	   selection.highlight_selected_pts();

   }
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(65.0, (GLfloat) w/(GLfloat) h, 1.0, 1000);

}

void mouse(int button, int state, int x, int y)
{
	int width = glutGet( GLUT_WINDOW_WIDTH ), height = glutGet( GLUT_WINDOW_HEIGHT );

	lastPos[0] = x;
	lastPos[1] = y;

	gl_button_type = button;

	gl_button_state = state;

    if(gl_button_state == GLUT_DOWN){
		m3dLoadVector2(left_bottom, x, height - y);
		m3dLoadVector2(right_top, x, height - y);
	}

	bool_ctrl_down = glutGetModifiers() == GLUT_ACTIVE_CTRL;

	glutPostRedisplay();
}

void motion(int x, int y)
{
	int width = glutGet( GLUT_WINDOW_WIDTH ), height = glutGet( GLUT_WINDOW_HEIGHT );

	int dx = x - lastPos[0];
	int dy = y - lastPos[1];


	m3dLoadVector2(right_top, x, height - y);
	
	if( bool_transform ){
		if( gl_button_type == GLUT_LEFT_BUTTON ){
			_rotation->addRotation(scalar_rotation * dx, 0, 1, 0);
			_rotation->addRotation(scalar_rotation * dy, 1, 0, 0);
		}
	}

	lastPos[0] = x;
	lastPos[1] = y;

	glutPostRedisplay();
}

/* ARGSUSED1 */
void keyboard (unsigned char k, int x, int y)
{
	camera_pos[0] = k=='a'? 1 : k=='d'? -1:0;
	camera_pos[1] = k=='q'? 1 : k=='e'? -1:0;
	camera_pos[2] = k=='w'? 1 : k=='s'? -1:0;


	_translation->addTranslation(camera_pos[0] * 0.5,camera_pos[1] * 0.5, camera_pos[2]);
	glutPostRedisplay();
}

void action_option(int option)
{

	vector<int> idx;
	switch( option ){
	case 0: 
		bool_transform = true;
		bool_select_area = false;

		break;

	case 1:
		bool_select_area = true;
		bool_transform = false;
		break;

	case 2:
		reset_max_min_xyz(pts);
		color_handler(pts, last_color_option);
		break;

	case 3:
		selection.get_selected_pts_index(idx);
		cut_points(pts, idx);
		break;
	default:;
	}

	glutPostRedisplay();
}

void color_option(int option)
{
    if(option < 3 && last_color_option != option){
		color_handler(pts, option);
	}


	last_color_option = option;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{

   if(argc != 2){
	   cout<<"Please input filename"<<endl;
	   return 0;
   }
   sprintf(filename, "./%s", argv[1]);

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display);
   int handle_color = glutCreateMenu(color_option);
	   glutAddMenuEntry("X", 0);
	   glutAddMenuEntry("Y", 1);
	   glutAddMenuEntry("Z", 2);

   glutCreateMenu(action_option);
	   glutAddSubMenu("Color", handle_color);
	   glutAddMenuEntry("Reset", 0);
	   glutAddMenuEntry("Select Points", 1);
	   glutAddMenuEntry("Reset Color", 2);
	   glutAddMenuEntry("Cut Selected Points", 3);
	   
	   

   glutAttachMenu(GLUT_RIGHT_BUTTON);

    
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);

   glutMainLoop();

   if(_rotation) delete _rotation;
   if(_translation) delete _translation;
   return 0;
}
