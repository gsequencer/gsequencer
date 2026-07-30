/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/file/ags_gstreamer_pipeline_helper.h>

GObject*
ags_gstreamer_pipeline_helper_create_ro_pipeline(AgsGstreamerFile *gstreamer_file)
{
  GstElement *read_pipeline;
  GstElement *playbin;
  GstElement *audio_sink;
  GstElement *video_sink;

  gchar *caps;
  
  GstState current_state;
  GstStateChangeReturn state_change_retval;
  
  GRecMutex *gstreamer_file_mutex;
  
  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* ro-pipeline */
  read_pipeline = gst_pipeline_new("AGS ro-pipeline");

  gst_element_set_state(read_pipeline,
			GST_STATE_NULL);
  
  playbin = gst_element_factory_make("playbin", "AGS playbin");
  
  audio_sink = gst_element_factory_make("appsink", "AGS audio sink");
  
  g_object_set(audio_sink,
	       "sync", FALSE,
	       NULL);
  
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = %s, rate = %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16BE }, layout = (string) { interleaved }, channels = %s, rate = %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#else
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = %s, rate = %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#endif

  g_object_set(audio_sink,
	       "caps", gst_caps_from_string(caps),
	       NULL);

  g_free(caps);

  video_sink = gst_element_factory_make("fakesink", "AGS video sink");
  g_object_set(video_sink,
	       "sync", FALSE,
	       NULL);

  file_uri = NULL;
  
#if defined (AGS_W32API)
  if(filename != NULL){
    gchar *w32_filename;
    gchar **w32_split;

    w32_split = g_strsplit(filename, "\\", -1);

    w32_filename = g_strjoinv("/",
			      w32_split);

    g_strfreev(w32_split);
    
    if(g_path_is_absolute(filename)){
      file_uri = g_strdup_printf("file:///%s",
				 w32_filename);
    }else{
      gchar *current_dir;
      gchar *w32_current_dir;
      gchar **w32_dir_split;

      current_dir = g_get_current_dir();

      w32_dir_split = g_strsplit(current_dir, "\\", -1);

      w32_current_dir = g_strjoinv("/",
				   w32_dir_split);
      
      file_uri = g_strdup_printf("file:///%s/%s",
				 w32_current_dir,
				 w32_filename);

      g_strfreev(w32_dir_split);

      g_free(w32_current_dir);
      g_free(current_dir);
    }

    g_free(w32_filename);
  }  
#else
  if(filename != NULL){
    if(g_path_is_absolute(filename)){
      file_uri = g_strdup_printf("file://%s",
				 filename);
    }else{
      gchar *current_dir;

      current_dir = g_get_current_dir();

      file_uri = g_strdup_printf("file://%s/%s",
				 current_dir,
				 filename);

      g_free(current_dir);
    }
  }
#endif
  
  g_object_set(playbin,
	       "uri", file_uri,
	       "audio-sink", audio_sink,
	       "video-sink", video_sink,
	       NULL);

  g_free(file_uri);

  g_object_get(playbin,
	       "flags", &flags,
	       NULL);

  flags |= GST_PLAY_FLAG_AUDIO;
  flags &= (~GST_PLAY_FLAG_VIDEO);
  flags &= (~GST_PLAY_FLAG_TEXT);
  
  g_object_set(playbin,
	       "flags", flags,
	       NULL);

  gst_bin_add_many(GST_BIN(read_pipeline),
		   playbin,
		   NULL);

  /* apply */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->read_pipeline = read_pipeline;
  gstreamer_file->read_pipeline_running = TRUE;

  gstreamer_file->playbin = playbin;
  gstreamer_file->audio_sink = audio_sink;
  gstreamer_file->video_sink = video_sink;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  state_change_retval = gst_element_set_state(read_pipeline,
					      GST_STATE_PLAYING);

  if(state_change_retval == GST_STATE_CHANGE_FAILURE){
    g_critical("unable to start AGS ro-pipeline");
  }else{
    current_state = 0;
    
    do{
      gst_element_get_state(GST_ELEMENT(read_pipeline), &current_state, NULL, 4000000);
    }while(current_state != GST_STATE_PLAYING);
  }
  
  return(read_pipeline);
}

