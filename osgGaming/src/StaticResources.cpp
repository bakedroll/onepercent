#include <osgGaming/StaticResources.h>

using namespace std;
using namespace osgGaming;

const string StaticResources::ShaderBrightpassFp = 

	"uniform sampler2D hdrInput;" \
	"uniform sampler2D lumInput;" \
	"uniform sampler2D texAdaptedLuminance;" \
	"uniform float g_fMiddleGray;" \

	"float computeScaledLuminance(float avg, float lum)" \
	"{" \
	"	float scaledLum = lum * (g_fMiddleGray / (avg + 0.001));" \
	"	scaledLum = min(scaledLum, 65504.0);" \
	"	return scaledLum / (1.0 + scaledLum);" \
	"}" \

	"void main(void)" \
	"{" \
	"	const float BRIGHT_PASS_THRESHOLD = 0.9;" \
	"	const float BRIGHT_PASS_OFFSET = 1.0;" \
	
	"	float fLuminance = texture2D(lumInput, gl_TexCoord[0].st).r;" \
	"	float fAdaptedLum = texture2D(texAdaptedLuminance, vec2(0.5, 0.5)).w;" \
	"	float fScaledLum = computeScaledLuminance(fAdaptedLum, fLuminance);" \

	"	vec3 vSample = texture2D(hdrInput, gl_TexCoord[0].st).rgb;" \
	"	vSample *= fScaledLum;" \
	"	vSample -= BRIGHT_PASS_THRESHOLD;" \
	"	vSample = max(vSample, vec3(0.0, 0.0, 0.0));" \
	"	vSample /= (BRIGHT_PASS_OFFSET + vSample);" \

	"	gl_FragColor.rgb = vSample;" \
	"	gl_FragColor.a = fAdaptedLum;" \
	"}";

const string StaticResources::ShaderDepthOfFieldFp =

	"uniform sampler2D texColorMap;" \
	"uniform sampler2D texBlurredColorMap;" \
	"uniform sampler2D texStrongBlurredColorMap;" \
	"uniform sampler2D texDepthMap;" \
	"uniform float focalLength;" \
	"uniform float focalRange;" \
	"uniform float zNear;" \
	"uniform float zFar;" \

	"float convertZ(in float near, in float far, in float depthBufferValue)" \
	"{" \
	"	float clipZ = (depthBufferValue - 0.5) * 2.0;" \
	"	return -(2.0 * far * near) / (clipZ * (far - near) - (far + near));" \
	"}" \

	"void main(void)" \
	"{" \
	"	vec2 inTex = gl_TexCoord[0].st;" \

	"	float a = zFar / (zFar - zNear);" \
	"	float b = zFar * zNear / (zNear - zFar);" \
	"	float depth = texture2D(texDepthMap, inTex).x;" \
	"	float dist = b / (depth - a);" \

	"	vec4 colorValue = texture2D(texColorMap, inTex).rgba;" \
	"	vec4 blurredValue1 = texture2D(texBlurredColorMap, inTex).rgba;" \
	"	vec4 blurredValue2 = texture2D(texStrongBlurredColorMap, inTex).rgba;" \

	"	float blur = clamp(abs(dist - focalLength) / focalRange, 0.0, 1.0);" \
	"	float factor1 = 1.0;" \
	"	float factor2 = 0.0;" \

	"	if (blur > 0.5)" \
	"		factor2 = (blur - 0.5) * 2.0;" \
	"	else" \
	"		factor1 = blur * 2.0;" \

	"	vec4 result = mix(colorValue, blurredValue1, factor1);" \
	"	gl_FragColor = mix(result, blurredValue2, factor2);" \
	"}";

const string StaticResources::ShaderGaussConvolution1dxFp =

	"uniform sampler2D texUnit0;" \
	"uniform float radius;" \
	"uniform float sigma;" \
	"varying float sigma2;" \
	"varying float c;" \
	"uniform float osgppu_ViewportWidth;" \
	"uniform float osgppu_ViewportHeight;" \

	"void main(void)" \
	"{" \
	"	vec4 color = vec4(0.0);" \
	"	float totalWeigth = 0.0;" \
	"	float inputTexTexelWidth = 1.0 / osgppu_ViewportWidth;" \

	"	for (float i = -radius; i < radius; i += 1.0)" \
	"	{" \
	"		float weight = c * exp((i*i) / (-sigma2));" \
	"		totalWeigth += weight;" \
	"		color += texture2D(texUnit0, gl_TexCoord[0].xy + vec2(i * inputTexTexelWidth, 0)) * weight;" \
	"	}" \
	"	color /= totalWeigth;" \

	"	gl_FragColor = color;" \
	"}";

const string StaticResources::ShaderGaussConvolution1dyFp =

	"uniform sampler2D texUnit0;" \
	"uniform float radius;" \
	"uniform float sigma;" \
	"varying float sigma2;" \
	"varying float c;" \
	"uniform float osgppu_ViewportWidth;" \
	"uniform float osgppu_ViewportHeight;" \

	"void main(void)" \
	"{" \
	"	vec4 color = vec4(0.0);" \
	"	float totalWeigth = 0.0;" \
	"	float inputTexTexelWidth = 1.0 / osgppu_ViewportHeight;" \

	"	for (float i = -radius; i < radius; i += 1.0)" \
	"	{" \
	"		float weight = c * exp((i*i) / (-sigma2));" \
	"		totalWeigth += weight;" \

	"		color += texture2D(texUnit0, gl_TexCoord[0].xy + vec2(0, i * inputTexTexelWidth)) * weight;" \
	"	}" \
	"	color /= totalWeigth;" \

	"	gl_FragColor = color;" \
	"}";

