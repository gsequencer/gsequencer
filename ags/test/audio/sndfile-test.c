/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2018 Joël Krähemann
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
#include <glib-object.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#define SNDFILE_TEST_FILENAME "/home/joelkraehemann/Downloads/output.ogg"

#define SNDFILE_TEST_CONFIG "[generic]\n"		\
  "autosave-thread=false\n"				\
  "simple-file=true\n"					\
  "disable-feature=experimental\n"			\
  "segmentation=4/4\n"					\
  "\n"							\
  "[thread]\n"						\
  "model=super-threaded\n"				\
  "super-threaded-scope=channel\n"			\
  "lock-global=ags-thread\n"				\
  "lock-parent=ags-recycling-thread\n"			\
  "\n"							\
  "[soundcard]\n"					\
  "backend=alsa\n"					\
  "device=hw:0\n"					\
  "samplerate=44100\n"					\
  "buffer-size=16384\n"					\
  "pcm-channels=2\n"					\
  "dsp-channels=2\n"					\
  "format=16\n"						\
  "\n"							\
  "[recall]\n"						\
  "auto-sense=true\n"					\
  "\n"

extern AgsApplicationContext *ags_application_context;

int
main(int argc, char **argv)
{
  AgsAudioApplicationContext *audio_application_context;
  AgsDevout *devout;
  AgsAudioFile *audio_file;

  AgsThread *polling_thread;
  
  AgsConfig *config;

  GList *start_wave, *wave;

  guint total_frame_count;
  guint64 offset;

  struct timespec req;

  req.tv_sec = 0;
  req.tv_nsec = AGS_NSEC_PER_SEC * (16384.0 / 44100.0);
  
  GError *error;
  
  config = ags_config_get_instance();
  ags_config_load_from_data(config,
			    SNDFILE_TEST_CONFIG,
			    strlen(SNDFILE_TEST_CONFIG));

  audio_application_context = ags_audio_application_context_new();
  ags_application_context = audio_application_context;

  ags_thread_start(ags_application_context->main_loop);
  
  ags_thread_pool_start(audio_application_context->thread_pool);

  polling_thread = ags_polling_thread_new();
  ags_thread_add_child_extended(AGS_THREAD(ags_application_context->main_loop),
				(AgsThread *) polling_thread,
				FALSE, FALSE);
  
  devout = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(audio_application_context))->data;
  
  audio_file = ags_audio_file_new(SNDFILE_TEST_FILENAME,
				  devout,
				  -1);
  ags_audio_file_open(audio_file);
  
  wave =
    start_wave = ags_sound_resource_read_wave(AGS_SOUND_RESOURCE(audio_file->sound_resource),
					      devout,
					      -1,
					      0,
					      0.0, 0);

  ags_sound_resource_info(AGS_SOUND_RESOURCE(audio_file->sound_resource),
			  &total_frame_count,
			  NULL, NULL);
  
  error = NULL;
  ags_soundcard_play_init(AGS_SOUNDCARD(devout),
			  &error);

  devout->poll_fd = NULL;
  offset = 0;

  for(; offset < total_frame_count; ){
    wave = start_wave;
    
    while(wave != NULL){
      GList *start_buffer, *buffer;
    
      void *buffer0;

      guint line;

      g_object_get(wave->data,
		   "line", &line,
		   //		   "buffer", &start_buffer,
		   NULL);

      buffer0 = ags_soundcard_get_buffer(AGS_SOUNDCARD(devout));
      ags_audio_buffer_util_clear_buffer(buffer0, 2,
					 16384, AGS_AUDIO_BUFFER_UTIL_S16);
      
      buffer = AGS_WAVE(wave->data)->buffer;

      while(buffer != NULL){
	guint64 current_offset;

	g_object_get(buffer->data,
		     "x", &current_offset,
		     NULL);

	if(current_offset > offset){
	  break;
	}

	if(current_offset == offset){
	  //	  g_message("%d", current_offset);
	  
	  ags_audio_buffer_util_copy_s16_to_s16(((gint16 *) buffer0) + line, 2,
						AGS_BUFFER(buffer->data)->data, 1,
						16384);
	}
      
	buffer = buffer->next;
      }

      error = NULL;
      ags_soundcard_play(AGS_SOUNDCARD(devout),
			 &error);

      usleep(AGS_USEC_PER_SEC / 44100 * 16384);
      //      g_list_free(start_buffer);

      /* iterate */      
      wave = wave->next;
    }
    
    offset += 16384;
  }
  
  ags_soundcard_stop(AGS_SOUNDCARD(devout));
  
  return(0);
}
