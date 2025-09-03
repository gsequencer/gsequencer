/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <AppKit/AppKit.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/AUComponent.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <CoreAudio/CoreAudio.h>

#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE (512)

static struct sigaction ags_audio_unit_test_sigact;

gboolean audio_unit_test_success = FALSE;

void
ags_audio_unit_test_plugin_signal_handler(int signr)
{
  if(signr == SIGINT ||
     signr == SIGABRT ||
     signr == SIGSEGV ||
     signr == SIGPIPE ||
     signr == SIGILL ||
     signr == SIGKILL){
    sigemptyset(&(ags_audio_unit_test_sigact.sa_mask));
    
    if(audio_unit_test_success){
      _Exit(0);
    }else{
      _Exit(-1);
    }
  }else{
    sigemptyset(&(ags_audio_unit_test_sigact.sa_mask));
  }
}

void
ags_audio_unit_test_exception_handler(NSException *e)
{
  if(audio_unit_test_success){
    _Exit(0);
  }else{
    _Exit(-1);
  }
}

gboolean
ags_audio_unit_test_plugin(gchar *au_type,
			   gchar *au_sub_type,
			   gchar *au_manufacturer,
			   guint pcm_channels,
			   guint samplerate,
			   guint buffer_size,
			   gboolean super_threaded_channel)
{
  AVAudioEngine *audio_engine;

  AVAudioOutputNode *av_output_node;
  AVAudioInputNode *av_input_node;
  AVAudioUnitEffect *av_audio_unit_effect;
  AVAudioUnitMIDIInstrument *av_audio_unit_midi_instrument;
  AVAudioUnit *av_audio_unit;
  AVAudioSequencer *av_audio_sequencer;
  AVMusicTrack *av_music_track;

  AVAudioFormat *av_format;
  
  AudioComponent comp;
  AudioComponentDescription desc;
  AudioStreamBasicDescription stream_description;

  AVAudioPCMBuffer *av_output;

  gdouble bpm;
  guint plugin_channels;
  gint status;
  gboolean is_instrument;
  
  NSError *ns_error;

  pid_t p;

  @try {
    bpm = 120.0;
    
    memset(&desc, 0, sizeof(desc));

    desc.componentType = au_type[3] | (au_type[2]<<8) | (au_type[1]<<16) | (au_type[0]<<24);
  
    desc.componentSubType = au_sub_type[3] | (au_sub_type[2]<<8) | (au_sub_type[1]<<16) | (au_sub_type[0]<<24);

    desc.componentManufacturer = au_manufacturer[3] | (au_manufacturer[2]<<8) | (au_manufacturer[1]<<16) | (au_manufacturer[0]<<24);

    comp = AudioComponentFindNext(NULL,
				  &desc);

    if(comp == NULL){
      return(FALSE);
    }
  
    AudioComponentGetDescription(comp,
				 &desc);
  
    is_instrument = (desc.componentType == kAudioUnitType_MusicDevice) ? TRUE: FALSE;

    if(desc.componentType != kAudioUnitType_MusicDevice &&
       desc.componentType != kAudioUnitType_Effect &&
       desc.componentType != kAudioUnitType_MusicEffect &&
       desc.componentType != kAudioUnitType_Panner){
      return(FALSE);
    }
    
    /* plugin channels */
    plugin_channels = pcm_channels;
  
    if(!super_threaded_channel){
      plugin_channels = 1;
    }else{
      switch(pcm_channels){
      case 1:
	{
	}
	break;
      case 2:
	{
	}
	break;
      case 3:
	{
	}
	break;
      case 6:
	{
	}
	break;
      case 8:
	{
	}
	break;
      default:
	{
	  plugin_channels = 1;
	}
      }
    }
  
    /* audio unit */
    stream_description = (AudioStreamBasicDescription) {0,};

    stream_description.mFormatID = kAudioFormatLinearPCM;
    stream_description.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
    stream_description.mSampleRate = (double) samplerate;
    stream_description.mBitsPerChannel = 8 * sizeof(gfloat);
    stream_description.mFramesPerPacket = 1;
    stream_description.mChannelsPerFrame = plugin_channels;
    stream_description.mBytesPerFrame = sizeof(gfloat);
    stream_description.mBytesPerPacket = sizeof(gfloat);

    av_format = [[AVAudioFormat alloc] initWithStreamDescription:&stream_description];

    /* audio engine */
    audio_engine = [[AVAudioEngine alloc] init];
  
    ns_error = NULL;
  
    [audio_engine enableManualRenderingMode:AVAudioEngineManualRenderingModeOffline
     format:av_format
     maximumFrameCount:BUFFER_SIZE
     error:&ns_error];

    if(ns_error != NULL &&
       [ns_error code] != noErr){
      g_warning("enable manual rendering mode error - %d", [ns_error code]);
    }

    /* av audio unit */
    if(!is_instrument){
      av_audio_unit_effect = [[AVAudioUnitEffect alloc] initWithAudioComponentDescription:desc];

      av_audio_unit = (AVAudioUnit *) av_audio_unit_effect;
    }else{
      av_audio_unit_midi_instrument = [[AVAudioUnitMIDIInstrument alloc] initWithAudioComponentDescription:desc];

      av_audio_unit = (AVAudioUnit *) av_audio_unit_midi_instrument;
    }

    /* output node */
    av_output_node = [audio_engine outputNode];

    /* input node */
    av_input_node = [audio_engine inputNode];

    /* audio unit */
    [audio_engine attachNode:av_audio_unit];

    if(!is_instrument){
      [audio_engine connect:av_input_node to:av_audio_unit format:av_format];
    }

    [audio_engine connect:av_audio_unit to:av_output_node format:av_format];

    /*  */
    if(!is_instrument){
      input_success = [av_input_node setManualRenderingInputPCMFormat:av_format
		       inputBlock:^(AVAudioFrameCount inNumberOfFrames){
	  AudioBufferList *audio_buffer_list;

	  guint audio_channels;
	  guint i;

	  audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + (pcm_channels * sizeof(AudioBuffer)));
		
	  audio_buffer_list->mNumberBuffers = pcm_channels;
	
	  for(j = 0; j < pcm_channels; j++){
	    audio_buffer_list->mBuffers[j].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	    memset(audio_buffer_list->mBuffers[j].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	    audio_buffer_list->mBuffers[j].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	    audio_buffer_list->mBuffers[j].mNumberChannels = 1;
	  }
	  
	  audio_channels = [av_format channelCount];

	  audio_buffer_list = NULL;
				
	  /* fill av input buffer */
	  for(i = 0; i < audio_channels; i++){
	    if(BUFFER_SIZE <= inNumberOfFrames){
	      memset(audio_buffer_list->mBuffers[i].mData, 0, BUFFER_SIZE * sizeof(gfloat));
	    }else{
	      memset(audio_buffer_list->mBuffers[i].mData, 0, inNumberOfFrames * sizeof(gfloat));
	    }
	  }
      
	  return((const AudioBufferList *) audio_buffer_list);
	}];

      if(input_success != YES){
	g_warning("set manual rendering input failed");
      }
    }
    /* audio sequencer */
    av_audio_sequencer = [[AVAudioSequencer alloc] initWithAudioEngine:audio_engine];
  
    /* sequencer */
    av_audio_sequencer.currentPositionInBeats = 0.0; // a 16th

    //FIXME:JK: available since macOS 13.0
    av_music_track = [av_audio_sequencer createAndAppendTrack];

    av_music_track.destinationAudioUnit = av_audio_unit;
  
    av_music_track.offsetTime = 0.0; // 0

    av_music_track.lengthInBeats = 19200.0 * 4.0; // a 16th

    av_music_track.lengthInSeconds = 19200.0 * 4.0 * (60.0 / bpm);
  
    av_music_track.usesAutomatedParameters = NO;
  
    av_music_track.muted = NO;

    [audio_engine prepare];
    [audio_engine startAndReturnError:&ns_error];

    if(ns_error != NULL &&
       [ns_error code] != noErr){
      g_warning("error during audio engine start - %d", [ns_error code]);
    }
  
    [av_audio_sequencer prepareToPlay];

    ns_error = NULL;
  
    [av_audio_sequencer startAndReturnError:&ns_error];

    if(ns_error != NULL &&
       [ns_error code] != noErr){
      g_warning("error during audio sequencer start - %d", [ns_error code]);
    }
  
    /* output */
    av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		 frameCapacity:(plugin_channels * BUFFER_SIZE)];

    /* render */
    ns_error = NULL;
	  
    status = [audio_engine renderOffline:BUFFER_SIZE toBuffer:av_output error:&ns_error];
    
    if(ns_error != NULL &&
       [ns_error code] != noErr){
      g_warning("render offline error - %d", [ns_error code]);
    }
  
    switch(status){
    case AVAudioEngineManualRenderingStatusSuccess:
      {
	//NOTE:JK: this is fine
	//	    g_message("OK");
      }
      break;
    case AVAudioEngineManualRenderingStatusInsufficientDataFromInputNode:
      {
	g_message("insufficient data from input");
      }
      break;
    case AVAudioEngineManualRenderingStatusCannotDoInCurrentContext:
      {
	g_message("cannot do in current context");
      }
      break;
    case AVAudioEngineManualRenderingStatusError:
      {
	g_message("error");
      }
      break;
    }
  
    [av_audio_sequencer stop];

    [audio_engine stop];
  }
  @catch (NSException *exception) {
    g_critical("exception occured - %s", [exception.reason UTF8String]);
    
    _Exit(-1);
  }

  audio_unit_test_success = TRUE;
  
  return(TRUE);
}

