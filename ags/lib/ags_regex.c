/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <locale.h>
#include <pthread.h>

static pthread_mutex_t locale_mutex = PTHREAD_MUTEX_INITIALIZER;

static locale_t c_locale;
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

  locale_t current;

  pthread_mutex_lock(&locale_mutex);
  
  if(!locale_initialized){
    c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
    
    locale_initialized = TRUE;
  }

  pthread_mutex_unlock(&locale_mutex);

  current = uselocale(c_locale);
  
  retval = regcomp(preg, regex, cflags);

  uselocale(current);
  
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
 * Execute regular expression languange independent. It sets temporaly
 * the locale to C and then reverts it.
 * 
 * Since: 0.8.6
 */
int
ags_regexec(const regex_t *preg, const char *string, size_t nmatch,
	    regmatch_t pmatch[], int eflags)
{
  int retval;

  locale_t current;

  pthread_mutex_lock(&locale_mutex);
  
  if(!locale_initialized){
    c_locale = newlocale(LC_ALL_MASK, "C", (locale_t) 0);
    
    locale_initialized = TRUE;
  }

  pthread_mutex_unlock(&locale_mutex);

  current = uselocale(c_locale);

  retval = regexec(preg, string, nmatch, pmatch, eflags);

  uselocale(current);
  
  return(retval);
}
