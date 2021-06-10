#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform float ambientStrength;
uniform vec3 ambientColor;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float specularStrength;
uniform vec3 viewPos;
uniform sampler2D aTexture;

void main()
{
	// calculate ambient lighting
	vec3 ambient = ambientColor * ambientStrength;

	// calculate diffuse lighting
	vec3 norm = normalize(Normal);						// normalize Normal vector incase does not already
															// have a magnitude of 1
	vec3 lightDir = normalize(lightPos - FragPos);		// calculate direction of ray hitting fragment and
															// normalize the result
	float diff = max(dot(norm, lightDir), 0.0);			// calculate how bright the fragment should be based
															// on the angle between the normal and ray of light
	vec3 diffuse = diff * lightColor;

	// calculate specular lighting
	vec3 viewDir = normalize(viewPos - FragPos);				// calculate view direction and normalize
	vec3 reflectDir = reflect(-lightDir, norm);					// calculate direction of reflected light
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);	// calculate specular constant
	vec3 specular = specularStrength * spec * lightColor;
	
	// calculate fragment color
	FragColor = vec4(ambient + diffuse + specular, 1.0) * texture(aTexture, TexCoord);
}