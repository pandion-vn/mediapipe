// https://www.shadertoy.com/view/lljGDt

float rayStrength(vec2 raySource, vec2 rayRefDirection, vec2 coord, float seedA, float seedB, float speed)
{
	vec2 sourceToCoord = coord - raySource;
	float cosAngle = dot(normalize(sourceToCoord), rayRefDirection);
	
	return clamp(
		(0.45 + 0.15 * sin(cosAngle * seedA + iTime * speed)) +
		(0.3 + 0.2 * cos(-cosAngle * seedB + iTime * speed)),
		0.6, 0.9) *
		clamp((iResolution.x - length(sourceToCoord)) / iResolution.x, 0.6, 1.0);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	uv.y = 1.0 - uv.y;
	vec2 coord = vec2(fragCoord.x, iResolution.y - fragCoord.y);
	
	
	// Set the parameters of the sun rays
	vec2 rayPos1 = vec2(iResolution.x * -0.1, iResolution.y * -0.1);
	vec2 rayRefDir1 = normalize(vec2(1.0, -0.116));
	float raySeedA1 = 36.2214;
	float raySeedB1 = 21.11349;
	float raySpeed1 = 1.5;
	
	vec2 rayPos2 = vec2(iResolution.x * 0.8, iResolution.y * -0.6);
	vec2 rayRefDir2 = normalize(vec2(1.0, 0.241));
	const float raySeedA2 = 22.39910;
	const float raySeedB2 = 18.0234;
	const float raySpeed2 = 1.1;
	
	// Calculate the colour of the sun rays on the current fragment
	vec4 rays1 =
		vec4(0.9, 0.7, 0.6, 1.0) *
		rayStrength(rayPos1, rayRefDir1, coord, raySeedA1, raySeedB1, raySpeed1);
	 
	vec4 rays2 =
		vec4(1.0, 1.0, 1.0, 1.0) *
		rayStrength(rayPos2, rayRefDir2, coord, raySeedA2, raySeedB2, raySpeed2);
	
	fragColor = rays1 * 1.0;
	
	// Attenuate brightness towards the bottom, simulating light-loss due to depth.
	// Give the whole thing a blue-green tinge as well.
	float brightness = 1.3;
	fragColor.x *= 1.4 + (brightness * 1.8);
	fragColor.y *= 1.3 + (brightness * 1.6);
	fragColor.z *= 1.5 + (brightness * 1.5);
    
    vec2 uv2 = fragCoord.xy / iResolution.xy;
    fragColor *= texture(iChannel0, uv2.xy);
}
