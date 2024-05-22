#ifndef _SMBIOS_INTERFACE_H_
#define _SMBIOS_INTERFACE_H_

#define MAX_BASEBOARD_DEVICES 128
#define MAX_MEMORY_ARRAYS 128
#define MAX_MEMORY_DEVICES 128
#define MAX_SLOT_DEVICES 256
#define MAX_PEER_GROUPS 256

#define SMBIOS_SYSTEM_WAKEUP_TYPE_RESERVED 0x00
#define SMBIOS_SYSTEM_WAKEUP_TYPE_OTHER 0x01
#define SMBIOS_SYSTEM_WAKEUP_TYPE_UNKNOWN 0x02
#define SMBIOS_SYSTEM_WAKEUP_TYPE_APM_TIMER 0x03
#define SMBIOS_SYSTEM_WAKEUP_TYPE_MODEM_RING 0x04
#define SMBIOS_SYSTEM_WAKEUP_TYPE_LAN_REMOTE 0x05
#define SMBIOS_SYSTEM_WAKEUP_TYPE_POWER_SWITCH 0x06
#define SMBIOS_SYSTEM_WAKEUP_TYPE_PCI_PME 0x07
#define SMBIOS_SYSTEM_WAKEUP_TYPE_AC_POWER_RESTORED 0x08

#define SMBIOS_BASEBOARD_BOARD_TYPE_UNKNOWN 0x01
#define SMBIOS_BASEBOARD_BOARD_TYPE_OTHER 0x02
#define SMBIOS_BASEBOARD_BOARD_TYPE_SERVER_BLADE 0x03
#define SMBIOS_BASEBOARD_BOARD_TYPE_CONNECTIVITY_SWITCH 0x04
#define SMBIOS_BASEBOARD_BOARD_TYPE_SYSTEM_MANAGEMENT_MODULE 0x05
#define SMBIOS_BASEBOARD_BOARD_TYPE_PROCESSOR_MODULE 0x06
#define SMBIOS_BASEBOARD_BOARD_TYPE_IO_MODULE 0x07
#define SMBIOS_BASEBOARD_BOARD_TYPE_MEMORY_MODULE 0x08
#define SMBIOS_BASEBOARD_BOARD_TYPE_DAUGHTER_BOARD 0x09
#define SMBIOS_BASEBOARD_BOARD_TYPE_MOTHERBOARD 0x0A
#define SMBIOS_BASEBOARD_BOARD_TYPE_PROCESSOR_MEMORY_MODULE 0x0B
#define SMBIOS_BASEBOARD_BOARD_TYPE_PROCESSOR_IO_MODULE 0x0C
#define SMBIOS_BASEBOARD_BOARD_TYPE_INTERCONNECT_BOARD 0x0D

#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_OTHER 0x01
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_UNKNOWN 0x02
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_DESKTOP 0x03
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_LOW_PROFILE_DESKTOP 0x04
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_PIZZA_BOX 0x05
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_MINI_TOWER 0x06
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_TOWER 0x07
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_PORTABLE 0x08
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_LAPTOP 0x09
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_NOTEBOOK 0x0A
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_HANDHELD 0x0B
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_DOCKING_STATION 0x0C
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_ALL_IN_ONE 0x0D
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_SUB_NOTEBOOK 0x0E
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_SPACE_SAVING 0x0F
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_LUNCH_BOX 0x10
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_MAIN_SERVER_CHASSIS 0x11
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_EXPANSION_CHASSIS 0x12
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_SUB_CHASSIS 0x13
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_BUS_EXPANSION_CHASSIS 0x14
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_PERIPHERAL_CHASSIS 0x15
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_RAID_CHASSIS 0x16
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_RACK_MOUNT_CHASSIS 0x17
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_SEALED_CASE_PC 0x18
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_MULTI_SYSTEM_CHASSIS 0x19
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_COMPACT_PCI 0x1A
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_ADVANCED_TCA 0x1B
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_BLADE 0x1C
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_BLADE_ENCLOSURE 0x1D
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_TABLET 0x1E
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_CONVERTIBLE 0x1F
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_DETACHABLE 0x20
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_IOT_GATEWAY 0x21
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_EMBEDDED_PC 0x22
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_MINI_PC 0x23
#define SMBIOS_ENCLOSURE_CHASSIS_TYPE_STICK_PC 0x24

#define SMBIOS_ENCLOSURE_CHASSIS_STATE_OTHER 0x01
#define SMBIOS_ENCLOSURE_CHASSIS_STATE_UNKNOWN 0x02
#define SMBIOS_ENCLOSURE_CHASSIS_STATE_SAFE 0x03
#define SMBIOS_ENCLOSURE_CHASSIS_STATE_WARNING 0x04
#define SMBIOS_ENCLOSURE_CHASSIS_STATE_CRITICAL 0x05
#define SMBIOS_ENCLOSURE_CHASSIS_STATE_NON_RECOVERABLE 0x06

#define SMBIOS_ENCLOSURE_SECURITY_STATUS_OTHER 0x01
#define SMBIOS_ENCLOSURE_SECURITY_STATUS_UNKNOWN 0x02
#define SMBIOS_ENCLOSURE_SECURITY_STATUS_NONE 0x03
#define SMBIOS_ENCLOSURE_SECURITY_STATUS_EXTERNAL_INTERFACE_LOCKED_OUT 0x04
#define SMBIOS_ENCLOSURE_SECURITY_STATUS_EXTERNAL_INTERFACE_ENABLED 0x05

#define SMBIOS_PROCESSOR_TYPE_OTHER 0x01
#define SMBIOS_PROCESSOR_TYPE_UNKNOWN 0x02
#define SMBIOS_PROCESSOR_TYPE_CENTRAL_PROCESSOR 0x03
#define SMBIOS_PROCESSOR_TYPE_MATH_PROCESSOR 0x04
#define SMBIOS_PROCESSOR_TYPE_DSP_PROCESSOR 0x05
#define SMBIOS_PROCESSOR_TYPE_VIDEO_PROCESSOR 0x06

