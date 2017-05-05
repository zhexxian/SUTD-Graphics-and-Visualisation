#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
	// Fill in your implementation here.

	if (argc == 1) {
	std::cout << "Please run the program in the correct format: ./a4"
		<< "\t-inputFile <scene.txt>"
		<< "\t-size <width> <height>"
		<< "\t-outputFile <image.bmp>"
		<< "\t-depthFile <depth_min> <depth_max> <depth_image.bmp>\n";
		return 1;
	}

	const char* scene;
    int width = 0;
    int height = 0;
    const char* output;
	float depth_min = 0.0f;
    float depth_max = 0.0f;
    const char* depth;

    int argumentIndex = 1;
    while(argumentIndex<argc){
		if(!strcmp(argv[argumentIndex], "-inputFile")){
			scene = argv[argumentIndex+1];
			std::cout << "Input file is " << scene << std::endl;
			argumentIndex+=2;
		}else if(!strcmp(argv[argumentIndex],"-size")){
			width = std::atoi(argv[argumentIndex+1]);
			height = std::atoi(argv[argumentIndex+2]);
			std::cout << "Input file size is " << width << " * " << height << std::endl;
			argumentIndex+=3;
		}else if(!strcmp(argv[argumentIndex],"-outputFile")){
			output = argv[argumentIndex+1];
			std::cout << "Output file is " << output << std::endl;
			argumentIndex+=2;
		}else if(!strcmp(argv[argumentIndex],"-depthFile")){
			depth_min = (float)std::atof(argv[argumentIndex+1]);
			depth_max = (float)std::atof(argv[argumentIndex+2]);
			std::cout << "Depth file is min " << depth_min << " and max " << depth_max << std::endl;
			depth = argv[argumentIndex+3];
			std::cout << "Depth file is " << depth << std::endl;
			// Increase argument index by 4 to exit the while loop
			argumentIndex+=4;
		}
    }

	// Parse the scene using SceneParser

	SceneParser parser = SceneParser::SceneParser(scene);


	// Define variables
	Camera* camera = parser.getCamera();
	Image outputImage = Image(width, height);
	Image depthImage = Image(width, height);
	float x;
	float y;
	Group* group = parser.getGroup();
	float tmin = camera->getTMin();
	Vector3f backgroundColor = parser.getBackgroundColor();
	Vector3f ambientLight = parser.getAmbientLight();
	int numLights = parser.getNumLights();

	// Then loop over each pixel in the image, shooting a ray
	// through that pixel and finding its intersection with
	// the scene
	
	for(int i=0; i<width; i++){
		// -1 < x < 1
		//midpoint = width/2.0f
		x = (i-width/2.0f+1)/(width/2.0f);
		for(int j=0; j<height; j++){
			// -1 < y < 1
			//midpoint = height/2.0f
			y = (j-height/2.0f+1)/(height/2.0f);
			Hit hit = Hit();
			Ray ray = camera->generateRay(Vector2f(x,y));
			
			// Write the color at the intersection to that
			// pixel in your output image
			if (group->intersect(ray, hit, tmin)) {
				Vector3f color;
				// Check shade color at for light hit of each light source
				for (int k=0; k < numLights; k++) {
					Light* light = parser.getLight(k);
					// Define the variables to be modified by getIllumination function
					Vector3f direction;
					Vector3f illuminationColor;
					float distanceToLight;
					light->getIllumination(ray.pointAtParameter(hit.getT()), direction, illuminationColor, distanceToLight);
					// Update color
					color = color + hit.getMaterial()->Shade(ray, hit, direction, illuminationColor);
				}

				// Color = ambient + diffuse + specular
				color = color + parser.getAmbientLight();
				outputImage.SetPixel(i, j, color);
								
				// Set color to solid color for debugging
				//color = Vector3f(1, 0, 1);
			} else {
				// The pixels not intersecting with object group will be in background color
				outputImage.SetPixel(i, j, backgroundColor);
			}
			
			//generate depth output
			
			float depth = hit.getT();
			if (depth > depth_max) {
				depthImage.SetPixel(i, j, Vector3f(0.0f, 0.0f, 0.0f));
			} else if (depth < depth_min) {
				depthImage.SetPixel(i, j, Vector3f(1.0f, 1.0f, 1.0f));
			} else {
				float gradient = 1 - (depth - depth_min) / (depth_max - depth_min);
				depthImage.SetPixel(i, j, Vector3f(gradient, gradient, gradient));
			}
			

		}
	}

	outputImage.SaveImage(output);

    depthImage.SaveImage(depth);

	return 0;
}
