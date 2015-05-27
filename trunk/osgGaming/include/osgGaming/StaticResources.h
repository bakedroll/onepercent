#pragma once

#include <string>

namespace osgGaming
{
	class StaticResources
	{
	public:
		static const std::string ShaderBrightpassFp;
		static const std::string ShaderGaussConvolution1dxFp;
		static const std::string ShaderGaussConvolution1dyFp;
		static const std::string ShaderGaussConvolutionVp;
		static const std::string ShaderLuminanceAdaptedFp;
		static const std::string ShaderLuminanceFp;
		static const std::string ShaderLuminanceMipmapFp;
		static const std::string ShaderTonemapHdrFp;
	};
}