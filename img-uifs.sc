$input v_uv0

#include <bgfx_shader.sh>

SAMPLER2D(imgTex, 0);

void main()
{
  gl_FragColor = texture2D(imgTex, v_uv0.xy);
}
