#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

Curve evalBezier( const vector< Vector3f >& P, unsigned steps, unsigned dimension )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

	if( steps == 0 )
	{
		cerr << "steps must not be zero" << endl;
	}

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }
    cerr << "\t>>> Steps (type steps): " << steps << endl;

	unsigned numberOfPieces = (P.size()-1)/3;

	cout << "\t>>>numberOfPieces: " << numberOfPieces << endl;

	Curve resultCurve = Curve();

	for( unsigned i = 0; i < numberOfPieces; i++ )
    {

		Vector3f p1 = P[i*3];
		Vector3f p2 = P[i*3+1];
		Vector3f p3 = P[i*3+2];
		Vector3f p4 = P[i*3+3];

		Vector3f previousB = Vector3f(P[0][0],P[0][1]+1,1);
		for( unsigned j = 0; j <= steps; j++ ){
			float t = (float)j/(float)steps;
			/*
			//x, y, z
			float vx = p1[0]*pow(1-t, 3) + p2[0]*(3*t*pow(1-t, 2)) + p3[0]*3*pow(t, 2)*(1-t) + p4[0]*pow(t, 3);
			float vy = p1[1]*pow(1-t, 3) + p2[1]*(3*t*pow(1-t, 2)) + p3[1]*3*pow(t, 2)*(1-t) + p4[1]*pow(t, 3);
			float vz = p1[2]*pow(1-t, 3) + p2[2]*(3*t*pow(1-t, 2)) + p3[2]*3*pow(t, 2)*(1-t) + p4[2]*pow(t, 3);
			*/

			//x', y', z'
			float dvx = -3*p1[0]*pow(1-t, 2) + p2[0]*(9*pow(t, 2)-12*t+3) + p3[0]*3*t*(2-3*t) + 3*p4[0]*pow(t, 2);
			float dvy = -3*p1[1]*pow(1-t, 2) + p2[1]*(9*pow(t, 2)-12*t+3) + p3[1]*3*t*(2-3*t) + 3*p4[1]*pow(t, 2);
			float dvz = -3*p1[2]*pow(1-t, 2) + p2[2]*(9*pow(t, 2)-12*t+3) + p3[2]*3*t*(2-3*t) + 3*p4[2]*pow(t, 2);			
			

			Matrix4f bezierControlPoints = Matrix4f(
				p1[0],p2[0],p3[0],p4[0],
				p1[1],p2[1],p3[1],p4[1],
				p1[2],p2[2],p3[2],p4[2],
				0, 0, 0, 0
			);

			Matrix4f bBezier = Matrix4f(
				1, -3, 3, -1,
				0,  3, -6, 3,
				0,  0, 3, -3,
				0,  0, 0,  1
			);
			Matrix4f power = Matrix4f(
				pow(t,0), 0, 0, 0, 
				pow(t,1), 0, 0, 0, 
				pow(t,2), 0, 0, 0, 
				pow(t,3), 0, 0, 0
			);

			Matrix4f combinedPower = bBezier*power;

			Matrix4f resultV = bezierControlPoints * combinedPower;

			/*

			float vx = Vector4f().dot(bezierControlPoints.getRow(0),combinedPower.getCol(0));
			float vy = Vector4f().dot(bezierControlPoints.getRow(1),combinedPower.getCol(0));
			float vz = Vector4f().dot(bezierControlPoints.getRow(2),combinedPower.getCol(0));
			*/

			float vx = resultV.getCol(0)[0];
			float vy = resultV.getCol(0)[1];
			float vz = resultV.getCol(0)[2];

			Vector3f newV;
			Vector3f newT;
			Vector3f newN;

			if(dimension == 2) {
				newV = Vector3f(vx, vy, 0);
				newT = Vector3f(dvx, dvy, 0).normalized();
				newN = Vector3f().cross(previousB,newT).normalized();
				newN[2]=0;
			}
			else{
				newV = Vector3f(vx, vy, vz);
				newT = Vector3f(dvx, dvy, dvz).normalized();
				newN = Vector3f().cross(previousB,newT).normalized();
			}
			Vector3f newB = Vector3f().cross(newT,newN).normalized();
			previousB = newB;

			CurvePoint newCurvePoint = CurvePoint();
			newCurvePoint.V=newV;
			newCurvePoint.T=newT;
			newCurvePoint.N=newN;
			newCurvePoint.B=newB;

			if(dimension == 2) {
				newCurvePoint.V[2] = 0;
				newCurvePoint.T[2] = 0;
				newCurvePoint.N[2] = 0;
			}
			
			resultCurve.push_back(newCurvePoint);

			
		}
    }

    return resultCurve;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps, unsigned dimension )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
	

	Matrix4f bBezier = Matrix4f(
		1, -3, 3, -1,
		0,  3, -6, 3,
		0,  0, 3, -3,
		0,  0, 0,  1
	);

	float bBSplieCoefficient = float(1)/float(6);

	Matrix4f bBSpline = Matrix4f(
		1*bBSplieCoefficient, -3*bBSplieCoefficient, 3*bBSplieCoefficient, -1*bBSplieCoefficient,
		4*bBSplieCoefficient,  0*bBSplieCoefficient, -6*bBSplieCoefficient, 3*bBSplieCoefficient,
		1*bBSplieCoefficient,  3*bBSplieCoefficient, 3*bBSplieCoefficient, -3*bBSplieCoefficient,
		0*bBSplieCoefficient,  0*bBSplieCoefficient, 0*bBSplieCoefficient,  1*bBSplieCoefficient
	);

	Matrix4f changeBasis = bBSpline*bBezier.inverse();

	vector<Vector3f> newControlPoints;

	if(P.size() == 4){

			Matrix4f bSplineControlPoints = Matrix4f(
				P[0][0],P[1][0],P[2][0],P[3][0],
				P[0][1],P[1][1],P[2][1],P[3][1],
				P[0][2],P[1][2],P[2][2],P[3][2],
				0, 0, 0, 0
			);


			Matrix4f bezierControlPoints = bSplineControlPoints*changeBasis;
		
			Vector3f newP1 = Vector3f(bezierControlPoints.getCol(0)[0], bezierControlPoints.getCol(0)[1], bezierControlPoints.getCol(0)[2]);
			Vector3f newP2 = Vector3f(bezierControlPoints.getCol(1)[0], bezierControlPoints.getCol(1)[1], bezierControlPoints.getCol(1)[2]);
			Vector3f newP3 = Vector3f(bezierControlPoints.getCol(2)[0], bezierControlPoints.getCol(2)[1], bezierControlPoints.getCol(2)[2]);
			Vector3f newP4 = Vector3f(bezierControlPoints.getCol(3)[0], bezierControlPoints.getCol(3)[1], bezierControlPoints.getCol(3)[2]);
		
			
			newControlPoints.push_back(newP1);
			newControlPoints.push_back(newP2);
			newControlPoints.push_back(newP3);
			newControlPoints.push_back(newP4);
			
	}

	else {
		for(int i=0; i<=P.size()-3; i++){
			Matrix4f bSplineControlPoints = Matrix4f(
				P[i][0],P[i+1][0],P[i+2][0],P[i+3][0],
				P[i][1],P[i+1][1],P[i+2][1],P[i+3][1],
				P[i][2],P[i+1][2],P[i+2][2],P[i+3][2],
				0, 0, 0, 0
			);


			Matrix4f bezierControlPoints = bSplineControlPoints*changeBasis;
		
			Vector3f newP1 = Vector3f(bezierControlPoints.getCol(0)[0], bezierControlPoints.getCol(0)[1], bezierControlPoints.getCol(0)[2]);
			Vector3f newP2 = Vector3f(bezierControlPoints.getCol(1)[0], bezierControlPoints.getCol(1)[1], bezierControlPoints.getCol(1)[2]);
			Vector3f newP3 = Vector3f(bezierControlPoints.getCol(2)[0], bezierControlPoints.getCol(2)[1], bezierControlPoints.getCol(2)[2]);
			Vector3f newP4 = Vector3f(bezierControlPoints.getCol(3)[0], bezierControlPoints.getCol(3)[1], bezierControlPoints.getCol(3)[2]);
		
			if(i==0){
				newControlPoints.push_back(newP1);
				newControlPoints.push_back(newP2);
				newControlPoints.push_back(newP3);
				newControlPoints.push_back(newP4);
			}	
			else if(i==(P.size()-3)){
				newControlPoints.push_back(newP2);
				newControlPoints.push_back(newP3);
				newControlPoints.push_back(newP3);
			}
			else {
				newControlPoints.push_back(newP2);
				newControlPoints.push_back(newP3);
				newControlPoints.push_back(newP4);
			}
		
		}
	}

	int extraControlPointCount = (P.size()-1)%3;
	if(extraControlPointCount!=0){
		int j = P.size()-4;
		Matrix4f bSplineControlPoints = Matrix4f(
			P[j][0],P[j+1][0],P[j+2][0],P[j+3][0],
			P[j][1],P[j+1][1],P[j+2][1],P[j+3][1],
			P[j][2],P[j+1][2],P[j+2][2],P[j+3][2],
			0, 0, 0, 0
		);

		Matrix4f bezierControlPoints = bSplineControlPoints*changeBasis;
		
		Vector3f newP1 = Vector3f(bezierControlPoints.getCol(0)[0], bezierControlPoints.getCol(0)[1], bezierControlPoints.getCol(0)[2]);
		Vector3f newP2 = Vector3f(bezierControlPoints.getCol(1)[0], bezierControlPoints.getCol(1)[1], bezierControlPoints.getCol(1)[2]);
		Vector3f newP3 = Vector3f(bezierControlPoints.getCol(2)[0], bezierControlPoints.getCol(2)[1], bezierControlPoints.getCol(2)[2]);
		Vector3f newP4 = Vector3f(bezierControlPoints.getCol(3)[0], bezierControlPoints.getCol(3)[1], bezierControlPoints.getCol(3)[2]);
		
		newControlPoints.push_back(newP2);
		newControlPoints.push_back(newP3);
		newControlPoints.push_back(newP3);

	}

	cout << "number of control points for b spline: " << newControlPoints.size() << endl;
	Curve bsplineCurve = evalBezier(newControlPoints, steps, dimension);

    return bsplineCurve;

}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

