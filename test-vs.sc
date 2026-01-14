$input a_position, a_texcoord0, a_normal
$output v_norm, v_frag_pos, v_uv

uniform mat4  u_modelViewProj;
uniform mat4  u_model;
uniform mat4  u_view;
uniform mat4  u_proj;

void main()
{
  gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0);
  v_norm = vec4(mat3(transpose(inverse(u_model))) * a_normal.xyz, 0.0f);
  v_frag_pos = u_model * vec4(a_position, 1.0);
  v_uv = a_texcoord0;
}
