install(
    TARGETS scenegraph_exe
    RUNTIME COMPONENT scenegraph_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
