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