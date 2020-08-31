#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uarch.h"

/*
 * - cpuid codes are based on Todd Allen's cpuid program
 *   http://www.etallen.com/cpuid.html
 * - This should be updated from time to time, to support newer CPUs. A good reference to look at:
 *   https://en.wikichip.org/
 */

// From Todd Allen:
//
// MSR_CPUID_table* is a table that appears in Intel document 325462, "Intel 64
// and IA-32 Architectures Software Developer's Manual Combined Volumes: 1, 2A,
// 2B, 2C, 2D, 3A, 3B, 3C, 3D, and 4" (the name changes from version to version
// as more volumes are added).  The table moves around from version to version,
// but in version 071US, was in "Volume 4: Model-Specific Registers", Table 2-1:
// "CPUID Signature Values of DisplayFamily_DisplayModel".

// MRG* is a table that forms the bulk of Intel Microcode Revision Guidance (or
// Microcode Update Guidance).  Its purpose is not to list CPUID values, but
// it does so, and sometimes lists values that appear nowhere else.

// LX* indicates features that I have seen no documentation for, but which are
// used by the Linux kernel (which is good evidence that they're correct).
// The "hook" to find these generally is an X86_FEATURE_* flag in:
//    arch/x86/include/asm/cpufeatures.h
// For (synth) and (uarch synth) decoding, it often indicates
// family/model/stepping value which are documented nowhere else.  These usually
// can be found in:
//    arch/x86/include/asm/intel-family.h

typedef uint32_t MICROARCH;

// Data not available
#define NA                   -1

// Unknown manufacturing process
#define UNK                  -1

#define UARCH_UNKNOWN         0x000
#define UARCH_P5              0x001
#define UARCH_P6              0x002
#define UARCH_DOTHAN          0x003
#define UARCH_YONAH           0x004
#define UARCH_MEROM           0x005
#define UARCH_PENYR           0x006
#define UARCH_NEHALEM         0x007
#define UARCH_WESTMERE        0x008
#define UARCH_BONNELL         0x009
#define UARCH_SALTWELL        0x010
#define UARCH_SANDY_BRIDGE    0x011
#define UARCH_SILVERMONT      0x012
#define UARCH_IVY_BRIDGE      0x013
#define UARCH_HASWELL         0x014
#define UARCH_BROADWELL       0x015
#define UARCH_AIRMONT         0x016
#define UARCH_KABY_LAKE       0x017
#define UARCH_SKYLAKE         0x018
#define UARCH_CASCADE_LAKE    0x019
#define UARCH_COOPER_LAKE     0x020
#define UARCH_KNIGHTS_LANDING 0x021
#define UARCH_KNIGHTS_MILL    0x022
#define UARCH_GOLDMONT        0x023
#define UARCH_PALM_COVE       0x024
#define UARCH_SUNNY_COVE      0x025
#define UARCH_GOLDMONT_PLUS   0x026
#define UARCH_TREMONT         0x027
#define UARCH_WILLOW_COVE     0x028
#define UARCH_COFFE_LAKE      0x029
#define UARCH_ITANIUM         0x030
#define UARCH_KNIGHTS_FERRY   0x031
#define UARCH_KNIGHTS_CORNER  0x032
#define UARCH_WILLAMETTE      0x033
#define UARCH_NORTHWOOD       0x034
#define UARCH_PRESCOTT        0x035
#define UARCH_CEDAR_MILL      0x036
#define UARCH_ITANIUM2        0x037
#define UARCH_ICE_LAKE        0x038

#define UARCH_AM486           0x038
#define UARCH_AM5X86          0x039
#define UARCH_K6              0x040
#define UARCH_K7              0x041
#define UARCH_K8              0x042
#define UARCH_K10             0x043
#define UARCH_PUMA_2008       0x044
#define UARCH_BOBCAT          0x045
#define UARCH_BULLDOZER       0x046
#define UARCH_PILEDRIVER      0x047
#define UARCH_STEAMROLLER     0x048
#define UARCH_EXCAVATOR       0x049
#define UARCH_JAGUAR          0x050
#define UARCH_PUMA_2014       0x051
#define UARCH_ZEN             0x052
#define UARCH_ZEN_PLUS        0x053
#define UARCH_ZEN2            0x054
#define UARCH_ZEN3            0x055