#define SMBIOS_PROCESSOR_FAMILY_OTHER 0x01
#define SMBIOS_PROCESSOR_FAMILY_UNKNOWN 0x02
#define SMBIOS_PROCESSOR_FAMILY_8086 0x03
#define SMBIOS_PROCESSOR_FAMILY_80286 0x04
#define SMBIOS_PROCESSOR_FAMILY_INTEL386 0x05
#define SMBIOS_PROCESSOR_FAMILY_INTEL486 0x06
#define SMBIOS_PROCESSOR_FAMILY_8087 0x07
#define SMBIOS_PROCESSOR_FAMILY_80287 0x08
#define SMBIOS_PROCESSOR_FAMILY_80387 0x09
#define SMBIOS_PROCESSOR_FAMILY_80487 0x0A
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM 0x0B
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_PRO 0x0C
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_II 0x0D
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_MMX 0x0E
#define SMBIOS_PROCESSOR_FAMILY_CELERON 0x0F
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_II_XEON 0x10
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_III 0x11
#define SMBIOS_PROCESSOR_FAMILY_M1 0x12
#define SMBIOS_PROCESSOR_FAMILY_M2 0x13
#define SMBIOS_PROCESSOR_FAMILY_CELERON_M 0x14
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_4_HT 0x15
#define SMBIOS_PROCESSOR_FAMILY_AMD_DURON 0x18
#define SMBIOS_PROCESSOR_FAMILY_K5 0x19
#define SMBIOS_PROCESSOR_FAMILY_K6 0x1A
#define SMBIOS_PROCESSOR_FAMILY_K6_2 0x1B
#define SMBIOS_PROCESSOR_FAMILY_K6_3 0x1C
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON 0x1D
#define SMBIOS_PROCESSOR_FAMILY_AMD_29000 0x1E
#define SMBIOS_PROCESSOR_FAMILY_K6_2_PLUS 0x1F
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC 0x20
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_601 0x21
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_603 0x22
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_603_PLUS 0x23
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_604 0x24
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_620 0x25
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_X704 0x26
#define SMBIOS_PROCESSOR_FAMILY_POWER_PC_750 0x27
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_DUO 0x28
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_DUO_MOBILE 0x29
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_SOLO_MOBILE 0x2A
#define SMBIOS_PROCESSOR_FAMILY_INTEL_ATOM 0x2B
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_M 0x2C
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_M3 0x2D
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_M5 0x2E
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_M7 0x2F
#define SMBIOS_PROCESSOR_FAMILY_ALPHA 0x30
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21064 0x31
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21066 0x32
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21164 0x33
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21164PC 0x34
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21164A 0x35
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21264 0x36
#define SMBIOS_PROCESSOR_FAMILY_ALPHA_21364 0x37
#define SMBIOS_PROCESSOR_FAMILY_AMD_TURION_II_ULTRA_DUAL_CORE_MOBILE_M_PROCESSOR_FAMILY 0x38
#define SMBIOS_PROCESSOR_FAMILY_AMD_TURION_II_DUAL_CORE_MOBILE_M_PROCESSOR_FAMILY 0x39
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_II_DUAL_CORE_M_PROCESSOR_FAMILY 0x3A
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_6100_SERIES_PROCESSOR 0x3B
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_4100_SERIES_PROCESSOR 0x3C
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_6200_SERIES_PROCESSOR 0x3D
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_4200_SERIES_PROCESSOR 0x3E
#define SMBIOS_PROCESSOR_FAMILY_AMD_FX_SERIES_PROCESSOR 0x3F
#define SMBIOS_PROCESSOR_FAMILY_MIPS 0x40
#define SMBIOS_PROCESSOR_FAMILY_MIPS_R4000 0x41
#define SMBIOS_PROCESSOR_FAMILY_MIPS_R4200 0x42
#define SMBIOS_PROCESSOR_FAMILY_MIPS_R4400 0x43
#define SMBIOS_PROCESSOR_FAMILY_MIPS_R4600 0x44
#define SMBIOS_PROCESSOR_FAMILY_MIPS_R10000 0x45
#define SMBIOS_PROCESSOR_FAMILY_AMD_C_SERIES_PROCESSOR 0x46
#define SMBIOS_PROCESSOR_FAMILY_AMD_E_SERIES_PROCESSOR 0x47
#define SMBIOS_PROCESSOR_FAMILY_AMD_A_SERIES_PROCESSOR 0x48
#define SMBIOS_PROCESSOR_FAMILY_AMD_G_SERIES_PROCESSOR 0x49
#define SMBIOS_PROCESSOR_FAMILY_AMD_Z_SERIES_PROCESSOR 0x4A
#define SMBIOS_PROCESSOR_FAMILY_AMD_R_SERIES_PROCESSOR 0x4B
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_4300_SERIES_PROCESSOR 0x4C
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_6300_SERIES_PROCESSOR 0x4D
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_3300_SERIES_PROCESSOR 0x4E
#define SMBIOS_PROCESSOR_FAMILY_AMD_FIREPRO_SERIES_PROCESSOR 0x4F
#define SMBIOS_PROCESSOR_FAMILY_SPARC 0x50
#define SMBIOS_PROCESSOR_FAMILY_SUPER_SPARC 0x51
#define SMBIOS_PROCESSOR_FAMILY_MICRO_SPARC_II 0x52
#define SMBIOS_PROCESSOR_FAMILY_MICRO_SPARC_IIEP 0x53
#define SMBIOS_PROCESSOR_FAMILY_ULTRA_SPARC 0x54
#define SMBIOS_PROCESSOR_FAMILY_ULTRA_SPARC_II 0x55
#define SMBIOS_PROCESSOR_FAMILY_ULTRA_SPARC_III 0x56
#define SMBIOS_PROCESSOR_FAMILY_ULTRA_SPARC_III_PLUS 0x57
#define SMBIOS_PROCESSOR_FAMILY_ULTRA_SPARC_IV 0x58
#define SMBIOS_PROCESSOR_FAMILY_68040 0x60
#define SMBIOS_PROCESSOR_FAMILY_68XXX 0x61
#define SMBIOS_PROCESSOR_FAMILY_68000 0x62
#define SMBIOS_PROCESSOR_FAMILY_68010 0x63
#define SMBIOS_PROCESSOR_FAMILY_68020 0x64
#define SMBIOS_PROCESSOR_FAMILY_68030 0x65
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_X4_QUAD_CORE_PROCESSOR_FAMILY 0x66
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_X1000_SERIES_PROCESSOR 0x67
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_X2000_SERIES_APUS_PROCESSOR 0x68
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_A_SERIES_PROCESSOR 0x69
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_X3000_SERIES_APUS_PROCESSOR 0x6A
#define SMBIOS_PROCESSOR_FAMILY_AMD_ZEN_PROCESSOR 0x6B
#define SMBIOS_PROCESSOR_FAMILY_HOBBIT 0x70
#define SMBIOS_PROCESSOR_FAMILY_CRUSOE_TM5000 0x78
#define SMBIOS_PROCESSOR_FAMILY_CRUSOE_TM3000 0x79
#define SMBIOS_PROCESSOR_FAMILY_EFFICEON_TM8000 0x7A
#define SMBIOS_PROCESSOR_FAMILY_WEITEK 0x80
#define SMBIOS_PROCESSOR_FAMILY_ITANIUM 0x82
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_64_PROCESSOR_FAMILY 0x83
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_PROCESSOR_FAMILY 0x84
#define SMBIOS_PROCESSOR_FAMILY_AMD_SEMPRON_PROCESSOR_FAMILY 0x85
#define SMBIOS_PROCESSOR_FAMILY_AMD_TURION_64_MOBILE_TECHNOLOGY 0x86
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_AMD_OPTERON_PROCESSOR_FAMILY 0x87
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_64_X2_DUAL_CORE_PROCESSOR_FAMILY 0x88
#define SMBIOS_PROCESSOR_FAMILY_AMD_TURION_64_X2_MOBILE_TECHNOLOGY 0x89
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_AMD_OPTERON_PROCESSOR_FAMILY 0x8A
#define SMBIOS_PROCESSOR_FAMILY_THIRD_GENERATION_AMD_OPTERON_PROCESSOR_FAMILY 0x8B
#define SMBIOS_PROCESSOR_FAMILY_AMD_PHENOM_FX_QUAD_CORE_PROCESSOR_FAMILY 0x8C
#define SMBIOS_PROCESSOR_FAMILY_AMD_PHENOM_X4_QUAD_CORE_PROCESSOR_FAMILY 0x8D
#define SMBIOS_PROCESSOR_FAMILY_AMD_PHENOM_X2_DUAL_CORE_PROCESSOR_FAMILY 0x8E
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_X2_DUAL_CORE_PROCESSOR_FAMILY 0x8F
#define SMBIOS_PROCESSOR_FAMILY_PARISC 0x90
#define SMBIOS_PROCESSOR_FAMILY_PA_RISC_8500 0x91
#define SMBIOS_PROCESSOR_FAMILY_PA_RISC_8000 0x92
#define SMBIOS_PROCESSOR_FAMILY_PA_RISC_7300LC 0x93
#define SMBIOS_PROCESSOR_FAMILY_PA_RISC_7200 0x94
#define SMBIOS_PROCESSOR_FAMILY_PA_RISC_7100LC 0x95
#define SMBIOS_PROCESSOR_FAMILY_PA_RISC_7100 0x96
#define SMBIOS_PROCESSOR_FAMILY_V30 0xA0
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_3200_SERIES 0xA1
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_3000_SERIES 0xA2
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_5300_SERIES 0xA3
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_5100_SERIES 0xA4
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_5000_SERIES 0xA5
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_LV 0xA6
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_ULV 0xA7
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_7100_SERIES 0xA8
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_5400_SERIES 0xA9
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR 0xAA
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_5200_SERIES 0xAB
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_7200_SERIES 0xAC
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_7300_SERIES 0xAD
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_7400_SERIES 0xAE
#define SMBIOS_PROCESSOR_FAMILY_MULTI_CORE_INTEL_XEON_PROCESSOR_7400_SERIES 0xAF
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_III_XEON_PROCESSOR 0xB0
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_III_PROCESSOR_WITH_INTEL_SPEEDSTEP_TECHNOLOGY 0xB1
#define SMBIOS_PROCESSOR_FAMILY_PENTIUM_4_PROCESSOR 0xB2
#define SMBIOS_PROCESSOR_FAMILY_INTEL_XEON_PROCESSOR 0xB3
#define SMBIOS_PROCESSOR_FAMILY_AS400 0xB4
#define SMBIOS_PROCESSOR_FAMILY_INTEL_XEON_PROCESSOR_MP 0xB5
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_XP_PROCESSOR_FAMILY 0xB6
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_MP_PROCESSOR_FAMILY 0xB7
#define SMBIOS_PROCESSOR_FAMILY_INTEL_ITANIUM_2_PROCESSOR 0xB8
#define SMBIOS_PROCESSOR_FAMILY_INTEL_PENTIUM_M_PROCESSOR 0xB9
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CELERON_D_PROCESSOR 0xBA
#define SMBIOS_PROCESSOR_FAMILY_INTEL_PENTIUM_D_PROCESSOR 0xBB
#define SMBIOS_PROCESSOR_FAMILY_INTEL_PENTIUM_PROCESSOR_XE 0xBC
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_SOLO_PROCESSOR 0xBD
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_DUO_PROCESSOR 0xBF
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_SOLO_PROCESSOR 0xC0
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_EXTREME_PROCESSOR 0xC1
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_QUAD_PROCESSOR 0xC2
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_EXTREME_MOBILE_PROCESSOR 0xC3
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_DUO_MOBILE_PROCESSOR 0xC4
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_2_SOLO_MOBILE_PROCESSOR 0xC5
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_I7_PROCESSOR 0xC6
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_CELERON_PROCESSOR 0xC7
#define SMBIOS_PROCESSOR_FAMILY_IBM390 0xC8
#define SMBIOS_PROCESSOR_FAMILY_G4 0xC9
#define SMBIOS_PROCESSOR_FAMILY_G5 0xCA
#define SMBIOS_PROCESSOR_FAMILY_ESA_390_G6 0xCB
#define SMBIOS_PROCESSOR_FAMILY_Z_ARCHITECTURE_BASE 0xCC
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_I5_PROCESSOR 0xCD
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_I3_PROCESSOR 0xCE
#define SMBIOS_PROCESSOR_FAMILY_INTEL_CORE_I9_PROCESSOR 0xCF
#define SMBIOS_PROCESSOR_FAMILY_VIA_C7_M_PROCESSOR_FAMILY 0xD2
#define SMBIOS_PROCESSOR_FAMILY_VIA_C7_D_PROCESSOR_FAMILY 0xD3
#define SMBIOS_PROCESSOR_FAMILY_VIA_C7_PROCESSOR_FAMILY 0xD4
#define SMBIOS_PROCESSOR_FAMILY_VIA_EDEN_PROCESSOR_FAMILY 0xD5
#define SMBIOS_PROCESSOR_FAMILY_MULTI_CORE_INTEL_XEON_PROCESSOR 0xD6
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_3XXX_SERIES 0xD7
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_3XXX_SERIES 0xD8
#define SMBIOS_PROCESSOR_FAMILY_VIA_NANO_PROCESSOR_FAMILY 0xD9
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_5XXX_SERIES 0xDA
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_5XXX_SERIES 0xDB
#define SMBIOS_PROCESSOR_FAMILY_DUAL_CORE_INTEL_XEON_PROCESSOR_7XXX_SERIES 0xDD
#define SMBIOS_PROCESSOR_FAMILY_QUAD_CORE_INTEL_XEON_PROCESSOR_7XXX_SERIES 0xDE
#define SMBIOS_PROCESSOR_FAMILY_MULTI_CORE_INTEL_XEON_PROCESSOR_7XXX_SERIES 0xDF
#define SMBIOS_PROCESSOR_FAMILY_MULTI_CORE_INTEL_XEON_PROCESSOR_3400_SERIES 0xE0
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_3000_SERIES_PROCESSOR 0xE4
#define SMBIOS_PROCESSOR_FAMILY_AMD_SEMPRON_II_PROCESSOR 0xE5
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_QUAD_CORE_PROCESSOR 0xE6
#define SMBIOS_PROCESSOR_FAMILY_AMD_PHENOM_TRIPLE_CORE_PROCESSOR 0xE7
#define SMBIOS_PROCESSOR_FAMILY_AMD_TURION_II_ULTRA_DUAL_CORE_MOBILE_PROCESSOR 0xE8
#define SMBIOS_PROCESSOR_FAMILY_AMD_TURION_II_DUAL_CORE_MOBILE_PROCESSOR 0xE9
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_II_DUAL_CORE_PROCESSOR 0xEA
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_SI_CORE_PROCESSOR 0xEB
#define SMBIOS_PROCESSOR_FAMILY_AMD_PHENOM_II_PROCESSOR 0xEC
#define SMBIOS_PROCESSOR_FAMILY_AMD_ATHLON_II_PROCESSOR 0xED
#define SMBIOS_PROCESSOR_FAMILY_AMD_OPTERON_SIX_CORE_PROCESSOR 0xEE
#define SMBIOS_PROCESSOR_FAMILY_AMD_SEMPRON_M_PROCESSOR 0xEF
#define SMBIOS_PROCESSOR_FAMILY_I680_PROCESSOR 0xFA
#define SMBIOS_PROCESSOR_FAMILY_I960_PROCESSOR 0xFB
#define SMBIOS_PROCESSOR_FAMILY_READ_FIELD2_PROCESSOR 0xFE //IMPORTANT, READ FIELD 2
#define SMBIOS_PROCESSOR_FAMILY_ARM7 0x100
#define SMBIOS_PROCESSOR_FAMILY_ARM8 0x101
#define SMBIOS_PROCESSOR_FAMILY_ARM9 0x102
#define SMBIOS_PROCESSOR_FAMILY_ARM_FUTURE 0x103
#define SMBIOS_PROCESSOR_FAMILY_SH3 0x104
#define SMBIOS_PROCESSOR_FAMILY_SH4 0x105
#define SMBIOS_PROCESSOR_FAMILY_ARM 0x118
#define SMBIOS_PROCESSOR_FAMILY_STRONG_ARM 0x119
#define SMBIOS_PROCESSOR_FAMILY_CYRIX_6X86 0x12C
#define SMBIOS_PROCESSOR_FAMILY_MEDIA_GX 0x12D
#define SMBIOS_PROCESSOR_FAMILY_MII 0x12E
#define SMBIOS_PROCESSOR_FAMILY_WINCHIP 0x140
#define SMBIOS_PROCESSOR_FAMILY_DSP 0x15E
#define SMBIOS_PROCESSOR_FAMILY_VIDEO_PROCESSOR 0x1F4
#define SMBIOS_PROCESSOR_FAMILY_RISCV_32 0x200
#define SMBIOS_PROCESSOR_FAMILY_RISCV_64 0x201
#define SMBIOS_PROCESSOR_FAMILY_RISCV_128 0x202
#define SMBIOS_PROCESSOR_FAMILY_LOONGARCH 0x258
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_1 0x259
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_2 0x25A
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3 0x25B
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_2K 0x25C
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3A 0x25D
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3B 0x25E
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3C 0x25F
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3D 0x260
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3E 0x261
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_2K_DUAL_CORE 0x262
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3A_QUAD_CORE 0x26C
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3A_MULTI_CORE 0x26D
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3B_QUAD_CORE 0x26E
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3B_MULTI_CORE 0x26F
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3C_MULTI_CORE 0x270
#define SMBIOS_PROCESSOR_FAMILY_LOONGSON_3D_MULTI_CORE 0x271

