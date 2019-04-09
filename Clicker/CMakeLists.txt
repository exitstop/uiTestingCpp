add_definitions(
   -D_DEBUG·
   -D_CONSOLE·
   -DUNICODE
   -D_UNICODE
)


add_library(mylib STATIC
	myLib.h
	myLib.cpp
	util.cpp
	util.h
	)

add_executable(main
    Clicker.cpp
    )

target_include_directories(main PRIVATE 
    .)

target_link_libraries(main mylib oleacc oleaut32 ole32)

configure_file("contextMenu.ps1" "Release/contextMenu.ps1" COPYONLY)