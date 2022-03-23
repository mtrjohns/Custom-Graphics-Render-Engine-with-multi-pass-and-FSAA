uniform sampler2D	grabTexture;
varying vec2 	texCoord;
uniform float	ScreenHeight;
uniform float	ScreenWidth;

void main(void)
{	
	gl_Position 	= ftransform();
	texCoord	= gl_MultiTexCoord0.xy;
}