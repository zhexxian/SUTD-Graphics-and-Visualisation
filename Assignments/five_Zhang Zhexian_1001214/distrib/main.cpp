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
#include "RayTracer.h"

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
	if (argc == 1) {
	std::cout << "Please run the program in the correct format: ./a5.exe"
		<< "\t-inputFile <scene.txt>"
		<< "\t-size <width> <height>"
		<< "\t-outputFile <image.bmp>"
		<< "\t-jitter <0 or 1>"
		<< "\t-filter <0 or 1>\n";
		//<< "\t-depthFile <depth_min> <depth_max> <depth_image.bmp>\n";
		return 1;
	}

	const char* scene;
    int width = 0;
    int height = 0;
    const char* output;
	float depth_min = 0.0f;
    float depth_max = 0.0f;
    const char* depth;

	//anti-aliasing
	int jitter = 0;
	int filter = 0;

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
		}
		/*else if(!strcmp(argv[argumentIndex],"-depthFile")){
			depth_min = (float)std::atof(argv[argumentIndex+1]);
			depth_max = (float)std::atof(argv[argumentIndex+2]);
			std::cout << "Depth file is min " << depth_min << " and max " << depth_max << std::endl;
			depth = argv[argumentIndex+3];
			std::cout << "Depth file is " << depth << std::endl;
			argumentIndex+=4;
		}*/
		else if(!strcmp(argv[argumentIndex],"-jitter")){
			jitter = std::atoi(argv[argumentIndex+1]);
			std::cout << "Jitter is " << jitter << std::endl;
			argumentIndex+=2;
		}else if(!strcmp(argv[argumentIndex],"-filter")){
			filter = std::atoi(argv[argumentIndex+1]);
			std::cout << "Filter is " << filter << std::endl;
			argumentIndex+=2;
		}else{
			std::cout << "Error in argument parsing" << std::endl;
			return -1;
		}
    }

	//parse the scene using SceneParser

	SceneParser parser = SceneParser::SceneParser(scene);


	//define variables
	Camera* camera = parser.getCamera();
	Image outputImage = Image(width, height);
	Image depthImage = Image(width, height);
	float x;
	float y;
	Group* group = parser.getGroup();
	float tmin = camera->getTMin();
	Vector3f backgroundColor = parser.getBackgroundColor(Vector3f(0.0f, 0.0f, 0.0f)); //Update this to parse ray direction
	Vector3f ambientLight = parser.getAmbientLight();
	int numLights = parser.getNumLights();
	RayTracer rayTracer = RayTracer::RayTracer(&parser,10,true);


	//generate depth output
	depthImage.SetAllPixels(Vector3f(0.0f, 0.0f, 0.0f));
	for(int i=0; i<width; i++){
		// -1 < x < 1
		//midpoint = width/2.0f
		x = (i-width/2.0f+1)/(width/2.0f);
		for(int j=0; j<height; j++){
			// -1 < y < 1
			//midpoint = height/2.0f
			y = (j-height/2.0f+1)/(height/2.0f);
			Hit hit =  Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
			Ray ray = camera->generateRay(Vector2f(x,y));
			float depth = hit.getT();
			if (depth > depth_max) {
				depthImage.SetPixel(i, j, Vector3f(0.0f, 0.0f, 0.0f));
			} else if (depth < depth_min) {
				depthImage.SetPixel(i, j, Vector3f(1.0f, 1.0f, 1.0f));
			} else {
				//float gradient = 1 - (depth - depth_min) / (depth_max - depth_min);
				float gradient = (depth_max - depth) / (depth_max - depth_min);
				depthImage.SetPixel(i, j, Vector3f(gradient, gradient, gradient));
			}
		}
	}

	depthImage.SaveImage(depth);


	//generate image output
	if (jitter !=1 && filter!=1) {
		// Loop over each pixel in the image, shooting a ray
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
				Hit hit = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
				Ray ray = camera->generateRay(Vector2f(x,y));
			/*
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
			*/
				
				Vector3f pixelColor = rayTracer.traceRay(ray, camera->getTMin(), 0, 1.0f, hit);
				outputImage.SetPixel(i, j, pixelColor);
			}
		}
	}

	else //anti-aliasing
	{
		//jittered sampling
		int jitter_x = width*3;
		int jitter_y = height*3; //render image with 3x resolution (equivalent to subdivide a pixel into 3 × 3 sub-grids)
		vector<Vector3f> pixelColor;
		
		for (int i = 0; i<jitter_x; i++){
  			for (int j = 0; j<jitter_y; j++){
				float xJitterSampling = 2*float(i) / (jitter_x-1) -1;
				float yJitterSampling = 2*float(j) / (jitter_y-1) -1;
				Ray ray = camera->generateRay(Vector2f(xJitterSampling, yJitterSampling));
				//Vector2f coord = Vector2f( (2*float(i) / (widthHighRes-1)) -1 , (2*float(j) / (heightHighRes-1)) -1 );
				float xRandom = (float)rand()/(float)RAND_MAX - 0.5f;
				float yRandom = (float)rand()/(float)RAND_MAX - 0.5f;
				Vector2f newCoord = Vector2f(xJitterSampling + xRandom, yJitterSampling + yRandom);
				
				Hit h = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
				Vector3f color = rayTracer.traceRay(ray, camera->getTMin(), 0, 1.0f, h);
				pixelColor.push_back(color);
			}
		}

		const float K[] = {0.1201f, 0.2339f, 0.2931f, 0.2339f, 0.1201f}; //default kernel values
		//Gaussian blur (horizontal)
		vector<Vector3f> pixelColorBlurHorizontal;
		for (int i = 0; i<jitter_x; i++){
  			for (int j = 0; j<jitter_y; j++){//handle image boundaries
				if (j == jitter_y-1){//top: ignore (i,j+1) and (i,j+2) cases
					pixelColorBlurHorizontal.push_back(pixelColor[i*jitter_y+j]*K[2] + pixelColor[i*jitter_y+j-1]*K[1] + 
						pixelColor[i*jitter_y+j-2]*K[0]);   
				}
				else if (j == jitter_y-2){//second from top: ignore (i,j+2) case
					pixelColorBlurHorizontal.push_back(pixelColor[i*jitter_y+j+1]*K[3] + pixelColor[i*jitter_y+j  ]*K[2] + 
						pixelColor[i*jitter_y+j-1]*K[1] + pixelColor[i*jitter_y+j-2]*K[0]);   
				}
				else if (j == 0){//bottom: ignore (i,j-2) and (i,j-1) cases
					pixelColorBlurHorizontal.push_back(pixelColor[i*jitter_y+j]*K[2] + pixelColor[i*jitter_y+j+1]*K[3] + 
						pixelColor[i*jitter_y+j+2]*K[4]);   
				}
				else if (j == 1){//second from bottom: ignore (i,j-2) case
					pixelColorBlurHorizontal.push_back(pixelColor[i*jitter_y+j-1]*K[1] + pixelColor[i*jitter_y+j  ]*K[2] + 
						pixelColor[i*jitter_y+j+1]*K[3] + pixelColor[i*jitter_y+j+2]*K[4]);   
				}
				else{//normal cases
					//I' (i, j) = I(i, j - 2)K(0) + I(i, j - 1)K(1) + I(i, j)K(2) + I(i, j + 1)K(3) +  I(i, j + 2)K(4)   
					pixelColorBlurHorizontal.push_back(pixelColor[i*jitter_y+j-2]*K[0] + pixelColor[i*jitter_y+j-1]*K[1] +
						pixelColor[i*jitter_y+j]*K[2] + pixelColor[i*jitter_y+j+1]*K[3] + 
						pixelColor[i*jitter_y+j+2]*K[4]);
				}
			}
		}
		//Gaussian blur (vertical)
		vector<Vector3f> pixelColorBlurVertical;
		for (int j = 0; j<jitter_y; j++){
  			for (int i = 0; i<jitter_x; i++){//handle image boundaries
				if (i == jitter_x-1){//right: ignore (i,j+1) and (i,j+2) cases
					pixelColorBlurVertical.push_back(pixelColorBlurHorizontal[j*jitter_x+i]*K[2] + pixelColorBlurHorizontal[j*jitter_x+i-1]*K[1] + 
						pixelColorBlurHorizontal[j*jitter_x+i-2]*K[0] );    
				}
				else if (i == jitter_x-2){//second from right: ignore (i,j+2) case
					pixelColorBlurVertical.push_back(pixelColorBlurHorizontal[j*jitter_x+i+1]*K[3] + pixelColorBlurHorizontal[j*jitter_x+i  ]*K[2] + 
						pixelColorBlurHorizontal[j*jitter_x+i-1]*K[1] + pixelColorBlurHorizontal[j*jitter_x+i-2]*K[0] );  
				}
				else if (i == 0){//left: ignore (i,j-2) and (i,j-1) cases
					pixelColorBlurVertical.push_back(pixelColorBlurHorizontal[j*jitter_x+i]*K[2] + pixelColorBlurHorizontal[j*jitter_x+i+1]*K[3] + 
						pixelColorBlurHorizontal[j*jitter_x+i+2]*K[4] );   
				}
				else if (i == 1){//second from left: ignore (i,j-2) case
					pixelColorBlurVertical.push_back(pixelColorBlurHorizontal[j*jitter_x+i-1]*K[1] + pixelColorBlurHorizontal[j*jitter_x+i  ]*K[2] + 
						pixelColorBlurHorizontal[j*jitter_x+i+1]*K[3] + 
						pixelColorBlurHorizontal[j*jitter_x+i+2]*K[4] );     
				}
				else{//normal cases
					//I' (i, j) = I(i, j - 2)K(0) + I(i, j - 1)K(1) + I(i, j)K(2) + I(i, j + 1)K(3) +  I(i, j + 2)K(4)   
					pixelColorBlurVertical.push_back(pixelColorBlurHorizontal[j*jitter_x+i-2]*K[0] + pixelColorBlurHorizontal[j*jitter_x+i-1]*K[1] +
						pixelColorBlurHorizontal[j*jitter_x+i]*K[2] + pixelColorBlurHorizontal[j*jitter_x+i+1]*K[3] + 
						pixelColorBlurHorizontal[j*jitter_x+i+2]*K[4]);    
				}
			}
		}

		//down-sampling
		for (int i = 0; i<width; i++){
  			for (int j = 0; j<height; j++){//average each 3 × 3 neighborhood of pixels to get back 1 pixel
				Vector3f finalColor = 
					pixelColorBlurVertical[3*(i+jitter_x*j)+0] + pixelColorBlurVertical[3*(i+jitter_x*j)+1] +
					pixelColorBlurVertical[3*(i+jitter_x*j)+2] + pixelColorBlurVertical[3*(i+jitter_x*j)+jitter_x+0] +
					pixelColorBlurVertical[3*(i+jitter_x*j)+jitter_x+1] + pixelColorBlurVertical[3*(i+jitter_x*j)+jitter_x+2] +
					pixelColorBlurVertical[3*(i+jitter_x*j)+2*jitter_x+0] + pixelColorBlurVertical[3*(i+jitter_x*j)+2*jitter_x+1] +
					pixelColorBlurVertical[3*(i+jitter_x*j)+2*jitter_x+2];
				Vector3f finalColorAverage= finalColor/9.0f;//divide by 9 to get back to original resolution
				outputImage.SetPixel(j, i, finalColorAverage);
			}
		}
		
	}

	outputImage.SaveImage(output);

	return 0;
}
