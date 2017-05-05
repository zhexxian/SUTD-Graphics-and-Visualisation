#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

	Group(){
		this->num_objects = 0;
	}
	
	Group( int num_objects ){
		this->num_objects = num_objects;
	}

	//destructor
	~Group(){
		for (int i=0; i < groupList.size(); i++) {
			delete groupList[i];
		}
	}

	virtual bool intersect( const Ray& r , Hit& h , float tmin ) {
		for(int i=0; i<groupList.size(); i++){
			Object3D* obj = groupList[i];
			if(obj->intersect(r, h, tmin)){
				return true;
			}
		}
		return false;
	}

	
	void addObject( int index , Object3D* obj ){
		if (index != groupList.size()) {
			cerr << "Group object index error" << endl;
		}
		if (groupList.size() > num_objects) {
			cerr << "There are more objects in group than it is defined" << endl;
		}
		groupList.push_back(obj);
	}

	int getGroupSize(){ 
		return this->num_objects;
	}

private:
	vector<Object3D*> groupList;
	int num_objects;
};

#endif
	
