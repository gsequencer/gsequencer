/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <glib.h>

#include <Foundation/Foundation.h>

#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>

int
main(int argc, char **argv)
{
  struct passwd *pw;

  gchar *music_path;
  gchar *app_dir;
  gchar *default_path;
  gchar *default_filename, *template_default_filename;
  gchar *default_dirs_mkdir_cmd;
  gchar *default_file_cp_cmd;
  gchar *default_file_sed_cmd;
  gchar *free_sounds_cp_cmd;
  
  uid_t uid;

  GError *error;
  
  music_path = NULL;
  app_dir = NULL;

  uid = getuid();
  pw = getpwuid(uid);

  app_dir = [[NSBundle mainBundle] bundlePath].UTF8String;

  music_path = [[NSSearchPathForDirectoriesInDomains(NSMusicDirectory, NSUserDomainMask, YES) objectAtIndex:0] UTF8String];

  default_path = g_strdup_printf("%s%s",
				 music_path,
				 "/GSequencer/workspace/default");

  default_filename = g_strdup_printf("%s%s",
				     default_path,
				     "/gsequencer-default.xml");
  
  template_default_filename = g_strdup_printf("%s%s",
					      app_dir,
					      "/Contents/Resources/gsequencer-default.xml");

  default_dirs_mkdir_cmd = g_strdup_printf("mkdir -p %s/GSequencer/workspace/default",
					   music_path);

  default_file_cp_cmd = g_strdup_printf("cp -v %s/Contents/Resources/gsequencer-default.xml.in %s/GSequencer/workspace/default/gsequencer-default.xml",
					 app_dir,
					 music_path);
  
  default_file_sed_cmd = g_strdup_printf("sed -i '' 's,@MUSIC_DIR@,%s,g' %s/GSequencer/workspace/default/gsequencer-default.xml",
					 music_path);
  free_sounds_cp_cmd = g_strdup_printf("cp -rv %s/Contents/Resources/free-sounds %s/GSequencer",
				       app_dir,
				       music_path);

  error = NULL;
  g_spawn_command_line_sync(default_dirs_mkdir_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  error = NULL;
  g_spawn_command_line_sync(default_file_cp_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  error = NULL;
  g_spawn_command_line_sync(default_file_sed_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  error = NULL;
  g_spawn_command_line_sync(free_sounds_cp_cmd,
			    NULL,
			    NULL,
			    NULL,
			    &error);
  
  return(0);
}
