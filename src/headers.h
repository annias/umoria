// src/standard_library.h: standard library imports - conditional compilation macros.
//
// Copyright (c) 1989-94 James E. Wilson, Robert A. Koeneke
//
// Umoria is free software released under a GPL v2 license and comes with
// ABSOLUTELY NO WARRANTY. See https://www.gnu.org/licenses/gpl-2.0.html
// for further details.

#pragma once

#ifdef _WIN32
    #define _CRT_SECURE_NO_WARNINGS
    #define _CRT_NONSTDC_NO_DEPRECATE
    #define WIN32_LEAN_AND_MEAN

    #include <windows.h>

    #include <io.h> // <unistd.h>
    #include <sys/types.h>

#elif __APPLE__

    #include <pwd.h>
    #include <unistd.h>
    #include <sys/param.h>

#elif __linux__

    #include <pwd.h>
    #include <unistd.h>    // getuid() and others
    #include <sys/param.h> // Defines the timeval structure / fd_set

#else
#   error "Unknown compiler"
#endif


// Includes we can use on all supported systems!

#include <ctype.h>     // islower(), isupper(), isalpha(), etc.
#include <errno.h>
#include <fcntl.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/stat.h>  // Used only for chmod()