struct uarch {
  MICROARCH uarch;
  char* uarch_str;
  int32_t process; // measured in nanometers
};

#define UARCH_START if (false) {}
#define CHECK_UARCH(arch, ef_, f_, em_, m_, s_, str, uarch, process) \
   else if (ef_ == ef && f_ == f && (em_ == NA || em_ == em) && (m_ == NA || m_ == m) && (s_ == NA || s_ == s)) fill_uarch(arch, str, uarch, process);
#define UARCH_END else { arch->uarch = UARCH_UNKNOWN; }

void fill_uarch(struct uarch* arch, char* str, MICROARCH u, uint32_t process) {
  arch->uarch_str = malloc(sizeof(char) * strlen(str));
  strcpy(arch->uarch_str, str);
  arch->uarch = u;
  arch->process= process;
}

// iNApired in Todd Allen's decode_uarch_intel
struct uarch* get_uarch_from_cpuid_intel(uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s) {
  struct uarch* arch = malloc(sizeof(struct uarch));
  
  // EF: Extended Family                                                           //
  // F:  Family                                                                    //
  // EM: Extended Model                                                            //
  // M: Model                                                                      //
  // S: Stepping                                                                   //
  // ----------------------------------------------------------------------------- //
  //                EF  F  EM   M   S                                              //
  UARCH_START  
  CHECK_UARCH(arch, 0,  5,  0,  0, NA, "P5",              UARCH_P5,              800)
  CHECK_UARCH(arch, 0,  5,  0,  1, NA, "P5",              UARCH_P5,              800)
  CHECK_UARCH(arch, 0,  5,  0,  2, NA, "P5",              UARCH_P5,              UNK)
  CHECK_UARCH(arch, 0,  5,  0,  3, NA, "P5",              UARCH_P5,              600)
  CHECK_UARCH(arch, 0,  5,  0,  4, NA, "P5 MMX",          UARCH_P5,              UNK)
  CHECK_UARCH(arch, 0,  5,  0,  7, NA, "P5 MMX",          UARCH_P5,              UNK)
  CHECK_UARCH(arch, 0,  5,  0,  8, NA, "P5 MMX",          UARCH_P5,              250)
  CHECK_UARCH(arch, 0,  5,  0,  9, NA, "P5 MMX",          UARCH_P5,              UNK)
  CHECK_UARCH(arch, 0,  6,  0,  0, NA, "P6 Pentium II",   UARCH_P6,              UNK)
  CHECK_UARCH(arch, 0,  6,  0,  1, NA, "P6 Pentium II",   UARCH_P6,              UNK) // process depends on core
  CHECK_UARCH(arch, 0,  6,  0,  2, NA, "P6 Pentium II",   UARCH_P6,              UNK)
  CHECK_UARCH(arch, 0,  6,  0,  3, NA, "P6 Pentium II",   UARCH_P6,              350)
  CHECK_UARCH(arch, 0,  6,  0,  4, NA, "P6 Pentium II",   UARCH_P6,              UNK)
  CHECK_UARCH(arch, 0,  6,  0,  5, NA, "P6 Pentium II",   UARCH_P6,              250)
  CHECK_UARCH(arch, 0,  6,  0,  6, NA, "P6 Pentium II",   UARCH_P6,              UNK)
  CHECK_UARCH(arch, 0,  6,  0,  7, NA, "P6 Pentium III",  UARCH_P6,              250)
  CHECK_UARCH(arch, 0,  6,  0,  8, NA, "P6 Pentium III",  UARCH_P6,              180)
  CHECK_UARCH(arch, 0,  6,  0,  9, NA, "P6 Pentium M",    UARCH_P6,              130)
  CHECK_UARCH(arch, 0,  6,  0, 10, NA, "P6 Pentium III",  UARCH_P6,              180)
  CHECK_UARCH(arch, 0,  6,  0, 11, NA, "P6 Pentium III",  UARCH_P6,              130)
  CHECK_UARCH(arch, 0,  6,  0, 13, NA, "Dothan",          UARCH_DOTHAN,          UNK)  // process depends on core
  CHECK_UARCH(arch, 0,  6,  0, 14, NA, "Yonah",           UARCH_YONAH,            65)
  CHECK_UARCH(arch, 0,  6,  0, 15, NA, "Merom",           UARCH_MEROM,            65)
  CHECK_UARCH(arch, 0,  6,  1,  5, NA, "Dothan",          UARCH_DOTHAN,           90)
  CHECK_UARCH(arch, 0,  6,  1,  6, NA, "Merom",           UARCH_MEROM,            65)
  CHECK_UARCH(arch, 0,  6,  1,  7, NA, "Penryn",          UARCH_PENYR,            45)
  CHECK_UARCH(arch, 0,  6,  1, 10, NA, "Nehalem",         UARCH_NEHALEM,          45)
  CHECK_UARCH(arch, 0,  6,  1, 12, NA, "Bonnell",         UARCH_BONNELL,          45)
  CHECK_UARCH(arch, 0,  6,  1, 13, NA, "Penryn",          UARCH_PENYR,            45)
  CHECK_UARCH(arch, 0,  6,  1, 14, NA, "Nehalem",         UARCH_NEHALEM,          45)
  CHECK_UARCH(arch, 0,  6,  1, 15, NA, "Nehalem",         UARCH_NEHALEM,          45)
  CHECK_UARCH(arch, 0,  6,  2,  5, NA, "Westmere",        UARCH_WESTMERE,         32)
  CHECK_UARCH(arch, 0,  6,  2 , 6, NA, "Bonnell",         UARCH_BONNELL,          45)
  CHECK_UARCH(arch, 0,  6,  2,  7, NA, "Saltwell",        UARCH_SALTWELL,         32)
  CHECK_UARCH(arch, 0,  6,  2, 10, NA, "Sandy Bridge",    UARCH_SANDY_BRIDGE,     32)
  CHECK_UARCH(arch, 0,  6,  2, 12, NA, "Westmere",        UARCH_WESTMERE,         32)
  CHECK_UARCH(arch, 0,  6,  2, 13, NA, "Sandy Bridge",    UARCH_SANDY_BRIDGE,     32)
  CHECK_UARCH(arch, 0,  6,  2, 14, NA, "Nehalem",         UARCH_NEHALEM,          45)
  CHECK_UARCH(arch, 0,  6,  2, 15, NA, "Westmere",        UARCH_WESTMERE,         32)
  CHECK_UARCH(arch, 0,  6,  3,  5, NA, "Saltwell",        UARCH_SALTWELL,         14)
  CHECK_UARCH(arch, 0,  6,  3,  6, NA, "Saltwell",        UARCH_SALTWELL,         32)
  CHECK_UARCH(arch, 0,  6,  3,  7, NA, "Silvermont",      UARCH_SILVERMONT,       22)
  CHECK_UARCH(arch, 0,  6,  3, 10, NA, "Ivy Bridge",      UARCH_IVY_BRIDGE,       22)
  CHECK_UARCH(arch, 0,  6,  3, 12, NA, "Haswell",         UARCH_HASWELL,          22)
  CHECK_UARCH(arch, 0,  6,  3, 13, NA, "Broadwell",       UARCH_BROADWELL,        14)
  CHECK_UARCH(arch, 0,  6,  3, 14, NA, "Ivy Bridge",      UARCH_IVY_BRIDGE,       22)
  CHECK_UARCH(arch, 0,  6,  3, 15, NA, "Haswell",         UARCH_HASWELL,          22)
  CHECK_UARCH(arch, 0,  6,  4,  5, NA, "Haswell",         UARCH_HASWELL,          22)
  CHECK_UARCH(arch, 0,  6,  4,  6, NA, "Haswell",         UARCH_HASWELL,          22)
  CHECK_UARCH(arch, 0,  6,  4,  7, NA, "Broadwell",       UARCH_BROADWELL,        14)
  CHECK_UARCH(arch, 0,  6,  4, 10, NA, "Silvermont",      UARCH_SILVERMONT,       22) // no docs, but /proc/cpuinfo seen in wild
  CHECK_UARCH(arch, 0,  6,  4, 12, NA, "Airmont",         UARCH_AIRMONT,          14)
  CHECK_UARCH(arch, 0,  6,  4, 13, NA, "Silvermont",      UARCH_SILVERMONT,       22)
  CHECK_UARCH(arch, 0,  6,  4, 14,  8, "Kaby Lake",       UARCH_KABY_LAKE,        14)
  CHECK_UARCH(arch, 0,  6,  4, 14, NA, "Skylake",         UARCH_SKYLAKE,          14)
  CHECK_UARCH(arch, 0,  6,  4, 15, NA, "Broadwell",       UARCH_BROADWELL,        14)
  CHECK_UARCH(arch, 0,  6,  5,  5,  6, "Cascade Lake",    UARCH_CASCADE_LAKE,     14) // no docs, but example from Greg Stewart
  CHECK_UARCH(arch, 0,  6,  5,  5,  7, "Cascade Lake",    UARCH_CASCADE_LAKE,     14)
  CHECK_UARCH(arch, 0,  6,  5,  5, 10, "Cooper Lake",     UARCH_COOPER_LAKE,      14)
  CHECK_UARCH(arch, 0,  6,  5,  5, NA, "Skylake",         UARCH_SKYLAKE,          14)
  CHECK_UARCH(arch, 0,  6,  5,  6, NA, "Broadwell",       UARCH_BROADWELL,        14)
  CHECK_UARCH(arch, 0,  6,  5,  7, NA, "Knights Landing", UARCH_KNIGHTS_LANDING,  14)
  CHECK_UARCH(arch, 0,  6,  5, 10, NA, "Silvermont",      UARCH_SILVERMONT,       22) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  5, 12, NA, "Goldmont",        UARCH_GOLDMONT,         14)
  CHECK_UARCH(arch, 0,  6,  5, 13, NA, "Silvermont",      UARCH_SILVERMONT,       22) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  5, 14,  8, "Kaby Lake",       UARCH_KABY_LAKE,        14)
  CHECK_UARCH(arch, 0,  6,  5, 14, NA, "Skylake",         UARCH_SKYLAKE,          14)
  CHECK_UARCH(arch, 0,  6,  5, 15, NA, "Goldmont",        UARCH_GOLDMONT,         14)
  CHECK_UARCH(arch, 0,  6,  6,  6, NA, "Palm Cove",       UARCH_PALM_COVE,        10) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  6, 10, NA, "Sunny Cove",      UARCH_SUNNY_COVE,       10) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  6, 12, NA, "Sunny Cove",      UARCH_SUNNY_COVE,       10) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  7,  5, NA, "Airmont",         UARCH_AIRMONT,          14) // no spec update; whispers & rumors
  CHECK_UARCH(arch, 0,  6,  7, 10, NA, "Goldmont Plus",   UARCH_GOLDMONT_PLUS,    14)
  CHECK_UARCH(arch, 0,  6,  7, 13, NA, "Sunny Cove",      UARCH_SUNNY_COVE,       10) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  7, 14, NA, "Ice Lake",        UARCH_ICE_LAKE,         10)
  CHECK_UARCH(arch, 0,  6,  8,  5, NA, "Knights Mill",    UARCH_KNIGHTS_MILL,     14) // no spec update; only MSR_CPUID_table* so far
  CHECK_UARCH(arch, 0,  6,  8,  6, NA, "Tremont",         UARCH_TREMONT,          10) // LX*
  CHECK_UARCH(arch, 0,  6,  8, 10, NA, "Tremont",         UARCH_TREMONT,          10) // no spec update; only geekbench.com example
  CHECK_UARCH(arch, 0,  6,  8, 12, NA, "Willow Cove",     UARCH_WILLOW_COVE,      10) // found only on en.wikichip.org
  CHECK_UARCH(arch, 0,  6,  8, 13, NA, "Willow Cove",     UARCH_WILLOW_COVE,      10) // LX*
  CHECK_UARCH(arch, 0,  6,  8, 14, NA, "Kaby Lake",       UARCH_KABY_LAKE,        14)
  CHECK_UARCH(arch, 0,  6,  9,  6, NA, "Tremont",         UARCH_TREMONT,          10) // LX*
  CHECK_UARCH(arch, 0,  6,  9, 12, NA, "Tremont",         UARCH_TREMONT,          10) // LX*
  CHECK_UARCH(arch, 0,  6,  9, 13, NA, "Sunny Cove",      UARCH_SUNNY_COVE,       10) // LX*
  CHECK_UARCH(arch, 0,  6,  9, 14,  9, "Kaby Lake",       UARCH_KABY_LAKE,        14)
  CHECK_UARCH(arch, 0,  6,  9, 14, 10, "Coffee Lake",     UARCH_COFFE_LAKE,       14)
  CHECK_UARCH(arch, 0,  6,  9, 14, 11, "Coffee Lake",     UARCH_COFFE_LAKE,       14)
  CHECK_UARCH(arch, 0,  6,  9, 14, 12, "Coffee Lake",     UARCH_COFFE_LAKE,       14)
  CHECK_UARCH(arch, 0,  6,  9, 14, 13, "Coffee Lake",     UARCH_COFFE_LAKE,       14)
  CHECK_UARCH(arch, 0,  6, 10,  5, NA, "Kaby Lake",       UARCH_KABY_LAKE,        14) // LX*
  CHECK_UARCH(arch, 0,  6, 10,  6, NA, "Kaby Lake",       UARCH_KABY_LAKE,        14) // no spec update; only iNAtlatx64 example
  CHECK_UARCH(arch, 0, 11,  0,  0, NA, "Knights Ferry",   UARCH_KNIGHTS_FERRY,    45) // found only on en.wikichip.org
  CHECK_UARCH(arch, 0, 11,  0,  1, NA, "Knights Corner",  UARCH_KNIGHTS_CORNER,   22)
  CHECK_UARCH(arch, 0, 15,  0,  0, NA, "Willamette",      UARCH_WILLAMETTE,      180)
  CHECK_UARCH(arch, 0, 15,  0,  1, NA, "Willamette",      UARCH_WILLAMETTE,      180)
  CHECK_UARCH(arch, 0, 15,  0,  2, NA, "Northwood",       UARCH_NORTHWOOD,       130)
  CHECK_UARCH(arch, 0, 15,  0,  3, NA, "Prescott",        UARCH_PRESCOTT,         90)
  CHECK_UARCH(arch, 0, 15,  0,  4, NA, "Prescott",        UARCH_PRESCOTT,         90)
  CHECK_UARCH(arch, 0, 15,  0,  6, NA, "Cedar Mill",      UARCH_CEDAR_MILL,       65)
  CHECK_UARCH(arch, 1, 15,  0,  0, NA, "Itanium2",        UARCH_ITANIUM2,        180)
  CHECK_UARCH(arch, 1, 15,  0,  1, NA, "Itanium2",        UARCH_ITANIUM2,        130)
  CHECK_UARCH(arch, 1, 15,  0,  2, NA, "Itanium2",        UARCH_ITANIUM2,        130)
  UARCH_END
    
  return arch;
}

