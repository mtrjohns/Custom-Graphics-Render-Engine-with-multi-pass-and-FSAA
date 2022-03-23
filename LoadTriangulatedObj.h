#pragma once
//////////////////////////////////////////////////////////////////
////	LoadTriangulateObj Header File							//
////	Limited to loading triangulated obj files				//
////	Vertices (v), normals (vn) and texure coordinates (vt)	//
////	Sorting of faces ready to use in OpenGL draw calls		//
//////////////////////////////////////////////////////////////////

////////////////////////////////////
//// Dependencies
////////////////////////////////////
//// vec3 struct
//// vec2 struct
//// #include <stdio.h>
//// #include <tchar.h>
//// #include <iostream>
//// #include <string>
//// #include <fstream>
//// #include <sstream>
#include <vector>

using std::string; using std::ifstream; using std::stringstream;
using std::cout; using std::endl;
using std::vector; using std::getline;

class LoadTriangulatedObj
{
private:
	// Maximum storage value
	static const unsigned int NAME_MAX = 128;
	// Array to store file name
	char name[NAME_MAX];
	bool loadedFlag;

	// open file variables
	string fileContentString;
	ifstream fileStream;
	stringstream fileStringStream;
	const char *fileContentchar;

public:
	LoadTriangulatedObj();
	LoadTriangulatedObj(char *filePath);
	~LoadTriangulatedObj();

	void setName(char*);
	char* getName(void);

	vector<vec3> vertices, normals;
	vector<vec2> texCoords;
	int vertexNum = 0;
	int texCoordsNum = 0;
	int normalNum = 0;
	int facesNum = 0;
	
	vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;

	string loadFile();
	void loadOBJ();

	int getVertexAmount();
	int getTexCoordsAmount();
	int getNormalAmount();
	int getPolygonsAmount();
};

