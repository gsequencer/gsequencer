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

#ifdef __APPLE__
#include <stdlib.h>
#endif

#define _GNU_SOURCE
#include <locale.h>
#include <pthread.h>

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

static pthread_mutex_t locale_mutex = PTHREAD_MUTEX_INITIALIZER;
#ifndef __APPLE__
static locale_t c_locale;
#else
static char *locale_env;
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
 * Since: 0.8.6
 */
int
ags_regcomp(regex_t *preg, const char *regex, int cflags)
{
  int retval;

#ifndef __APPLE__
  locale_t current;
#endif

  pthread_mutex_lock(&locale_mutex);

  if(!locale_initialized){
#ifndef __APPLE__
    c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#else
    locale_env = getenv("LC_ALL");
#endif
    
    locale_initialized = TRUE;
  }

  pthread_mutex_unlock(&locale_mutex);

#ifndef __APPLE__
  current = uselocale(c_locale);
#else
  setlocale(LC_ALL, "C");
#endif

  retval = regcomp(preg, regex, cflags);

#ifndef __APPLE__  
  uselocale(current);
#else
  setlocale(LC_ALL, locale_env);
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
 * Since: 0.8.6
 */
int
ags_regexec(const regex_t *preg, const char *string, size_t nmatch,
	    regmatch_t pmatch[], int eflags)
{
  int retval;

#ifndef __APPLE__
  locale_t current;
#endif

  pthread_mutex_lock(&locale_mutex);
  
  if(!locale_initialized){
#ifndef __APPLE__
    c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
#else
    locale_env = getenv("LC_ALL");
#endif
    
    locale_initialized = TRUE;
  }

  pthread_mutex_unlock(&locale_mutex);

#ifndef __APPLE__
  current = uselocale(c_locale);
#else
  setlocale(LC_ALL, "C");
#endif

  retval = regexec(preg, string, nmatch, pmatch, eflags);

#ifndef __APPLE__
  uselocale(current);
#else
  setlocale(LC_ALL, locale_env);
#endif
    
  return(retval);
}