const string StaticResources::ShaderGaussConvolutionVp =

	"uniform float sigma;" \
	"const float PI = 3.1415926535897;" \
	"varying float sigma2;" \
	"varying float c;" \

	"void main(void)" \
	"{" \
	"	gl_TexCoord[0] = gl_MultiTexCoord0;" \
	"	gl_Position = ftransform();" \
	"	gl_FrontColor = gl_Color;" \

	"	sigma2 = 2.0 * sigma * sigma;" \
	"	c = sqrt((1.0 / (sigma2 * PI)));" \
	"}";

const string StaticResources::ShaderLuminanceAdaptedFp =

	"uniform sampler2D texLuminance;" \
	"uniform sampler2D texAdaptedLuminance;" \
	"uniform float maxLuminance;" \
	"uniform float minLuminance;" \
	"uniform float invFrameTime;" \
	"uniform float adaptScaleFactor;" \

	"const float TauCone = 0.01;" \
	"const float TauRod = 0.04;" \

	"void main(void)" \
	"{" \
	"	float current = texture2D(texLuminance, vec2(0.5, 0.5), 100.0).x;" \
	"	float old = texture2D(texAdaptedLuminance, vec2(0.5, 0.5)).w;" \
	"	float sigma = clamp(0.4 / (0.04 + current), 0.0, 1.0);" \
	"	float Tau = mix(TauCone, TauRod, sigma) / adaptScaleFactor;" \
	"	float lum = old + (current - old) * (1.0 - exp(-(invFrameTime) / Tau));" \

	"	gl_FragData[0].xyzw = vec4(clamp(lum, minLuminance, maxLuminance));" \
	"}";

const string StaticResources::ShaderLuminanceFp =

	"uniform sampler2D texUnit0;" \

	"void main(void)" \
	"{" \
	"	vec4 texColor0 = texture2D(texUnit0, gl_TexCoord[0].st);" \

	"	gl_FragColor.xyz = vec3(texColor0.r * 0.2125 + texColor0.g * 0.7154 + texColor0.b * 0.0721);" \
	"	gl_FragColor.a = texColor0.a;" \
	"}";

const string StaticResources::ShaderLuminanceMipmapFp =

	"uniform sampler2D texUnit0;" \
	"uniform float osgppu_ViewportWidth;" \
	"uniform float osgppu_ViewportHeight;" \
	"uniform float osgppu_MipmapLevel;" \
	"uniform float osgppu_MipmapLevelNum;" \

	"void main(void)" \
	"{" \
	"	const float epsilon = 0.001;" \
	"	float res = 0.0;" \
	"	float c[4];" \

	"	vec2 size = vec2(osgppu_ViewportWidth, osgppu_ViewportHeight) * 2.0;" \
	"	vec2 iCoord = gl_TexCoord[0].st;" \
	"	vec2 texel = vec2(1.0, 1.0) / (size);" \
	"	vec2 halftexel = vec2(0.5, 0.5) / size;" \

	"	vec2 st[4];" \
	"	st[0] = iCoord - halftexel + vec2(0, 0);" \
	"	st[1] = iCoord - halftexel + vec2(texel.x, 0);" \
	"	st[2] = iCoord - halftexel + vec2(0, texel.y);" \
	"	st[3] = iCoord - halftexel + vec2(texel.x, texel.y);" \

	"	for (int i = 0; i < 4; i++)" \
	"	{" \
	"		st[i] = clamp(st[i], vec2(0, 0), vec2(1, 1));" \
	"		c[i] = texture2D(texUnit0, st[i], osgppu_MipmapLevel - 1.0).r;" \
	"	}" \

	"	if (abs(osgppu_MipmapLevel - 1.0) < 0.00001)" \
	"	{" \
	"		res += log(epsilon + c[0]);" \
	"		res += log(epsilon + c[1]);" \
	"		res += log(epsilon + c[2]);" \
	"		res += log(epsilon + c[3]);" \
	"	}" \
	"	else" \
	"	{" \
	"		res += c[0];" \
	"		res += c[1];" \
	"		res += c[2];" \
	"		res += c[3];" \
	"	}" \

	"	res *= 0.25;" \

	"	if (osgppu_MipmapLevelNum - osgppu_MipmapLevel < 2.0)" \
	"	{" \
	"		res = exp(res);" \
	"	}" \

	"	gl_FragData[0].rgba = vec4(min(res, 65504.0));" \
	"}";

const string StaticResources::ShaderTonemapHdrFp =

	"uniform sampler2D blurInput;" \
	"uniform sampler2D hdrInput;" \
	"uniform sampler2D lumInput;" \
	"uniform sampler2D texAdaptedLuminance;" \
	"uniform float fBlurFactor;" \
	"uniform float g_fMiddleGray;" \

	"float computeScaledLuminance(float avg, float lum)" \
	"{" \
	"	float scaledLum = lum * (g_fMiddleGray / (avg + 0.001));" \
	"	scaledLum = min(scaledLum, 65504.0);" \
	"	return scaledLum / (1.0 + scaledLum);" \
	"}" \

	"void main(void)" \
	"{" \
	"	vec2 inTex = gl_TexCoord[0].st;" \
	"	vec4 blurColor = texture2D(blurInput, inTex);" \
	"	vec4 hdrColor = texture2D(hdrInput, inTex);" \

	"	float fLuminance = texture2D(lumInput, inTex).r;" \
	"	float fAdaptedLum = texture2D(texAdaptedLuminance, vec2(0.5, 0.5)).w;" \
	"	float fScaledLum = computeScaledLuminance(fAdaptedLum, fLuminance);" \

	"	vec4 color = hdrColor * fScaledLum;" \

	"	gl_FragColor.rgb = blurColor.rgb * fBlurFactor + color.rgb;" \
	"	gl_FragColor.a = 1.0;" \
	"}";