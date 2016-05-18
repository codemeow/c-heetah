/* «C-heetah» - memory managment watcher module
 *
 *  Copyright (C) Alexey Shishkin 2016
 *
 *  This file is part of project «C-heetah».
 *
 *  Project «C-heetah» is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Project «C-heetah» is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Project «C-heetah». If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#ifndef CTH_BUILD_LIBRARY
#   include <stdlib.h>
#else
    extern void exit(int status);
#   define EXIT_FAILURE 1
#endif // CTH_BUILD_LIBRARY
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>

#include "cheetah.h"

/* Remove macros */
#ifdef malloc
#undef malloc
#endif // malloc

#ifdef realloc
#undef realloc
#endif //realloc

#ifdef calloc
#undef calloc
#endif // calloc

#ifdef free
#undef free
#endif // free

enum CTH_Philosophy
{
    CTH_P_OPTIMISTIC,
    CTH_P_PESSIMISTIC,
    CTH_P_DEFAULT = CTH_P_OPTIMISTIC
};

enum CTH_OutputMethods
{
    CTH_O_STDOUT,
    CTH_O_STDERR, 
    CTH_O_FILE,

    CTH_O_DEFAULT = CTH_O_STDOUT
};

enum CTH_ActionsFailed
{
    CTH_A_NOTHING,
    CTH_A_EXIT,

    CTH_A_DEFAULT = CTH_A_NOTHING
};

///@todo global flag for colouring
/* Global flags */
unsigned int cth_philosophy =
#ifdef CTH_PHILOSOPHY_PESSIMISTIC
    CTH_P_PESSIMISTIC;
#elif CTH_PHILOSOPHY_OPTIMISTIC
    CTH_P_OPTIMISTIC;
#else
    CTH_P_DEFAULT;
#endif

unsigned int cth_output =
#ifdef CTH_OUTPUT_STDOUT
    CTH_O_STDOUT;
#elif CTH_OUTPUT_STDERR
    CTH_O_STDERR;
#elif CTH_OUTPUT_FILE
    CTH_O_FILE;
#else
    CTH_O_DEFAULT;
#endif

unsigned int cth_actionfailed =
#ifdef CTH_ACTION_NOTHING
    CTH_A_NOTHING;
#elif CTH_ACTION_EXIT
    CTH_A_EXIT;
#else
    CTH_A_DEFAULT;
#endif

/* Consts */
#ifndef CTH_COLOURING_ON
#   define CTH_MSG_SIGNATURE "\r[C-TH][%25s:%04u] "
#   define CTH_MSG_MALLOC    "Malloc : "
#   define CTH_MSG_CALLOC    "Calloc : "
#   define CTH_MSG_REALLOC   "Realloc: "
#   define CTH_MSG_FREE      "Free   : "
#   define CTH_MSG_BYTES     "0x%08X B "
#   define CTH_MSG_POINTER   "(0x%08X) "
#   define CTH_MSG_ALIGNER   "             "
#   define CTH_MSG_FAILED    ": Failed \n"
#   define CTH_MSG_SUCCESS   ": Success\n"
#   define CTH_MSG_EXITING   "Exiting\n"
#   define CTH_MSG_NOFILE    "[C-TH] Can't write to file %s\n"
#else // CTH_COLOURING_ON
#   define CTH_MSG_SIGNATURE CTH_SET_WHITE  "[C-TH][%25s:%04u] " CTH_SET_DEFAULT
#   define CTH_MSG_MALLOC    CTH_SET_YELLOW "Malloc : " CTH_SET_DEFAULT
#   define CTH_MSG_CALLOC    CTH_SET_YELLOW "Calloc : " CTH_SET_DEFAULT
#   define CTH_MSG_REALLOC   CTH_SET_CYAN   "Realloc: " CTH_SET_DEFAULT
#   define CTH_MSG_FREE      CTH_SET_GREEN  "Free   : " CTH_SET_DEFAULT
#   define CTH_MSG_BYTES     CTH_SET_WHITE  "0x%08X B " CTH_SET_DEFAULT
#   define CTH_MSG_POINTER   CTH_SET_BLUE   "(0x%08X) " CTH_SET_DEFAULT
#   define CTH_MSG_ALIGNER                  "             "
#   define CTH_MSG_FAILED    CTH_SET_RED    ": Failed \n" CTH_SET_DEFAULT
#   define CTH_MSG_SUCCESS   CTH_SET_GREEN  ": Success\n" CTH_SET_DEFAULT
#   define CTH_MSG_EXITING   CTH_SET_RED    ": Exiting\n" CTH_SET_DEFAULT
#   define CTH_MSG_NOFILE    CTH_SET_RED    "Can't write to file %s\n" CTH_SET_DEFAULT
#endif // CTH_COLOURING_ON

