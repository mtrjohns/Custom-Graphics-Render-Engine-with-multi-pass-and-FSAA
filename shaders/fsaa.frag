/************************************************************
*** Full Screen Antialiasing + Commented out C++ Function ***
************************************************************/

uniform sampler2D	grabTexture;

varying vec2 	texCoord;
uniform float	ScreenHeight;
uniform float	ScreenWidth;

void main(void)
{
	// temporary variables
	vec2 tc[9];
	vec4 colour = vec4(0.0,0.0,0.0,1.0);
	
	// convert texcoords into screen space
	tc[0] = vec2(texCoord.xy * ScreenWidth);
	
	// get neighbouring texture coordinates
	tc[1] = vec2(tc[0].x - 1.0, tc[0].y);
	tc[2] = vec2(tc[0].x + 1.0, tc[0].y);
	tc[3] = vec2(tc[0].x, tc[0].y - 1.0);
	tc[4] = vec2(tc[0].x, tc[0].y + 1.0);
	tc[5] = vec2(tc[0].x - 1.0, tc[0].y - 1.0);
	tc[6] = vec2(tc[0].x - 1.0, tc[0].y + 1.0);
	tc[7] = vec2(tc[0].x + 1.0, tc[0].y - 1.0);
	tc[8] = vec2(tc[0].x + 1.0, tc[0].y + 1.0);
	
	// Convert texcoords back to texture space
	for( int i = 0; i < 9; i++)
	{
		tc[i] = tc[i] / ScreenWidth;
	}
	
	// sum colour values for surrounding pixels
	for( int i = 0; i < 9; i++ )
	{
		colour = colour + vec4(texture2D(grabTexture, tc[i]));
	}
	
	// convert colours pixels to 0.0 - 1.0 values
	colour = colour / 9.0;
	
	// output final pixel colour
	gl_FragColor = vec4( vec3(colour), 1.0 );
}

// C++ Function to call
/*
void cRenderClass::FSAA(float screenWidth, float screenHeight, cShader *pList)
{
	glEnable(GL_TEXTURE_2D);

	glReadBuffer(GL_BACK);
	// Texture memory
	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	// Texture the size of the screen
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_sw, m_sh, 0);

	glBindTexture(GL_TEXTURE_2D, m_tex[0]);
	glDrawBuffer(GL_BACK);

	// render selected texture map here..
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	// Shader for .vert/.frag files
	int shdr = 6;

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

	// Full screen quad
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
}
*/