#include "keyframe-track-clip-private.h"

struct _KeyframeTrackClipWidget
{
    GtkWidget parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTrackClipWidget, keyframe_track_clip_widget, GTK_TYPE_WIDGET)

GtkWidget *
keyframe_track_clip_widget_new ()
{
    return GTK_WIDGET (g_object_new (KEYFRAME_TYPE_TRACK_CLIP_WIDGET, 0));
}

static void
keyframe_track_clip_widget_class_init (KeyframeTrackClipWidgetClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_css_name (widget_class, "clip");
    gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);
}

void
keyframe_track_clip_widget_set_active_cursor (KeyframeTrackClipWidget *self, gboolean pressed)
{
    gchar *cursor_name = pressed ? "grabbing" : "grab";
    GdkCursor *cursor = gdk_cursor_new_from_name (cursor_name, NULL);
    gtk_widget_set_cursor (GTK_WIDGET (self), cursor);
}

static void
keyframe_track_clip_widget_init (KeyframeTrackClipWidget *self)
{
    // TODO: Drag Handles
    keyframe_track_clip_widget_set_active_cursor (self, FALSE);
}
