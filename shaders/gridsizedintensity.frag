// Grid Based Intensity fragment shader

uniform sampler2D	grabTexture;
varying vec2 	texCoord;
uniform float 	GridSize;
uniform float	ScreenHeight;
uniform float	ScreenWidth;

void main(void)
{
	
	vec3 colour = vec3(1.0, 0.0, 0.0);
	
	// temporary variables
	vec2 tc[3000];

	// setup variables for circle sizes
	float squareSize = ScreenWidth / GridSize;
	float currentXSquareEnd = squareSize;
	float currentYSquareEnd = squareSize;
	bool startX = false;
	bool startY = false;
	
	vec2 startingPixel = vec2(0.0, 0.0); // Starting pixel of current box
	
	vec4 intensity = vec4(0.0, 0.0, 0.0, 1.0);
	
	// required to work on laptop (old version of glsl???)
	float temp = texCoord.x * ScreenWidth;
	// convert texcoords into screen space
	tc.x = temp; // unit texCoords * ScreenWidth
	temp = texCoord.y * ScreenHeight;
	tc.y = temp; // unit texCoords * ScreenHeight
	
	// modulus % can be used to increase optimisation of this code
	// 1) current box can be found using int values current texcoord / squaresize
	// 2) current pixel could be found with texcoord % square size
	
	// check where along x-axis current pixel is
	for(int i = 0; i < tc[0].x; i++)
	{
		// increase step everytime a new box is entered
		if(i > currentXSquareEnd)
		{
			currentXSquareEnd = currentXSquareEnd + squareSize;
			startX = true; // current pixel is starting pixel of current box
		}
		else
		{
			startX = false;
		}
	}
	// check where along y-axis current pixel is
	for(int i = 0; i < tc[0].y; i++)
	{
		// increase step everytime a new box is entered
		if(i > currentYSquareEnd)
		{
			currentYSquareEnd = currentYSquareEnd + squareSize;
			startY = true; // current pixel is starting pixel of current box
		}
		else
		{
			startY = false;
		}
	}
	
	if(startX == true && startY == true)
	{
		int xCount = 0;
		
		// scan along y axis
		for(int j = 0; j < squareSize; j++)
		{
			// scan along x axis
			for (int i = 1; i < squareSize; i++)
			{
				// Scan X line from start of box to end of box (squareSize)
				tc[i] = vec2(tc[0].x + i, tc[0].y + j);
				
				// convert back to tex coords
				tc[i] = tc[i] / ScreenWidth;
				
				// add colour to starting pixel
				intensity = intensity + vec4(texture2D(grabTexture, tc[i]));

				// increment x counter
				xCount++;
			}
		}
		// divide intensity by amount of pixels
		intensity.b = intensity.b / xCount;

		// apply final intensity calculation to alpha of starting pixel
		intensity.b = intensity.r + intensity.g + intensity.b + intensity.a;
		
		// apply to starting pixel in texture
		gl_FragColor = intensity;
	}
	else
	{	
		// set output colour to colour variable
		gl_FragColor = intensity;
	}
	
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}