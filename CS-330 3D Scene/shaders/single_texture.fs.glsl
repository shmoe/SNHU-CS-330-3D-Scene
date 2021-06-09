#version 330 core
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform sampler2D texture;

void main()
{
	// calculate ambient lighting
	float ambient_strength = 0.1;
	vec3 ambient_color = vec3(1.0, 1.0, 1.0);			// assume ambient lighting to be white (typical)
	vec3 ambient = ambient_color * ambient_strength;

	// calculate diffuse lighting
	vec3 norm = normalize(Normal);						// normalize Normal vector incase does not already
															// have a magnitude of 1
	vec3 lightDir = normalize(lightPos - FragPos);		// calculate direction of ray hitting fragment and
															// normalize the result
	float diff = max(dot(norm, lightDir), 0.0);			// calculate how bright the fragment should be based
															// on the angle between the normal and ray of light
	vec3 diffuse = diff * lightColor;

	// calculate specular lighting

	// calculate fragment color
	FragColor = vec4(ambient + diffuse, 1.0) * texture(texture, TexCoord);
}