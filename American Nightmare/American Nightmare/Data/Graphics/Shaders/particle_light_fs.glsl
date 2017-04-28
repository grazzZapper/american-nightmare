#version 400

// Input
in fData
{
	vec3 position;
	vec3 center;
    vec4 color;
}frag;  

// Output
out vec4 outputColor;

void main(void)
{
	vec4 color = frag.color;
	color = vec4(1, 1, 1, 1);

	float distance = abs(distance(frag.position.xy, frag.center.xy));
	outputColor = vec4(color.r, color.g, color.b, -0.50 + color.a / (8.f * distance));	
}