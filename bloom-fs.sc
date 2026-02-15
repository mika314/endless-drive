$input v_uv

#include <bgfx_shader.sh>

SAMPLER2D(emissionBuffer, 3);

void main()
{
  const float Pi2 = 6.28318530718;

  // GAUSSIAN BLUR SETTINGS {{{
  const float Directions = 6.f;  // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
  const float Quality = 20.f;     // BLUR QUALITY (Default 4.0 - More is better but slower)
  const float Size = .015f;     // BLUR SIZE (Radius)
  // GAUSSIAN BLUR SETTINGS }}}

  const vec2 Radius = vec2(Size, Size);
  const vec4 origColor = texture2D(emissionBuffer, v_uv);

  vec4 Color = origColor;

  for (float d = 0.0f; d < Pi2; d += Pi2 / Directions)
    for (float i = 4.f / Quality; i <= 1.f; i += 1.f / Quality)
      Color += .0002f * texture2D(emissionBuffer, v_uv + vec2(cos(d + .1f), sin(d + .1f)) * Radius * i) /
               (Size * i * Size * i);

  gl_FragColor = origColor + Color / (Quality * Directions);
}
