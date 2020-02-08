uniform sampler2D distancemap;

uniform vec4  overlayColor;
uniform vec4  takeoverColor;
uniform float takeover;
uniform float takeoverScale;

const float takeoverEdgeWidthNoise = 0.05;
const float takeoverEdgeWidthFill  = 0.01;

void main(void) 
{
  float distance        = texture2D(distancemap, gl_TexCoord[0].st).r;
  float intensity       = distance * 0.5 + 0.5;
  vec4  backgroundColor = vec4(overlayColor.rgb * intensity, overlayColor.a);

  // noise takeover
  float takeover_invers_noise = (1.0 - takeover) * (1.0 + takeoverEdgeWidthNoise * 2.0) - takeoverEdgeWidthNoise;

  float noiseVal = clamp((snoise(gl_TexCoord[1].xyz * takeoverScale) + 1.0) / 2.0, 0.0, 1.0);
  float val      = smoothstep(
    takeover_invers_noise - takeoverEdgeWidthNoise,
    takeover_invers_noise + takeoverEdgeWidthNoise,
    noiseVal);

  // fill takeover
  float takeover_invers_fill = (1.0 - takeover) * (1.0 + takeoverEdgeWidthFill * 2.0) - takeoverEdgeWidthFill;

  val = smoothstep(
    takeover_invers_fill - takeoverEdgeWidthFill,
    takeover_invers_fill + takeoverEdgeWidthFill,
    1.0 - gl_TexCoord[2].t);

  // Final color
  vec4 takeoverLayerColor = vec4(takeoverColor.rgb, takeoverColor.a * val);

  takeoverLayerColor.rgb *= takeoverLayerColor.a;
  backgroundColor.rgb    *= backgroundColor.a;

  gl_FragColor.rgb = takeoverLayerColor.rgb + (backgroundColor.rgb * (1.0 - takeoverLayerColor.a));       
  gl_FragColor.a   = takeoverLayerColor.a   + (backgroundColor.a   * (1.0 - takeoverLayerColor.a));
}
