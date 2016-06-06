AgsApplicationContext *application_context;
AgsFile *file;

GError *error;

static const gchar *filename = "my_file.xml\0";

application_context = ags_audio_application_context_new();
    
file = g_object_new(AGS_TYPE_FILE,
                    "application-context\0", application_context,
                    "filename\0", filename,
                    NULL);
error = NULL;
ags_file_open(file,
              &error);

ags_file_read(file);
ags_file_close(file);

ags_thread_start(application_context->main_loop);
