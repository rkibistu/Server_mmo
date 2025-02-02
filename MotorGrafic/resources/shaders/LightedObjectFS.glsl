#version 330

// Input
in vec3 world_position;
in vec3 world_normal;
in vec2 v_texCoord;

// Uniforms for light properties
uniform float point_lights_count;
uniform vec3 point_light_positions[10];
uniform vec3 point_light_colors[10];
uniform float spot_lights_count;
uniform vec3 spot_light_positions[10];
uniform vec3 spot_light_colors[10];
uniform vec3 spot_light_directions[10];
uniform float spot_light_angles[10];
uniform vec3 eye_position;
uniform vec3 material_ka;
uniform vec3 material_kd;
uniform vec3 material_ks;
uniform int material_shininess;
//directional light
uniform vec3 directional_light_direction = vec3(1.0f, -1.0f, 1.0f);
uniform vec3 directional_light_color = vec3(1, 1, 1);

uniform sampler2D u_Texture;

// Output
layout(location = 0) out vec4 out_color;

vec3 ComputePhongIllumination(vec3 light_position) {
	vec3 N = normalize(world_normal);
	vec3 L = normalize(light_position - world_position);


	// Compute the diffuse component
	vec3 diffuse_component = material_kd * max(dot(N, L), 0.0);

	// Compute the specular component
	vec3 V = normalize(eye_position - world_position);
	vec3 R = reflect(-L, N);
	float primeste_lumina = (dot(normalize(world_normal), L) > 0) ? 1 : 0;
	vec3 specular_component = material_ks * primeste_lumina * pow(max(dot(V, R), 0.0), material_shininess);

	// Compute the final illumination as the sum of the diffuse and specular components
	vec3 illumination = diffuse_component + specular_component;;

	return illumination;
}

float ComputeDistanceAttenuation(vec3 light_position, vec3 point_position) {
	// Compute the light attenuation factor based on the distance
	// between the position of the illuminated point and the position of the light source.
	float d = distance(light_position, point_position);

	return 1.0 / (d * d + 1.0);
}

vec3 ComputePointLightSourcesIllumination() {
	vec3 lights_illumination = vec3(0);

	for (int i = 0; i < 10; i++) {

		if (i >= point_lights_count)
			break;

		vec3 light_position = point_light_positions[i];
		vec3 light_color = point_light_colors[i];

		vec3 light_illumination = ComputePhongIllumination(light_position);
		float illumination_attenuation = ComputeDistanceAttenuation(light_position, world_position);

		// Add to the illumination of all light sources the result
		// of multiplying the illumination of the light source from the current iteration
		// with the attenuation of the illumination and the color of the illumination.
		lights_illumination += illumination_attenuation * light_color * light_illumination;
	}

	return lights_illumination;
}

vec3 ComputeSpotLightSourcesIllumination() {
	vec3 lights_illumination = vec3(0);

	for (int i = 0; i < 10; i++) {

		if (i >= spot_lights_count)
			break;

		vec3 light_position = spot_light_positions[i];
		vec3 light_color = spot_light_colors[i];
		vec3 light_direction = spot_light_directions[i];
		float cut_off_angle = spot_light_angles[i];

		vec3 L = normalize(light_position - world_position);
		float cos_theta_angle = dot(-L, light_direction);
		float cos_phi_angle = cos(cut_off_angle);
		if (cos_theta_angle > cos_phi_angle) {
			vec3 light_illumination = ComputePhongIllumination(light_position);
			float illumination_attenuation = ComputeDistanceAttenuation(light_position, world_position);

			// Compute the attenuation factor specific to the spot light source
			float spot_linear_att_factor = (cos_theta_angle - cos_phi_angle) / (1.0f - cos_phi_angle);
			float quadratic_spot_light_att_factor = pow(spot_linear_att_factor, 2);

			// Add to the illumination of all light sources the result
			// of multiplying the illumination of the light source from the current iteration
			// with the attenuation of the illumination, the attenuation factor specific
			// to the light spot source and the color of the illumination.
			lights_illumination += illumination_attenuation * quadratic_spot_light_att_factor * light_color * light_illumination;
		}
	}

	return lights_illumination;
}

vec3 ComputeAmbientComponent() {
	vec3 global_ambient_color = vec3(0.25f);

	// Compute the ambient component of global illumination
	vec3 ambient_component = material_ka * global_ambient_color;

	return ambient_component;
}

vec3 ComputeDirectionalLightIllumination() {
	vec3 N = normalize(world_normal);
	vec3 L = normalize(-directional_light_direction);  // Direction towards the light

	// Diffuse component
	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = material_kd * directional_light_color * diff;

	// Specular component
	vec3 V = normalize(eye_position - world_position);  // View direction
	vec3 R = reflect(-L, N);                            // Reflection of L around the normal
	float spec = pow(max(dot(V, R), 0.0), material_shininess);
	vec3 specular = material_ks * directional_light_color * spec;

	// Combine diffuse and specular
	return diffuse + specular;
}

void main() {
	vec3 illumination = ComputePointLightSourcesIllumination()
		+ ComputeSpotLightSourcesIllumination()
		+ ComputeAmbientComponent()
		+ ComputeDirectionalLightIllumination();


	vec4 texColor = texture2D(u_Texture, v_texCoord);
	if (texColor.a < 0.2)
		discard;

	vec3 finalColor = vec3(texColor) * illumination;

	out_color = vec4(finalColor, 1);
}