#define SMBIOS_PROCESSOR_UPGRADE_OTHER 0x01
#define SMBIOS_PROCESSOR_UPGRADE_UNKNOWN 0x02
#define SMBIOS_PROCESSOR_UPGRADE_DAUGHTER_BOARD 0x03
#define SMBIOS_PROCESSOR_UPGRADE_ZIF_SOCKET 0x04
#define SMBIOS_PROCESSOR_UPGRADE_REPLACEABLE_PIGGY_BACK 0x05
#define SMBIOS_PROCESSOR_UPGRADE_NONE 0x06
#define SMBIOS_PROCESSOR_UPGRADE_LIF_SOCKET 0x07
#define SMBIOS_PROCESSOR_UPGRADE_SLOT_1 0x08
#define SMBIOS_PROCESSOR_UPGRADE_SLOT_2 0x09
#define SMBIOS_PROCESSOR_UPGRADE_370_PIN_SOCKET 0x0A
#define SMBIOS_PROCESSOR_UPGRADE_SLOT_A 0x0B
#define SMBIOS_PROCESSOR_UPGRADE_SLOT_M 0x0C
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_423 0x0D
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_A_462 0x0E
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_478 0x0F    
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_754 0x10
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_940 0x11
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_939 0x12
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_MPGA604 0x13
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA771 0x14
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA775 0x15
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_S1 0x16
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_AM2 0x17
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_F_1207 0x18
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1366 0x19
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_G34 0x1A
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_AM3 0x1B
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_C32 0x1C
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1156 0x1D
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1567 0x1E
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_PGA988A 0x1F
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1288 0x20
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_RPGA988B 0x21
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1023 0x22
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1224 0x23
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1155 0x24
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1356 0x25
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA2011 0x26
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_FS1 0x27
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_FS2 0x28
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_FM1 0x29
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_FM2 0x2A
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA2011_3 0x2B
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1356_3 0x2C
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1150 0x2D
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1168 0x2E
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1234 0x2F
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1364 0x30
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_AM4 0x31
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1151 0x32
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1356 0x33
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1440 0x34
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1515 0x35
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA3647_1 0x36
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_SP3 0x37
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_SP3R2 0x38
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA2066 0x39
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1392 0x3A
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1510 0x3B
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1528 0x3C
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA4189 0x3D
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1200 0x3E
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA4677 0x3F
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1700 0x40
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1744 0x41
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1781 0x42
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA1211 0x43
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA2422 0x44
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA1211 0x45
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA2422 0x46
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_LGA5773 0x47
#define SMBIOS_PROCESSOR_UPGRADE_SOCKET_BGA5773 0x48

