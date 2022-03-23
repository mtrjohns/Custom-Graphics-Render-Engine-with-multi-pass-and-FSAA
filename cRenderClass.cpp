// cRenderClass.cpp
// 
//////////////////////////////////////////////////////////////////////////////////////////
// includes 
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////
// Declarations 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass graphics( SCREEN_WIDTH, SCREEN_HEIGHT );
void processSpecialKeys(int key, int x, int y);
void processKeys(unsigned char key, int x, int y);

unsigned int	m_tex[256];

//////////////////////////////////////////////////////////////////////////////////////////
// cRenderClass() - constructor 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass::cRenderClass(const int w, const int h)
{
	m_sw = w;
	m_sh = h;

	int total_count = w*h*4;

	m_buffer = new float[total_count];

	for( int i=0; i<total_count; i++ )
	{
		m_buffer[i] = 1;
	}

	m_point_size = 1;

	m_OPENGL_2_0_SUPPORTED = true;

	// generate a number of textures...
	glGenTextures(2, m_tex);
}

//////////////////////////////////////////////////////////////////////////////////////////
// cRenderClass() - destructor 
//////////////////////////////////////////////////////////////////////////////////////////
cRenderClass::~cRenderClass()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
// loop() - enters game loop
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::loop()
{
	// process input from keyboard
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

	glutMainLoop();
}

extern cShaderInfo	ShaderInfo;

//////////////////////////////////////////////////////////////////////////////////////////
// initShaders() - init GLSL shaders
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::initShaders()
{
	GLenum err = glewInit();

	if( GLEW_OK != err )
	{
		printf("Error: Glew is not supported\n\n");
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		printf("OpenGL 2.0 is supported\n\n");
	}
	else
	{
		printf("Error: OpenGL 2.0 is not supported\n\n");
		m_OPENGL_2_0_SUPPORTED = false;
	}

	ShaderInfo.create();
}

//////////////////////////////////////////////////////////////////////////////////////////
// initialize glut stuff
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::create(int argc, _TCHAR* argv[])
{
	// initialise the glut library
	glutInit(&argc, argv);

	// set up the initial display mode
	// need both double buffering and z-buffering
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// set the initial window position
	glutInitWindowPosition(100, 100);

	// set the initial window size
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// create and name the window
	glutCreateWindow("Daily Halftone Shader - MJohns - P147264");

	// reshape callback for current window
	glutReshapeFunc(winReshapeFunc);

	// set display callback for current window
	glutDisplayFunc(renderScene);	

	// set up the global idle callback
	glutIdleFunc(update);

	initShaders();	
	Torus = new LoadTriangulatedObj("objects\\Torus.obj");
	Lamp = new LoadTriangulatedObj("objects\\Lamp.obj");
	Fly = new LoadTriangulatedObj("objects\\fly.obj");
}

