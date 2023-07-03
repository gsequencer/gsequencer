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

#ifndef __AGS_SOUND_PROVIDER_H__
#define __AGS_SOUND_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define AGS_TYPE_SOUND_PROVIDER                    (ags_sound_provider_get_type())
#define AGS_SOUND_PROVIDER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUND_PROVIDER, AgsSoundProvider))
#define AGS_SOUND_PROVIDER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SOUND_PROVIDER, AgsSoundProviderInterface))
#define AGS_IS_SOUND_PROVIDER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SOUND_PROVIDER))
#define AGS_IS_SOUND_PROVIDER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SOUND_PROVIDER))
#define AGS_SOUND_PROVIDER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SOUND_PROVIDER, AgsSoundProviderInterface))

typedef struct _AgsSoundProvider AgsSoundProvider;
typedef struct _AgsSoundProviderInterface AgsSoundProviderInterface;

struct _AgsSoundProviderInterface
{
  GTypeInterface ginterface;

  void (*set_default_soundcard)(AgsSoundProvider *sound_provider,
				GObject *soundcard);
  GObject* (*get_default_soundcard)(AgsSoundProvider *sound_provider);

  void (*set_default_soundcard_thread)(AgsSoundProvider *sound_provider,
				       GObject *soundcard_thread);
  GObject* (*get_default_soundcard_thread)(AgsSoundProvider *sound_provider);
  
  void (*set_soundcard)(AgsSoundProvider *sound_provider,
			GList *soundcard);
  GList* (*get_soundcard)(AgsSoundProvider *sound_provider);

  void (*set_sequencer)(AgsSoundProvider *sound_provider,
			GList *sequencer);
  GList* (*get_sequencer)(AgsSoundProvider *sound_provider);

  void (*set_audio)(AgsSoundProvider *sound_provider,
		    GList *audio);
  GList* (*get_audio)(AgsSoundProvider *sound_provider);
  
  void (*set_sound_server)(AgsSoundProvider *sound_provider,
			   GList *sound_server);
  GList* (*get_sound_server)(AgsSoundProvider *sound_provider);

  void (*set_osc_server)(AgsSoundProvider *sound_provider,
			 GList *osc_server);
  GList* (*get_osc_server)(AgsSoundProvider *sound_provider);

  void (*set_program)(AgsSoundProvider *sound_provider,
		      GList *program);
  GList* (*get_program)(AgsSoundProvider *sound_provider);

  void (*set_tempo)(AgsSoundProvider *sound_provider,
		    GList *tempo);
  GList* (*get_tempo)(AgsSoundProvider *sound_provider);
};

GType ags_sound_provider_get_type();

void ags_sound_provider_set_default_soundcard(AgsSoundProvider *sound_provider,
					      GObject *soundcard);
GObject* ags_sound_provider_get_default_soundcard(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_default_soundcard_thread(AgsSoundProvider *sound_provider,
						     GObject *soundcard_thread);
GObject* ags_sound_provider_get_default_soundcard_thread(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_soundcard(AgsSoundProvider *sound_provider,
				      GList *soundcard);
GList* ags_sound_provider_get_soundcard(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_sequencer(AgsSoundProvider *sound_provider,
				      GList *sequencer);
GList* ags_sound_provider_get_sequencer(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_audio(AgsSoundProvider *sound_provider,
				  GList *audio);
GList* ags_sound_provider_get_audio(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_sound_server(AgsSoundProvider *sound_provider,
					 GList *sound_server);
GList* ags_sound_provider_get_sound_server(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_osc_server(AgsSoundProvider *sound_provider,
				       GList *osc_server);
GList* ags_sound_provider_get_osc_server(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_program(AgsSoundProvider *sound_provider,
				    GList *program);
GList* ags_sound_provider_get_program(AgsSoundProvider *sound_provider);

void ags_sound_provider_set_tempo(AgsSoundProvider *sound_provider,
				  GList *tempo);
GList* ags_sound_provider_get_tempo(AgsSoundProvider *sound_provider);

G_END_DECLS

#endif /*__AGS_SOUND_PROVIDER_H__*/