#define SMBIOS_MEMORY_ARRAY_LOCATION_OTHER 0x01
#define SMBIOS_MEMORY_ARRAY_LOCATION_UNKNOWN 0x02
#define SMBIOS_MEMORY_ARRAY_LOCATION_SYSTEM_BOARD_OR_MOTHERBOARD 0x03
#define SMBIOS_MEMORY_ARRAY_LOCATION_ISA_ADD_ON_CARD 0x04
#define SMBIOS_MEMORY_ARRAY_LOCATION_EISA_ADD_ON_CARD 0x05
#define SMBIOS_MEMORY_ARRAY_LOCATION_PCI_ADD_ON_CARD 0x06
#define SMBIOS_MEMORY_ARRAY_LOCATION_MCA_ADD_ON_CARD 0x07
#define SMBIOS_MEMORY_ARRAY_LOCATION_PCMCIA_ADD_ON_CARD 0x08
#define SMBIOS_MEMORY_ARRAY_LOCATION_PROPRIETARY_ADD_ON_CARD 0x09
#define SMBIOS_MEMORY_ARRAY_LOCATION_NUBUS 0x0A
#define SMBIOS_MEMORY_ARRAY_LOCATION_PC98_C20_ADD_ON_CARD 0xA0
#define SMBIOS_MEMORY_ARRAY_LOCATION_PC98_C24_ADD_ON_CARD 0xA1
#define SMBIOS_MEMORY_ARRAY_LOCATION_PC98_E_ADD_ON_CARD 0xA2
#define SMBIOS_MEMORY_ARRAY_LOCATION_PC98_LOCAL_BUS_ADD_ON_CARD 0xA3

#define SMBIOS_MEMORY_ARRAY_USE_OTHER 0x01
#define SMBIOS_MEMORY_ARRAY_USE_UNKNOWN 0x02
#define SMBIOS_MEMORY_ARRAY_USE_SYSTEM_MEMORY 0x03
#define SMBIOS_MEMORY_ARRAY_USE_VIDEO_MEMORY 0x04
#define SMBIOS_MEMORY_ARRAY_USE_FLASH_MEMORY 0x05
#define SMBIOS_MEMORY_ARRAY_USE_NON_VOLATILE_RAM 0x06
#define SMBIOS_MEMORY_ARRAY_USE_CACHE_MEMORY 0x07

#define SMBIOS_MEMORY_ERROR_CORRECTION_OTHER 0x01
#define SMBIOS_MEMORY_ERROR_CORRECTION_UNKNOWN 0x02
#define SMBIOS_MEMORY_ERROR_CORRECTION_NONE 0x03
#define SMBIOS_MEMORY_ERROR_CORRECTION_PARITY 0x04
#define SMBIOS_MEMORY_ERROR_CORRECTION_SINGLE_BIT_ERROR_CORRECTION 0x05
#define SMBIOS_MEMORY_ERROR_CORRECTION_MULTIPLE_BIT_ERROR_CORRECTION 0x06
#define SMBIOS_MEMORY_ERROR_CORRECTION_CRC 0x07

#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_OTHER 0x01
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_UNKNOWN 0x02
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_SIMM 0x03
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_SIP 0x04
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_CHIP 0x05
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_DIP 0x06
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_ZIP 0x07
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_PROPRIETARY_CARD 0x08
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_DIMM 0x09
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_TSOP 0x0A
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_ROW_OF_CHIPS 0x0B
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_RIMM 0x0C
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_SO_DIMM 0x0D
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_SODIMM 0x0D
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_SRIMM 0x0E
#define SMBIOS_MEMORY_DEVICE_FORM_FACTOR_FBDIMM 0x0F

