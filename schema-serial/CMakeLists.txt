add_executable(schema-serial
    src/main.cpp
    src/widget.fbs
)

target_link_libraries(schema-serial PRIVATE ce::utils)

function(flat_buffers_autogen target)
    get_property(flatc TARGET flatbuffers::flatc PROPERTY LOCATION)
    get_property(sources TARGET ${target} PROPERTY SOURCES)
    foreach(source ${sources})
        if(source MATCHES \.fbs$)
            if(NOT source MATCHES "^src/.*")
                message(SEND_ERROR "${source} doesn't start with src subdirectory")
            endif()
            get_filename_component(output_dir "${source}" DIRECTORY)
            string(SUBSTRING "${output_dir}" 3 -1 output_dir)
            set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/include${output_dir}")
            get_filename_component(output "${source}" NAME_WLE)
            set(output "${output_dir}/${output}_generated.h")
            set(source "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
            add_custom_command(
                OUTPUT "${output}"
                COMMAND "${flatc}" --cpp -o "${output_dir}" "${source}"
                MAIN_DEPENDENCY "${source}"
                VERBATIM
            )
            target_sources(${target} PRIVATE "${output}")
            # Ensure include directory exists before first build so that
            # ntc_target adds it to compiler options.
            file(MAKE_DIRECTORY "${output_dir}")
        endif()
    endforeach()
    target_link_libraries(${target} PRIVATE flatbuffers::flatbuffers_shared)
endfunction()

flat_buffers_autogen(schema-serial)

ntc_target(schema-serial)
