// Blinn Phong vertex shader
varying vec3 FragPos;
varying vec3 Normal;

void main()
{
	Normal = gl_NormalMatrix * gl_Normal;
	gl_Position = ftransform();
	FragPos =  gl_Vertex.xyz;
}