#define SMBIOS_MEMORY_DEVICE_TYPE_OTHER 0x01
#define SMBIOS_MEMORY_DEVICE_TYPE_UNKNOWN 0x02
#define SMBIOS_MEMORY_DEVICE_TYPE_DRAM 0x03
#define SMBIOS_MEMORY_DEVICE_TYPE_EDRAM 0x04
#define SMBIOS_MEMORY_DEVICE_TYPE_VRAM 0x05
#define SMBIOS_MEMORY_DEVICE_TYPE_SRAM 0x06
#define SMBIOS_MEMORY_DEVICE_TYPE_RAM 0x07
#define SMBIOS_MEMORY_DEVICE_TYPE_ROM 0x08
#define SMBIOS_MEMORY_DEVICE_TYPE_FLASH 0x09
#define SMBIOS_MEMORY_DEVICE_TYPE_EEPROM 0x0A
#define SMBIOS_MEMORY_DEVICE_TYPE_FEPROM 0x0B
#define SMBIOS_MEMORY_DEVICE_TYPE_EPROM 0x0C
#define SMBIOS_MEMORY_DEVICE_TYPE_CDRAM 0x0D
#define SMBIOS_MEMORY_DEVICE_TYPE_3DRAM 0x0E
#define SMBIOS_MEMORY_DEVICE_TYPE_SDRAM 0x0F
#define SMBIOS_MEMORY_DEVICE_TYPE_SGRAM 0x10
#define SMBIOS_MEMORY_DEVICE_TYPE_RDRAM 0x11
#define SMBIOS_MEMORY_DEVICE_TYPE_DDR 0x12
#define SMBIOS_MEMORY_DEVICE_TYPE_DDR2 0x13
#define SMBIOS_MEMORY_DEVICE_TYPE_DDR2_FB_DIMM 0x14
#define SMBIOS_MEMORY_DEVICE_TYPE_RESERVED_1 0x15
#define SMBIOS_MEMORY_DEVICE_TYPE_RESERVED_2 0x16
#define SMBIOS_MEMORY_DEVICE_TYPE_RESERVED_3 0x17
#define SMBIOS_MEMORY_DEVICE_TYPE_DDR3 0x18
#define SMBIOS_MEMORY_DEVICE_TYPE_FBD2 0x19
#define SMBIOS_MEMORY_DEVICE_TYPE_DDR4 0x1A
#define SMBIOS_MEMORY_DEVICE_TYPE_LPDDR 0x1B
#define SMBIOS_MEMORY_DEVICE_TYPE_LPDDR2 0x1C
#define SMBIOS_MEMORY_DEVICE_TYPE_LPDDR3 0x1D
#define SMBIOS_MEMORY_DEVICE_TYPE_LPDDR4 0x1E
#define SMBIOS_MEMORY_DEVICE_TYPE_LOGICAL_NON_VOLATILE_DEVICE 0x1F

#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_OTHER 0x01
#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_UNKNOWN 0x02
#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_DRAM 0x03
#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_NVDIMM_N 0x04
#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_NVDIMM_F 0x05
#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_NVDIMM_P 0x06
#define SMBIOS_MEMORY_DEVICE_TECHNOLOGY_INTEL_PERSISTENT_MEMORY 0x07

#include <stdint.h>               

struct smbios_data_entry {
    void * pointer;
    uint64_t length;
};

struct smbios_bitfield_entry {
    void * pointer;
    uint64_t bit;    //Starting bit
    uint64_t length; //Number of bits
};

struct smbios_bios_data {

    uint8_t loaded;

    struct smbios_data_entry vendor;
    struct smbios_data_entry version;
    struct smbios_data_entry start_address_segment;
    struct smbios_data_entry release_date;
    struct smbios_data_entry rom_size;
    
    //Characteristics bitfield (8 bytes)
    //Reserved
    //Reserved
    //Unknown
    struct smbios_bitfield_entry supports_characteristics;
    struct smbios_bitfield_entry supports_isa;
    struct smbios_bitfield_entry supports_mca;
    struct smbios_bitfield_entry supports_eisa;
    struct smbios_bitfield_entry supports_pci;
    struct smbios_bitfield_entry supports_pcmcia;
    struct smbios_bitfield_entry supports_pnp;
    struct smbios_bitfield_entry supports_apm;
    struct smbios_bitfield_entry supports_flash_upgrade;
    struct smbios_bitfield_entry supports_bios_shadowing;
    struct smbios_bitfield_entry supports_vl_vesa;
    struct smbios_bitfield_entry supports_escd;
    struct smbios_bitfield_entry supports_boot_from_cd;
    struct smbios_bitfield_entry supports_selectable_boot;
    struct smbios_bitfield_entry supports_bios_rom_socketed;
    struct smbios_bitfield_entry supports_boot_from_pc_card;
    struct smbios_bitfield_entry supports_edd;
    struct smbios_bitfield_entry supports_int_13h_japanese_floppy_nec9800;
    struct smbios_bitfield_entry supports_int_13h_japanese_floppy_toshiba;
    struct smbios_bitfield_entry supports_int_13h_525_360_kb_floppy;
    struct smbios_bitfield_entry supports_int_13h_525_1200_kb_floppy;
    struct smbios_bitfield_entry supports_int_13h_35_720_kb_floppy;
    struct smbios_bitfield_entry supports_int_13h_35_2880_kb_floppy;
    struct smbios_bitfield_entry supports_int_5h_print_screen;
    struct smbios_bitfield_entry supports_int_9h_842_keyboad;
    struct smbios_bitfield_entry supports_int_14h_serial_services;
    struct smbios_bitfield_entry supports_int_17h_printer_services;
    struct smbios_bitfield_entry supports_int_10h_cga_mono_video;
    struct smbios_bitfield_entry supports_nec_pc_98;
    //Bits 32-47
    struct smbios_bitfield_entry reserved_bios_vendor;
    //Bits 48-63
    struct smbios_bitfield_entry reserved_system_vendor;

    //Characteristics extended byte 1
    struct smbios_bitfield_entry supports_acpi;
    struct smbios_bitfield_entry supports_usb_legacy;
    struct smbios_bitfield_entry supports_agp;
    struct smbios_bitfield_entry supports_i2o_boot;
    struct smbios_bitfield_entry supports_ls_120_boot;
    struct smbios_bitfield_entry supports_atapi_zip_drive_boot;
    struct smbios_bitfield_entry supports_1394_boot;
    struct smbios_bitfield_entry supports_smart_battery;

    //Characteristics extended byte 2
    struct smbios_bitfield_entry supports_bios_boot;
    struct smbios_bitfield_entry fnkey_initiated_netboot;
    struct smbios_bitfield_entry supports_targeted_content_distribution;
    struct smbios_bitfield_entry supports_uefi;
    struct smbios_bitfield_entry is_virtual_machine;
    struct smbios_bitfield_entry manufacturing_mode_supported;
    struct smbios_bitfield_entry manufacturing_mode_enabled;
    //Reserved bit here

    struct smbios_data_entry system_bios_major_release;
    struct smbios_data_entry system_bios_minor_release;
    struct smbios_data_entry embedded_controller_major_release;
    struct smbios_data_entry embedded_controller_minor_release;

    struct smbios_data_entry extended_bios_rom_size;
};

struct smbios_system_data {

    uint8_t loaded;

