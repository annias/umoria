/* source/config.h: configuration definitions
 *
 * Copyright (C) 1989-2008 James E. Wilson, Robert A. Koeneke,
 *                         David J. Grabiner
 *
 * This file is part of Umoria.
 *
 * Umoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Umoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Umoria.  If not, see <http://www.gnu.org/licenses/>.
 */

#define CONFIG_H_INCLUDED
#ifdef CONSTANT_H_INCLUDED
Constant.h should always be included after config.h,
    because it uses some of the system defines set up here.
#endif

/* Person to bother if something goes wrong.  */
/* Recompile files.c and misc2.c if this changes.  */
#define WIZARD "David Grabiner <grabiner@alumni.princeton.edu>"
/* The wizard password and wizard uid are no longer used.  */

/* System definitions.  You must define one of these as appropriate for
   the system you are compiling moria on.  */

/* No system definition is needed for 4.3BSD, SUN OS, DG/UX.  */
/* Unless you're on a system, like an HP Apollo, that doesn't let
   more than one machine access a file at a time; then define this.  */
/* #define APOLLO */

/* If compiling on Debian (also works on other versions of Linux),
   define this. */
#define DEBIAN_LINUX

/* If you are compiling on an ultrix/4.2BSD/Dynix/etc. version of UNIX,
   define this.  It is not needed for SUNs.  */
/* #ifndef ultrix
#define ultrix
#endif */

/* If you are compiling under IBM's AIX 3.0, then you can either define
   SYS_V, or you can define nothing (thus compiling as if on a BSD system)
   but you must comment out the AIX LFLAG line in the Makefile so that
   moria will be linked with -lbsd.  */

/* If you are compiling on a SYS V version of UNIX, define this.  */
/* #define SYS_V */

/* If you are compiling on a SYS III version of UNIX, define this.
   The SYS_III support may not be complete.  I do not know if this works.  */
/* #define SYS_III */

/* If you are compiling on a Macintosh with MPW C 3.0, define this.  */
/* #define MAC */

/* If we are in Think C, then we must be on a mac.  */
#ifdef THINK_C
#define MAC
#endif

/* For Xenix systems, define SYS_V and unix.  */
#ifdef M_XENIX
#define SYS_V
#define unix
#endif

/* If on HP-UX, define the name as we use it. */
#ifdef __hpux
#define HPUX
#endif

/* Files used by moria, set these to valid pathnames for your system.  */

#ifdef MAC
/* These files are concatenated into the data fork of the app */
/* The names are retained to find the appropriate text */
#define MORIA_MOR "news"
#define MORIA_GPL "COPYING"
#define MORIA_HELP "roglcmds.hlp"
#define MORIA_ORIG_HELP "origcmds.hlp"
#define MORIA_WIZ_HELP "rwizcmds.hlp"
#define MORIA_OWIZ_HELP "owizcmds.hlp"
#define MORIA_WELCOME "welcome.hlp"
#define MORIA_VER "version.hlp"
/* Do not know what will happen with these yet */
#define MORIA_TOP "Moria High Scores"
/* File types and creators for the Mac */
#define MORIA_FCREATOR 'MRIA'
#define SAVE_FTYPE 'SAVE'
#define INFO_FTYPE 'TEXT'
#define SCORE_FTYPE 'SCOR'
#define CONFIG_FTYPE 'CNFG'
/* Options for building resources:
   THINK C doesn't have -D switch, so we need to define this stuff here.
   Uncomment RSRC when building DumpRes1 or DumpRes2; uncomment RSRC_PARTn
   as appropriate.  When building application, comment all of them.
   I don't think any of this is necessary for MPW C -- BS.  */
#ifdef THINK_C
/* #define RSRC */ /* This copy is for creating resources.  */
/* THINK C can only take 32K data, so we need to dump the resources in
   two parts.  */
/* #define RSRC_PART1 */
/* #define RSRC_PART2 */
#endif
#else // else MAC



