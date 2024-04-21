set ( PNDRIVER_DEFAULT_VERSION 2.1.2 )

if (TARGET PNDRIVER)
  # check if version requirements are matched
else()
    if ( PNDRIVER_FIND_VERSION )
      set ( PNDRIVER_VERSION ${PNDRIVER_FIND_VERSION} )
    else()
      set ( PNDRIVER_VERSION ${PNDRIVER_DEFAULT_VERSION} )
    endif()
    
    
if ( APPLE )
	file( GLOB_RECURSE LIB_SRC LIST_DIRECTORIES  true
		"${CMAKE_CURRENT_LIST_DIR}/source/acp/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/acp/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/clrpc/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/clrpc/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/cm/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/cm/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/dcp/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/dcp/*.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/edds/*.h"
		# "${CMAKE_CURRENT_LIST_DIR}/source/edds/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/hif/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/hif/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/lldp/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/lldp/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/mem3/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/mem3/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/mem3/*.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/nare/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/nare/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/oha/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/oha/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/*.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnio/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnio/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pntrc/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pntrc/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/psi/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/psi/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/snmpx/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/snmpx/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/sock/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/sock/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/tcip/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/tcip/*.c"
	)
	list( REMOVE_ITEM LIB_SRC
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/linux_irte_drv/irte.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/plf/eps_plf_adonis.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/plf/eps_plf_linux.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/plf/eps_plf_windows.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/windows/posix2win.c"
		
		
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pciox_psi_cfg_plf_all_e200.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pciox_psi_cfg_plf_all_e200_obsd.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pciox_psi_cfg_plf_all_e400.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pciox_psi_cfg_plf_all_e400_obsd.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pciox_psi_cfg_plf_all_soc1.h"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pciox_psi_cfg_plf_all_applonly.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pnboards_psi_cfg_plf_advanced_soc1_ertec200p_interniche.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/pnboards_psi_cfg_plf_advanced_soc1_ertec200p_obsd.h"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/eps_plf.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/psi_cfg.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/psi_cfg_hif.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/psi/psi_cfg_sock.c"
		
		# "${CMAKE_CURRENT_LIST_DIR}/source/psi/src/pnstack/lsa/lsa_cfg.h"
		# "${CMAKE_CURRENT_LIST_DIR}/source/psi/src/pnstack/lsa/lsa_sys.h"
		# "${CMAKE_CURRENT_LIST_DIR}/source/psi/src/pnstack/lsa/lsa_usr.h"
		
		
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/plf/eps_plf_linux.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/posix/eps_posix_cfg_adonis.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/posix/eps_posix_cfg_linux.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adb_cause_halt_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_cache_drv_hera.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_cache_drv_soc.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_ipc_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_vdd.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_vdd_drv.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_caching.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_c66x_bootloader.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_hif_ipcsiedma_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_hif_pncorestd_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_hw_edds.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_ipc_generic_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_ipc_linux_soc_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_msg.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_am5728_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_eb200p_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_ertec200_drv.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_ertec400_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_hera_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pncore.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_soc1_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_vdd_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_shm_file.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_tt_a15_drv.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_vdd.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_vdd_helper.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/cfg/pnd_eddi_cfg.c"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/io_base_core/pnd_iodu_eddi.cpp"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/pnio_user_core/iso/pnd_ISOUser.cpp"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/io_base_core/pnd_IOCCProcessImageUpdater.cpp"
		# "${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/io_base_core/pnd_EDDIPiMemManager.cpp"
		# "${CMAKE_CURRENT_LIST_DIR}/source/tcip/src/src/tcip_linux.c"
		
	)
	
	file( GLOB_RECURSE LIB_SRC_DIRS LIST_DIRECTORIES  true
		"${CMAKE_CURRENT_LIST_DIR}/source/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/*.c"
	)

	set( DIRS "" )
	foreach( DIR ${LIB_SRC_DIRS} )
		if ( IS_DIRECTORY  ${DIR} )
			list( APPEND DIRS ${DIR} )
		endif()
	endforeach()
	
	add_library ( PNDriver STATIC 
		${LIB_SRC}
	)
	target_include_directories( PNDriver PRIVATE
		"${DIRS}"
	)
	target_include_directories( PNDriver PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/common"
	)
	target_compile_definitions( PNDriver PRIVATE
		PNDEV_OS_LINUX
		PNIO_PNDRIVER
		PSI_EDDS_CFG_HW_INTEL
		EPS_PSI_CFG_PLATFORM_H=pnd_psi_cfg_plf_linux_intel_obsd.h
		_CRT_SECURE_NO_WARNINGS
		EPS_CFG_DO_NOT_USE_TGROUPS
		TOOL_CHAIN_GNU
		EPS_RTOS_CFG_INCLUDE_H=eps_posix_cfg_linux.h
	    PNTRC_CFG_CUSTOM_SUBSYSTEM_HEADER=pndriver_pntrc_sub.h
		_SYS_STDLIB_H_
		NO_DEBUG
		_GNU_SOURCE
		PNDEV_TEST_INTERNAL
		UINT_ALREADY
		_SYS_SELECT_H
	)
	
