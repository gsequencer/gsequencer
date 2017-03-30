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

#include <ags/X/ags_effect_bridge_callbacks.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

void
ags_effect_bridge_set_audio_channels_callback(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old,
					      AgsEffectBridge *effect_bridge)
{
  AgsWindow *window;
  
  AgsGuiThread *gui_thread;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_bridge);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type((AgsThread *) main_loop,
						     AGS_TYPE_GUI_THREAD);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  ags_effect_bridge_resize_audio_channels(effect_bridge,
					  audio_channels, audio_channels_old);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  gdk_threads_leave();
}

void
ags_effect_bridge_set_pads_callback(AgsAudio *audio,
				    GType channel_type,
				    guint pads, guint pads_old,
				    AgsEffectBridge *effect_bridge)
{  
  AgsWindow *window;
  
  AgsGuiThread *gui_thread;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) effect_bridge);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type((AgsThread *) main_loop,
						     AGS_TYPE_GUI_THREAD);

  /*  */
  pthread_mutex_lock(gui_thread->dispatch_mutex);

  ags_effect_bridge_resize_pads(effect_bridge,
				channel_type,
				pads, pads_old);

  pthread_mutex_unlock(gui_thread->dispatch_mutex);

  gdk_threads_leave();
}

