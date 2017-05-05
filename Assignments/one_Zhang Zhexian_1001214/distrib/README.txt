This assignment is mainly an independent work. I have asked classmate William for suggestions on rendering normals correctly for generalized cylinder.





Below are the list of references for completing the assignment:

1. General knowledge (course page): http://people.sutd.edu.sg/~saikit/courses/50.017/Password_Only/Assignment/Assignment1/index.html

2. Vecmath: https://fossies.org/linux/celestia/src/celmath/vecmath.h



There is one known problem with my code: the B-spline curves for curves with number of control points that is not (3*n+1) may not be properly rendered by base change of Bezier curves. The handling of the "extra points" were attempted in my code (refer to curve.cpp).

Due to the result of this, the surface generated based on B-splines may not be shown correctly due to error carried forward.



Extra credits:

- Change material for models by changing the r, g, b values of the material


Comments about the assignment:

Honestly I find this assignment rather challenging. It took me almost 3 whole days to complete the basic requirements with no time to attempt the extra credit. Being not familiar with the library and programming language may be reasons why it is difficult. The graphics concepts were hard to translate to C++ code too. I do hope that I will get better in future assignments.