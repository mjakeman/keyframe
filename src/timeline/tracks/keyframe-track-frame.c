#include "keyframe-track-frame-private.h"

#include "../../keyframe-control-point-dialog.h"

G_DEFINE_FINAL_TYPE (KeyframeTrackFrame, keyframe_track_frame, KEYFRAME_TYPE_TRACK)

enum {
    PROP_0,
    PROP_FLOAT_VALUE,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
keyframe_track_frame_new (void)
{
    return g_object_new (KEYFRAME_TYPE_TRACK_FRAME, NULL);
}

static void
keyframe_track_frame_finalize (GObject *object)
{
    KeyframeTrackFrame *self = (KeyframeTrackFrame *)object;

    if (self->float_value)
        g_boxed_free (KEYFRAME_TYPE_VALUE_FLOAT, self->float_value);

    if (self->control_points)
        g_slist_free_full (self->control_points, (GDestroyNotify)gtk_widget_unparent);

    G_OBJECT_CLASS (keyframe_track_frame_parent_class)->finalize (object);
}

static void
keyframe_track_frame_get_property (GObject    *object,
                                           guint       prop_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
    KeyframeTrackFrame *self = KEYFRAME_TRACK_FRAME (object);

    switch (prop_id)
    {
    case PROP_FLOAT_VALUE:
        g_value_set_boxed (value, self->float_value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
recreate_keyframes (KeyframeTrackFrame *self)
{
    g_slist_free_full (g_steal_pointer (&self->control_points),
                       (GDestroyNotify)gtk_widget_unparent);

    // TODO: Check static before obtaining keyframes
    // There should be some sort of check inside `get_keyframes()` so we don't crash
    GSList *keyframes = keyframe_value_float_get_keyframes (self->float_value);

    for (GSList *l = keyframes; l != NULL; l = l->next)
    {
        KeyframeValueFloatPair *frame = l->data;

        GtkWidget *point_widget = keyframe_track_frame_point_new (frame->timestamp, frame->value);
        gtk_widget_set_parent (point_widget, GTK_WIDGET (self));

        self->control_points = g_slist_append (self->control_points, point_widget);
    }

    gtk_widget_queue_allocate (GTK_WIDGET (self));
    gtk_widget_queue_draw (GTK_WIDGET (self));
}

static void
keyframe_track_frame_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    KeyframeTrackFrame *self = KEYFRAME_TRACK_FRAME (object);

    switch (prop_id)
    {
    case PROP_FLOAT_VALUE:
        if (self->float_value)
            g_boxed_free (KEYFRAME_TYPE_VALUE_FLOAT, self->float_value);

        // TODO: Rename to something like `value_float_ref()`
        self->float_value = g_boxed_copy (KEYFRAME_TYPE_VALUE_FLOAT, g_value_get_boxed (value));

        // Setup Keyframes
        if (self->float_value)
            recreate_keyframes (self);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_track_frame_adjustment_changed (KeyframeTrack *self,
                                                  GtkAdjustment         *adj)
{
    if (!GTK_IS_WIDGET (self))
    {
        g_critical ("TrackFrame is invalid.");
        return;
    }
    KeyframeTrackFrame *frame_track = KEYFRAME_TRACK_FRAME (self);
    frame_track->start_position = gtk_adjustment_get_value (adj);
    gtk_widget_queue_allocate (GTK_WIDGET (self));
    gtk_widget_queue_draw (GTK_WIDGET (self));
}

static void
keyframe_track_frame_class_init (KeyframeTrackFrameClass *klass)
{
    KeyframeTrackClass *track_class = KEYFRAME_TRACK_CLASS (klass);

    track_class->adjustment_changed = keyframe_track_frame_adjustment_changed;

    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_track_frame_finalize;
    object_class->get_property = keyframe_track_frame_get_property;
    object_class->set_property = keyframe_track_frame_set_property;

    properties [PROP_FLOAT_VALUE]
        = g_param_spec_boxed ("float-value",
                              "Float Value",
                              "Float Value",
                              KEYFRAME_TYPE_VALUE_FLOAT,
                              G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_layout_manager_type (widget_class, KEYFRAME_TYPE_TRACK_FRAME_LAYOUT);
}

static void
cb_drag_begin (GtkGestureDrag     *gesture,
               gdouble             start_x,
               gdouble             start_y,
               KeyframeTrackFrame *self)
{
    GtkWidget *drag_target = gtk_widget_pick (GTK_WIDGET (self), start_x, start_y, GTK_PICK_DEFAULT);

    if (!KEYFRAME_IS_TRACK_FRAME_POINT (drag_target) || self->inhibit_drag)
    {
        gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_DENIED);
        return;
    }

    g_print ("Begin Drag\n");

    float timestamp;
    g_object_get (drag_target, "timestamp", &timestamp, NULL);

    // TODO: Scaling/Zoom
    self->drag_start_x = timestamp;
    self->drag_current_x = timestamp;
    self->drag_active = true;
    self->drag_widget = drag_target;
}

static void
cb_drag_update (GtkGestureDrag            *gesture,
                gdouble                    offset_x,
                gdouble                    offset_y,
                KeyframeTrackFrame *self)
{
    g_print ("Update Drag\n");

    self->drag_current_x = self->drag_start_x + offset_x;
    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
cb_drag_end (GtkGestureDrag     *gesture,
             gdouble             offset_x,
             gdouble             offset_y,
             KeyframeTrackFrame *self)
{
    GdkEventSequence *sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
    if (gtk_gesture_get_sequence_state (GTK_GESTURE (gesture), sequence) == GTK_EVENT_SEQUENCE_DENIED)
        return;

    g_print ("End Drag\n");

    float timestamp, value;
    g_object_get (self->drag_widget,
                  "timestamp", &timestamp,
                  "value", &value,
                  NULL);

    // TODO: Create a move_keyframe () method inside FloatValue
    keyframe_value_float_delete_keyframe (self->float_value, timestamp);
    keyframe_value_float_push_keyframe (self->float_value, self->drag_current_x, value);

    self->drag_active = FALSE;
    self->drag_start_x = 0;
    self->drag_current_x = 0;
    self->drag_widget = NULL;

    // TODO: Recycle rather than recreate - This is way too expensive
    recreate_keyframes (self);
}

static void
cb_edit_keyframe_response (KeyframeControlPointDialog *dlg,
                           gint                        response_id,
                           KeyframeTrackFrame         *self)
{
    g_print ("Edit Keyframe\n");

    if (response_id == GTK_RESPONSE_OK)
    {
        float old_time, new_time, new_value;
        g_object_get (dlg,
                      "old-timestamp", &old_time,
                      "timestamp", &new_time,
                      "value", &new_value,
                      NULL);
        keyframe_value_float_delete_keyframe (self->float_value, old_time);
        keyframe_value_float_push_keyframe (self->float_value, new_time, new_value);

        recreate_keyframes (self);
    }

    gtk_window_destroy (GTK_WINDOW (dlg));
    self->inhibit_drag = FALSE;
}

static void
cb_pressed (GtkGestureClick    *gesture,
            gint                n_press,
            gdouble             x,
            gdouble             y,
            KeyframeTrackFrame *self)
{
    // Only consider double clicks
    if (n_press < 2)
        return;

    // Prefer pressed gesture to drag (if both run, then the drag operates on
    // keyframes which no longer exist, causing a crash).
    // TODO: There must surely be a better way of doing this
    self->inhibit_drag = TRUE;

    GtkWidget *target = gtk_widget_pick (GTK_WIDGET (self), x, y, GTK_PICK_DEFAULT);

    if (KEYFRAME_IS_TRACK_FRAME_POINT (target))
    {
        KeyframeTrackFramePoint *point = KEYFRAME_TRACK_FRAME_POINT (target);

        float timestamp, value;
        g_object_get (point,
                      "timestamp", &timestamp,
                      "value", &value,
                      NULL);

        GtkWidget *dlg = keyframe_control_point_dialog_new (timestamp, value);
        GtkWindow *parent = GTK_WINDOW (gtk_widget_get_root (GTK_WIDGET (self)));
        gtk_window_set_transient_for (GTK_WINDOW (dlg), parent);
        gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);
        g_signal_connect (dlg, "response", G_CALLBACK (cb_edit_keyframe_response), self);
        gtk_window_present (GTK_WINDOW (dlg));
    }
    else
    {
        float timestamp = self->start_position + x;
        keyframe_value_float_push_keyframe (self->float_value, timestamp, 100);

        // TODO: Recycle rather than recreate - This is way too expensive
        recreate_keyframes (self);
        self->inhibit_drag = FALSE;
    }
}

static void
keyframe_track_frame_init (KeyframeTrackFrame *self)
{
    GtkGesture *drag_gesture = gtk_gesture_drag_new ();
    gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (drag_gesture));
    gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (drag_gesture), GTK_PHASE_BUBBLE);

    g_signal_connect (drag_gesture, "drag-begin", G_CALLBACK (cb_drag_begin), self);
    g_signal_connect (drag_gesture, "drag-update", G_CALLBACK (cb_drag_update), self);
    g_signal_connect (drag_gesture, "drag-end", G_CALLBACK (cb_drag_end), self);

    GtkGesture *pressed_gesture = gtk_gesture_click_new ();
    gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (pressed_gesture));
    gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (pressed_gesture), GTK_PHASE_BUBBLE);

    g_signal_connect (pressed_gesture, "pressed", G_CALLBACK (cb_pressed), self);
}
