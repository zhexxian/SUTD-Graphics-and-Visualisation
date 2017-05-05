ASSIGNMENT 5
=================

*Reference*

This assignment is mainly my independent work. 

Below are the list of references for completing the assignment:

1. General knowledge (course page): 
http://people.sutd.edu.sg/~saikit/courses/50.017/Password_Only/Assignment/Assignment5/index.html

2. Notes:
	10. RayTracing
	14. Anti-Aliasing


3. Checking output file from sample solution exe file


*Manual*

Sample usage:
./a5.exe -inputFile scene01_plane.txt -size 200 200 -outputFile output01.bmp -jitter 1 -filter 1

*Known Problem*

After countless times of debugging, it still seems that the refraction in RayTracer.cpp does not work as expected. The refraction direction is calculated per assignment notes, and the Fresnel's equation is applied properly too. Not sure if the issue is with the parameter value setup for maximum number of bounces as I could not possibly try every value.
