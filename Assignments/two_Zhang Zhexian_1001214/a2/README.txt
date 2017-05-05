known problem: mesh rendering for model's left hand/lower arm

Need to include artifact screenshot


This assignment is mainly my independent work. 


Below are the list of references for completing the assignment:

1. General knowledge (course page): http://people.sutd.edu.sg/~saikit/courses/50.017/Password_Only/Assignment/Assignment2/index.html

2. Vecmath: https://fossies.org/linux/celestia/src/celmath/vecmath.h

3. Setting submatrix: http://web.mit.edu/6.837/public/include/vecmath/Matrix4f.h



There is one known problem with my code: the mesh rendering for model's left hand & left lower arm is problematic. I have attempted to solve it and have come to the conclusion that the total number of bindVertices may not be correct (fewer than actual number of vertices) so the part for the arm is lost. I have examined all the functions implemented by me but could not find the exact location of the bug. Also, I note that the model in bind pose it looking fine, hence doubting if the problem is with the bindVertices.



Extra credits:

- Change material for model to light green by changing the r, g, b values of the material