/* Colours */
#define CTH_COLOUR_BLACK   "0"
#define CTH_COLOUR_RED     "1"
#define CTH_COLOUR_GREEN   "2"
#define CTH_COLOUR_YELLOW  "3"
#define CTH_COLOUR_BLUE    "4"
#define CTH_COLOUR_MAGENTA "5"
#define CTH_COLOUR_CYAN    "6"
#define CTH_COLOUR_WHITE   "7"

/* ANSI codes */
#define CTH_SET_FORE_P1 "\033[1;40;3" /* NOTE: Also sets black background */
#define CTH_SET_FORE_P2 "m"
#define CTH_SET_DEFAULT "\033[0m"

/* Colouring shortcuts */
#define CTH_SET_BLACK   CTH_SET_FORE_P1 CTH_COLOUR_BLACK   CTH_SET_FORE_P2
#define CTH_SET_RED     CTH_SET_FORE_P1 CTH_COLOUR_RED     CTH_SET_FORE_P2
#define CTH_SET_GREEN   CTH_SET_FORE_P1 CTH_COLOUR_GREEN   CTH_SET_FORE_P2
#define CTH_SET_YELLOW  CTH_SET_FORE_P1 CTH_COLOUR_YELLOW  CTH_SET_FORE_P2
#define CTH_SET_BLUE    CTH_SET_FORE_P1 CTH_COLOUR_BLUE    CTH_SET_FORE_P2
#define CTH_SET_MAGENTA CTH_SET_FORE_P1 CTH_COLOUR_MAGENTA CTH_SET_FORE_P2
#define CTH_SET_CYAN    CTH_SET_FORE_P1 CTH_COLOUR_CYAN    CTH_SET_FORE_P2
#define CTH_SET_WHITE   CTH_SET_FORE_P1 CTH_COLOUR_WHITE   CTH_SET_FORE_P2

///@todo /tmp to actual tmp
#define CTH_LOG_FILENAME  "/tmp/c-heetah-%u.log"

/* Format */
/* [SIGN][FILE:LINE] TYPE OLDPTR SIZE NEWPTR RESULT */

/* Protectors */
pthread_mutex_t cth_mutex_printer = PTHREAD_MUTEX_INITIALIZER;
/* NOTE: mutex will not be released in the code. */

#ifdef CTH_BUILD_LIBRARY
#   include <dlfcn.h>

#   define CTH_REGISTER_MALLOC  void * (*libc_malloc )(                size_t) = dlsym(RTLD_NEXT, "malloc" );
#   define CTH_REGISTER_CALLOC  void * (*libc_calloc )(        size_t, size_t) = dlsym(RTLD_NEXT, "calloc" );
#   define CTH_REGISTER_REALLOC void * (*libc_realloc)(void *,         size_t) = dlsym(RTLD_NEXT, "realloc");
#   define CTH_REGISTER_FREE    void * (*libc_free   )(void *                ) = dlsym(RTLD_NEXT, "free"   );

#   define malloc   libc_malloc
#   define calloc   libc_calloc
#   define realloc  libc_realloc
#   define free     libc_free
#else // Include-mode
#   define CTH_REGISTER_MALLOC
#   define CTH_REGISTER_CALLOC
#   define CTH_REGISTER_REALLOC
#   define CTH_REGISTER_FREE
#endif //CTH_BUILD_LIBRARY

void CTH_Print(char * format, ...)
{
    pthread_mutex_lock(&cth_mutex_printer);
    {
        va_list args;
        va_start(args, format);
        switch (cth_output)
        {
        case CTH_O_STDOUT:  vprintf(        format, args); break;
        case CTH_O_STDERR: vfprintf(stderr, format, args); break;
        case CTH_O_FILE  :
            {
                char namebuff[64];
                sprintf(namebuff, CTH_LOG_FILENAME, getpid());
                FILE * file = fopen(namebuff, "ab");
                if (!file)
                {
                    fprintf(stderr, CTH_MSG_NOFILE, namebuff);
                    exit(EXIT_FAILURE);
                }
                vfprintf(file, format, args);
                fclose(file);
            }
        }        
        va_end(args);
    }
    pthread_mutex_unlock(&cth_mutex_printer);
} 

