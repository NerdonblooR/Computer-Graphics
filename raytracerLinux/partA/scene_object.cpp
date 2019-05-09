/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
    double t = 0;
    Point3D inter_point(0,0,0);
    Point3D ray_origin = worldToModel * ray.origin;
    Vector3D ray_dir = worldToModel * ray.dir;
    Vector3D normal(0,0,1);
    //calculate intersection;
    if(ray_dir[2] != 0){    
        t = -ray_origin[2]/ray_dir[2];
    }else{
        //on the same plane
        return false;
    }
    //get the index of the intersection point
    inter_point = ray_origin + t * ray_dir; 
    //check if the index is in the range (-0.5 0.5)
    if(inter_point[0] >= -0.5 and inter_point[0] <= 0.5 and
    inter_point[1] >= -0.5 and inter_point[1] <= 0.5){
        if (!ray.intersection.none && t > ray.intersection.t_value){
            return false;
        }
        //convert back to world coordinate
        ray.intersection.point = modelToWorld * inter_point;
        ray.intersection.normal = transNorm(worldToModel, normal);
        ray.intersection.normal.normalize(); 
        ray.intersection.none = false;
        ray.intersection.t_value = t; 
        return true;    
    }   
    return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	double t;
    double t1;
    double t2;
    Point3D inter_point(0,0,0);
    Point3D center(0,0,0);
    Vector3D normal;
    //just for the convenience of using dot product function.
    Vector3D ray_origin = worldToModel*ray.origin - center;
    //convert the direction of the ray to model coordinates
    Vector3D ray_dir = worldToModel * ray.dir;
    double A = ray_dir.dot(ray_dir);
    double B = 2 * ray_dir.dot(ray_origin);
    double C = ray_origin.dot(ray_origin) - 1;
    double D = B*B - 4 * A * C;
    if (D < 0) return false;
    t = (-sqrt(D) - B) / (2 * A);
    if (!ray.intersection.none && ray.intersection.t_value < t){
        return false;
    }
    //get the intersection point
    inter_point = worldToModel * ray.origin + t * ray_dir; 
    //get the normal at interection point
    normal = inter_point - center; 
    //convert back to world coordinate
    ray.intersection.point = modelToWorld * inter_point;
    ray.intersection.normal = transNorm(worldToModel,normal);
    ray.intersection.normal.normalize();
    ray.intersection.none = false;
    ray.intersection.t_value = t;  
	return true;
}

