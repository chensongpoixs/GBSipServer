

project(GbSipServer)
cmake_minimum_required(VERSION 2.8)
MESSAGE(STATUS  "####################################")
MESSAGE(STATUS  "#############${PROJECT_NAME}###############")
MESSAGE(STATUS  "####################################")
set(CMAKE_CXX_STANDARD 17)
# apt-get install libsqlite3-dev -y


#set(DEPS_DIR  "D:/dep/c_dep")

#add_definitions(
				#	-DWIN32
				#	-D_DEBUG
				##	-D_WINDOWS
				#	-D_UNICODE
				#	-DUNICODE
				#	-D_AFXDLL
#)
ADD_DEFINITIONS(-DHAVE_NETDB_H 
		-DHAVE_NETINET_IN_H
		-DHAVE_SYS_IOCTL_H
		-D__POCC__
		-DHAVE_LIMITS_H
		-DHAVE_FCNTL_H
		-DHAVE_FCNTL_O_NONBLOCK
		-DHAVE_STRUCT_TIMEVAL=0
		-DHAVE_PTHREAD
		-DHAVE_SEMAPHORE_H
		-DHAVE_ERRNO_H
		#-DHAVE_RECV
		#-DHAVE_SEND
		#-DRECV_TYPE_RETV
		#-DHAVE_SEND
		-DHAVE_SYS_SOCKET_H
		-DHAVE_CONFIG_H   
		#-DHAVE_SETSOCKOPT_SO_NONBLOCK
		)
add_definitions(


#-D_WINSOCK_DEPRECATED_NO_WARNINGS
#-D_CRT_SECURE_NO_DEPRECATE
#-D_CRT_NONSTDC_NO_WARNINGS
#-DWIN32
#-D_DEBUG
#-D_DEBUG
#-D_CONSOLE


        ## define path to swagger-ui static resources folder
        -DOATPP_SWAGGER_RES_PATH="swagger/res"

        ## SQLite database file
        -DDATABASE_FILE="db/db.sqlite"
        ## SQLite database test file
        -DTESTDATABASE_FILE="db/test-db.sqlite"

        ## Path to database migration scripts
        -DDATABASE_MIGRATIONS="sql"
)


 include_directories(
	 ../../libwebrtc
	#../../deps/libyuv/include
	 ../../deps/abseil-cpp/
	.
	../libsip
	../libsip/c-ares 
	../libsip/eXosip2
	../libsip/osip2
	../libsip/osipparser2
	./utils 
	#../libswagger
	#	/usr/local/include/oatpp-1.4.0
	#/usr/local/include/oatpp-1.4.0/oatpp
	#/usr/local/include/oatpp-1.4.0/oatpp-sqlite
	#/usr/local/include/oatpp-1.4.0/oatpp-swagger
	#/usr/local/include/oatpp-1.4.0/oatpp-sqlite/sqlite
	#/usr/local/include/oatpp-1.4.0/oatpp-sqlite
	#/usr/local/include/oatpp-1.4.0/oatpp-sqlite

	${DEPS_DIR}/oatpp-1.4.0/oatpp
	${DEPS_DIR}/oatpp-1.4.0/oatpp-sqlite
	${DEPS_DIR}/oatpp-1.4.0/oatpp-swagger
	${DEPS_DIR}/oatpp-1.4.0/oatpp-sqlite/sqlite
	${DEPS_DIR}/oatpp-1.4.0/oatpp-sqlite
	${DEPS_DIR}/oatpp-1.4.0/oatpp-sqlite
	
	 
)
link_directories( 

${DEPS_DIR}/oatpp-1.4.0/lib
/usr/local/lib/oatpp-1.4.0
)


