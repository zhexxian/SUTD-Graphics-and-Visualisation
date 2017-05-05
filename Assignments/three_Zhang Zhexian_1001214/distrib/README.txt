ASSIGNMENT 3
=================

*Reference*

This assignment is mainly my independent work. 

Below are the list of references for completing the assignment:

1. General knowledge (course page): 
http://people.sutd.edu.sg/~saikit/courses/50.017/Password_Only/Assignment/Assignment3/index.html

2. GL_LINES: https://www.opengl.org/discussion_boards/showthread.php/142763-GL_LINES



*Optional Feature*

User is able to run different systems with different time steppers.

Run application in this format: a3 s|p|c e|t|r stepSize numParticle|gridSize

Where the 1st argument needs to be either 's' (Particle System), 'p' (Pendulum System), or 'c' (Cloth System); and the 2nd argument needs to be either 'e' (ForwardEuler), 't' (Trapezoidal), or 'r' (RK4).
	 
Example 1: a3 s r 0.05
Example 2: a3 p r 0.05 1
Example 3: a3 c r 0.005 8


*Extra Credit*

1. Add a random wind force to your cloth simulation that emulates a gentle breeze. You should be able to toggle it on and off using a key. 

It is implemented and user can toggle the wind by pressing 'b'



2. Rather than display the cloth as a wireframe mesh, implement smooth shading. The most challenging part of this is defining surface normals at each vertex, which you should approximate using the positions of adjacent particles.

It is implemented and user may toggle between wireframe and smooth shading by pressing 'w'
