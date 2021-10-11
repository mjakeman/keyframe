#include "keyframe-timeline-track-clip.h"

struct _KeyframeTimelineTrackClip
{
    KeyframeTimelineTrack parent_instance;

    GtkWidget *clip;
    gboolean drag_active;
    float drag_start_x;
    float drag_current_x;

    // TODO: Make properties?
    float clip_timestamp_start;
    float clip_timestamp_end;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineTrackClip, keyframe_timeline_track_clip, KEYFRAME_TYPE_TIMELINE_TRACK)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];



// START CLIP WIDGET


#define KEYFRAME_TYPE_TIMELINE_CLIP (keyframe_timeline_clip_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineClip, keyframe_timeline_clip, KEYFRAME, TIMELINE_CLIP, GtkWidget)

struct _KeyframeTimelineClip
{
    GtkWidget parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineClip, keyframe_timeline_clip, GTK_TYPE_WIDGET)

static GtkWidget *
keyframe_timeline_clip_new ()
{
    return GTK_WIDGET (g_object_new (KEYFRAME_TYPE_TIMELINE_CLIP, 0));
}

static void
keyframe_timeline_clip_class_init (KeyframeTimelineClipClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_css_name (widget_class, "clip");
    gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);
}

static void
keyframe_timeline_clip_set_active_cursor (KeyframeTimelineClip *self, gboolean pressed)
{
    gchar *cursor_name = pressed ? "grabbing" : "grab";
    GdkCursor *cursor = gdk_cursor_new_from_name (cursor_name, NULL);
    gtk_widget_set_cursor (GTK_WIDGET (self), cursor);
}

static void
keyframe_timeline_clip_init (KeyframeTimelineClip *self)
{
    // TODO: Drag Handles
    keyframe_timeline_clip_set_active_cursor (self, FALSE);
}


// END CLIP WIDGET

// START LAYOUT MANAGER


#define KEYFRAME_TYPE_TIMELINE_TRACK_CLIP_LAYOUT (keyframe_timeline_track_clip_layout_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineTrackClipLayout, keyframe_timeline_track_clip_layout, KEYFRAME, TIMELINE_TRACK_CLIP_LAYOUT, GtkLayoutManager)

struct _KeyframeTimelineTrackClipLayout
{
    GtkLayoutManager parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineTrackClipLayout, keyframe_timeline_track_clip_layout, GTK_TYPE_LAYOUT_MANAGER)

static void
keyframe_timeline_track_clip_layout_measure (GtkLayoutManager *layout_manager,
                                             GtkWidget        *widget,
                                             GtkOrientation    orientation,
                                             int               for_size,
                                             int              *minimum,
                                             int              *natural,
                                             int              *minimum_baseline,
                                             int              *natural_baseline)
{
    *minimum = 0;
    *natural = 0;
}

static void
keyframe_timeline_track_clip_layout_allocate (GtkLayoutManager *layout_manager,
                                              GtkWidget        *widget,
                                              int               width,
                                              int               height,
                                              int               baseline)
{
    KeyframeTimelineTrackClip *clip_track = KEYFRAME_TIMELINE_TRACK_CLIP (widget);

    if (!gtk_widget_should_layout (clip_track->clip))
        return;

    float clip_width = clip_track->clip_timestamp_end - clip_track->clip_timestamp_start;

    if (clip_track->drag_active)
    {
        gtk_widget_size_allocate (clip_track->clip,
                              &(const GtkAllocation){ clip_track->drag_current_x, 0, clip_width, height },
                              -1);
    }
    else
    {
        g_print ("%f %f\n", clip_track->clip_timestamp_start, clip_track->clip_timestamp_end);
        gtk_widget_size_allocate (clip_track->clip,
                              &(const GtkAllocation){ clip_track->clip_timestamp_start, 0, clip_width, height },
                              -1);
    }
}

static GtkSizeRequestMode
keyframe_timeline_track_clip_layout_get_request_mode (GtkLayoutManager *layout_manager,
                                                      GtkWidget        *widget)
{
    return GTK_SIZE_REQUEST_CONSTANT_SIZE;
}

static void
keyframe_timeline_track_clip_layout_class_init (KeyframeTimelineTrackClipLayoutClass *klass)
{
    GtkLayoutManagerClass *layout_manager_class = GTK_LAYOUT_MANAGER_CLASS (klass);

    layout_manager_class->measure = keyframe_timeline_track_clip_layout_measure;
    layout_manager_class->allocate = keyframe_timeline_track_clip_layout_allocate;
    layout_manager_class->get_request_mode = keyframe_timeline_track_clip_layout_get_request_mode;
}

