begin_project(VectorizerLib LIBRARY OPTIONAL)

require_library(OpenSceneGraph)
require_library(OpenCV)

require_project(osgHelper PATH osgHelper)

add_source_directory(src)
add_source_directory(include/Vectorizer)

add_include_directory(include)