elseif ( WIN32 )
	file( GLOB_RECURSE LIB_SRC LIST_DIRECTORIES  true
		"${CMAKE_CURRENT_LIST_DIR}/source/acp/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/acp/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/clrpc/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/clrpc/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/cm/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/cm/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/dcp/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/dcp/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/edds/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/edds/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/hif/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/hif/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/lldp/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/lldp/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/mem3/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/mem3/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/mem3/*.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/nare/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/nare/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/oha/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/oha/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/*.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnio/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnio/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pntrc/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pntrc/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/psi/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/psi/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/snmpx/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/snmpx/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/sock/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/sock/*.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/tcip/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/tcip/*.c"
	)
	list( REMOVE_ITEM LIB_SRC
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/linux_irte_drv/irte.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/plf/eps_plf_linux.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/posix/eps_posix_cfg_adonis.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/etc/posix/eps_posix_cfg_linux.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adb_cause_halt_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_cache_drv_hera.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_cache_drv_soc.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_ipc_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_vdd.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_adonis_vdd_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_caching.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_c66x_bootloader.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_hif_pncorestd_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_ipc_generic_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_ipc_linux_soc_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_am5728_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_eb200p_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_ertec200_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_ertec400_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_hera_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_soc1_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_pn_vdd_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_tt_a15_drv.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_vdd.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnboards/eps/epssys/src/eps_vdd_helper.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/cfg/pnd_eddi_cfg.c"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/io_base_core/pnd_iodu_eddi.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/pnio_user_core/iso/pnd_ISOUser.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/io_base_core/pnd_IOCCProcessImageUpdater.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/io_base_core/pnd_EDDIPiMemManager.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/source/tcip/src/src/tcip_linux.c"
	)
	
	file( GLOB_RECURSE LIB_SRC_DIRS LIST_DIRECTORIES  true
		"${CMAKE_CURRENT_LIST_DIR}/source/*.h"
		"${CMAKE_CURRENT_LIST_DIR}/source/*.c"
	)

	set( DIRS "" )
	foreach( DIR ${LIB_SRC_DIRS} )
		if ( IS_DIRECTORY  ${DIR} )
			list( APPEND DIRS ${DIR} )
		endif()
	endforeach()
	
	add_library ( PNDriver STATIC 
		${LIB_SRC}
	)
	target_link_libraries( PNDriver WinPcap Packet winmm )
	target_include_directories( PNDriver PRIVATE
		"${DIRS}"
	)
	target_include_directories( PNDriver PUBLIC
		"${CMAKE_CURRENT_LIST_DIR}/source/pnd/src/common"
	)
	target_compile_definitions( PNDriver PRIVATE
		PASCAL
		PSI_EDDS_CFG_HW_PACKET32
		TOOL_CHAIN_MICROSOFT
		EPS_PSI_CFG_PLATFORM_H=pnd_psi_cfg_plf_windows_wpcap_obsd.h
		EPS_RTOS_CFG_INCLUDE_H=eps_posix_cfg_windows.h
		PNTRC_CFG_CUSTOM_SUBSYSTEM_HEADER=pndriver_pntrc_sub.h
		_CRT_SECURE_NO_WARNINGS
		$<$<CONFIG:Debug>:_DEBUG>
		_CONSOLE
		EPS_CFG_DO_NOT_USE_TGROUPS
		PNDEV_OS_WIN
		$<$<CONFIG:Debug>:PND_DEBUG>
	)
	
	target_compile_options( PNDriver PRIVATE
		/Od
		/RTC1
		/experimental:external
	)
	
	set_target_properties( PNDriver PROPERTIES 
		FOLDER "ExternalProjectTargets"
		CXX_STANDARD 17
		VS_PLATFORM_TOOLSET v141
	)

  endif ( )

  message( STATUS "PNDriver version: ${PNDRIVER_VERSION}" )
endif ( )