void CTH_ActionFailed(void * ptr)
{
    if (cth_actionfailed == CTH_A_NOTHING)
        return;
    else if (cth_actionfailed == CTH_A_EXIT)
    {
        if (ptr) free(ptr);
        CTH_Print(CTH_MSG_EXITING);
        exit(EXIT_FAILURE);
    }
}

void * CTH_Malloc(size_t size, char * filename, unsigned int line)
{
    CTH_REGISTER_MALLOC
    CTH_REGISTER_CALLOC
    CTH_Print(CTH_MSG_SIGNATURE CTH_MSG_MALLOC CTH_MSG_ALIGNER CTH_MSG_BYTES, 
              filename, line, size);
    void * result = NULL;
    if (cth_philosophy == CTH_P_OPTIMISTIC)
        result = malloc(size);
    else if (cth_philosophy == CTH_P_PESSIMISTIC)
        result = calloc(size, 1); 
        
    if (!result)
    {
        CTH_Print(CTH_MSG_ALIGNER CTH_MSG_FAILED);
        CTH_ActionFailed(NULL);
        return NULL;
    }

    CTH_Print(CTH_MSG_POINTER CTH_MSG_SUCCESS, result);

    return result;
}

void * CTH_Realloc(void * ptr, size_t size, char * filename, unsigned int line)
{
    CTH_REGISTER_REALLOC
    CTH_Print(CTH_MSG_SIGNATURE CTH_MSG_REALLOC CTH_MSG_POINTER CTH_MSG_BYTES, 
              filename, line, ptr, size);
    void * result;
    void * oldptr = ptr;
    if ((result = realloc(ptr, size)) == NULL)
    {
        CTH_Print(CTH_MSG_ALIGNER CTH_MSG_FAILED);
        CTH_ActionFailed(oldptr);
        return NULL;
    }

    CTH_Print(CTH_MSG_POINTER CTH_MSG_SUCCESS, result);

    return result;
}

void * CTH_Calloc(size_t nmemb, size_t size, char * filename, unsigned int line)
{
    CTH_REGISTER_CALLOC
    CTH_Print(CTH_MSG_SIGNATURE CTH_MSG_CALLOC CTH_MSG_ALIGNER CTH_MSG_BYTES, 
              filename, line, nmemb * size);
    void * result;

    result = calloc(nmemb, size);

    if (!result)
    {
        CTH_Print(CTH_MSG_ALIGNER CTH_MSG_FAILED);
        CTH_ActionFailed(NULL);
        return NULL;
    }

    CTH_Print(CTH_MSG_POINTER CTH_MSG_SUCCESS, result);

    return result;
}

void   CTH_Free(void * ptr, char * filename, unsigned int line)
{
    CTH_REGISTER_FREE
    CTH_Print(CTH_MSG_SIGNATURE CTH_MSG_FREE CTH_MSG_POINTER CTH_MSG_ALIGNER, 
              filename, line, ptr);

    free(ptr);

    CTH_Print(CTH_MSG_ALIGNER CTH_MSG_SUCCESS);
}

#ifdef CTH_BUILD_LIBRARY
#   define CTH_LIBRARY_SIGNATURE "C-heetah library"

#   undef malloc
#   undef calloc  
#   undef realloc 
#   undef free    

    void * malloc(size_t size)
    {
        return CTH_Malloc(size, CTH_LIBRARY_SIGNATURE, 0);
    }

    void * calloc(size_t nmemb, size_t size)
    {
       return CTH_Calloc(nmemb, size, CTH_LIBRARY_SIGNATURE, 0);
    }

    void * realloc(void * ptr, size_t size)
    {
        return CTH_Realloc(ptr, size, CTH_LIBRARY_SIGNATURE, 0);
    }

    void free(void * ptr)
    {
        CTH_Free(ptr, CTH_LIBRARY_SIGNATURE, 0);
    }
#endif // CTH_BUILD_LIBRARY