    struct smbios_data_entry manufacturer;
    struct smbios_data_entry product_name;
    struct smbios_data_entry version;
    struct smbios_data_entry serial_number;
    struct smbios_data_entry uuid;
    struct smbios_data_entry wake_up_type;
    struct smbios_data_entry sku_number;
    struct smbios_data_entry family;
};

struct smbios_baseboard_data {

    uint8_t loaded;

    struct smbios_data_entry manufacturer;
    struct smbios_data_entry product;
    struct smbios_data_entry version;
    struct smbios_data_entry serial_number;
    struct smbios_data_entry asset_tag;

    //Feature flags
    struct smbios_bitfield_entry is_hosting_board;
    struct smbios_bitfield_entry requires_daughter_board;
    struct smbios_bitfield_entry is_removable;
    struct smbios_bitfield_entry replaceable;
    struct smbios_bitfield_entry is_hot_swappable;
    //Reserved 3 bits

    struct smbios_data_entry location;
    struct smbios_data_entry chassis_handle;
    struct smbios_data_entry board_type;
    struct smbios_data_entry number_of_contained_object_handles;
    struct smbios_data_entry contained_object_handles; //Array of (number_of_contained_object_handles) handles
};

struct smbios_cpu_data {

    uint8_t loaded;

    struct smbios_data_entry socket_designation;
    struct smbios_data_entry processor_type;
    struct smbios_data_entry processor_family;
    struct smbios_data_entry processor_manufacturer;
    struct smbios_data_entry processor_id;
    struct smbios_data_entry processor_version;

    struct smbios_bitfield_entry voltage_legacy;
    struct smbios_bitfield_entry voltage_5V;
    struct smbios_bitfield_entry voltage_3_3V;
    struct smbios_bitfield_entry voltage_2_9V;

    struct smbios_data_entry external_clock;
    struct smbios_data_entry max_speed;
    struct smbios_data_entry current_speed;
  
    struct smbios_bitfield_entry socket_populated;
    struct smbios_bitfield_entry cpu_status;

    struct smbios_data_entry processor_upgrade;
    struct smbios_data_entry l1_cache_handle;
    struct smbios_data_entry l2_cache_handle;
    struct smbios_data_entry l3_cache_handle;
    struct smbios_data_entry serial_number;
    struct smbios_data_entry asset_tag;
    struct smbios_data_entry part_number;
    struct smbios_data_entry core_count;
    struct smbios_data_entry core_enabled;
    struct smbios_data_entry thread_count;

    struct smbios_bitfield_entry supports_64_bits;
    struct smbios_bitfield_entry supports_multicore;
    struct smbios_bitfield_entry supports_hardware_thread;
    struct smbios_bitfield_entry supports_execute_protection;
    struct smbios_bitfield_entry supports_enhanced_virtualization;
    struct smbios_bitfield_entry supports_power_management;
    struct smbios_bitfield_entry supports_128_bits;
    struct smbios_bitfield_entry is_arm_soc;

    struct smbios_data_entry processor_family2;
    struct smbios_data_entry core_count2;
    struct smbios_data_entry core_enabled2;
    struct smbios_data_entry thread_count2;
    struct smbios_data_entry thread_enabled;
};

struct smbios_memory_array_data {
    struct smbios_data_entry location;
    struct smbios_data_entry use;
    struct smbios_data_entry error_correction;
    struct smbios_data_entry maximum_capacity;
    struct smbios_data_entry error_information_handle;
    struct smbios_data_entry number_of_memory_devices;
    struct smbios_data_entry extended_maximum_capacity;
};

struct smbios_memory_device_data {
    struct smbios_data_entry physical_memory_array_handle;
    struct smbios_data_entry memory_error_information_handle;
    struct smbios_data_entry total_width;
    struct smbios_data_entry data_width;
    struct smbios_data_entry size;
    struct smbios_data_entry form_factor;
    struct smbios_data_entry device_set;
    struct smbios_data_entry device_locator;
    struct smbios_data_entry bank_locator;
    struct smbios_data_entry memory_type;
    //Reserved bit
    struct smbios_bitfield_entry type_detail_other;
    struct smbios_bitfield_entry type_detail_unknown;
    struct smbios_bitfield_entry type_detail_fast_paged;
    struct smbios_bitfield_entry type_detail_static_column;
    struct smbios_bitfield_entry type_detail_pseudo_static;
    struct smbios_bitfield_entry type_detail_ram_bus;
    struct smbios_bitfield_entry type_detail_synchronous;
    struct smbios_bitfield_entry type_detail_cmos;
    struct smbios_bitfield_entry type_detail_edo;
    struct smbios_bitfield_entry type_detail_window_dram;
    struct smbios_bitfield_entry type_detail_cache_dram;
    struct smbios_bitfield_entry type_detail_non_volatile;
    struct smbios_bitfield_entry type_detail_registered_buffered;
    struct smbios_bitfield_entry type_detail_unbuffered;
    struct smbios_bitfield_entry type_detail_lr_dimm;

    struct smbios_data_entry speed;
    struct smbios_data_entry manufacturer;
    struct smbios_data_entry serial_number;
    struct smbios_data_entry asset_tag;
    struct smbios_data_entry part_number;
    struct smbios_data_entry attributes;
    struct smbios_data_entry extended_size;
    struct smbios_data_entry configured_memory_clock_speed;
    struct smbios_data_entry minimum_voltage;
    struct smbios_data_entry maximum_voltage;
    struct smbios_data_entry configured_voltage;
    struct smbios_data_entry memory_technology;

    //Reserved 0 bit
    struct smbios_bitfield_entry memory_operating_mode_capability_other;
    struct smbios_bitfield_entry memory_operating_mode_capability_unknown;
    struct smbios_bitfield_entry memory_operating_mode_capability_volatile;
    struct smbios_bitfield_entry memory_operating_mode_capability_byte_access;
    struct smbios_bitfield_entry memory_operating_mode_capability_block_access;
    //Reserved 6:15

    struct smbios_data_entry firmware_version;
    struct smbios_data_entry module_manufacturer_id;
    struct smbios_data_entry module_product_id;
    struct smbios_data_entry memory_subsystem_controller_manufacturer_id;
    struct smbios_data_entry memory_subsystem_controller_product_id;
    struct smbios_data_entry non_volatile_size;
    struct smbios_data_entry volatile_size;
    struct smbios_data_entry cache_size;
    struct smbios_data_entry logical_size;
};

struct smbios_slot_data {
    struct smbios_data_entry slot_designation;
    struct smbios_data_entry slot_type;
    struct smbios_data_entry slot_data_bus_width;
    struct smbios_data_entry current_usage;
    struct smbios_data_entry slot_length;
    struct smbios_data_entry slot_id;
    struct smbios_bitfield_entry slot_characteristics1_unknown;
    struct smbios_bitfield_entry slot_characteristics1_provides_5V;
    struct smbios_bitfield_entry slot_characteristics1_provides_3_3V;
    struct smbios_bitfield_entry slot_characteristics1_opening_is_shared;
    struct smbios_bitfield_entry slot_characteristics1_supports_card_16;
    struct smbios_bitfield_entry slot_characteristics1_supports_cardbus;
    struct smbios_bitfield_entry slot_characteristics1_supports_zv;
    struct smbios_bitfield_entry slot_characteristics1_supports_modem_ring_resume;
    struct smbios_bitfield_entry slot_characteristics2_supports_pme_signal;
    struct smbios_bitfield_entry slot_characteristics2_supports_hot_plug_devices;
    struct smbios_bitfield_entry slot_characteristics2_supports_smbus_signal;
    struct smbios_bitfield_entry slot_characteristics2_supports_pcie_bifurcation;
    struct smbios_bitfield_entry slot_characteristics2_supports_surprise_removal;
    struct smbios_bitfield_entry slot_characteristics2_supports_flexbus_cxl_1_0;
    struct smbios_bitfield_entry slot_characteristics2_supports_flexbus_cxl_2_0;
    struct smbios_data_entry segment_group_number;
    struct smbios_data_entry bus_number;
    struct smbios_bitfield_entry function_number;
    struct smbios_bitfield_entry device_number;
    struct smbios_data_entry data_bus_width;
    struct smbios_data_entry peer_grouping_n;
    struct smbios_data_entry peer_group[MAX_PEER_GROUPS];
    struct smbios_data_entry slot_information;
    struct smbios_data_entry slot_physical_width;
    struct smbios_data_entry slot_pitch;
    struct smbios_data_entry slot_height;
};