// iNApired in Todd Allen's decode_uarch_amd
struct uarch* get_uarch_from_cpuid_amd(uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s) {
  struct uarch* arch = malloc(sizeof(struct uarch));
  
  // EF: Extended Family                                                           //
  // F:  Family                                                                    //
  // EM: Extended Model                                                            //
  // M: Model                                                                      //
  // S: Stepping                                                                   //
  // ----------------------------------------------------------------------------- //
  //                 EF  F  EM   M   S                                             //
  UARCH_START  
  CHECK_UARCH(arch,  0,  4,  0,  3, NA, "Am486",       UARCH_AM486,      UNK)
  CHECK_UARCH(arch,  0,  4,  0,  7, NA, "Am486",       UARCH_AM486,      UNK)
  CHECK_UARCH(arch,  0,  4,  0,  8, NA, "Am486",       UARCH_AM486,      UNK)
  CHECK_UARCH(arch,  0,  4,  0,  9, NA, "Am486",       UARCH_AM486,      UNK)
  CHECK_UARCH(arch,  0,  4, NA, NA, NA, "Am5x86",      UARCH_AM5X86,     UNK)
  CHECK_UARCH(arch,  0,  5,  0,  6, NA, "K6",          UARCH_K6,         300)
  CHECK_UARCH(arch,  0,  5,  0,  7, NA, "K6",          UARCH_K6,         250) // *p from sandpile.org
  CHECK_UARCH(arch,  0,  5,  0, 13, NA, "K6",          UARCH_K6,         80)  // *p from sandpile.org
  CHECK_UARCH(arch,  0,  5, NA, NA, NA, "K6",          UARCH_K6,         UNK)
  CHECK_UARCH(arch,  0,  6,  0,  1, NA, "K7",          UARCH_K7,         250)
  CHECK_UARCH(arch,  0,  6,  0,  2, NA, "K7",          UARCH_K7,         180)
  CHECK_UARCH(arch,  0,  6, NA, NA, NA, "K7",          UARCH_K7,         UNK)
  CHECK_UARCH(arch,  0, 15,  0,  4,  8, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0,  4, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0,  5, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0,  7, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0,  8, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0, 11, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0, 12, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0, 14, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  0, 15, NA, "K8",          UARCH_K8,         130)
  CHECK_UARCH(arch,  0, 15,  1,  4, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  1,  5, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  1,  7, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  1,  8, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  1, 11, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  1, 12, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  1, 15, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2,  1, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2,  3, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2,  4, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2,  5, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2,  7, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2, 11, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2, 12, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  2, 15, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  4,  1, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  4,  3, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  4,  8, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  4, 11, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  4, 12, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  4, 15, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  5, 13, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  5, 15, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  0, 15,  6,  8, NA, "K8",          UARCH_K8,          65)
  CHECK_UARCH(arch,  0, 15,  6, 11, NA, "K8",          UARCH_K8,          65)
  CHECK_UARCH(arch,  0, 15,  6, 12, NA, "K8",          UARCH_K8,          65)
  CHECK_UARCH(arch,  0, 15,  6, 15, NA, "K8",          UARCH_K8,          65)
  CHECK_UARCH(arch,  0, 15,  7, 12, NA, "K8",          UARCH_K8,          65)
  CHECK_UARCH(arch,  0, 15,  7, 15, NA, "K8",          UARCH_K8,          65)
  CHECK_UARCH(arch,  0, 15, 12,  1, NA, "K8",          UARCH_K8,          90)
  CHECK_UARCH(arch,  1, 15,  0,  0, NA, "K10",         UARCH_K10,         65) // sandpile.org
  CHECK_UARCH(arch,  1, 15,  0,  2, NA, "K10",         UARCH_K10,         65)
  CHECK_UARCH(arch,  1, 15,  0,  4, NA, "K10",         UARCH_K10,         45)
  CHECK_UARCH(arch,  1, 15,  0,  5, NA, "K10",         UARCH_K10,         45)
  CHECK_UARCH(arch,  1, 15,  0,  6, NA, "K10",         UARCH_K10,         45)
  CHECK_UARCH(arch,  1, 15,  0,  8, NA, "K10",         UARCH_K10,         45)
  CHECK_UARCH(arch,  1, 15,  0,  9, NA, "K10",         UARCH_K10,         45)
  CHECK_UARCH(arch,  1, 15,  0, 10, NA, "K10",         UARCH_K10,         45)
  CHECK_UARCH(arch,  2, 15, NA, NA, NA, "Puma 2008",   UARCH_PUMA_2008,   65)
  CHECK_UARCH(arch,  3, 15, NA, NA, NA, "K10",         UARCH_K10,         32)
  CHECK_UARCH(arch,  5, 15, NA, NA, NA, "Bobcat",      UARCH_BOBCAT,      40)
  CHECK_UARCH(arch,  6, 15,  0,  0, NA, "Bulldozer",   UARCH_BULLDOZER,   32) // iNAtlatx64 engr sample
  CHECK_UARCH(arch,  6, 15,  0,  1, NA, "Bulldozer",   UARCH_BULLDOZER,   32)
  CHECK_UARCH(arch,  6, 15,  0,  2, NA, "Piledriver",  UARCH_PILEDRIVER,  32)
  CHECK_UARCH(arch,  6, 15,  1,  0, NA, "Piledriver",  UARCH_PILEDRIVER,  32)
  CHECK_UARCH(arch,  6, 15,  1,  3, NA, "Piledriver",  UARCH_PILEDRIVER,  32)
  CHECK_UARCH(arch,  6, 15,  3,  0, NA, "Steamroller", UARCH_STEAMROLLER, 28)
  CHECK_UARCH(arch,  6, 15,  3,  8, NA, "Steamroller", UARCH_STEAMROLLER, 28)
  CHECK_UARCH(arch,  6, 15,  4,  0, NA, "Steamroller", UARCH_STEAMROLLER, 28) // Software Optimization Guide (15h) says it has the same iNAt latencies as (6,15),(3,x).
  CHECK_UARCH(arch,  6, 15,  6,  0, NA, "Excavator",   UARCH_EXCAVATOR,   28) // undocumented, but iNAtlatx64 samples
  CHECK_UARCH(arch,  6, 15,  6,  5, NA, "Excavator",   UARCH_EXCAVATOR,   28) // undocumented, but sample from Alexandros Couloumbis
  CHECK_UARCH(arch,  6, 15,  7,  0, NA, "Excavator",   UARCH_EXCAVATOR,   28)
  CHECK_UARCH(arch,  7, 15,  0,  0, NA, "Jaguar",      UARCH_JAGUAR,      28)
  CHECK_UARCH(arch,  7, 15,  3,  0, NA, "Puma 2014",   UARCH_PUMA_2014,   28)
  CHECK_UARCH(arch,  8, 15,  0,  0, NA, "Zen",         UARCH_ZEN,         14) // iNAtlatx64 engr sample
  CHECK_UARCH(arch,  8, 15,  0,  1, NA, "Zen",         UARCH_ZEN,         14)
  CHECK_UARCH(arch,  8, 15,  0,  8, NA, "Zen+",        UARCH_ZEN_PLUS,    12)
  CHECK_UARCH(arch,  8, 15,  1,  1, NA, "Zen",         UARCH_ZEN,         14) // found only on en.wikichip.org & iNAtlatx64 examples
  CHECK_UARCH(arch,  8, 15,  1,  8, NA, "Zen+",        UARCH_ZEN_PLUS,    12) // found only on en.wikichip.org
  CHECK_UARCH(arch,  8, 15,  3,  1, NA, "Zen 2",       UARCH_ZEN2,         7) // found only on en.wikichip.org
  CHECK_UARCH(arch,  8, 15,  6,  0, NA, "Zen 2",       UARCH_ZEN2,         7) // undocumented, geekbench.com example
  CHECK_UARCH(arch,  8, 15,  7,  1, NA, "Zen 2",       UARCH_ZEN2,         7) // undocumented, but samples from Steven Noonan
  CHECK_UARCH(arch, 10, 15, NA, NA, NA, "Zen 3",       UARCH_ZEN3,         7) // undocumented, LX*
  UARCH_END
    
  return arch;
}

struct uarch* get_uarch_from_cpuid(struct cpuInfo* cpu, uint32_t ef, uint32_t f, uint32_t em, uint32_t m, int s) {
  if(cpu->cpu_vendor == VENDOR_INTEL)
    return get_uarch_from_cpuid_intel(ef, f, em, m, s);
  else 
    return get_uarch_from_cpuid_amd(ef, f, em, m, s);
}

int get_number_of_vpus(struct cpuInfo* cpu) {
  if(cpu->cpu_vendor == VENDOR_AMD)
    return 1;
  
  switch(cpu->arch->uarch) {
      case UARCH_HASWELL:
      case UARCH_BROADWELL:
          
      case UARCH_SKYLAKE:
      case UARCH_CASCADE_LAKE:                    
      case UARCH_KABY_LAKE:
      case UARCH_COFFE_LAKE:
      case UARCH_PALM_COVE:    
      
      case UARCH_KNIGHTS_LANDING:
      case UARCH_KNIGHTS_MILL:
          
      case UARCH_ICE_LAKE:      
        return 2;
      default:
        return 1;
  }
}