This assignment is an independent work.





Below are the list of references for completing the assignment:

1. General knowledge (course page): http://people.sutd.edu.sg/~saikit/courses/50.017/Password_Only/Assignment/Assignment0/index.html

2. Rotation: https://www.opengl.org/discussion_boards/showthread.php/181208-Rotate-oblect-using-glutTimerFunc

3. Toggle rotation: http://math.hws.edu/eck/cs324/s04/lab7/open_gl_wireframe_starter.cc

4. Mouse control: https://www.opengl.org/discussion_boards/showthread.php/173157-Glut-Mouse-Func

5. Scrolling: http://stackoverflow.com/questions/14378/using-the-mouse-scrollwheel-in-glut






There is one known problem with my code: the object loaded is not displayed properly, that some faces are missing. There is no issue with the “torus.obj”, but only half of the “sphere.obj” is rendered, and the “garg.obj” has holes on its surface. I have tried to draw polygon using glVertex3f, it is working fine; the default teapot is loaded properly too. There may be issues in vertex and normal assignment, but I could not figure out what is the problem.





Extra credits:

1. Hit ‘r’ to spin the model by implementing StepRot function that makes use of glutTimerFunc

2. Implement a mouse-based camera control to allow the user to rotate and zoom in on the object. (Left mouse press: clockwise step rotation; right mouse press: anti-clockwise step rotation; scroll up: zoom in; scroll down: zoom out)