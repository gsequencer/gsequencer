/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_SOUND_SERVER_H__
#define __AGS_SOUND_SERVER_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOUND_SERVER                    (ags_sound_server_get_type())
#define AGS_SOUND_SERVER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUND_SERVER, AgsSoundServer))
#define AGS_SOUND_SERVER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SOUND_SERVER, AgsSoundServerInterface))
#define AGS_IS_SOUND_SERVER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SOUND_SERVER))
#define AGS_IS_SOUND_SERVER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SOUND_SERVER))
#define AGS_SOUND_SERVER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SOUND_SERVER, AgsSoundServerInterface))

typedef struct _AgsSoundServer AgsSoundServer;
typedef struct _AgsSoundServerInterface AgsSoundServerInterface;

struct _AgsSoundServerInterface
{
  GTypeInterface ginterface;

  void (*set_url)(AgsSoundServer *sound_server,
		  gchar *url);
  gchar* (*get_url)(AgsSoundServer *sound_server);

  void (*set_ports)(AgsSoundServer *sound_server,
		    guint *ports, guint port_count);
  guint* (*get_ports)(AgsSoundServer *sound_server,
		      guint *port_count);
  
  void (*set_soundcard)(AgsSoundServer *sound_server,
			gchar *client_uuid,
			GList *soundcard);
  GList* (*get_soundcard)(AgsSoundServer *sound_server,
			  gchar *client_uuid);

  void (*set_sequencer)(AgsSoundServer *sound_server,
			gchar *client_uuid,
			GList *sequencer);
  GList* (*get_sequencer)(AgsSoundServer *sound_server,
			  gchar *client_uuid);

  GObject* (*register_soundcard)(AgsSoundServer *sound_server,
				 gboolean is_output);
  void (*unregister_soundcard)(AgsSoundServer *sound_server,
			       GObject *soundcard);

  GObject* (*register_sequencer)(AgsSoundServer *sound_server,
				 gboolean is_output);
  void (*unregister_sequencer)(AgsSoundServer *sound_server,
			       GObject *sequencer);
};

GType ags_sound_server_get_type();

void ags_sound_server_set_url(AgsSoundServer *sound_server,
			      gchar *url);
gchar* ags_sound_server_get_url(AgsSoundServer *sound_server);

void ags_sound_server_set_ports(AgsSoundServer *sound_server,
				guint *port, guint port_count);
guint* ags_sound_server_get_ports(AgsSoundServer *sound_server,
				  guint *port_count);

void ags_sound_server_set_soundcard(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *soundcard);
GList* ags_sound_server_get_soundcard(AgsSoundServer *sound_server,
				      gchar *client_uuid);

void ags_sound_server_set_sequencer(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *sequencer);
GList* ags_sound_server_get_sequencer(AgsSoundServer *sound_server,
				      gchar *client_uuid);

GObject* ags_sound_server_register_soundcard(AgsSoundServer *sound_server,
					     gboolean is_output);
void ags_sound_server_unregister_soundcard(AgsSoundServer *sound_server,
					   GObject *soundcard);

GObject* ags_sound_server_register_sequencer(AgsSoundServer *sound_server,
					     gboolean is_output);
void ags_sound_server_unregister_sequencer(AgsSoundServer *sound_server,
					   GObject *sequencer);

G_END_DECLS

#endif /*__AGS_SOUND_SERVER_H__*/
