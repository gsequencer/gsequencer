/* This file is part of GSequencer.
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

#ifndef __AGS_PREFERENCES_H__
#define __AGS_PREFERENCES_H__

#include <gtk/gtk.h>

#include <ags/X/ags_generic_preferences.h>
#include <ags/X/ags_audio_preferences.h>
#include <ags/X/ags_performance_preferences.h>
#include <ags/X/ags_server_preferences.h>

#define AGS_TYPE_PREFERENCES                (ags_preferences_get_type())
#define AGS_PREFERENCES(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PREFERENCES, AgsPreferences))
#define AGS_PREFERENCES_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PREFERENCES, AgsPreferencesClass))
#define AGS_IS_PREFERENCES(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PREFERENCES))
#define AGS_IS_PREFERENCES_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PREFERENCES))
#define AGS_PREFERENCES_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PREFERENCES, AgsPreferencesClass))

#define AGS_PREFERENCES_DEFAULT_FILENAME "ags_current.xml\0"

typedef struct _AgsPreferences AgsPreferences;
typedef struct _AgsPreferencesClass AgsPreferencesClass;

typedef enum{
  AGS_PREFERENCES_SHUTDOWN               = 1,
}AgsPreferencesFlags;

struct _AgsPreferences
{
  GtkDialog dialog;

  guint flags;

  GtkWindow *window;

  AgsGenericPreferences *generic_preferences;
  AgsAudioPreferences *audio_preferences;
  AgsPerformancePreferences *performance_preferences;
  AgsServerPreferences *server_preferences;
};

struct _AgsPreferencesClass
{
  GtkDialogClass dialog;
};

GType ags_preferences_get_type(void);

AgsPreferences* ags_preferences_new();

#endif /*__AGS_PREFERENCES_H__*/