//////////////////////////////////////////////////////////////////////////////////////////
// drawPixel() - draw a pixel into an off-screen buffer
//				 if m_point_size>1 then draw a block of pixels
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::drawPixel(int x, int y)
{
	int start, end;

	start = -(m_point_size/2);
	end	= (int)((m_point_size/2.0f) + 0.5);

	for( int i=start; i<end; i++ )
	{
		for( int j=start; j<end; j++ )
		{
			// reject values outside m_buffer range
			if( (x+j < 0) || (x+j >= m_sw) )
				continue;

			if( (y+i < 0) || (y+i >= m_sh) )
				continue;

			m_buffer[(((y+i)*m_sw+(x+j))*4) + 0] = m_colour.r;
			m_buffer[(((y+i)*m_sw+(x+j))*4) + 1] = m_colour.g;
			m_buffer[(((y+i)*m_sw+(x+j))*4) + 2] = m_colour.b;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// setViewport2D() - sets up the viewport ready for a screen grab
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::setViewport2D()
{
	glLoadIdentity();
		
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, m_sw, 0, m_sh);
	glMatrixMode(GL_MODELVIEW);

	// set up the viewport
	glViewport
	(
		0,							// lower left x position
		0,							// lower left y position
		m_sw,	// viewport width
		m_sh	// viewport height
	);
}

//////////////////////////////////////////////////////////////////////////////////////////
// setViewport3D() - sets up the viewport ready for a screen grab
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::setViewport3D()
{
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// Change to perspective viewing mode
	gluPerspective(45, m_sw / m_sh, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);

	// set up the viewport
	gluLookAt	(0.0, 0.0, 5.0,		// eye is at (0,0,5)
				0.0, 0.0, 0.0,		// center is at (0,0,0)
				0.0, 1.0, 0.0);		// up is in positive Y direction

}

float angle = 0;


//////////////////////////////////////////////////////////////////////////////////////////
// render() - 
//////////////////////////////////////////////////////////////////////////////////////////
void cRenderClass::render( int pass )
{
	if( pass == 0 )
	{
		setViewport3D();
		while( (angle += 0.4) >= 360.0f )
			angle-=360.0f;

		// disable shader program
		glUseProgram(0);

		// clear the back buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set depth test for 3D viewing of objects
		glEnable(GL_DEPTH_TEST);

		// Apply Blinn-Phong lighting lighting
		BlinnPhong(ShaderInfo.getList());

		/////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////	
		return;		
	}
 	else if(pass == 1)
	{
		if (halfTone == true)
		{
			// Apply intensity change
			GridBasedIntensity(gridSize, m_sw, m_sh, ShaderInfo.getList());
		}
		return;
	}
	else if(pass == 2)
	{
		if (halfTone == true)
		{
			// Apply Half Tone effect
			HalfTone(gridSize, m_sw, m_sh, ShaderInfo.getList());
		}
		return;
	}
	else
	{
		if (fsaa == true)
		{
			// Apply fullscreen antialiasing
			FSAA(m_sw, m_sh, ShaderInfo.getList());
		}
	}

	// finally swap the buffers of the current window
	glutSwapBuffers();
	
	// clear out the temp buffer
	clear();
}

// Blinn-Phong Lighting Shader (shader list)
void cRenderClass::BlinnPhong(cShader *pList)
{

	/////////////////////////////////////////////////////////////
	// Activate Lighting Shader
	/////////////////////////////////////////////////////////////
	int shdr = 1;

	if (shdr >= ShaderInfo.shaderCount())
	{
		printf("Error: Can't access requested shader\n");
		shdr = 0;
	}

	// Activate Shader to be used
	glUseProgram(pList[shdr].program());

	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("viewPosition")), 0.0, 0.0, 5.0);

	// material properties
	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("material.Ia")), 0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("material.Kd")), 0.9f, 0.5f, 0.4f); // Object colour
	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("material.Ks")), 0.5f, 0.5f, 0.5f);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "material.Ns"), 32.0f);

	// Light properties
	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("light.Ka")), 1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("light.Il")), 1.0f, 1.0f, 1.0f); // Light colour (Intensity)
	glUniform3f(glGetUniformLocation(pList[shdr].program(), ("light.lightPos")), lightPos.x, lightPos.y, lightPos.z);

	/////////////////////////////////////////////////////////////
	// perform transforms and rotations using OpenGL standard calls
	/////////////////////////////////////////////////////////////
	if (rotation == true)
	{
		glRotatef(angle, 1, 1, 1);
	}
	glTranslatef(0, 0, 0);

	// Draw Object call
	// Draw Torus
	if (torus == true)
	{
		DrawTorus();
	}

	// Draw Lamp
	if (rotation == true)
	{
		glRotatef(angle, 1, 1, 1);
	}
	glTranslatef(0, -1, 0);


	if (lamp == true)
	{
		DrawLamp();
	}

	// Draw Fly
	if (rotation == true)
	{
		glRotatef(angle, 1, 1, 1);
	}
	glTranslatef(0, 0, 0);


	if (fly == true)
	{
		DrawFly();
	}
	glUseProgram(0);
}

// Full Screen Antialiasing, Render to Texture
// (Screen Height, Screen Width, Shader List)
void cRenderClass::FSAA(float screenWidth, float screenHeight, cShader *pList)
{
	setViewport2D();
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	glReadBuffer(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, screenWidth, screenHeight, 0);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDrawBuffer(GL_BACK);

	// render selected texture map here..
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int shdr = 3;

	if (shdr >= ShaderInfo.shaderCount())
	{
		printf("Error: Can't access requested shader\n");
		shdr = 0;
	}
	// enable shader program..
	glUseProgram(pList[shdr].program());

	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ScreenWidth"), screenWidth);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ScreenHeight"), screenHeight);


	glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);

	glActiveTexture(GL_TEXTURE0 + m_tex[0]);

	glBegin(GL_QUADS);

	// render the final disordered image here..
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0, screenHeight);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(screenWidth, screenHeight);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(screenWidth, 0);

	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
}

