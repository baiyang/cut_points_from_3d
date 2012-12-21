#ifndef FILEOP__H
#define FILEOP__H

#include <string>
#include <fstream>
using namespace std;

#include "math3d.h"
class Points;

void open_xyz(string filename, Points &pts)
{
	float x, y, z, ignore;
	ifstream input = ifstream(filename);


	int x_min, x_max, y_min, y_max, z_min, z_max;
	input >>x>>y>>z>>ignore;

	x_min = x;
	x_max = x;
	y_min = y;
	y_max = y;
	z_min = z;
	z_max = z;

	pts.pos.push_back( Position(x, y, z) );

	while( !input.eof() ){
		input>>x>>y>>z>>ignore;

		if( x > x_max){
			x_max = x;
		}else if( x < x_min ){
			x_min = x;
		}

		if( y > y_max ){
			y_max = y;
		}else if( y< y_min){
			y_min = y;
		}

		if( z > z_max){
			z_max = z;
		}else if( z < z_min){
			z_min = z;
		}

		pts.pos.push_back( Position(x, y, z) );
	}

	pts.x_max = x_max;
	pts.x_min = x_min;

	pts.y_max = y_max;
	pts.y_min = y_min;

	pts.z_max = z_max;
	pts.z_min = z_min;

	input.close();
}

#endif
