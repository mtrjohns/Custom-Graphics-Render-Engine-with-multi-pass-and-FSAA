// colour fragment shader
// Andy Baker
// Digital Whorehouse Productions 2008

uniform float		Intensity;
uniform sampler2D	grabTexture;

varying vec2 		texCoord;

void main(void)
{

	vec4	grab = vec4(texture2D(grabTexture, texCoord.xy));

	vec3	colour = vec3(grab.xyz * Intensity);


	gl_FragColor = vec4( vec3(colour), 1.0 );

}