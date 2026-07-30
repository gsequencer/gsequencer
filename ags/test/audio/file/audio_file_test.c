//
//  audio_file_test.c
//  com.gsequencer.GSequencer
//
//  Created by Joël Krähemann on 06.06.2025.
//

#include <AudioToolbox/AudioToolbox.h>

#include <stdio.h>

long long audio_toolbox_proc_get_size(void *data);
int audio_toolbox_proc_set_size(void *data, long long buffer_size);

OSStatus audio_toolbox_read_proc(void *data,
                                  SInt64 in_position,
                                  UInt32 request_count,
                                  void *buffer,
                                  UInt32 *actual_count);
OSStatus audio_toolbox_write_proc(void *data,
                                  SInt64 in_position,
                                  UInt32 request_count,
                                  void *buffer,
                                  UInt32 *actual_count);

#define FORMAT_S32 (0x1)
#define FORMAT_FLOAT (0xf1)

struct _AudioFileTest{
  UInt32 major;
  UInt32 minor;
};

AudioFileID audio_file;
ExtAudioFileRef ext_audio_file;

void *full_buffer;

unsigned int audio_channels;

unsigned int samplerate;
unsigned int buffer_size;
unsigned int format;
unsigned int frame_count;

UInt64 offset;
                                      
long long
audio_toolbox_proc_get_size(void *data)
{
  return(offset);
}

int
audio_toolbox_proc_set_size(void *data, long long buffer_size)
{
  return(0);
}

OSStatus audio_toolbox_read_proc(void *data,
                                 SInt64 in_position,
                                 UInt32 request_count,
                                 void *buffer,
                                 UInt32 *actual_count)
{
  return(0);
}
  
OSStatus
audio_toolbox_write_proc(void *data,
                         SInt64 in_position,
                         UInt32 request_count,
                         void *buffer,
                         UInt32 *actual_count)
{
  return(0);
}


int
main(int argc, char **argv)
{
  struct _AudioFileTest test;
  
  AudioChannelLayout file_channel_layout;
  AudioFileTypeID file_format;
  AudioStreamBasicDescription file_data_format;
  
  UInt32 prop_size;
  
  UInt64 file_byte_count;
  UInt64 file_packet_count;
  UInt32 file_max_packet_size;
  SInt64 file_length_frames;
  
  UInt32 status;
  
  const char *filename = "/Users/joelkraehemann/Music/test.mp4";
  
  /* audio file URL */
  NSString *audio_file_path = [[NSString stringWithUTF8String:filename]
                               stringByExpandingTildeInPath];

  NSURL *audio_url = [[NSURL alloc] initFileURLWithPath:audio_file_path];
  
  audio_file = NULL;
  
  status = AudioFileOpenWithCallbacks(&test,
                                      (AudioFile_ReadProc) audio_toolbox_read_proc,
                                      NULL,
                                      (AudioFile_GetSizeProc) audio_toolbox_proc_get_size,
                                      NULL,
                                      kAudioFileMPEG4Type,
                                      &audio_file);
  
  if(status != noErr){
    fprintf(stdout, "failed to open %s - status %u\n", filename, status);
    
    return(-1);
  }
  
  status = ExtAudioFileWrapAudioFileID(audio_file,
                                       FALSE,
                                       &ext_audio_file);

  if(status != noErr){
    fprintf(stdout, "failed to wrap audio file ID\n");
  }
  
  /* file byte count */
  status = AudioFileGetPropertyInfo(audio_file,
                                    kAudioFilePropertyAudioDataByteCount,
                                    &prop_size,
                                    nil);

  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  file_byte_count = 0;
  
  status = AudioFileGetProperty(audio_file,
                                kAudioFilePropertyAudioDataByteCount,
                                &prop_size,
                                &file_byte_count);
  
  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  /* file packet count */
  status = AudioFileGetPropertyInfo(audio_file,
                                    kAudioFilePropertyAudioDataPacketCount,
                                    &prop_size,
                                    nil);

  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  file_packet_count = 0;
  
  status = AudioFileGetProperty(audio_file,
                                kAudioFilePropertyAudioDataPacketCount,
                                &prop_size,
                                &file_packet_count);
  
  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  /* file max packet size */
  status = AudioFileGetPropertyInfo(audio_file,
                                    kAudioFilePropertyMaximumPacketSize,
                                    &prop_size,
                                    nil);

  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  file_max_packet_size = 0;
  
  status = AudioFileGetProperty(audio_file,
                                kAudioFilePropertyMaximumPacketSize,
                                &prop_size,
                                &file_max_packet_size);
  
  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  /* file channel layout */
  status = AudioFileGetPropertyInfo(audio_file,
                                    kAudioFilePropertyChannelLayout,
                                    &prop_size,
                                    nil);

  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  file_channel_layout = (AudioChannelLayout) {0,};
  
  status = AudioFileGetProperty(audio_file,
                                kAudioFilePropertyChannelLayout,
                                &prop_size,
                                &file_channel_layout);
  
  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  /* file format */
  file_format = 0;
  
  status = AudioFileGetPropertyInfo(audio_file,
                                    kAudioFilePropertyFileFormat,
                                    &prop_size,
                                    nil);

  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  status = AudioFileGetProperty(audio_file,
                                kAudioFilePropertyFileFormat,
                                &prop_size,
                                &file_format);
  
  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  /* file data format */
  status = AudioFileGetPropertyInfo(audio_file,
                                    kAudioFilePropertyDataFormat,
                                    &prop_size,
                                    nil);

  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  file_data_format = (AudioStreamBasicDescription) {0,};
  
  status = AudioFileGetProperty(audio_file,
                                kAudioFilePropertyDataFormat,
                                &prop_size,
                                &file_data_format);
  
  if(status != noErr){
    fprintf(stdout, "AudioToolbox status != noErr\n");
  }
  
  samplerate = (unsigned int) file_data_format.mSampleRate;
  
  audio_channels = 1;
  
  if((kAudioChannelLayoutTag_Mono & (file_channel_layout.mChannelLayoutTag)) != 0){
    audio_channels = 1;
  }else if((kAudioChannelLayoutTag_Stereo & (file_channel_layout.mChannelLayoutTag)) != 0 ||
           (kAudioChannelLayoutTag_StereoHeadphones & (file_channel_layout.mChannelLayoutTag)) != 0){
    audio_channels = 2;
  }else{
    fprintf(stdout, "AudioToolbox channel layout not supported\n");
  }
  
  format = FORMAT_FLOAT;

  if(file_data_format.mFormatFlags == kAudioFormatFlagIsFloat){
    format = FORMAT_FLOAT;
  }else if(file_data_format.mFormatFlags == kAudioFormatFlagIsSignedInteger){
    format = FORMAT_S32;
  }
  
  frame_count = 0;
    
  prop_size = sizeof(int64_t);
  ExtAudioFileGetProperty(ext_audio_file,
                          kExtAudioFileProperty_FileLengthFrames,
                          &prop_size, &file_length_frames);
  
  frame_count = file_length_frames;
  
  if(frame_count > 0){
    full_buffer = malloc((audio_channels * frame_count) * sizeof(float));
  }

  
  return(0);
}
