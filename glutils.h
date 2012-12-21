#ifndef GLUTILS__H
#define GLUTILS__H

#include <GL/glut.h>
#include <vector>
#include <algorithm>
using namespace std;

struct Position{
	float r;
	float g;
	float b;

	float x;
	float y;
	float z;

	Position(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->r = this->g = this->b = 1.0;
	}

	void set_pos(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void set_color(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

};

struct Points{
	vector<Position> pos;

	float x_min;
	float x_max;
	float y_min;
	float y_max;
	float z_min;
	float z_max;
};


void draw_with_vertex_array(M3DVector3f *vertex, int *idx, int nr_points)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertex);
	glDrawElements(GL_POINTS, nr_points, GL_UNSIGNED_INT, idx);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void draw_with_interleaved(void *data, int offset, int nr)
{
	glInterleavedArrays(GL_C3F_V3F, offset, data);
	glDrawArrays(GL_POINTS, 0, nr);
}


void draw_cube(float x1, float y1, float z1, float x2, float y2, float z2)
{
   glColor3f (1.0, 1.0, 1.0);
   glBegin (GL_LINE_LOOP);
   glVertex3f (x1, y1, z1);
   glVertex3f (x1, y2, z1);
   glVertex3f (x2, y2, z1);
   glVertex3f (x2, y1, z1);
   glEnd ();

   glBegin (GL_LINE_LOOP);
   glVertex3f (x1, y1, z2);
   glVertex3f (x1, y2, z2);
   glVertex3f (x2, y2, z2);
   glVertex3f (x2, y1, z2);
   glEnd ();

   glBegin (GL_LINES);	/*  4 lines	*/
   glVertex3f (x1, y1, z1);
   glVertex3f (x1, y1, z2);
   glVertex3f (x1, y2, z1);
   glVertex3f (x1, y2, z2);
   glVertex3f (x2, y1, z1);
   glVertex3f (x2, y1, z2);
   glVertex3f (x2, y2, z1);
   glVertex3f (x2, y2, z2);
   glEnd ();
}

void color_handler(Points &pts, int axis = 1)
{
	float v_min, v_max;

	switch( axis ){
	case 0:
		v_min = pts.x_min;
		v_max = pts.x_max;
		break;
	case 1:
		v_min = pts.y_min;
		v_max = pts.y_max;
		break;

	case 2:
		v_min = pts.z_min;
		v_max = pts.z_max;
		break;
	default:
		return ;
	}

	vector<Position>::iterator iter;

	float diff = v_max - v_min;
	float p;
	float r, g, b;

	for(iter = pts.pos.begin(); iter != pts.pos.end(); iter++){
		switch( axis ){
		case 0:
			p = (iter->x - v_min) / diff;
			break;
		case 1:
			p = (iter->y - v_min) / diff;
			break;
		case 2:
			p = (iter->z - v_min) / diff;
			break;
		}

		if( p > 0.5 ){
			r = (p - 0.5) * 2;
			g = (1 - p) * 2;
			b = 0;
		}else {
			r = 0;
			g = p * 2;
			b = (0.5 - p) * 2;
		}

		iter->set_color(r, g, b);
	}
}


void reset_max_min_xyz(Points &pts)
{
	int x_min, x_max, y_min, y_max, z_min, z_max;
	x_min = pts.pos[0].x;
	x_max = pts.pos[0].x;
	y_min = pts.pos[0].y;
	y_max = pts.pos[0].y;
	z_min = pts.pos[0].z;
	z_max = pts.pos[0].z;

	vector<Position>::const_iterator iter;
	for(iter = pts.pos.cbegin(); iter != pts.pos.cend(); iter++){
		if(iter->x > x_max){
			x_max = iter->x;
		}else if (iter->x < x_min){
			x_min = iter->x;
		}
		
		if(iter->y > y_max){
			y_max = iter->y;
		}else if( iter->y < y_min){
			y_min = iter->y;
		}

		if(iter->z > z_max){
			z_max = iter->z;
		}else if(iter->z < z_min){
			z_min = iter->z;
		}
	}

	pts.x_max = x_max;
	pts.x_min = x_min;

	pts.y_max = y_max;
	pts.y_min = y_min;

	pts.z_max = z_max;
	pts.z_min = z_min;

}


void cut_points(Points &pts, vector<int> &idx)
{
	vector<int>::reverse_iterator idx_iter;

	int index = pts.pos.size() - 1;
	for(idx_iter = idx.rbegin(); index != -1 && idx_iter != idx.rend(); index--){

		if(index == *idx_iter){
			swap(pts.pos[index], pts.pos[ pts.pos.size() - 1]);
			pts.pos.pop_back();

			idx_iter++;
		}
	}
}

#endif

