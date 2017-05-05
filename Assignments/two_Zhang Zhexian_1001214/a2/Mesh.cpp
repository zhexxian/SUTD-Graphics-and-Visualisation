#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
	ifstream meshFile(filename);
	string line;
	string s;
	Vector3f v;
	Tuple3u f;
	if(!meshFile)
	{
		cerr<< filename << " not found\a" << endl;
        exit(0);
	}
	cout << "*** loading mesh ***" << endl;
	if(meshFile.is_open())
	{
		while(getline(meshFile,line)){
			stringstream ss(line);
			ss>>s;
			if(s=="v"){
				ss>>v[0]>>v[1]>>v[2];
				bindVertices.push_back(v);
			}
			else if(s=="f"){
				ss>>f[0]>>f[1]>>f[2];
				faces.push_back(f);
			 }
			else{
				cout << "Unable to open file"; 
			}
		}
		meshFile.close();
	}
	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".
	Tuple3u face;
	
	Vector3f vertex1;
	Vector3f vertex2;
	Vector3f vertex3;
	
	Vector3f normal1;
	Vector3f normal2;
	Vector3f normal3;
	for(int i=0; i<faces.size(); i++){
		face = faces[i];
		vertex1 = currentVertices[face[0]-1];
		vertex2 = currentVertices[face[1]-1];
		vertex3 = currentVertices[face[2]-1];

		normal1 = Vector3f::cross(vertex2-vertex1, vertex3-vertex1);
		normal2 = Vector3f::cross(vertex3-vertex2, vertex1-vertex2);
		normal3 = Vector3f::cross(vertex1-vertex3, vertex2-vertex3);

		normal1.normalize();
		normal2.normalize();
		normal3.normalize();

		glBegin(GL_TRIANGLES);

		glNormal3d(normal1[0], normal1[1], normal1[2]);
		glVertex3d(vertex1[0], vertex1[1],vertex1[2]);
		glNormal3d(normal2[0], normal2[1], normal2[2]);
		glVertex3d(vertex2[0], vertex2[1],vertex2[2]);
		glNormal3d(normal3[0], normal3[1], normal3[2]);
		glVertex3d(vertex3[0], vertex3[1],vertex3[2]);

		glEnd();
	}
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments

	
	// list of vertex to joint attachments
	// each element of attachments is a vector< float > containing
	// one attachment weight per joint
	// std::vector< std::vector< float > > attachments;

	ifstream attachmentFile(filename);
	string line;
	if(!attachmentFile)
	{
		cerr<< filename << " not found\a" << endl;
        exit(0);
	}
	cout << "*** loading attachment weight ***" << endl;
	if(attachmentFile.is_open())
	{
		while(getline(attachmentFile,line)){
			stringstream ss(line);
			vector<float> weights;
			float weight;
			weights.push_back(0);
			for(int i=1; i<numJoints-1; i++){
				ss>>weight;
				weights.push_back(weight);
			} 
			attachments.push_back(weights);
		}
		attachmentFile.close();
	}

}
