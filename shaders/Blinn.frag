// Blinn Phong fragment shader
varying vec3 FragPos;
varying vec3 Normal;

struct Material
{
	// Reflectivity and object material properties
	vec3 Ia;     // colour of object material
	vec3 Kd;     // diffuse reflection coefficient ( diffuse incident light scattered reflection )
	vec3 Ks;     // specular reflection coefficient
	float Ns;    // specular shininess
};


struct Light
{
	vec3 Ka; 		// ambient reflection coefficient
	vec3 Il;  		// diffuse light intensity
	vec3 lightPos; 	// light position
};

// uniforms
uniform Material material;
uniform Light light;
uniform vec3 viewPosition;

void main(void)
{	
	/////////////////////////////////////
	//		Lighting calculations		/
	/////////////////////////////////////
		
	// get unit vector for the normal
	vec3 normal = normalize(Normal);
	
	// calculate light direction unit vector
	vec3 lightDirection = normalize(vec3(light.lightPos - FragPos));
	
	// View direction
	vec3 viewDirection = normalize(viewPosition - FragPos);
	
	// Calculate halfwayVector for blinn phong
	vec3 halfwayVector = normalize(lightDirection + viewDirection);  

	/////////////////////////////////////////////
	// AMBIENT
	/////////////////////////////////////////////
	// ambient lighting contribution (light colour(Ka) * object colour(Ia) )
	vec3 ambient = light.Ka * material.Ia;

	
	/////////////////////////////////////////////
	// DIFFUSE
	/////////////////////////////////////////////
	
	// Diffuse ( Material diffuse * Intensity of light source(Il) * dot(normal * light direction) )
	// Material diffuse used when part of more than just a diffuse calculation
	// final 0.0 variable used to only apply when diffuse is above 0.0
	// Kd * Il * incident
	vec3 diffuse = material.Kd * light.Il * vec3(max(dot( normal, lightDirection), 0.0));
	
	
	/////////////////////////////////////////////
	// SPECULAR
	/////////////////////////////////////////////
	// Material specular reflection(Ks) * Light intensity (Il) * (normal * halfwayVector)^shininess(Ns)
	vec3 specular = material.Ks * light.Il * pow(max(dot(normal, halfwayVector), 0.0), material.Ns);
	
	
	/////////////////////////////////////////////
	// RESULTS
	/////////////////////////////////////////////
	vec3 lightingResult = (ambient + diffuse + specular);

	// output result
	gl_FragColor = vec4(lightingResult, 1.0);
}