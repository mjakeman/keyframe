#include "keyframe-export-dialog.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

typedef struct
{
    KeyframeComposition *composition;
    KeyframeRenderer *renderer;
} KeyframeExportDialogPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeExportDialog, keyframe_export_dialog, GTK_TYPE_DIALOG)

enum {
    PROP_0,
    PROP_COMPOSITION,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_export_dialog_new:
 *
 * Create a new #KeyframeExportDialog.
 *
 * Returns: (transfer full): a newly created #KeyframeExportDialog
 */
GtkWidget *
keyframe_export_dialog_new (KeyframeComposition *composition)
{
    return g_object_new (KEYFRAME_TYPE_EXPORT_DIALOG,
                         "composition", composition,
                         NULL);
}

static void
keyframe_export_dialog_finalize (GObject *object)
{
    KeyframeExportDialog *self = (KeyframeExportDialog *)object;
    KeyframeExportDialogPrivate *priv = keyframe_export_dialog_get_instance_private (self);

    g_object_unref (priv->composition);

    G_OBJECT_CLASS (keyframe_export_dialog_parent_class)->finalize (object);
}

static void
keyframe_export_dialog_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
    KeyframeExportDialog *self = KEYFRAME_EXPORT_DIALOG (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_export_dialog_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
    KeyframeExportDialog *self = KEYFRAME_EXPORT_DIALOG (object);
    KeyframeExportDialogPrivate *priv = keyframe_export_dialog_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_COMPOSITION:
            priv->composition = g_object_ref (g_value_get_object (value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_export_dialog_class_init (KeyframeExportDialogClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_export_dialog_finalize;
    object_class->get_property = keyframe_export_dialog_get_property;
    object_class->set_property = keyframe_export_dialog_set_property;

    properties [PROP_COMPOSITION] =
        g_param_spec_object ("composition", "Composition", "Composition",
                             KEYFRAME_TYPE_COMPOSITION, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void prepare_buffer(GstAppSrc* appsrc, KeyframeComposition *composition) {
    static int frames = 0;
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;

    if (frames++ > 10) {
        gst_app_src_end_of_stream (appsrc);
        return;
    }

    int width, height;

    g_object_get (composition,
                  "width", &width,
                  "height", &height,
                  NULL);

    size = width * height * 4;

    {
        KeyframeRenderer *renderer = keyframe_renderer_new ();

        int width, height;
        g_object_get (composition, "width", &width, "height", &height, NULL);

        keyframe_renderer_begin_frame (renderer,
                                   width,
                                   height);

        GSList *layers = keyframe_composition_get_layers (composition);
        for (GSList *l = layers; l != NULL; l = l->next)
        {
            KeyframeLayer *layer = l->data;
            g_assert (KEYFRAME_IS_LAYER (layer));

            keyframe_layer_fill_command_buffer (layer, renderer);
        }

        cairo_surface_t *surface = keyframe_renderer_end_frame (renderer);

        unsigned char *data = cairo_image_surface_get_data (surface);
        buffer = gst_buffer_new_wrapped_full(0, (gpointer)data, size, 0, size, NULL, NULL);

        cairo_surface_destroy (surface);
        g_object_unref (renderer);
    }

    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 4);

    timestamp += GST_BUFFER_DURATION (buffer);

    ret = gst_app_src_push_buffer(appsrc, buffer);

    if (ret != GST_FLOW_OK) {
        /* something wrong, stop pushing */
        // g_main_loop_quit (loop);
        g_print ("Oops!\n");
    }
}

static void cb_need_data (GstElement *appsrc, guint unused_size, KeyframeComposition *composition) {
    g_print ("Need data!\n");
    prepare_buffer((GstAppSrc*)appsrc, composition);
}

static void
cb_render (GtkWidget *btn, KeyframeExportDialog *self)
{
    KeyframeExportDialogPrivate *priv = keyframe_export_dialog_get_instance_private (self);

    char *title;
    int width, height;

    g_object_get (priv->composition,
                  "title", &title,
                  "width", &width,
                  "height", &height,
                  NULL);

    g_print ("Rendering %s\n", title);

    GstElement *pipeline, *appsrc, *conv, *enc, *mux, *videosink;

    // setup pipeline
    pipeline = gst_pipeline_new ("pipeline");
    appsrc = gst_element_factory_make ("appsrc", "source");
    conv = gst_element_factory_make ("videoconvert", "conv");
    // enc = gst_element_factory_make ("x264enc", "enc");
    // mux = gst_element_factory_make ("matroskamux", "mux");
    videosink = gst_element_factory_make ("autovideosink", "videosink");
    // videosink = gst_element_factory_make ("filesink", "videosink");

    if (!appsrc || !conv || !enc || !mux || !videosink) {
        g_print ("Something went wrong!\n");
        return;
    }

    // setup
    g_object_set (G_OBJECT (appsrc), "caps",
	    gst_caps_new_simple ("video/x-raw",
			         "format", G_TYPE_STRING, "BGRA",
			         "width", G_TYPE_INT, width,
			         "height", G_TYPE_INT, height,
			         "framerate", GST_TYPE_FRACTION, 0, 1,
			         NULL), NULL);
    // g_object_set (G_OBJECT (videosink), "location", "test.mkv", NULL);
    gst_bin_add_many (GST_BIN (pipeline), appsrc, conv, /*enc, mux,*/ videosink, NULL);
    gst_element_link_many (appsrc, conv, /*enc, mux,*/ videosink, NULL);

    // setup appsrc
    g_object_set (G_OBJECT (appsrc),
	    "stream-type", 0, // GST_APP_STREAM_TYPE_STREAM
	    "format", GST_FORMAT_TIME,
        // "is-live", TRUE,
        NULL);

    g_signal_connect (appsrc, "need-data", G_CALLBACK (cb_need_data), priv->composition);

    // pipeline
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    GstBus *bus = gst_element_get_bus (pipeline);
    GstMessage *msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    if (msg != NULL) {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error (msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n",
                GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n",
                debug_info ? debug_info : "none");
            g_clear_error (&err);
            g_free (debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print ("End-Of-Stream reached.\n");
            break;
        default:
            /* We should not reach here because we only asked for ERRORs and EOS */
            g_printerr ("Unexpected message received.\n");
            break;
        }
        gst_message_unref (msg);
    }

    /* clean up */
    // gst_element_set_state (pipeline, GST_STATE_NULL);
    // gst_object_unref (GST_OBJECT (pipeline));

    g_print ("Finished %s\n", title);
}

static void
keyframe_export_dialog_init (KeyframeExportDialog *self)
{
    GtkWidget *box = gtk_dialog_get_content_area (GTK_DIALOG (self));

    GtkWidget *btn = gtk_button_new_with_label ("Render");
    g_signal_connect (btn, "clicked", cb_render, self);
    gtk_box_append (GTK_BOX (box), btn);
}
