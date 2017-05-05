#include "SkeletalModel.h"

#include <FL/Fl.H>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
	string line;
	ifstream skeletonFile(filename);
    if (!skeletonFile)
    {
		cerr<< filename << " not found\a" << endl;
        exit(0);
    }

	cout << "*** loading skeleton ***" << endl;

	if (skeletonFile.is_open())
	{
		while ( getline (skeletonFile,line) )
		{
			float x;
			float y;
			float z;
		
			int index;
			stringstream ss(line); 
			ss >> x >> y >> z >> index;
		
			Joint *joint = new Joint;
			joint->transform = Matrix4f(
					1, 0, 0, x,
					0, 1, 0, y,
					0, 0, 1, z,
					0, 0, 0, 1
				);
			/* Alternative definition for translation matrix:
			ss >> v[0] >> v[1] >> v[2] >> index;
			Vector3f v;
			joint->transform = Matrix4f::translation(v);
			*/
			m_joints.push_back(joint);
			if(index == -1){
				m_rootJoint = joint;
			}
			else{
				m_joints[index]->children.push_back(joint);
			}
		}
	skeletonFile.close();
	}
	
	else 
	{
		cout << "Unable to open file"; 
	}
}

//Recursive helper function for drawJoints()
void SkeletalModel::helperDrawJoints(Joint* parentJoint, MatrixStack matrixStack){
	if(parentJoint->children.size() <= 0){
		matrixStack.push(parentJoint->transform);
		glLoadMatrixf(matrixStack.top());
		glutSolidSphere(0.025f, 12, 12);
		matrixStack.pop();
	}
	else {		
		matrixStack.push(parentJoint->transform);
		for(int i=0; i<parentJoint->children.size(); i++){
			helperDrawJoints(parentJoint->children[i], matrixStack);
		}	
		glLoadMatrixf(matrixStack.top());
		glutSolidSphere(0.025f, 12, 12);
		matrixStack.pop();
	}
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.

	helperDrawJoints(m_rootJoint, m_matrixStack);	

}

//Recursive helper function for drawSkeleton()
void SkeletalModel::helperDrawSkeleton(Joint* parentJoint, MatrixStack matrixStack){
	//At each joint, you should draw a box between the joint and the joint's parent (unless it is the root node).
	matrixStack.push(parentJoint->transform);
	if(parentJoint->children.size() <= 0){
	}
	else {
		//For the translation, scaling, and rotation of the box primitive, you must push the transforms 
		//onto the stack before calling glutSolidCube, but you must pop it off before drawing any of its 
		//children, as these transformations are not part of the skeleton hierarchy.
		for(int i=0; i<parentJoint->children.size(); i++){
			Matrix4f translationMatrix = Matrix4f::translation(0,0,0.5);

			Vector3f offset = parentJoint->children[i]->transform.getCol(3).xyz();
			float interJointDistance = offset.abs();
			Matrix4f scalingMatrix = Matrix4f::scaling(0.025,0.025,interJointDistance);

			Vector3f z = Vector3f(offset.normalized()[0], offset.normalized()[1], offset.normalized()[2]);
			//Vector3f z = offset.normalized();
			Vector3f rnd = Vector3f(0, 0, 1);
			Vector3f y = Vector3f::cross(rnd, z).normalized();
			//Vector3f x = Vector3f::cross(y, z).normalized();
			Matrix4f rotationMatrix = Matrix4f::rotation(y,acos(z[2]));

			Matrix4f transformationMatrix = rotationMatrix * scalingMatrix * translationMatrix;

			matrixStack.push(transformationMatrix);
			glLoadMatrixf(matrixStack.top());
			glutSolidCube(1.0f);
			matrixStack.pop();
			helperDrawSkeleton(parentJoint->children[i], matrixStack);
		}	
	}
	matrixStack.pop();
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.

	helperDrawSkeleton(m_rootJoint, m_matrixStack);

}


void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.
	Joint* joint = m_joints[jointIndex];
	Matrix4f rotationMatrix = Matrix4f::rotateZ(rZ) * Matrix4f::rotateY(rY) * Matrix4f::rotateX(rX);
	
	Matrix4f transformationMatrix = joint->transform;
	transformationMatrix.setSubmatrix3x3(0,0,rotationMatrix.getSubmatrix3x3(0,0));
	joint->transform = transformationMatrix;
}


//Recursive helper function for computeBindWorldToJointTransforms()
void SkeletalModel::helperComputeBindWorldToJointTransforms(Joint* parentJoint, MatrixStack matrixStack){
	//Set the bindWorldToJointTransform matrix of each Joint.
	matrixStack.push(parentJoint->transform);
	if(parentJoint->children.size() <= 0){
		parentJoint->bindWorldToJointTransform = matrixStack.top();
		matrixStack.pop();
	}
	else {
		for(int i=0; i<parentJoint->children.size(); i++){
			helperComputeBindWorldToJointTransforms(parentJoint->children[i], matrixStack);
		}
		parentJoint->bindWorldToJointTransform = matrixStack.top();
		matrixStack.pop();
	}
	matrixStack.pop();
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.

	helperComputeBindWorldToJointTransforms(m_rootJoint, m_matrixStack);
}


//Recursive helper function for updateCurrentJointToWorldTransforms()
void SkeletalModel::helperUpdateCurrentJointToWorldTransforms(Joint* parentJoint, MatrixStack matrixStack){
	//Update the currentJointToWorldTransform matrix of each Joint
	matrixStack.push(parentJoint->transform);
	if(parentJoint->children.size() <= 0){
		parentJoint->currentJointToWorldTransform = matrixStack.top();
		matrixStack.pop();
	}
	else {
		for(int i=0; i<parentJoint->children.size(); i++){
			helperUpdateCurrentJointToWorldTransforms(parentJoint->children[i], matrixStack);
		}
		parentJoint->currentJointToWorldTransform = matrixStack.top();
		matrixStack.pop();
	}
	matrixStack.pop();

}


void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.

	helperUpdateCurrentJointToWorldTransforms(m_rootJoint, m_matrixStack);

}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.
	
	//update the current position of each vertex in m_mesh.currentVertices according to the current 
	//pose of the skeleton by blending together transformations of your bind pose vertex positions 
	//in m_mesh.bindVertices. 
	
	float currentWeight;
	vector<float> weightPerVertex;
	Vector4f currentVertex;
    Vector4f newVertex;
	Vector4f transformationUpdate;
	
	for(int i=0; i<m_mesh.currentVertices.size(); i++)
	{
		weightPerVertex  = m_mesh.attachments[i];
		currentVertex = Vector4f(m_mesh.bindVertices[i][0], m_mesh.bindVertices[i][1],m_mesh.bindVertices[i][2],1);
		newVertex = Vector4f(0,0,0,0);
		

		for(int j=0; j<weightPerVertex.size(); j++){
			//w
			currentWeight = m_mesh.attachments[i][j];

			//wTB^(-1)p
			transformationUpdate = (m_joints[j]->currentJointToWorldTransform)*(m_joints[j]->bindWorldToJointTransform.inverse())*currentVertex;

			//w1T1B1^(-1)p + w2T2B2^(-1)p + ...
			newVertex = newVertex+currentWeight*transformationUpdate;
			
		}
		m_mesh.currentVertices[i] = 10*newVertex.xyz();
	}
	
	

}



