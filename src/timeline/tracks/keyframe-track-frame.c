#include "keyframe-track-frame.h"

#include "../../model/keyframe-value-float.h"

struct _KeyframeTrackFrame
{
    KeyframeTrack parent_instance;

    KeyframeValueFloat *float_value;

    gboolean drag_active;
    float drag_start_x;
    float drag_current_x;

    float start_position;
};

G_DEFINE_FINAL_TYPE (KeyframeTrackFrame, keyframe_track_frame, KEYFRAME_TYPE_TRACK)

enum {
    PROP_0,
    PROP_FLOAT_VALUE,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];



// START LAYOUT MANAGER


#define KEYFRAME_TYPE_TRACK_FRAME_LAYOUT (keyframe_track_frame_layout_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackFrameLayout, keyframe_track_frame_layout, KEYFRAME, TRACK_FRAME_LAYOUT, GtkLayoutManager)

struct _KeyframeTrackFrameLayout
{
    GtkLayoutManager parent_instance;
};

G_DEFINE_FINAL_TYPE (KeyframeTrackFrameLayout, keyframe_track_frame_layout, GTK_TYPE_LAYOUT_MANAGER)

static void
keyframe_track_frame_layout_measure (GtkLayoutManager *layout_manager,
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
keyframe_track_frame_layout_allocate (GtkLayoutManager *layout_manager,
                                               GtkWidget        *widget,
                                               int               width,
                                               int               height,
                                               int               baseline)
{
    KeyframeTrackFrame *frame_track = KEYFRAME_TRACK_FRAME (widget);

    g_print ("Allocating Keyframe Track\n");
}

static GtkSizeRequestMode
keyframe_track_frame_layout_get_request_mode (GtkLayoutManager *layout_manager,
                                                       GtkWidget        *widget)
{
    return GTK_SIZE_REQUEST_CONSTANT_SIZE;
}

static void
keyframe_track_frame_layout_class_init (KeyframeTrackFrameLayoutClass *klass)
{
    GtkLayoutManagerClass *layout_manager_class = GTK_LAYOUT_MANAGER_CLASS (klass);

    layout_manager_class->measure = keyframe_track_frame_layout_measure;
    layout_manager_class->allocate = keyframe_track_frame_layout_allocate;
    layout_manager_class->get_request_mode = keyframe_track_frame_layout_get_request_mode;
}

static void
keyframe_track_frame_layout_init (KeyframeTrackFrameLayout *self)
{

}


// END LAYOUT MANAGER




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
            gtk_widget_queue_allocate (GTK_WIDGET (self));
            gtk_widget_queue_draw (GTK_WIDGET (self));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_track_frame_adjustment_changed (KeyframeTrack *self,
                                                  GtkAdjustment         *adj)
{
    g_print ("Adjustment Changed?\n");
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
test_snapshot (GtkWidget *widget, GtkSnapshot *snapshot)
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);

    KeyframeTrackFrame *self = KEYFRAME_TRACK_FRAME (widget);

    if (!self->float_value)
        return;

    // TODO: Check static before obtaining keyframes
    // There should be some sort of check inside `get_keyframes()` so we don't crash
    GSList *keyframes = keyframe_value_float_get_keyframes (self->float_value);

    int dim = gtk_widget_get_height (widget);

    for (GSList *l = keyframes; l != NULL; l = l->next)
    {
        KeyframeValueFloatPair *frame = l->data;

        float x = frame->timestamp - self->start_position;
        float y = 0;
        float w = dim, h = dim;

        g_print ("Drawing Keyframe\n");
        GdkRGBA colour;
        gdk_rgba_parse (&colour, "cyan");
        gtk_snapshot_append_color (snapshot, &colour, &GRAPHENE_RECT_INIT (x, y, w, h));
    }
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

    widget_class->snapshot = test_snapshot;

    gtk_widget_class_set_layout_manager_type (widget_class, KEYFRAME_TYPE_TRACK_FRAME_LAYOUT);
}

static void
cb_drag_begin (GtkGestureDrag            *gesture,
               gdouble                    start_x,
               gdouble                    start_y,
               KeyframeTrackFrame *self)
{
    // TODO: Scaling/Zoom
    self->drag_start_x = start_x;   // self->clip_timestamp_start;
    self->drag_current_x = start_x; // self->clip_timestamp_start;
    self->drag_active = true;
}

static void
cb_drag_update (GtkGestureDrag            *gesture,
                gdouble                    offset_x,
                gdouble                    offset_y,
                KeyframeTrackFrame *self)
{
    self->drag_current_x = self->drag_start_x + offset_x;
    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
cb_drag_end (GtkGestureDrag            *gesture,
             gdouble                    offset_x,
             gdouble                    offset_y,
             KeyframeTrackFrame *self)
{
    GdkEventSequence *sequence = gtk_gesture_single_get_current_sequence (GTK_GESTURE_SINGLE (gesture));
    if (gtk_gesture_get_sequence_state (GTK_GESTURE (gesture), sequence) == GTK_EVENT_SEQUENCE_DENIED)
        return;

    self->drag_active = FALSE;
    self->drag_start_x = 0;
    self->drag_current_x = 0;

    gtk_widget_queue_allocate (GTK_WIDGET (self));
}

static void
keyframe_track_frame_init (KeyframeTrackFrame *self)
{
    GtkGesture *drag_gesture = gtk_gesture_drag_new ();
    gtk_widget_add_controller (self, GTK_EVENT_CONTROLLER (drag_gesture));

    g_signal_connect (drag_gesture, "drag-begin", G_CALLBACK (cb_drag_begin), self);
    g_signal_connect (drag_gesture, "drag-update", G_CALLBACK (cb_drag_update), self);
    g_signal_connect (drag_gesture, "drag-end", G_CALLBACK (cb_drag_end), self);
}