// Calculate Grid Based Intensity, Render To Texture
// (Grid Size, Screen Height, Screen Width, Shader List)
void cRenderClass::GridBasedIntensity(float gridSize, float screenWidth, float screenHeight, cShader *pList)
{
	// Set viewport to 2D for render to texture
	setViewport2D();
	// ensure no depth test enabled
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	glReadBuffer(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, screenWidth, screenHeight, 0);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDrawBuffer(GL_BACK);

	// render selected texture map here..
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int shdr = 5;

	if (shdr >= ShaderInfo.shaderCount())
	{
		printf("Error: Can't access requested shader\n");
		shdr = 0;
	}

	// enable shader program..
	glUseProgram(pList[shdr].program());

	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ScreenWidth"), screenWidth);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ScreenHeight"), screenHeight);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "GridSize"), gridSize);

	glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);

	//cout << "Grid Size = " << gridSize << " * " << gridSize << endl;

	glActiveTexture(GL_TEXTURE0 + m_tex[0]);

	glBegin(GL_QUADS);

	// render the final disordered image here..
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0, screenHeight);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(screenWidth, screenHeight);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(screenWidth, 0);

	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
}

// Full Screen HalfTone Shader, Render To Texture
// (Grid Size, Screen Height, Screen Width, Shader List)
void cRenderClass::HalfTone(float gridSize, float screenWidth, float screenHeight, cShader *pList)
{
	// Set viewport to 2D for render to texture
	setViewport2D();
	// ensure no depth test enabled
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	glReadBuffer(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, screenWidth, screenHeight, 0);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDrawBuffer(GL_BACK);

	// render selected texture map here..
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int shdr = 4;

	if (shdr >= ShaderInfo.shaderCount())
	{
		printf("Error: Can't access requested shader\n");
		shdr = 0;
	}

	// enable shader program..
	glUseProgram(pList[shdr].program());

	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ScreenWidth"), screenWidth);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ScreenHeight"), screenHeight);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "GridSize"), gridSize);
	glUniform1f(glGetUniformLocation(pList[shdr].program(), "ApplyObjectColour"), applyObjectColour);

	glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);

	//cout << "Grid Size = " << gridSize << " * " << gridSize << endl;

	glActiveTexture(GL_TEXTURE0 + m_tex[0]);

	glBegin(GL_QUADS);

	// render the final disordered image here..
	glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0, screenHeight);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(screenWidth, screenHeight);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(screenWidth, 0);

	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
}

// Full Screen Intensity Shader, Render to Texture
// (Intensity(0.0 - 1.0), Screen Height, Screen Width, Shader List)
void cRenderClass::Intensity(float intensity, float screenWidth, float screenHeight, cShader *pList)
{
	// flatten the viewport - i.e. convert to image space
	setViewport2D();

	// Disable depth testing
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glReadBuffer(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_sw, m_sh, 0);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDrawBuffer(GL_BACK);

	// render selected texture map here..
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int shdr = 0;

	if (shdr >= ShaderInfo.shaderCount())
	{
		printf("Error: Can't access requested shader\n");
		shdr = 0;
	}

	// enable shader program..
	glUseProgram(pList[shdr].program());

	glUniform1i(pList[shdr].get_grabLoc(), (int)m_tex[0]);
	glUniform1f(pList[shdr].intensity(), intensity);

	glActiveTexture(GL_TEXTURE0 + m_tex[0]);

	//glLineWidth(2);

	glBegin(GL_QUADS);

	// render the final disordered image here..
	//glColor3f(1.0, 1.0, 1.0);

	glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(0, m_sh);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(m_sw, m_sh);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(m_sw, 0);

	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
}

// Draw Triangulated Torus Object
void cRenderClass::DrawTorus()
{
	/////////////////////////////////////////////////////////////
	// draw to screen using OpenGL standard calls
	/////////////////////////////////////////////////////////////
	glBegin(GL_TRIANGLES);

	// Draw Object
	for (int i = 0; i < Torus->vertexNum * 3; i++)
	{
		// Point 1
		// Normals before vertex to ensure smooth shading
		glNormal3f(Torus->normals[i].x,
			Torus->normals[i].y,
			Torus->normals[i].z);
		glVertex3f(Torus->vertices[i].x,
			Torus->vertices[i].y,
			Torus->vertices[i].z);

		i++;

		// Point 2
		glNormal3f(Torus->normals[i].x,
			Torus->normals[i].y,
			Torus->normals[i].z);
		glVertex3f(Torus->vertices[i].x,
			Torus->vertices[i].y,
			Torus->vertices[i].z);

		i++;

		// Point 3
		glNormal3f(Torus->normals[i].x,
			Torus->normals[i].y,
			Torus->normals[i].z);
		glVertex3f(Torus->vertices[i].x,
			Torus->vertices[i].y,
			Torus->vertices[i].z);
	}

	glEnd();
}

