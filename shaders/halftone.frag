// half tone fragment shader
uniform sampler2D	grabTexture;
varying vec2 	texCoord;
uniform float 	GridSize;
uniform float	ScreenHeight;
uniform float	ScreenWidth;
uniform bool 	ApplyObjectColour;
float radius;

void main(void)
{
	// temporary variables
	vec2 tc;
	vec2 startPixel;
	// setup variables for circle sizes
	float squareSize = ScreenWidth / GridSize;
	float currentXSquareEnd = squareSize;
	float currentYSquareEnd = squareSize;
	float CircleCentreOffset = squareSize / 2;
	
	vec2 circleCentre; //circles to split up screen
	
	vec4 colour = vec4(1.0, 1.0, 1.0, 1.0);

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
	for(int i = 0; i < tc.x; i++)
	{

		// increase step everytime a new box is entered
		if(i > currentXSquareEnd)
		{
			currentXSquareEnd = currentXSquareEnd + squareSize;
			startPixel.x = i + 1;
		}

	}
	
	// check where along y-axis current pixel is
	for(int i = 0; i < tc.y; i++)
	{
		// increase step everytime a new box is entered
		if(i > currentYSquareEnd)
		{
			currentYSquareEnd = currentYSquareEnd + squareSize;
			startPixel.y = i + 1;
		}
	}
	
	// check where along y-axis current pixel is
	if(tc.x < currentXSquareEnd && tc.y < currentYSquareEnd)
	{
		circleCentre = vec2(currentXSquareEnd - CircleCentreOffset, currentYSquareEnd - CircleCentreOffset);
		
		if(ApplyObjectColour)
		{
			// Apply object colour
			colour = vec4(texture2D(grabTexture, vec2(tc.x / ScreenWidth, tc.y / ScreenHeight)));
		}
		// use 2nd pass intensity value from texture file gridsizedintensity.frag
		radius = texture2D(grabTexture, vec2(startPixel.x / ScreenWidth, startPixel.y / ScreenHeight)).b;
		radius = radius * CircleCentreOffset;
	}
	
	// ensure radius is clamped to half of square size
	if(radius > CircleCentreOffset)
	{
		radius = CircleCentreOffset - 2;
	}
	
	// calculate radius using line distance check
    float tempX = tc.x - circleCentre.x;
    float tempY = tc.y - circleCentre.y;
    float lengthResult = tempX * tempX + tempY * tempY;
	
	float radiussq = radius * radius;
	
	if(lengthResult >= radiussq)
	{
		colour = vec4(0.0, 0.0, 0.0, 1.0);
	}

	// set output colour to colour variable
	gl_FragColor = vec4(colour);
}