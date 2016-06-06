AgsApplicationContext *application_context;
AgsThread *task_thread;

application_context = (AgsApplicationContext *) ags_audio_application_context_new();
task_thread = ags_concurrency_provider(AGS_CONCURRENCY_PROVIDER(application_context));