struct smbios_oem_data {

    uint8_t loaded;

    struct smbios_data_entry count;
    struct smbios_data_entry strings;
};

struct smbios_data {
    uint8_t baseboard_devices;
    uint8_t memory_array_devices;
    uint8_t memory_device_devices;
    uint8_t slot_devices;

    struct smbios_bios_data bios;
    struct smbios_system_data system;
    struct smbios_baseboard_data baseboard[MAX_BASEBOARD_DEVICES];
    struct smbios_cpu_data cpu;
    struct smbios_oem_data oem;
    struct smbios_memory_array_data memory_array[MAX_MEMORY_ARRAYS];
    struct smbios_memory_device_data memory_device[MAX_MEMORY_DEVICES];
    struct smbios_slot_data slot[MAX_SLOT_DEVICES];
};

void init_smbios_interface();
void insert_smbios_data(struct smbios_data_entry *, void *, uint16_t);
void insert_smbios_bitfield(struct smbios_bitfield_entry *, void *, uint8_t, uint8_t);

void set_smbios_bios_loaded(uint8_t);
void set_smbios_system_loaded(uint8_t);
void set_smbios_cpu_loaded(uint8_t);
void set_smbios_oem_loaded(uint8_t);
void set_smbios_memory_array_loaded(uint8_t);

uint8_t get_smbios_bios_loaded();
uint8_t get_smbios_system_loaded();
uint8_t get_smbios_cpu_loaded();
uint8_t get_smbios_oem_loaded();
uint8_t get_smbios_memory_array_loaded();

//BIOS: type 0
char *   get_smbios_bios_vendor();
char *   get_smbios_bios_version();
uint16_t get_smbios_bios_start_address_segment();
char *   get_smbios_bios_release_date();
uint8_t  get_smbios_bios_rom_size();
uint8_t  get_smbios_bios_supports_characteristics();
uint8_t  get_smbios_bios_supports_isa();
uint8_t  get_smbios_bios_supports_mca();
uint8_t  get_smbios_bios_supports_eisa();
uint8_t  get_smbios_bios_supports_pci();
uint8_t  get_smbios_bios_supports_pcmcia();
uint8_t  get_smbios_bios_supports_pnp();
uint8_t  get_smbios_bios_supports_apm();
uint8_t  get_smbios_bios_supports_flash_upgrade();
uint8_t  get_smbios_bios_supports_bios_shadowing();
uint8_t  get_smbios_bios_supports_vl_vesa();
uint8_t  get_smbios_bios_supports_escd();
uint8_t  get_smbios_bios_supports_boot_from_cd();
uint8_t  get_smbios_bios_supports_selectable_boot();
uint8_t  get_smbios_bios_supports_bios_rom_socketed();
uint8_t  get_smbios_bios_supports_boot_from_pc_card();
uint8_t  get_smbios_bios_supports_edd();
uint8_t  get_smbios_bios_supports_int_13h_japanese_floppy_nec9800();
uint8_t  get_smbios_bios_supports_int_13h_japanese_floppy_toshiba();
uint8_t  get_smbios_bios_supports_int_13h_525_360_kb_floppy();
uint8_t  get_smbios_bios_supports_int_13h_525_1200_kb_floppy();
uint8_t  get_smbios_bios_supports_int_13h_35_720_kb_floppy();
uint8_t  get_smbios_bios_supports_int_13h_35_2880_kb_floppy();
uint8_t  get_smbios_bios_supports_int_5h_print_screen();
uint8_t  get_smbios_bios_supports_int_9h_842_keyboad();
uint8_t  get_smbios_bios_supports_int_14h_serial_services();
uint8_t  get_smbios_bios_supports_int_17h_printer_services();
uint8_t  get_smbios_bios_supports_int_10h_cga_mono_video();
uint8_t  get_smbios_bios_supports_nec_pc_98();
uint16_t get_smbios_bios_reserved_bios_vendor();
uint16_t get_smbios_bios_reserved_system_vendor();
uint8_t  get_smbios_bios_supports_acpi();
uint8_t  get_smbios_bios_supports_usb_legacy();
uint8_t  get_smbios_bios_supports_agp();
uint8_t  get_smbios_bios_supports_i2o_boot();
uint8_t  get_smbios_bios_supports_ls_120_boot();
uint8_t  get_smbios_bios_supports_atapi_zip_drive_boot();
uint8_t  get_smbios_bios_supports_1394_boot();
uint8_t  get_smbios_bios_supports_smart_battery();
uint8_t  get_smbios_bios_supports_bios_boot();
uint8_t  get_smbios_bios_fnkey_initiated_netboot();
uint8_t  get_smbios_bios_supports_targeted_content_distribution();
uint8_t  get_smbios_bios_supports_uefi();
uint8_t  get_smbios_bios_is_virtual_machine();
uint8_t  get_smbios_bios_manufacturing_mode_enabled();
uint8_t  get_smbios_bios_system_bios_major_release();
uint8_t  get_smbios_bios_system_bios_minor_release();
uint8_t  get_smbios_bios_embedded_controller_major_release();
uint8_t  get_smbios_bios_embedded_controller_minor_release();
uint16_t get_smbios_bios_extended_bios_rom_size();

//System: type 1
char *    get_smbios_system_manufacturer();
char *    get_smbios_system_product_name();
char *    get_smbios_system_version();
char *    get_smbios_system_serial_number();
uint8_t * get_smbios_system_uuid(); //16 bytes
uint8_t   get_smbios_system_wake_up_type();
char *    get_smbios_system_sku_number();
char *    get_smbios_system_family();

//Baseboard: type 2
uint8_t get_smbios_baseboard_deviceno();
char * get_smbios_baseboard_manufacturer();
char * get_smbios_baseboard_product();
char * get_smbios_baseboard_version();
char * get_smbios_baseboard_serial_number();
char * get_smbios_baseboard_asset_tag();
uint8_t get_smbios_baseboard_is_hosting_board();
uint8_t get_smbios_baseboard_requires_daughter_board();
uint8_t get_smbios_baseboard_is_removable();
uint8_t get_smbios_baseboard_replaceable();
uint8_t get_smbios_baseboard_is_hot_swappable();
char * get_smbios_baseboard_location();


//CPU: type 4
char * get_smbios_cpu_socket_designation();
uint8_t get_smbios_cpu_processor_type();
uint8_t get_smbios_cpu_processor_family();
char * get_smbios_cpu_processor_manufacturer();
uint64_t get_smbios_cpu_processor_id();
char * get_smbios_cpu_processor_version();

