layout(location = 0) in vec3 aPos;

uniform float xOffset;

out vec4 VertexColor;
out vec4 VertexPosition;

void main()
{
	gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
	VertexColor = vec4(.5f, .0f, .0f, 1.f);
	VertexPosition = gl_Position;
}
