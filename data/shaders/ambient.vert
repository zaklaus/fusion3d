#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 m;
uniform mat4 lm;

out vec2 uv0;
out vec3 normal0;
out vec3 frag0;
out vec4 lfrag0;
out float vdist;
out vec3 ambColor0;
out int hasShadow0;

struct Camera
{
  mat4 projection;
  mat4 view;
  vec3 position;
  vec2 angle;

  float fieldOfView;
  float aspectRatio;
  float nearPlane;
  float farPlane;

  vec3 ambColor;
};

uniform sampler2D renderTexture;
uniform int renderType;
uniform int hasShadow;
uniform Camera camera;

void main() {

  if(renderType == 0)
  {
      mat4 mvp = camera.projection * camera.view * m;
      gl_Position = mvp * vec4(vertex, 1);
      frag0 = vec3(m * vec4(vertex, 1));
      lfrag0 = lm * m * vec4(vertex, 1);
      ambColor0 = camera.ambColor;
      uv0 = uv;
      vdist = distance(camera.position, (m * vec4(vertex, 1)).xyz) / camera.farPlane;
      normal0 = mat3(transpose(inverse(m))) * normal;
      hasShadow0 = hasShadow;
  }
  else if(renderType == 2)
  {
      uv0 = uv;
      gl_Position = lm * m * vec4(vertex, 1);
  }
  else
  {
      gl_Position = vec4(vertex, 1);
      uv0 = (vertex.xy+vec2(1,1))/2.0;
  }
}
