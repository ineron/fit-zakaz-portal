cmake_minimum_required(VERSION 2.8)
#define __GXX_EXPERIMENTAL_CXX0X__ 1
INCLUDE(CMakeForceCompiler)

CMAKE_FORCE_C_COMPILER( /usr/local/bin/gcc48 gcc.4.8)
CMAKE_FORCE_CXX_COMPILER ( /usr/local/bin/g++48 gcc.4.8 )


if(CMAKE_COMPILER_IS_GNUCXX)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
	message(STATUS "C++11 activated.")
	add_definitions("-std=gnu++11")
    elseif(GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
         message(WARNING "C++0x activated. If you get any errors update to a compiler which fully supports C++11")
	 add_definitions("-std=gnu++0x")
    else ()
	message(FATAL_ERROR "C++11 needed. Therefore a gcc compiler with a version higher than 4.3 is needed.")   
    endif()
else(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-std=c++0x") 
endif(CMAKE_COMPILER_IS_GNUCXX)
    	                                              
add_definitions(-DGSOAP)
#add_definitions(-DDEBUG)

find_package (Threads)

SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 ") 
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -lpthread")
#SET(CMAKE_CSS_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -lpthread")
#-g debug
 
SET(BOOST_LIBS /usr/local/lib)
#SET(BOOST_INCLUDE /usr/include/boost-1_49/)
SET(BOOST_INCLUDE /usr/local/include/boost)
 
#SET(MYSQLPP_LIBS /usr/local/lib/libmysqlpp.so)
#SET(MYSQLPP_INCLUDE /usr/local/include/mysql++)
 
#SET(VMIME_LIBS /usr/local/lib/libvmime.so)
#SET(VMIME_INCLUDE /usr/local/include/vmime)
 
#SET(MYSQL_LIBS /usr/lib/mysql)
#SET(MYSQL_INCLUDE /usr/include/mysql)
 
#SET(GD_LIBS /usr/lib/libgd.so)
#SET(GD_INCLUDE /usr/include)
 

SET(WT_LIBS /usr/local/lib)
set(WT_LIBS32 /usr/lib32)
#SET(WT_INCLUDE /usr/local/include/Wt)
SET(WT_INCLUDE /usr/local/include)

ADD_EXECUTABLE(portal.wt
		AppPortal.cpp
		CatalogView.cpp
		CatalogRightMenu.cpp
		DocumentDetails.cpp
		DefaultTheme.cpp
		ImportOrder.cpp
		FoldersPanel.cpp
		FolderView.cpp
		Messages.cpp
		MainMenu.cpp
		NavigationPanel.cpp
		OrderDetails.cpp
		OrdersJournal.cpp
		Portal.cpp
		PortalLoginWidget.cpp
		PortalServer.cpp
		PrintResource.cpp
		PriceEditor.cpp
		PriceEditDialog.cpp
		ReportDeptStrucure.cpp
		ReportMutual.cpp
		ReportOrders.cpp
		Reports.cpp
		Session.cpp
		SearchPanel.cpp
		User.cpp
		UserProfile.cpp
		soapClient.cpp
		#WidgetSlider.cpp
		db/DBUtils.cpp
		db/MsSql7.cpp
		utils/CsvUtil.C
		utils/LineEdit.cpp
		utils/MenuItem.cpp
		gSoap/soapC.cpp
		gSoap/soapInfoBaseSoapBindingProxy.cpp
		main.cpp
)

TARGET_LINK_LIBRARIES(portal.wt
					${WT_LIBS}/libodbc.so
#					${WT_LIBS}/libtdsodbc.so
					${WT_LIBS}/libboost_signals.so
					${WT_LIBS32}/libcrypt.so
					${WT_LIBS}/libhtmlcxx.so
#					${WT_LIBS}/libcryptopp.so

					${WT_LIBS}/libwtdbopostgres.so
					#${WT_LIBS}/libwthttp.so
					${WT_LIBS}/libwt.so
					${WT_LIBS}/libgsoap++.a
					${WT_LIBS}/libwtdbo.so
					${WT_LIBS}/libwtfcgi.so
					#${MYSQLPP_LIBS}
					#${VMIME_LIBS}
					${GD_LIBS}	
					#${BOOST_LIBS}/libboost_date_time-gcc42-mt-1_37.so
					#${BOOST_LIBS}/libboost_filesystem-gcc42-mt-1_37.so
					#${BOOST_LIBS}/libboost_program_options-gcc42-mt-1_37.so
					#${BOOST_LIBS}/libboost_regex-gcc42-mt-1_37.so
					#${BOOST_LIBS}/libboost_signals-gcc42-mt-1_37.so
					${WT_LIBS}/libboost_system.so
					${BOOST_LIBS}/libboost_thread.so
					${CMAKE_THREAD_LIBS_INIT}
					)

INCLUDE_DIRECTORIES(${WT_INCLUDE}	
					${WT_INCLUDE}/Ext
					${WT_INCLUDE}/Http
					${WT_INCLUDE}/Test
					#${MYSQLPP_INCLUDE}
					#${VMIME_INCLUDE}
					${BOOST_INCLUDE}
					${MYSQL_INCLUDE}
					${GD_INCLUDE})

add_custom_command(
    OUTPUT command.log
    COMMAND cp portal.wt portal.wt.20131129
)
