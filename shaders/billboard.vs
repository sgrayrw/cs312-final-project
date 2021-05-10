#version 400

layout (location = 0) in vec3 vPos;

uniform vec3 uCameraPos;
uniform float uSize;
uniform vec3 uOffset;
uniform vec4 uColor;
uniform mat4 uVP;
uniform mat4 uM;

out vec4 color;
out vec2 uv;

void main()
{
   color = uColor;
   uv = vPos.xy;
   gl_Position = uVP * uM * vec4(vPos, 1.0);
}
