/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/lib/ags_regex.h>

#define _GNU_SOURCE
#include <locale.h>
#include <pthread.h>

#include <sys/types.h>
#include <regex.h>

#include <ags/config.h>

/**
 * SECTION:ags_regex
 * @short_description: coding-neutral regex
 * @title: AgsRegex
 * @section_id:
 * @include: ags/lib/ags_regex.h
 *
 * Wrapper around glibc regex functions to provide language coding-neutral
 * processing.
 */

static pthread_mutex_t locale_mutex = PTHREAD_MUTEX_INITIALIZER;

#if defined __APPLE__ || AGS_W32API
static char *locale_env;
#else
static locale_t c_locale;
#endif

static gboolean locale_initialized = FALSE;

/**
 * ags_regcomp:
 * @preg: pattern buffer storage area
 * @regex: regular expression as string
 * @cflags: flags
 * 
 * Compile regular expression language independent. It sets temporaly
 * the locale to C and then reverts it.
 * 
 * Since: 2.0.0
 */
int
ags_regcomp(regex_t *preg, const char *regex, int cflags)
{
  int retval;

#if defined __APPLE__ || AGS_W32API
#else
  locale_t current;
#endif

  pthread_mutex_lock(&locale_mutex);

  if(!locale_initialized){
#if defined __APPLE__ || AGS_W32API
    locale_env = getenv("LC_ALL");
#else
    c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#endif
    
    locale_initialized = TRUE;
  }

  pthread_mutex_unlock(&locale_mutex);

#if defined __APPLE__ || AGS_W32API
  setlocale(LC_ALL, "C");
#else
  current = uselocale(c_locale);
#endif

  retval = regcomp(preg, regex, cflags);

#if defined __APPLE__ || AGS_W32API
  setlocale(LC_ALL, locale_env);
#else
  uselocale(current);
#endif
  
  return(retval);
}

/**
 * ags_regexec:
 * @preg: pattern buffer storage area
 * @string: the string to match
 * @nmatch: match count
 * @pmatch: match array
 * @eflags: error flags
 * 
 * Execute regular expression language independent. It sets temporaly
 * the locale to C and then reverts it.
 * 
 * Since: 2.0.0
 */
int
ags_regexec(const regex_t *preg, const char *string, size_t nmatch,
	    regmatch_t pmatch[], int eflags)
{
  int retval;

#if defined __APPLE__ || AGS_W32API
#else
  locale_t current;
#endif

  pthread_mutex_lock(&locale_mutex);
  
  if(!locale_initialized){
#if defined __APPLE__ || AGS_W32API
    locale_env = getenv("LC_ALL");
#else
    c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#endif
    
    locale_initialized = TRUE;
  }

  pthread_mutex_unlock(&locale_mutex);

#if defined __APPLE__ || AGS_W32API
  setlocale(LC_ALL, "C");
#else
  current = uselocale(c_locale);
#endif

  retval = regexec(preg, string, nmatch, pmatch, eflags);

#if defined __APPLE__ || AGS_W32API
  setlocale(LC_ALL, locale_env);
#else
  uselocale(current);
#endif
    
  return(retval);
}
