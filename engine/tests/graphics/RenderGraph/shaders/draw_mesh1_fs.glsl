
layout(location=0) in MeshOutput {
	vec4	color;
} Input;

layout(location=0) out vec4  out_Color;

void main() {
	out_Color = Input.color;
}
