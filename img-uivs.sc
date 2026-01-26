$input a_position, a_texcoord0
$output v_uv0

#include <bgfx_shader.sh>

void main()
{
  gl_Position = u_modelViewProj * vec4(a_position, 0.0, 1.0);
  v_uv0 = a_texcoord0;
}