// Draw Triangulated Lamp Object
void cRenderClass::DrawLamp()
{
	/////////////////////////////////////////////////////////////
	// draw to screen using OpenGL standard calls
	/////////////////////////////////////////////////////////////
	glBegin(GL_TRIANGLES);

	// Draw Object
	for (int i = 0; i < Lamp->vertexNum * 3; i++)
	{
		// Point 1
		// Normals before vertex to ensure smooth shading
		glNormal3f(Lamp->normals[i].x,
			Lamp->normals[i].y,
			Lamp->normals[i].z);
		glVertex3f(Lamp->vertices[i].x,
			Lamp->vertices[i].y,
			Lamp->vertices[i].z);

		i++;

		// Point 2
		glNormal3f(Lamp->normals[i].x,
			Lamp->normals[i].y,
			Lamp->normals[i].z);
		glVertex3f(Lamp->vertices[i].x,
			Lamp->vertices[i].y,
			Lamp->vertices[i].z);

		i++;

		// Point 3
		glNormal3f(Lamp->normals[i].x,
			Lamp->normals[i].y,
			Lamp->normals[i].z);
		glVertex3f(Lamp->vertices[i].x,
			Lamp->vertices[i].y,
			Lamp->vertices[i].z);
	}

	glEnd();
}

// Draw Triangulated Lamp Object
void cRenderClass::DrawFly()
{
	/////////////////////////////////////////////////////////////
	// draw to screen using OpenGL standard calls
	/////////////////////////////////////////////////////////////
	glBegin(GL_TRIANGLES);

	// Draw Object
	for (int i = 0; i < Fly->vertexNum * 3; i++)
	{
		// Point 1
		// Normals before vertex to ensure smooth shading
		glNormal3f(Fly->normals[i].x,
			Fly->normals[i].y,
			Fly->normals[i].z);
		glVertex3f(Fly->vertices[i].x,
			Fly->vertices[i].y,
			Fly->vertices[i].z);

		i++;

		// Point 2
		glNormal3f(Fly->normals[i].x,
			Fly->normals[i].y,
			Fly->normals[i].z);
		glVertex3f(Fly->vertices[i].x,
			Fly->vertices[i].y,
			Fly->vertices[i].z);

		i++;

		// Point 3
		glNormal3f(Fly->normals[i].x,
			Fly->normals[i].y,
			Fly->normals[i].z);
		glVertex3f(Fly->vertices[i].x,
			Fly->vertices[i].y,
			Fly->vertices[i].z);
	}

	glEnd();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// winReshapeFunc() - gets called initially and whenever the window get resized
// resizing has been locked
//////////////////////////////////////////////////////////////////////////////////////////
void winReshapeFunc(GLint w, GLint h)
{
	// specify current matrix
	glMatrixMode(GL_PROJECTION);

	// load an identity matrix
	glLoadIdentity();	

	// create a projection matrix... i.e. 2D projection onto xy plane
	glOrtho( -SCREEN_WIDTH, SCREEN_WIDTH, -SCREEN_HEIGHT, SCREEN_HEIGHT, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set up the viewport
	glViewport
	(
		0,				// lower left x position
		0,				// lower left y position
		(GLsizei) SCREEN_WIDTH,	// viewport width
		(GLsizei) SCREEN_HEIGHT	// viewport height
	);
}

// process key input that uses special keys
void processSpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		// Increase blob amount
		gridSize = gridSize + 1;

		// Limit upper bounds of gridSize
		if (gridSize > 80)
		{
			gridSize = 80;
		}
		break;
	case GLUT_KEY_DOWN:
		// Decrease blob amount
		gridSize = gridSize - 1;

		// Limit lower bounds of gridSize
		if (gridSize < 35)
		{
			gridSize = 35;
		}
		break;
	default:
		break;
	}
}

// process keyboard input
void processKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
		// Quit Application
		case 27:
			exit(0);
			break;
		// Toggle Halftone Shader Pass
		case 'h':
			if (halfTone == true)
			{
				halfTone = false;
			}
			else
			{
				halfTone = true;
			}
			break;
		// Toggle Halftone Shader Object Colour
		case 'c':
			if (applyObjectColour == true)
			{
				applyObjectColour = false;
			}
			else
			{
				applyObjectColour = true;
			}
			break;
		// Toggle Halftone Shader Pass
		case 'a':
			if (fsaa == true)
			{
				fsaa = false;
			}
			else
			{
				fsaa = true;
			}
			break;
		// Torus Object
		case 't':
			if (torus == true)
			{
				torus = false;
			}
			else
			{
				torus = true;
			}
			break;
		// Fly Object
		case 'f':
			if (fly == true)
			{
				fly = false;
			}
			else
			{
				fly = true;
			}
			break;
		// Lamp Object
		case 'l':
			if (lamp == true)
			{
				lamp = false;
			}
			else
			{
				lamp = true;
			}
			break;
		// Toggle Object Rotation
		case 'r':
			if (rotation == true)
			{
				rotation = false;
			}
			else
			{
				rotation = true;
			}
			break;
		default:
			break;
	}
}