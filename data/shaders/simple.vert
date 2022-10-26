#version 330 core

uniform mat4 obj_mat;
uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat3 normal_mat;
uniform mat2 texcoord_mat;

in vec3 vtx_position;
in vec3 vtx_color;
in vec3 vtx_normal;
in vec2 vtx_texcoord;
in vec3 vtx_tangent;
in vec3 vtx_bitangent;

out vec3 v_normal;
out vec3 v_view;
out vec3 v_color;
out vec2 texcoord;
out vec3 v_tangent;
out vec3 v_bitangent;

void main()
{
  v_color = vtx_color;
  v_normal = normalize(normal_mat * vtx_normal);
  vec4 p = view_mat * (obj_mat * vec4(vtx_position, 1.));
  v_view = normalize(-p.xyz);
  gl_Position = proj_mat * p;
  texcoord = vtx_texcoord;

  v_normal = normalize(vtx_normal);
  v_tangent = normalize(vtx_tangent);
  v_bitangent = normalize(vtx_bitangent);
}
