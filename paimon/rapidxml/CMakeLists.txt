add_library(rapidxml INTERFACE
    include/rapidxml/rapidxml.hpp
    include/rapidxml/rapidxml_iterators.hpp
    include/rapidxml/rapidxml_print.hpp
    include/rapidxml/rapidxml_utils.hpp
    )

target_include_directories(rapidxml INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
