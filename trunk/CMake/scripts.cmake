function(add_files ADD_FILES_OUTPUT ADD_FILES_NAME ADD_FILES_EXTENSION)

  file(GLOB_RECURSE FILES_SOURCE_TMP RELATIVE
    "${CMAKE_CURRENT_SOURCE_DIR}/${ADD_FILES_NAME}"
    "${CMAKE_CURRENT_SOURCE_DIR}/${ADD_FILES_NAME}/*.${ADD_FILES_EXTENSION}")
    
  set(FILES_OUTPUT_TMP "")
  foreach(filename ${FILES_SOURCE_TMP})
    list(APPEND FILES_OUTPUT_TMP ${ADD_FILES_NAME}/${filename})
  endforeach(filename)
  
  source_group("${ADD_FILES_NAME}" FILES ${FILES_OUTPUT_TMP})
 
  set(${ADD_FILES_OUTPUT} ${${ADD_FILES_OUTPUT}} ${FILES_OUTPUT_TMP} PARENT_SCOPE)
  
endfunction()

function(add_source_directory ADD_SOURCE_DIRECTORY_OUTPUT ADD_SOURCE_DIRECTORY_NAME)
  
  add_files(ADD_SOURCE_DIRECTORY_OUTPUT_TMP ${ADD_SOURCE_DIRECTORY_NAME} "cpp")
  add_files(ADD_SOURCE_DIRECTORY_OUTPUT_TMP ${ADD_SOURCE_DIRECTORY_NAME} "h")
  
  set(${ADD_SOURCE_DIRECTORY_OUTPUT} ${${ADD_SOURCE_DIRECTORY_OUTPUT}} ${ADD_SOURCE_DIRECTORY_OUTPUT_TMP} PARENT_SCOPE)
 
endfunction()

function(add_ui_directory ADD_UI_DIRECTORY_OUTPUT ADD_UI_DIRECTORY_NAME)
  
  add_files(ADD_UI_DIRECTORY_OUTPUT_TMP ${ADD_UI_DIRECTORY_NAME} "ui")
  
  set(${ADD_UI_DIRECTORY_OUTPUT} ${${ADD_UI_DIRECTORY_OUTPUT}} ${ADD_UI_DIRECTORY_OUTPUT_TMP} PARENT_SCOPE)
 
endfunction()

function(add_other_directory ADD_OTHER_DIRECTORY_OUTPUT ADD_OTHER_DIRECTORY_NAME)
  
  add_files(ADD_OTHER_DIRECTORY_OUTPUT_TMP ${ADD_OTHER_DIRECTORY_NAME} "*")
  
  set(${ADD_OTHER_DIRECTORY_OUTPUT} ${${ADD_OTHER_DIRECTORY_OUTPUT}} ${ADD_OTHER_DIRECTORY_OUTPUT_TMP} PARENT_SCOPE)
 
endfunction()

function(get_libraries GET_LIBRARIES_OUTPUT GET_LIBRARIES_PATH GET_LIBRARIES_SUFFIX GET_LIBRARIES_LIBS)

  if(MSVC)
    set(GET_LIBRARIES_EXTENSION ".lib")
  else()
    set(GET_LIBRARIES_EXTENSION ".a")
  endif()
  
  string(REPLACE " " ";" GET_LIBRARIES_LIBS_LIST ${GET_LIBRARIES_LIBS})
  string(REPLACE "\\" "/" GET_LIBRARIES_PATH_ESC ${GET_LIBRARIES_PATH})
  
  set(FILES_OUTPUT_TMP "")
  foreach(lib ${GET_LIBRARIES_LIBS_LIST})  
    list(APPEND FILES_OUTPUT_TMP ${GET_LIBRARIES_PATH_ESC}/${lib}${GET_LIBRARIES_SUFFIX}${GET_LIBRARIES_EXTENSION})
  endforeach()
  
  set(${GET_LIBRARIES_OUTPUT} ${FILES_OUTPUT_TMP} PARENT_SCOPE)
  
endfunction()

function(adjust_cached_path ADJUST_CACHED_PATH_VAR ADJUST_CACHED_PATH_DOC ADJUST_CACHED_PATH_VALUE)

    get_filename_component(ADJUST_CACHED_PATH_VALUE ${ADJUST_CACHED_PATH_VALUE} REALPATH)
    
    unset(${ADJUST_CACHED_PATH_VAR} CACHE)
    set(${ADJUST_CACHED_PATH_VAR} ${ADJUST_CACHED_PATH_VALUE} CACHE STRING ${ADJUST_CACHED_PATH_DOC})

endfunction()

function(adjust_cached_path_if_exists ADJUST_CACHED_PATH_VAR ADJUST_CACHED_PATH_DOC ADJUST_CACHED_PATH_VALUE)

  cmake_parse_arguments(adjust_cached_path_if_exists "CACHE" "MESSAGES DEFAULT" "" ${ARGN})
  
  if (adjust_cached_path_if_exists_CACHE)
    set(${ADJUST_CACHED_PATH_VAR} "${ADJUST_CACHED_PATH_VAR}-NOTFOUND" CACHE STRING ${ADJUST_CACHED_PATH_DOC})
  endif()
  
  if (EXISTS ${ADJUST_CACHED_PATH_VALUE})
      adjust_cached_path(${ADJUST_CACHED_PATH_VAR} ${ADJUST_CACHED_PATH_DOC} ${ADJUST_CACHED_PATH_VALUE})
  else()
    if (DEFINED adjust_cached_path_if_exists_DEFAULT)
      set(${ADJUST_CACHED_PATH_VAR} ${adjust_cached_path_if_exists_DEFAULT})
    elseif (DEFINED adjust_cached_path_if_exists_MESSAGES)
      set(${adjust_cached_path_if_exists_MESSAGES} ${${adjust_cached_path_if_exists_MESSAGES}} "Please specify ${adjust_cached_path_if_exists_MESSAGES}\n" PARENT_SCOPE)
    endif()
  endif()

endfunction()

function(copy_dlls COPY_DLLS_SRC COPY_DLLS_DST COPY_DLLS_PREFIX COPY_DLLS_SUFFIX COPY_DLLS_FILES)

  string(REPLACE " " ";" COPY_DLLS_FILES_LIST ${COPY_DLLS_FILES})
  foreach(dll ${COPY_DLLS_FILES_LIST})
    set(FILE_NAME ${COPY_DLLS_PREFIX}${dll}${COPY_DLLS_SUFFIX}.dll)
    set(FILE_FROM ${COPY_DLLS_SRC}/${FILE_NAME})
    
    message(STATUS "copy library ${FILE_NAME}")
    
    if (EXISTS ${FILE_FROM})
      configure_file(${FILE_FROM} ${COPY_DLLS_DST}/${FILE_NAME} COPYONLY)
    else()
      message(STATUS "Could not copy file ${FILE_FROM}")
    endif()
  endforeach()

endfunction()
