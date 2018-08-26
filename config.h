#pragma once

/// Uses more memory to align all commands to the command packet such that allocations are faster.
#define CB_COMMAND_PACKET_ALIGNED 1

/// Tag commands with message for debugging purposes.
#define CB_DEBUG_TAG_COMMANDS 0
/// Print the commands in the standard output(or custom logger).
#if CB_DEBUG_TAG_COMMANDS
#define CB_DEBUG_COMMANDS_PRINT 0
#endif

#ifdef _MSC_VER
#define CB_FORCE_INLINE inline __forceinline
#elif defined(__GNUC__)
#define CB_FORCE_INLINE inline __attribute__((always_inline))
#else
#define CB_FORCE_INLINE inline
#endif
