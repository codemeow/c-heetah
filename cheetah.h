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

#include <string.h>

///@todo ifdef for library building

/* Convert compile-time filename to basename */
#define CTH_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define CTH_PLACE    CTH_FILENAME, __LINE__

/* Remove possible previous definitions */
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

#define malloc(BYTES) CTH_Malloc((BYTES), CTH_PLACE)
#define realloc(POINTER, BYTES) CTH_Realloc((POINTER), (BYTES), CTH_PLACE)
#define calloc(MEMB, SIZE) CTH_Calloc((MEMB), (SIZE), CTH_PLACE)
#define free(POINTER) CTH_Free((POINTER), CTH_PLACE)

void * CTH_Malloc (            size_t size,               char * filename, unsigned int line);
void * CTH_Realloc(void * ptr, size_t size,               char * filename, unsigned int line);
void * CTH_Calloc (            size_t nmemb, size_t size, char * filename, unsigned int line);
void   CTH_Free   (void * ptr,                            char * filename, unsigned int line);
