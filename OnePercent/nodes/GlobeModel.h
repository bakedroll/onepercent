#pragma once

#include <memory>

#include <osg/Geometry>

#include <osgHelper/CameraAlignedQuad.h>
#include <osgHelper/ioc/Injector.h>

namespace onep
{
  /**
   * The GlobeModel represents an osg node, which initializes and holds
   * meshes, textures and shaders for the earth, its atmosphere and clouds
   * to visualize our planet.
   */
	class GlobeModel : public osg::Group
	{
	public:
    typedef osg::ref_ptr<GlobeModel> Ptr;

    /**
     * The constructor
     * @param injector to inject all dependencies
     */
		GlobeModel(osgHelper::ioc::Injector& injector);

    /**
     * The destructor
     */
    ~GlobeModel();

    /**
     * Creates all the meshes and loads textures and shaders
     * \param isFp64Supported Determines if the OpenGL implementation supports
     *                        64-bit floating point arithmetic
     */
    void makeGlobeModel(bool isFp64Supported);

    /**
     * Returns a camera-aligned-quad which covers the whole screen space
     * and uses a shader to render an atmospheric scattering effect.
     * @return a osgGaming::CameraAlignedQuad::Ptr
     */
    osgHelper::CameraAlignedQuad::Ptr getScatteringQuad();

    /**
     * Updates the light direction for the scattering shader
     */
		void updateLightDir(const osg::Vec3f& direction);

    /**
     * Updates the animation time of the clouds
     * @param time in seconds
     */
		void updateClouds(float time);

	private:

    /**
     * The PIMPL struct that is defined in the cpp file and
     * holds all the private members
     */
    struct Impl;
    std::unique_ptr<Impl> m;

	};
}