void
ags_audio_unit_test_print_usage()
{
   printf("GSequencerAudioUnitTest is an Audio Unit test utility\n\n");

   printf("Usage:\n\t%s\n\t%s\n\n",
	  "Report bugs to <jkraehemann@gmail.com>\n",
	  "type sub_type manufacturer pcm_channels samplerate buffer_size super_threaded_channel");
}

int
main(int argc, char **argv)
{
  gchar au_type[8];
  gchar au_sub_type[8];
  gchar au_manufacturer[8];
  
  guint pcm_channels;
  guint samplerate;
  guint buffer_size;
  gboolean super_threaded_channel;
  
  if(argc != 8){
    ags_audio_unit_test_print_usage();
    
    return(-1);
  }
  
  ags_audio_unit_test_sigact.sa_handler = ags_audio_unit_test_plugin_signal_handler;

  sigemptyset(&ags_audio_unit_test_sigact.sa_mask);
  ags_audio_unit_test_sigact.sa_flags = 0;

  sigaction(SIGINT, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);
  sigaction(SIGABRT, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);
  sigaction(SIGSEGV, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);
  sigaction(SIGPIPE, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);
  sigaction(SIGILL, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);
  sigaction(SIGKILL, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);
  sigaction(SA_RESTART, &ags_audio_unit_test_sigact, (struct sigaction *) NULL);

  NSSetUncaughtExceptionHandler(&ags_audio_unit_test_exception_handler);
  
  memcpy(au_type, argv[1], 4 * sizeof(char));
  
  memcpy(au_sub_type, argv[2], 4 * sizeof(char));
  
  memcpy(au_manufacturer, argv[3], 4 * sizeof(char));
  
  pcm_channels = g_ascii_strtoull(argv[4],
				  NULL,
				  10);
  
  samplerate = g_ascii_strtoull(argv[5],
				NULL,
				10);

  buffer_size = g_ascii_strtoull(argv[6],
				 NULL,
				 10);

  super_threaded_channel = (!g_strcmp0(argv[7], "false") == FALSE) ? TRUE: FALSE;

  g_message("test %s %s %s with channels %d and samplerate %d", argv[1], argv[2], argv[3], pcm_channels, samplerate);
  
  if(!ags_audio_unit_test_plugin(au_type, au_sub_type, au_manufacturer, pcm_channels, samplerate, buffer_size, super_threaded_channel)){
    return(-1);
  }
  
  return(0);
}