GObject*
ags_gstreamer_pipeline_helper_create_rw_pipeline(AgsGstreamerFile *gstreamer_file)
{
  GstElement *write_pipeline;

  GstElement *rw_audio_app_src;
  GstElement *rw_audio_file_sink_queue;
  GstElement *rw_audio_convert;
  GstElement *rw_audio_resample;
  GstElement *rw_audio_mixer;
  GstElement *rw_file_encoder;
  GstElement *rw_file_sink;
  GstPad *rw_audio_mixer_src_pad;
  GstPad *rw_file_encoder_audio_pad;

  GstEncodingProfile *encoding_profile;
  
  gchar *caps;
  gchar *filename;
  
  GstState current_state;
  GstStateChangeReturn state_change_retval;

  guint audio_channels;
  guint samplerate;
  guint buffer_size;
  gint64 channel_mask;
  gint flags;
  gboolean success;
  
  GRecMutex *gstreamer_file_mutex;
  
  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  success = TRUE;
  
  g_rec_mutex_lock(gstreamer_file_mutex);

  filename = g_strdup(gstreamer_file->filename);
  
  audio_channels = gstreamer_file->audio_channels;
  samplerate = gstreamer_file->samplerate;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  /* rw-pipeline */
  write_pipeline = gst_pipeline_new("AGS rw-pipeline (write)");
 
  gst_element_set_state(write_pipeline,
  			GST_STATE_NULL);

  rw_audio_app_src = gst_element_factory_make("appsrc", "AGS rw audio source");

  rw_audio_file_sink_queue = gst_element_factory_make("queue", "AGS rw audio file sink queue");
  
  rw_audio_convert = gst_element_factory_make("audioconvert", "AGS rw audio convert");
  rw_audio_resample = gst_element_factory_make("audioresample", "AGS rw audio resample");
  rw_audio_mixer = gst_element_factory_make("audiomixer", "AGS rw audio mixer");

  rw_file_encoder = gst_element_factory_make("encodebin", "AGS rw file encoder");
  rw_file_sink = gst_element_factory_make("filesink", "AGS rw file sink");  
  
  /* write file */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->write_pipeline_running = TRUE;
  gstreamer_file->write_pipeline_need_data = FALSE;

  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  if(!write_pipeline ||
     !rw_audio_app_src ||
     !rw_audio_convert || !rw_audio_resample || !rw_audio_mixer ||
     !rw_audio_file_sink_queue ||
     !rw_file_encoder || !rw_file_sink){
    g_rec_mutex_lock(gstreamer_file_mutex);
    
    gstreamer_file->write_pipeline_running = FALSE;

    g_rec_mutex_unlock(gstreamer_file_mutex);

    success = FALSE;
    
    g_critical("not all elements of pipeline could be created");
  }

  /* configure audio app src */
  channel_mask = -2;

  switch(audio_channels){
  case 2:
    {
      channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT);
    }
    break;
  case 3:
    {
      channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_LFE1);
    }
    break;
  case 6:
    {
      channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER) |
	(1 << GST_AUDIO_CHANNEL_POSITION_SIDE_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_SIDE_RIGHT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_LFE1);
    }
    break;
  case 8:
    {
      //TODO:JK: implement me
    }
    break;
  case 10:
    {
      //TODO:JK: implement me
    }
    break;
  }
  
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = (int) %d, channel-mask = (bitmask) %#018lx, rate = (int) %d",
			 audio_channels,
			 channel_mask,
			 samplerate);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16BE }, layout = (string) { interleaved }, channels = (int) %d, channel-mask = (bitmask) %#018lx, rate = (int) %d",
			 audio_channels,
			 channel_mask,
			 samplerate);
#else
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = (int) %d, channel-mask = (bitmask) %#018lx, rate = (int) %d",
			 audio_channels,
			 channel_mask,
			 samplerate);
#endif

  g_object_set(rw_audio_app_src,
	       "caps", gst_caps_fixate(gst_caps_from_string(caps)),
	       "format", GST_FORMAT_TIME,
	       NULL);

  g_free(caps);

  /* configure encoder */
  ags_gstreamer_file_detect_encoding_profile(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  encoding_profile = gstreamer_file->encoding_profile;

  if(encoding_profile != NULL){
    g_object_ref(encoding_profile);
  }
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  g_object_set(rw_file_encoder,
	       "profile", encoding_profile,
	       NULL);

  if(encoding_profile != NULL){
    g_object_unref(encoding_profile);
  }

  /* configure file sink */
  g_object_set(rw_file_sink,
	       "location", filename,
	       NULL);

  gst_bin_add_many(GST_BIN(write_pipeline),
		   rw_audio_app_src,
		   rw_audio_convert,
		   rw_audio_resample,
		   rw_audio_mixer,
		   rw_file_encoder,
		   rw_file_sink,
		   NULL);

  gst_element_link_many(rw_audio_app_src, rw_audio_convert, rw_audio_resample, rw_audio_mixer, NULL);
  gst_element_link_many(rw_file_encoder, rw_file_sink, NULL);

  rw_audio_mixer_src_pad = gst_element_get_static_pad(rw_audio_mixer, "src");

  rw_file_encoder_audio_pad = gst_element_get_request_pad(rw_file_encoder, "audio_%u");

  gst_pad_link(rw_audio_mixer_src_pad,
	       rw_file_encoder_audio_pad);

  /* apply */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->write_pipeline = write_pipeline;

  gstreamer_file->rw_audio_app_src = rw_audio_app_src;
  gstreamer_file->rw_audio_convert = rw_audio_convert;
  gstreamer_file->rw_audio_resample = rw_audio_resample;
  gstreamer_file->rw_audio_mixer = rw_audio_mixer;
  gstreamer_file->rw_file_encoder = rw_file_encoder;
  gstreamer_file->rw_file_sink = rw_file_sink;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  state_change_retval = gst_element_set_state(write_pipeline,
					      GST_STATE_PLAYING);

  if(state_change_retval == GST_STATE_CHANGE_FAILURE){
    g_rec_mutex_lock(gstreamer_file_mutex);
    
    gstreamer_file->write_pipeline_running = FALSE;

    g_rec_mutex_unlock(gstreamer_file_mutex);

    success = FALSE;
    
    g_critical("unable to start AGS rw-pipeline (write)");
  }else{
#if 0
    do{
      gst_element_get_state(GST_ELEMENT(write_pipeline), &current_state, NULL, 4000000);
    }while(current_state != GST_STATE_PLAYING);
#endif
  }
  
  return(write_pipeline);
}
