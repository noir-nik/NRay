float DistributionGGX(float dotNH, float roughness) {
	const float PI = 3.14159265359;
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = (dotNH * dotNH * (alpha2 - 1.0) + 1.0);
	return alpha2 / (PI * denom * denom);
}

float SchlickSmithGGX(float dotN, float k){
	return dotN / (dotN * (1.0 - k) + k);
}

float GeometrySchlickGGX(float dotN, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	return dotN / (dotN * (1.0 - k) + k);
}

float GeometrySmith(float dotNL, float dotNV, float roughness) {
	float GL = GeometrySchlickGGX(dotNL, roughness);
	float GV = GeometrySchlickGGX(dotNV, roughness);
	return GL * GV;
}

// cosTheta ~ [0, 1]
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// cosTheta ~ [0, 1]
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}