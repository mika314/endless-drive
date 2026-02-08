$input a_position, a_texcoord0, a_normal, i_data0, i_data1, i_data2, i_data3
$output v_norm, v_frag_pos, v_uv

#include <bgfx_shader.sh>
#include "shaderlib.sh"

void main()
{
  mat4 instanceMatrix = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
  gl_Position = mul(u_proj, mul(u_view, mul(instanceMatrix, vec4(a_position, 1.0))));
  mat4 invTrans = transpose(inverse(instanceMatrix));
  v_norm = vec4(mul(mat3(invTrans[0].xyz, invTrans[1].xyz, invTrans[2].xyz), a_normal.xyz), 0.0f);
  v_frag_pos = mul(instanceMatrix, vec4(a_position, 1.0));
  v_uv = a_texcoord0;
}