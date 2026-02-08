$input a_position, a_texcoord0, a_normal
$output v_norm, v_frag_pos, v_uv

#include <bgfx_shader.sh>
#include "shaderlib.sh"

void main()
{
  gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
  mat4 invTrans = transpose(inverse(u_model[0]));
  v_norm = vec4(mul(mat3(invTrans[0].xyz, invTrans[1].xyz, invTrans[2].xyz), a_normal.xyz), 0.0f);
  v_frag_pos = mul(u_model[0], vec4(a_position, 1.0));
  v_uv = a_texcoord0;
}
