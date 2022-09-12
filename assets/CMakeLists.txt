cmake_minimum_required(VERSION 3.20)

# Build HLSL shaders

add_custom_target(shaders)

set(HLSL_SHADER_FILES BuiltIn_Vertex.hlsl BuiltIn_Pixel.hlsl)

set_source_files_properties(BuiltIn_Vertex.hlsl PROPERTIES ShaderType "vs")
set_source_files_properties(BuiltIn_Pixel.hlsl PROPERTIES ShaderType "ps")
set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "4_0")

foreach(FILE ${HLSL_SHADER_FILES})
  get_filename_component(FILE_WE ${FILE} NAME_WE)
  get_source_file_property(shadertype ${FILE} ShaderType)
  get_source_file_property(shadermodel ${FILE} ShaderModel)
  add_custom_command(TARGET shaders
                     COMMAND fxc.exe /nologo /Emain /T${shadertype}_${shadermodel} $<IF:$<CONFIG:DEBUG>,/Od,/O1> /Zi /Fo ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${FILE_WE}.cso /Fd ${CMAKE_BINARY_DIR}/${FILE_WE}.pdb ${FILE}
                     MAIN_DEPENDENCY ${FILE}
                     COMMENT "HLSL ${FILE}"
                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                     VERBATIM)
endforeach(FILE)
