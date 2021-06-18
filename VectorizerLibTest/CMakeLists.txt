begin_project(VectorizerLibTest EXECUTABLE OPTIONAL)

require_library(GTest)
require_library(OpenSceneGraph MODULES osg osgUtil osgText OpenThreads)
require_library(OpenCV MODULES core highgui imgcodecs imgproc video)

require_project(VectorizerLib)
require_project(osgHelper PATH osgHelper)

add_source_directory(test)
