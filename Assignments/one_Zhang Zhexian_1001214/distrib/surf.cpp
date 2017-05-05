#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

	// generate vertices

	double stepAngle;
	Matrix3f rotationAboutY;
	Vector3f originalV;
	Vector3f originalN;
	Matrix3f originalCurvePointMatrix;
	Matrix3f newCurvePointMatrix;
	Vector3f newV;
	Vector3f newN;
	for(unsigned i=0; i<steps; i++){
		//stepAngle = double(i)*double(360)/double(steps);
		stepAngle = 2.0f * M_PI * float(i)/float(steps);
		 rotationAboutY = Matrix3f(
			cos(stepAngle), 0, sin(stepAngle),
			0, 1, 0,
			-sin(stepAngle), 0, cos(stepAngle)
		);

		 //Matrix3f::rotateY(stepAngle);
		for(unsigned j=0; j<profile.size(); j++){
			originalV = Vector3f(profile[j].V[0], profile[j].V[1], profile[j].V[2]);
			originalN = Vector3f(profile[j].N[0], profile[j].N[1], profile[j].N[2]);

			originalCurvePointMatrix = Matrix3f(
				originalV[0], originalN[0], 0,
				originalV[1], originalN[1], 0, 
				originalV[2], originalN[2], 0
			);
			
			newCurvePointMatrix = rotationAboutY * originalCurvePointMatrix;

			newV = Vector3f(newCurvePointMatrix.getCol(0)[0], newCurvePointMatrix.getCol(0)[1], newCurvePointMatrix.getCol(0)[2]);
			newN = Vector3f(newCurvePointMatrix.getCol(1)[0], newCurvePointMatrix.getCol(1)[1], newCurvePointMatrix.getCol(1)[2]);
			
			surface.VV.push_back(newV);
			surface.VN.push_back(-newN); // reverse
		}
		
	}
	Tup3u newFace;
	Tup3u newFace2;
	unsigned meshHeight = profile.size();
	for(unsigned i=0; i<steps-1; i++){
		for(unsigned j=0; j<meshHeight; j++){
			 newFace = Tup3u(j+(i+1)*meshHeight+1, j+(i+1)*meshHeight,j+i*meshHeight+1);
			 surface.VF.push_back(newFace);
			 newFace2 = Tup3u(j+(i+1)*meshHeight,j+i*meshHeight,j+i*meshHeight+1);
			 surface.VF.push_back(newFace2);
		}
	}
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
	Vector3f originalV;
	Vector3f originalN;
	Matrix3f trajectoryMatrix;
	Matrix3f trajectoryMatrixForNormal;
	Matrix3f originalCurvePointMatrix;
	Matrix3f newCurvePointMatrix;
	Matrix3f newCurvePointMatrixForNormal;
	Vector3f newV;
	Vector3f newN;
	for(unsigned j=0; j<sweep.size(); j++){

		/*
		It turns out that, if we transform a vertex by a homogeneous transformation matrix M, 
			its normal should be transformed by the inverse transpose of the top-left 3 x 3 submatrix of M. 
		*/

		trajectoryMatrix = Matrix3f(
			sweep[j].N[0], sweep[j].B[0], sweep[j].T[0],
			sweep[j].N[1], sweep[j].B[1], sweep[j].T[1],
			sweep[j].N[2], sweep[j].B[2], sweep[j].T[2]
		);

		trajectoryMatrixForNormal = trajectoryMatrix.inverse().transposed();

		for(unsigned i=0; i<profile.size(); i++){
			originalV = Vector3f(profile[i].V[0], profile[i].V[1], profile[i].V[2]);
			originalN = Vector3f(profile[i].N[0], profile[i].N[1], profile[i].N[2]);

			originalCurvePointMatrix = Matrix3f(
				originalV[0], originalN[0], 0,
				originalV[1], originalN[1], 0,
				originalV[2], originalN[2], 0
			);


			newCurvePointMatrix = trajectoryMatrix * originalCurvePointMatrix;

			newV = Vector3f(newCurvePointMatrix.getCol(0)[0]+sweep[j].V[0], newCurvePointMatrix.getCol(0)[1]+sweep[j].V[1], newCurvePointMatrix.getCol(0)[2]+sweep[j].V[2]);

			newCurvePointMatrixForNormal = trajectoryMatrixForNormal * originalCurvePointMatrix;

			newN = Vector3f(newCurvePointMatrixForNormal.getCol(1)[0], newCurvePointMatrixForNormal.getCol(1)[1], newCurvePointMatrixForNormal.getCol(1)[2]);
			
			surface.VV.push_back(newV);
			surface.VN.push_back(-newN); // reverse
		}
		
	}

	Tup3u newFace;
	Tup3u newFace2;
	unsigned meshHeight = profile.size();
	// -1 is used to avoid segmentation fault
	for(unsigned i=0; i<sweep.size()-1; i++){
		for(unsigned j=0; j<meshHeight; j++){
			newFace = Tup3u(j+(i+1)*meshHeight+1, j+(i+1)*meshHeight,j+i*meshHeight+1);
			surface.VF.push_back(newFace);
			newFace2 = Tup3u(j+(i+1)*meshHeight,j+i*meshHeight,j+i*meshHeight+1);
			surface.VF.push_back(newFace2);
		}
	}
    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
