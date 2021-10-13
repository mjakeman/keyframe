#include "keyframe-track-clip.h"

G_DEFINE_FINAL_TYPE (KeyframeTrackClip, keyframe_track_clip, KEYFRAME_TYPE_TRACK)

enum {
    PROP_0,
    PROP_START_TIME,
    PROP_END_TIME,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
keyframe_track_clip_new (void)
{
    return g_object_new (KEYFRAME_TYPE_TRACK_CLIP, NULL);
}

static void
keyframe_track_clip_finalize (GObject *object)
{
    KeyframeTrackClip *self = (KeyframeTrackClip *)object;

    gtk_widget_unparent (self->clip);

    G_OBJECT_CLASS (keyframe_track_clip_parent_class)->finalize (object);
}

static void
keyframe_track_clip_get_property (GObject    *object,
                                           guint       prop_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
    KeyframeTrackClip *self = KEYFRAME_TRACK_CLIP (object);

    switch (prop_id)
    {
        case PROP_START_TIME:
            g_value_set_float (value, self->clip_timestamp_start);
            break;
        case PROP_END_TIME:
            g_value_set_float (value, self->clip_timestamp_end);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_track_clip_set_property (GObject      *object,
                                           guint         prop_id,
                                           const GValue *value,
                                           GParamSpec   *pspec)
{
    KeyframeTrackClip *self = KEYFRAME_TRACK_CLIP (object);

    switch (prop_id)
    {
        case PROP_START_TIME:
            self->clip_timestamp_start = g_value_get_float (value);
            break;
        case PROP_END_TIME:
            self->clip_timestamp_end = g_value_get_float (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_track_clip_adjustment_changed (KeyframeTrack *self,
                                                 GtkAdjustment         *adj)
{
    if (!GTK_IS_WIDGET (self))
    {
        g_critical ("TrackClip is invalid.");
        return;
    }
    KeyframeTrackClip *clip_track = KEYFRAME_TRACK_CLIP (self);
    clip_track->start_position = gtk_adjustment_get_value (adj);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
keyframe_track_clip_class_init (KeyframeTrackClipClass *klass)
{
    KeyframeTrackClass *track_class = KEYFRAME_TRACK_CLASS (klass);

    track_class->adjustment_changed = keyframe_track_clip_adjustment_changed;

    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_track_clip_finalize;
    object_class->get_property = keyframe_track_clip_get_property;
    object_class->set_property = keyframe_track_clip_set_property;

    properties [PROP_START_TIME] =
        g_param_spec_float ("start-time",
                            "Start Time",
                            "Start Time",
                            0, G_MAXFLOAT, 0,
                            G_PARAM_READWRITE);

    properties [PROP_END_TIME] =
        g_param_spec_float ("end-time",
                            "End Time",
                            "End Time",
                            0, G_MAXFLOAT, 0,
                            G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_layout_manager_type (widget_class, KEYFRAME_TYPE_TRACK_CLIP_LAYOUT);
}

static void
cb_drag_begin (GtkGestureDrag            *gesture,
               gdouble                    start_x,
               gdouble                    start_y,
               KeyframeTrackClip *self)
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

    keyframe_track_clip_widget_set_active_cursor (KEYFRAME_TRACK_CLIP_WIDGET (self->clip), TRUE);
}

static void
cb_drag_update (GtkGestureDrag            *gesture,
                gdouble                    offset_x,
                gdouble                    offset_y,
                KeyframeTrackClip *self)
{
    self->drag_current_x = self->drag_start_x + offset_x;
    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
cb_drag_end (GtkGestureDrag            *gesture,
             gdouble                    offset_x,
             gdouble                    offset_y,
             KeyframeTrackClip *self)
{
    GdkEventSequence *sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
    if (gtk_gesture_get_sequence_state (GTK_GESTURE (gesture), sequence) == GTK_EVENT_SEQUENCE_DENIED)
        return;

    // Update Properties
    float clip_width = self->clip_timestamp_end - self->clip_timestamp_start;
    self->clip_timestamp_start = self->drag_current_x;
    self->clip_timestamp_end = self->drag_current_x + clip_width;

    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_START_TIME]);
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_END_TIME]);

    self->drag_active = FALSE;
    self->drag_start_x = 0;
    self->drag_current_x = 0;

    keyframe_track_clip_widget_set_active_cursor (KEYFRAME_TRACK_CLIP_WIDGET (self->clip), FALSE);

    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
keyframe_track_clip_init (KeyframeTrackClip *self)
{
    self->clip = keyframe_track_clip_widget_new ();
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
