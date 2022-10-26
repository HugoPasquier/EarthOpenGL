#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_view;
in vec2 texcoord;
in vec3 v_tangent;
in vec3 v_bitangent;

uniform vec3 lightDir;
uniform sampler2D image;
uniform sampler2D image2;
uniform sampler2D image3;
uniform sampler2D image4;

out vec4 out_color;

vec3 blinn(vec3 n, vec3 v, vec3 l, vec3 dCol, vec3 sCol, float s)
{
  vec3 res = vec3(0,0,0);
  float dc = max(0,dot(n,l));
  if(dc>0) {
    res = dCol * dc;
    float sc = max(0,dot(n,normalize(v+l)));
    if(sc>0)
      res += sCol * pow(sc,s) * dc;
  }
  return res;
}

float cos_t = max(0, dot(v_normal, lightDir));

vec4 mix_tex_cloud = mix(texture(image, texcoord), 
                          texture(image2, texcoord),
                          texture(image2, texcoord));

vec4 mix_tex_night = mix(
                          texture(image3, texcoord),
                          mix_tex_cloud,
                          cos_t);

void main(void) {
  float ambient = 0.0;
  float shininess = 50;
  vec3 spec_color = vec3(1,1,1);
  //out_color = vec4(ambient * v_color + blinn(normalize(v_normal),normalize(v_view), lightDir, v_color, spec_color, shininess),1.0);
  //out_color = vec4(texcoord, 0, 1);
  out_color = mix_tex_night;

  // ----------------- Normal Mapping -----------------
  vec4 texcoord = texcoord(image4, texcoord);
  vec3 normal_map = normal_mat * (2*textget.xyz - 1);
  vec3 color_normal = texture(image3, texcoord).xyz;

  vec3 v_n = normalize(v_normal);
  vec3 v_t = normalize(v_tangent);
  vec3 v_b = normalize(v_bitangent);

  mat3 tbnMatrix = transpose(mat3(v_t, v_b, v_n));

  out_color = vec4(ambient * color_normal + blinn(normalize(normal_map), tbnMatrix * normalize(v_view), tbnMatrix * lightDir, color_normal, spec_color, shininess),1.0);
  out_color = vec4(v_t, 1.0);
}
