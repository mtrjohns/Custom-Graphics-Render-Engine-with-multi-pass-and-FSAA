// colour vertex shader
// Andy Baker
// Digital Whorehouse Productions 2008

uniform float	Intensity;

varying vec2 	texCoord;

void main(void)
{	
	gl_Position 	= ftransform();
	texCoord	= gl_MultiTexCoord0.xy;
}