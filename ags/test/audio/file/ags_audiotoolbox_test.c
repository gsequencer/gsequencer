#include <Foundation/Foundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <string.h>

int
main(int argc, char **argv)
{
  ExtAudioFileRef audio_file;
  AudioBufferList *audio_buffer_list;
  AudioStreamBasicDescription *stream;
  AudioStreamBasicDescription *client_stream;

  short *sbuffer;
  
  char *filename;

  int multi_frames;
  int retval;

  const int samplerate = 44100;
  const int audio_channels = 2;
  const int buffer_size = 2048;
  
  filename = "~/Music/test.mp4";
  
  NSString *audio_file_path = [[NSString stringWithUTF8String:filename]
			       stringByExpandingTildeInPath];

  NSURL *audio_url = [NSURL fileURLWithPath:audio_file_path];

  client_stream = (AudioStreamBasicDescription *) malloc(sizeof(AudioStreamBasicDescription));
  memset(client_stream, 0, sizeof(AudioStreamBasicDescription));

  client_stream->mSampleRate = samplerate;
  client_stream->mFormatID = kAudioFormatLinearPCM;
  client_stream->mFormatFlags = kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
  client_stream->mBitsPerChannel = 16;
  client_stream->mChannelsPerFrame = audio_channels;
  client_stream->mBytesPerFrame = client_stream->mChannelsPerFrame * 2;
  client_stream->mFramesPerPacket = 1;
  client_stream->mBytesPerPacket = client_stream->mFramesPerPacket * client_stream->mBytesPerFrame; 

  audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList));

  audio_buffer_list->mBuffers[0].mDataByteSize = audio_channels * buffer_size * sizeof(short);
  
  audio_buffer_list->mNumberBuffers = 1;
  audio_buffer_list->mBuffers[0].mNumberChannels = audio_channels;
  audio_buffer_list->mBuffers[0].mData = (short *) malloc(audio_channels * buffer_size * sizeof(short));

  stream = (AudioStreamBasicDescription *) malloc(sizeof(AudioStreamBasicDescription));
  memset(stream, 0, sizeof(AudioStreamBasicDescription));
    
  stream->mSampleRate = samplerate;
  stream->mFormatID = kAudioFormatMPEG4AAC;
  stream->mFormatFlags = kMPEG4Object_AAC_Main;
  stream->mChannelsPerFrame = audio_channels;
    
  retval = ExtAudioFileCreateWithURL((CFURLRef) audio_url,
				     kAudioFileM4AType,
				     stream,
				     NULL,
				     kAudioFileFlags_EraseFile,
				     &(audio_file));

  ExtAudioFileSetProperty(audio_file,
			  kExtAudioFileProperty_ClientDataFormat,
			  sizeof(AudioStreamBasicDescription),
			  client_stream);
  
  multi_frames = audio_channels * buffer_size;

  sbuffer = (short *) malloc(multi_frames * sizeof(short));
  memset(sbuffer, 0, multi_frames * sizeof(short));

  audio_buffer_list->mBuffers[0].mDataByteSize = multi_frames * sizeof(short);
  audio_buffer_list->mBuffers[0].mData = sbuffer;

  ExtAudioFileWrite(audio_file, multi_frames, audio_buffer_list);
  
  return(0);
}
