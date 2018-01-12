{
  guint tics_per_second;

  tics_per_second = (guint) ceil((AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE /
				  AGS_SOUNDCARD_DEFAULT_SAMPLERATE) *
				 AGS_THREAD_MAX_PRECISION);
}
