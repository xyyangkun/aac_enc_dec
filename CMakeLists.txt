# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (aac_enc_dec)

#add debug operation  true false
set(ENABLE_ASAN true)

if(ENABLE_ASAN)
	# 以下问内存泄漏检测
	add_compile_options(-g)
	add_compile_options(-fsanitize=address)
	add_compile_options(-fno-omit-frame-pointer)
	add_compile_options(-Wl,--no-undefined)
	#add_compile_options(-O1)
	#添加链接选项, 只能添加一行
	SET(CMAKE_EXE_LINKER_FLAGS "-fsanitize=address -lasan")
endif()

# 指定生成目标
add_executable(aac_enc aac-enc.c wavreader.c)
target_link_libraries (aac_enc fdk-aac ${CMAKE_EXE_LINKER_FLAGS}) 