static void
keyframe_timeline_track_clip_layout_init (KeyframeTimelineTrackClipLayout *self)
{

}


// END LAYOUT MANAGER




GtkWidget *
keyframe_timeline_track_clip_new (void)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE_TRACK_CLIP, NULL);
}

static void
keyframe_timeline_track_clip_finalize (GObject *object)
{
    KeyframeTimelineTrackClip *self = (KeyframeTimelineTrackClip *)object;

    gtk_widget_unparent (self->clip);

    G_OBJECT_CLASS (keyframe_timeline_track_clip_parent_class)->finalize (object);
}

static void
keyframe_timeline_track_clip_get_property (GObject    *object,
                                           guint       prop_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
    KeyframeTimelineTrackClip *self = KEYFRAME_TIMELINE_TRACK_CLIP (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_track_clip_set_property (GObject      *object,
                                           guint         prop_id,
                                           const GValue *value,
                                           GParamSpec   *pspec)
{
    KeyframeTimelineTrackClip *self = KEYFRAME_TIMELINE_TRACK_CLIP (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_track_clip_class_init (KeyframeTimelineTrackClipClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_track_clip_finalize;
    object_class->get_property = keyframe_timeline_track_clip_get_property;
    object_class->set_property = keyframe_timeline_track_clip_set_property;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_layout_manager_type (widget_class, KEYFRAME_TYPE_TIMELINE_TRACK_CLIP_LAYOUT);
}

static void
cb_drag_begin (GtkGestureDrag            *gesture,
               gdouble                    start_x,
               gdouble                    start_y,
               KeyframeTimelineTrackClip *self)
{
    double test_x = 0;
    double test_y = 0;
    gtk_widget_translate_coordinates (GTK_WIDGET (self),
                                      self->clip,
                                      start_x, start_y,
                                      &test_x, &test_y);

    // Only accept the gesture if the clip is being dragged
    if (!gtk_widget_contains (self->clip, test_x, test_y))
    {
        GdkEventSequence *sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
        gtk_gesture_set_sequence_state (GTK_GESTURE (gesture), sequence, GTK_EVENT_SEQUENCE_DENIED);
        return;
    }

    // TODO: Scaling/Zoom
    self->drag_start_x = self->clip_timestamp_start;
    self->drag_current_x = self->clip_timestamp_start;
    self->drag_active = true;

    keyframe_timeline_clip_set_active_cursor (KEYFRAME_TIMELINE_CLIP (self->clip), TRUE);
}

static void
cb_drag_update (GtkGestureDrag            *gesture,
                gdouble                    offset_x,
                gdouble                    offset_y,
                KeyframeTimelineTrackClip *self)
{
    self->drag_current_x = self->drag_start_x + offset_x;
    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
cb_drag_end (GtkGestureDrag            *gesture,
             gdouble                    offset_x,
             gdouble                    offset_y,
             KeyframeTimelineTrackClip *self)
{
    GdkEventSequence *sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
    if (gtk_gesture_get_sequence_state (GTK_GESTURE (gesture), sequence) == GTK_EVENT_SEQUENCE_DENIED)
        return;

    float clip_width = self->clip_timestamp_end - self->clip_timestamp_start;
    self->clip_timestamp_start = self->drag_current_x;
    self->clip_timestamp_end = self->drag_current_x + clip_width;

    self->drag_active = FALSE;
    self->drag_start_x = 0;
    self->drag_current_x = 0;

    keyframe_timeline_clip_set_active_cursor (KEYFRAME_TIMELINE_CLIP (self->clip), FALSE);

    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
keyframe_timeline_track_clip_init (KeyframeTimelineTrackClip *self)
{
    self->clip = keyframe_timeline_clip_new ();
    gtk_widget_set_parent (self->clip, GTK_WIDGET (self));

    // TODO: Scaling/Zoom
    self->clip_timestamp_start = 50;
    self->clip_timestamp_end = 150;

    GtkGesture *drag_gesture = gtk_gesture_drag_new ();
    gtk_widget_add_controller (self, GTK_EVENT_CONTROLLER (drag_gesture));

    g_signal_connect (drag_gesture, "drag-begin", G_CALLBACK (cb_drag_begin), self);
    g_signal_connect (drag_gesture, "drag-update", G_CALLBACK (cb_drag_update), self);
    g_signal_connect (drag_gesture, "drag-end", G_CALLBACK (cb_drag_end), self);
}