#if 0
/* Debian standards for file location */
/* This must be unix; change file names as appropriate.  */
#define MORIA_SAV ".moria-save"
#define MORIA_HOU "/etc/moria-hours"
#define MORIA_MOR "/usr/lib/games/moria/news"
#define MORIA_GPL "/usr/lib/games/moria/COPYING"
#define MORIA_TOP "/var/games/moria/scores"
#define MORIA_HELP "/usr/lib/games/moria/roglcmds.hlp"
#define MORIA_ORIG_HELP "/usr/lib/games/moria/origcmds.hlp"
#define MORIA_WIZ_HELP "/usr/lib/games/moria/rwizcmds.hlp"
#define MORIA_OWIZ_HELP "/usr/lib/games/moria/owizcmds.hlp"
#define MORIA_WELCOME "/usr/lib/games/moria/welcome.hlp"
#define MORIA_VER "/usr/lib/games/moria/version.hlp"
#else // else DEBIAN standard.


/* Generic UNIX */
/* This must be unix; change file names as appropriate.  */
#define MORIA_SAV "moria-save"
#define MORIA_HOU "/home/michael/moria-56/files/hours"
#define MORIA_MOR "/home/michael/moria-56/files/news"
#define MORIA_GPL "/home/michael/moria-56/files/COPYING"
#define MORIA_TOP "/home/michael/moria-56/files/scores"
#define MORIA_HELP "/home/michael/moria-56/files/roglcmds.hlp"
#define MORIA_ORIG_HELP "/home/michael/moria-56/files/origcmds.hlp"
#define MORIA_WIZ_HELP "/home/michael/moria-56/files/rwizcmds.hlp"
#define MORIA_OWIZ_HELP "/home/michael/moria-56/files/owizcmds.hlp"
#define MORIA_WELCOME "/home/michael/moria-56/files/welcome.hlp"
#define MORIA_VER "/home/michael/moria-56/files/version.hlp"

#endif // end DEBIAN standard.
#endif // end MAC


/* This sets the default user interface.  */
/* To use the original key bindings (keypad for movement) set ROGUE_LIKE
   to FALSE; to use the rogue-like key bindings (vi style movement)
   set ROGUE_LIKE to TRUE.  */
/* If you change this, you only need to recompile main.c.  */
#define ROGUE_LIKE FALSE

/* For the ANDREW distributed file system, define this to ensure that
   the program is secure with respect to the setuid code, this prohibits
   inferior shells.  It also does not relinquish setuid privileges at the
   start, but instead calls the ANDREW library routines bePlayer(), beGames(),
   and Authenticate().  */
/* #define SECURE */

/* System dependent defines follow.  You should not need to change anything
   below.  */

#if defined(__linux__) /* Linux supports System V */
#define SYS_V
#endif

/* Substitute strchr for index on USG versions of UNIX.  */
#if defined(SYS_V) || defined(MAC)
#define index strchr
#endif

#ifdef SYS_III
    char *
    index();
#endif

/* Define USG for many systems, this is basically to select SYS V style
   system calls (as opposed to BSD style).  */
#if defined(SYS_III) || defined(SYS_V) || defined(MAC)
#ifndef USG
#define USG
#endif
#endif

/* Pyramid runs 4.2BSD-like UNIX version */
#if defined(Pyramid)
#define ultrix
#endif

#if defined(_MSC_VER) && (_MSC_VER < 600)
#define register /* MSC 4.0 still has a problem with register bugs ... */
#endif

#ifdef MAC
#ifdef RSRC
#define MACRSRC /* Defined if we are building the resources.  */
#else
#define MACGAME /* Defined if we are building the game.  */
#endif
#endif

#ifdef MAC
/* Screen dimensions */
#define SCRN_ROWS 24
#define SCRN_COLS 80
#endif

#if defined(SYS_V) && defined(lint)
/* Define this to prevent <string.h> from including <NLchar.h> on a PC/RT
   running AIX.  This prevents a bunch of lint errors.  */
#define RTPC_NO_NLS
#endif

#ifdef SECURE
extern int PlayerUID;
#define getuid() PlayerUID
#define geteuid() PlayerUID
#endif

#ifdef THINK_C
/* Apparently, THINK C is only happy if this is defined.  This can not
 * be defined in general, because some systems have include files which
 * merely test whether STDC is defined, they do not test the value.
 */
/* Check how standard we are: Some code tests value of __STDC__.  */
#ifndef __STDC__
#define __STDC__ 0
#endif
#endif
