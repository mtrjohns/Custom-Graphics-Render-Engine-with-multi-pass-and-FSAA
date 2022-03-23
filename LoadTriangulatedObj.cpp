#include "stdafx.h"
//////////////////////////////////////////////////////////////////
////	LoadTriangulateObj Source File							//
////	Limited to loading triangulated obj files				//
////	Vertices (v), normals (vn) and texure coordinates (vt)	//
////	Sorting of faces ready to use in OpenGL draw calls		//
//////////////////////////////////////////////////////////////////

LoadTriangulatedObj::LoadTriangulatedObj()
{
	setName("NoNameSet");
}

LoadTriangulatedObj::LoadTriangulatedObj(char *filename)
{
	setName(filename);
	loadOBJ();
}


LoadTriangulatedObj::~LoadTriangulatedObj()
{

}

void LoadTriangulatedObj::setName(char *filename)
{
	strcpy_s(this->name, filename);
}

char * LoadTriangulatedObj::getName(void)
{
	return name;
}

// Load file into a string
string LoadTriangulatedObj::loadFile()
{
	// ensure ifstream objects can throw exceptions:
	fileStream.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		// Open File
		fileStream.open(this->name);

		// Add File content to Stringstream Buffer
		fileStringStream << fileStream.rdbuf();

		// Close File
		fileStream.close();

		// Convert Stringstream Buffer to string
		fileContentString = fileStringStream.str();
	}
	// throw error if unable to read file
	catch (ifstream::failure e)
	{
		cout << "ERROR: Unable to read File" << endl;
	}

	// return string
	return fileContentString;
}

void LoadTriangulatedObj::loadOBJ()
{
	
	loadFile();

	// used to store variables ready to be pushed back into vector
	vector<vec3> temp_vertices;
	vector<vec2> temp_texCoords;
	vector<vec3> temp_normals;

	// store whole line
	string line;

	// while /n has not been reached
	while (getline(fileStringStream, line))
	{
		// char at start of each line
		string identifier;

		// Use file stream declared in loadFile()
		fileStringStream >> identifier;

		// Load vertices
		if (identifier == "v")
		{
			vec3 vertex;

			// stream each vertex float(x, y, z) into a vector component
			fileStringStream >> vertex.x;
			fileStringStream >> vertex.y;
			fileStringStream >> vertex.z;

			vertexNum++;

			// store in temporary vector to be sorted by faces
			temp_vertices.push_back(vertex);
		}

		// Load texture coordinates
		else if (identifier == "vt")
		{
			vec2 texCoord;

			fileStringStream >> texCoord.x;
			fileStringStream >> texCoord.y;
			temp_texCoords.push_back(texCoord);

			texCoordsNum++;
		}

		// Load Normals
		else if (identifier == "vn")
		{
			vec3 normal;

			fileStringStream >> normal.x;
			fileStringStream >> normal.y;
			fileStringStream >> normal.z;
			temp_normals.push_back(normal);

			normalNum++;
		}
		// Load Faces and sort vertices, texcoords and normals
		else if (identifier == "f")
		{
			char junk[1]; // used to ignore '/'

			// number of faces
			facesNum++;

			// Temporary variables for this function only
			string vertex1, vertex2, vertex3;
			// this loops face values
			unsigned int vertexIndex[3], texCoordsIndex[3], normalIndex[3];

			// store indexes and ignore junk
			fileStringStream >> vertexIndex[0];
			// junk in the form of forward slash in faces
			fileStringStream >> junk[0];
			fileStringStream >> texCoordsIndex[0];
			fileStringStream >> junk[0];
			fileStringStream >> normalIndex[0];

			fileStringStream >> vertexIndex[1];
			fileStringStream >> junk[0];
			fileStringStream >> texCoordsIndex[1];
			fileStringStream >> junk[0];
			fileStringStream >> normalIndex[1];

			fileStringStream >> vertexIndex[2];
			fileStringStream >> junk[0];
			fileStringStream >> texCoordsIndex[2];
			fileStringStream >> junk[0];
			fileStringStream >> normalIndex[2];

			//cout << "faces = " << vertexIndex[0] << "/" << texCoordsIndex[0] << "/" << normalIndex[0] << " "
			//<< vertexIndex[1] << "/" << texCoordsIndex[1] << "/" << normalIndex[1] << " "
			//<< vertexIndex[2] << "/" << texCoordsIndex[2] << "/" << normalIndex[2] << endl;

			// Push indices into array ready for drawing per type 
			// (vertices, texure coordinates and normals)
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			texCoordIndices.push_back(texCoordsIndex[0]);
			texCoordIndices.push_back(texCoordsIndex[1]);
			texCoordIndices.push_back(texCoordsIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else
		{
			// do nothing
		}

	}

	// Setup each triangle values using the indices
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		// Store in buffers to be used directly by opengl
		vertices.push_back(temp_vertices[vertexIndices[i] - 1]);
		texCoords.push_back(temp_texCoords[texCoordIndices[i] - 1]);
		normals.push_back(temp_normals[normalIndices[i] - 1]);
	}
}

int LoadTriangulatedObj::getVertexAmount()
{
	return vertexNum;
}

int LoadTriangulatedObj::getTexCoordsAmount()
{
	return texCoordsNum;
}

int LoadTriangulatedObj::getNormalAmount()
{
	return normalNum;
}

int LoadTriangulatedObj::getPolygonsAmount()
{
	return facesNum;
}