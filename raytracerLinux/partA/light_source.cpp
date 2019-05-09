/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.

    //get the intersection  
    Intersection intersection = ray.intersection;
    //get the normal
    Vector3D normal = intersection.normal;
    normal.normalize();
    //get the ray direction
    Vector3D ray_dir = ray.dir;
    ray_dir.normalize();
    //get the light direction
    Vector3D light_dir = _pos - intersection.point;
    light_dir.normalize();
    //calculate the reflection diretion
    Vector3D reflect_dir = light_dir - 2.0 * (light_dir.dot(normal)) * normal;
    reflect_dir.normalize();
    //get Material data
    Material* mat = intersection.mat;
    //calculate ambient 
    Colour ambient_component = (mat->ambient) * _col_ambient;
    //calculate diffusion
    Colour diffuse_component = fmax(0 , light_dir.dot(normal)) * (mat->diffuse) * _col_diffuse;
    //calculate speculation
    Colour specular_component = pow(fmax(0 , ray_dir.dot(reflect_dir)),
            mat->specular_exp) * _col_specular * mat->specular;
    ambient_component.clamp();
    diffuse_component.clamp();
    specular_component.clamp();
    //add all component together
    Colour ray_col = ambient_component + diffuse_component + specular_component;
    ray_col.clamp();
    ray.col = ray.col + ray_col;    

}