FILE(GLOB  swagger_source

		controller/*.cpp 
        controller/*.hpp
		controller/*.h
        db/*.hpp
		db/*.cpp 
		db/*.h
        dto/*.h
        dto/*.hpp
        dto/*.cpp 
		service/*.h
        service/*.cpp
        service/*.hpp
		
)
 
file(GLOB GbSipServer_source
        
		utils/*.cpp 
		utils/*.h 
		utils/*.c 
		 utils/*.hpp

		*.h
	*.cpp 
	
	sql/*.sql 
	*.hpp
        )
		


#
#add_subdirectory(curl-7.81.0)
#add_subdirectory(D:/Work/rtc/crtcserver/deps/curl-7.81.0)
#add_subdirectory(libsip)
 
add_executable(${PROJECT_NAME}     ${GbSipServer_source} 

							${swagger_source}
								)
#add_dependencies(${PROJECT_NAME} libswagger)
 add_dependencies(${PROJECT_NAME} libsip)
 



set(ALL_LIB 
	libsip
			  oatpp
 oatpp-sqlite
 oatpp-swagger
 
			api 
			libice
		libp2p_peerconnection 
		libmedia_codec 
		libmedia_transfer_protocol
		#libcross_platform_collection_render
			audio_device
			#audio_processing
			desktop_capture
			video_capture
			video_coding
			audio_coding

			call
			common_audio
			common_video
			libwebrtc 
			logging 
			media
			p2p 
			pc 
			rtc_base
		 	
			 ###########################
 
				 ## AAC
				 faad
				 opus
				 libsrtp2
				 x264
		   jpeg-static
		

		#curl 
		jsoncpp_static
		 
		 

	 


		  
	 
		
		
		yaml_cpp
		 
		usrsctp

	 
		 
		 
		#abseil_dll
		absl_cordz_functions
		absl_cordz_handle
		absl_cordz_info
		absl_cordz_sample_token
		absl_flags
		absl_flags_commandlineflag_internal
		absl_flags_internal
		absl_flags_marshalling
		absl_flags_parse
		absl_flags_private_handle_accessor
		absl_flags_program_name
		absl_flags_reflection
		absl_flags_usage
		absl_flags_usage_internal
		absl_low_level_hash
		absl_random_internal_distribution_test_util
		
			)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "current platform: windows")
    set(WIN_LIB 
	sqlite
		libvpx
	     audio_processing
		 libvpx_asm
		Dnsapi.lib   
			delayimp.lib
			ws2_32.lib
			 Qwave.lib
			 crypt32.lib
			iphlpapi.lib
			secur32.lib
			crypt32.lib
			iphlpapi.lib
			secur32.lib
			 iphlpapi.lib
		 
		ws2_32   
		crypt32
		Shell32.lib
		 advapi32
              iphlpapi
              psapi
              shell32
              user32
              userenv
              ws2_32
			  mswsock.lib
			  #  webrtc  timeGetTime  
			  winmm.lib  
			  dbghelp.lib
		 crypt32.lib
		iphlpapi.lib
       secur32.lib
	   d3d11.lib
      dxgi.lib
	  	 dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib

		Ws2_32.lib
		Wldap32.lib
	 Imm32 
			comctl32
			ws2_32.lib
			winmm.lib

			libcrypto
			libssl
			yuv
		#libcrypto.lib 
		#libssl.lib 
		crypt32
		Shell32.lib
		 advapi32
              iphlpapi
              psapi
              shell32
              user32
              userenv
              ws2_32
			  mswsock.lib
			  #  webrtc  timeGetTime  
			  winmm.lib
			  #ucrt.lib
			  #  libuv.lib  的依赖库
			  dbghelp.lib
			   dmoguids.lib
          wmcodecdspuuid.lib
        amstrmid.lib
         msdmo.lib
        oleaut32.lib
	)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
	message(STATUS "current platform: unix")
    set(LINUX_LIB 
	sqlite3
		vpx

				crypto
				ssl
				m 
				dl
				rt
	)

endif()
 
target_link_libraries(${PROJECT_NAME}   ${ALL_LIB}   

	${WIN_LIB}
	${LINUX_LIB}
)


 
target_link_libraries(${PROJECT_NAME}   ${ALL_LIB} )

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")


set(AllFile ${GbSipServer_source} 

	${swagger_source}
)
foreach(fileItem ${AllFile})       
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${fileItem}" DIRECTORY)
	
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
	#message(status   "---> fileItem = ${fileItem}     GROUP ---->  ${GROUP}")
    # Group into "Source Files" and "Header Files"
    set(GROUP "${GROUP}")
    source_group("${GROUP}" FILES "${fileItem}")
endforeach()



set_property(TARGET ${PROJECT_NAME}  				PROPERTY FOLDER  ${DIR_NAME_GBServer} ) 

#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