uint8_t get_smbios_cpu_voltage_legacy();
uint8_t get_smbios_cpu_voltage_5V();
uint8_t get_smbios_cpu_voltage_3_3V();
uint8_t get_smbios_cpu_voltage_2_9V();

uint16_t get_smbios_cpu_external_clock();
uint16_t get_smbios_cpu_max_speed();
uint16_t get_smbios_cpu_current_speed();

uint8_t get_smbios_cpu_socket_populated();
uint8_t get_smbios_cpu_status();

uint16_t get_smbios_cpu_l1_cache_handle();
uint16_t get_smbios_cpu_l2_cache_handle();
uint16_t get_smbios_cpu_l3_cache_handle();

uint8_t get_smbios_cpu_processor_upgrade();
char * get_smbios_cpu_serial_number();
char * get_smbios_cpu_asset_tag();
char * get_smbios_cpu_part_number();
uint8_t get_smbios_cpu_core_count();
uint8_t get_smbios_cpu_core_enabled();
uint8_t get_smbios_cpu_thread_count();

uint8_t get_smbios_cpu_supports_64_bits();
uint8_t get_smbios_cpu_supports_multicore();
uint8_t get_smbios_cpu_supports_hardware_thread();
uint8_t get_smbios_cpu_supports_execute_protection();
uint8_t get_smbios_cpu_supports_enhanced_virtualization();
uint8_t get_smbios_cpu_supports_power_management();
uint8_t get_smbios_cpu_supports_128_bits();
uint8_t get_smbios_cpu_is_arm_soc();

uint16_t get_smbios_cpu_processor_family2(); 
uint16_t get_smbios_cpu_core_count2();
uint16_t get_smbios_cpu_core_enabled2();
uint16_t get_smbios_cpu_thread_count2();

uint16_t get_smbios_cpu_thread_enabled();

//Slots: Type 9

uint8_t get_smbios_slot_deviceno();
const char * get_smbios_slot_type_name(uint8_t slot_type);
char * get_smbios_slot_designation(uint8_t slot);
uint8_t get_smbios_slot_type(uint8_t slot);
uint8_t get_smbios_slot_data_bus_width(uint8_t slot);
uint8_t get_smbios_slot_current_usage(uint8_t slot);
uint8_t get_smbios_slot_length(uint8_t slot);
uint16_t get_smbios_slot_id(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_unknown(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_provides_5V(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_provides_3_3V(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_opening_is_shared(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_supports_card_16(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_supports_cardbus(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_supports_zv(uint8_t slot);
uint8_t get_smbios_slot_characteristics1_supports_modem_ring_resume(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_pme_signal(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_hot_plug_devices(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_smbus_signal(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_pcie_bifurcation(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_surprise_removal(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_flexbus_cxl_1_0(uint8_t slot);
uint8_t get_smbios_slot_characteristics2_supports_flexbus_cxl_2_0(uint8_t slot);
uint8_t get_smbios_slot_segment_group_number(uint8_t slot);
uint8_t get_smbios_slot_bus_number(uint8_t slot);
uint8_t get_smbios_slot_function_number(uint8_t slot);
uint8_t get_smbios_slot_device_number(uint8_t slot);
uint8_t get_smbios_slot_data_bus_width(uint8_t slot);
uint8_t get_smbios_slot_peer_grouping_n(uint8_t slot);
uint8_t get_smbios_slot_peer_group(uint8_t slot, uint8_t peer_group);
uint8_t get_smbios_slot_information(uint8_t slot);
uint8_t get_smbios_slot_physical_width(uint8_t slot);
uint8_t get_smbios_slot_pitch(uint8_t slot);
uint8_t get_smbios_slot_height(uint8_t slot);

//OEM: type 11

uint8_t * get_smbios_oem_count();
char * get_smbios_oem_strings();

//Memory array: type 16

uint8_t get_smbios_memory_array_deviceno();
uint8_t get_smbios_memory_array_location(uint8_t device);
uint8_t get_smbios_memory_array_use(uint8_t device);
uint8_t get_smbios_memory_array_error_correction(uint8_t device);
uint64_t get_smbios_memory_array_maximum_capacity(uint8_t device);
uint16_t get_smbios_memory_array_error_information_handle(uint8_t device);
uint8_t get_smbios_memory_array_number_of_memory_devices(uint8_t device);
uint64_t get_smbios_memory_array_extended_maximum_capacity(uint8_t device);

//Memory device: type 17

uint8_t get_smbios_memory_device_deviceno();
uint16_t get_smbios_memory_device_physical_memory_array_handle(uint8_t device);
uint16_t get_smbios_memory_device_memory_error_information_handle(uint8_t device);
uint16_t get_smbios_memory_device_total_width(uint8_t device);
uint16_t get_smbios_memory_device_data_width(uint8_t device);
uint16_t get_smbios_memory_device_size(uint8_t device);
uint8_t get_smbios_memory_device_form_factor(uint8_t device);
uint8_t get_smbios_memory_device_device_set(uint8_t device);
char * get_smbios_memory_device_device_locator(uint8_t device);
char * get_smbios_memory_device_bank_locator(uint8_t device);
uint8_t get_smbios_memory_device_memory_type(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_other(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_unknown(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_fast_paged(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_static_column(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_pseudo_static(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_ram_bus(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_synchronous(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_cmos(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_edo(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_window_dram(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_cache_dram(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_non_volatile(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_registered_buffered(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_unbuffered(uint8_t device);
uint8_t get_smbios_memory_device_type_detail_lr_dimm(uint8_t device);
uint16_t get_smbios_memory_device_speed(uint8_t device);
char * get_smbios_memory_device_manufacturer(uint8_t device);
char * get_smbios_memory_device_serial_number(uint8_t device);
char * get_smbios_memory_device_asset_tag(uint8_t device);
char * get_smbios_memory_device_part_number(uint8_t device);
uint8_t get_smbios_memory_device_attributes(uint8_t device);
uint64_t get_smbios_memory_device_extended_size(uint8_t device);
uint16_t get_smbios_memory_device_configured_memory_clock_speed(uint8_t device);
uint16_t get_smbios_memory_device_minimum_voltage(uint8_t device);
uint16_t get_smbios_memory_device_maximum_voltage(uint8_t device);
uint16_t get_smbios_memory_device_configured_voltage(uint8_t device);
uint8_t get_smbios_memory_device_memory_technology(uint8_t device);
uint8_t get_smbios_memory_device_memory_operating_mode_capability_other(uint8_t device);
uint8_t get_smbios_memory_device_memory_operating_mode_capability_unknown(uint8_t device);
uint8_t get_smbios_memory_device_memory_operating_mode_capability_volatile(uint8_t device);
uint8_t get_smbios_memory_device_memory_operating_mode_capability_byte_access(uint8_t device);
uint8_t get_smbios_memory_device_memory_operating_mode_capability_block_access(uint8_t device);
char * get_smbios_memory_device_firmware_version(uint8_t device);
uint16_t get_smbios_memory_device_module_manufacturer_id(uint8_t device);
uint16_t get_smbios_memory_device_module_product_id(uint8_t device);
uint16_t get_smbios_memory_device_memory_subsystem_controller_manufacturer_id(uint8_t device);
uint16_t get_smbios_memory_device_memory_subsystem_controller_product_id(uint8_t device);
uint64_t get_smbios_memory_device_non_volatile_size(uint8_t device);
uint64_t get_smbios_memory_device_volatile_size(uint8_t device);
uint64_t get_smbios_memory_device_cache_size(uint8_t device);
uint64_t get_smbios_memory_device_logical_size(uint8_t device);

#endif