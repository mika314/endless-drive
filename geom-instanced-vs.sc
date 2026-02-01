$input a_position, a_texcoord0, a_normal, i_data0, i_data1, i_data2, i_data3
$output v_norm, v_frag_pos, v_uv

#include <bgfx_shader.sh>

void main()
{
  mat4 instanceMatrix = mat4(i_data0, i_data1, i_data2, i_data3);
  gl_Position = u_proj * u_view * instanceMatrix * vec4(a_position, 1.0);
  v_norm = vec4(mat3(transpose(inverse(instanceMatrix))) * a_normal.xyz, 0.0f);
  v_frag_pos = instanceMatrix * vec4(a_position, 1.0);
  v_uv = a_texcoord0;
}