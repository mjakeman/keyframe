#include "keyframe-timeline-track-clip.h"

struct _KeyframeTimelineTrackClip
{
    KeyframeTimelineTrack parent_instance;

    GtkWidget *clip;
    float drag_start_x;
};

G_DEFINE_FINAL_TYPE (KeyframeTimelineTrackClip, keyframe_timeline_track_clip, KEYFRAME_TYPE_TIMELINE_TRACK)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

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

    gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_FIXED_LAYOUT);
}

static void
cb_drag_begin (GtkGestureDrag            *gesture,
               gdouble                    start_x,
               gdouble                    start_y,
               KeyframeTimelineTrackClip *self)
{
    GtkLayoutManager *fixed = gtk_widget_get_layout_manager (GTK_WIDGET (self));
    GtkFixedLayoutChild *fixed_child = GTK_FIXED_LAYOUT_CHILD (gtk_layout_manager_get_layout_child (fixed, self->clip));

    GtkAllocation alloc;
    gtk_widget_get_allocation (self->clip, &alloc);
    self->drag_start_x = alloc.x;

    g_print ("Drag Start!\n");
}

static void
cb_drag_update (GtkGestureDrag            *gesture,
                gdouble                    offset_x,
                gdouble                    offset_y,
                KeyframeTimelineTrackClip *self)
{


    g_print ("Drag Update! %f\n", offset_x);
}

static void
cb_drag_end (GtkGestureDrag            *gesture,
             gdouble                    offset_x,
             gdouble                    offset_y,
             KeyframeTimelineTrackClip *self)
{
    GtkLayoutManager *fixed = gtk_widget_get_layout_manager (GTK_WIDGET (self));
    GtkFixedLayoutChild *fixed_child = GTK_FIXED_LAYOUT_CHILD (gtk_layout_manager_get_layout_child (fixed, self->clip));

    GskTransform *transform = gsk_transform_new ();
    transform = gsk_transform_translate (transform, &GRAPHENE_POINT_INIT (self->drag_start_x + offset_x, 0));
    gtk_fixed_layout_child_set_transform (fixed_child, transform);

    g_print ("Drag End!\n");
}

static void
keyframe_timeline_track_clip_init (KeyframeTimelineTrackClip *self)
{
    self->clip = gtk_label_new ("Boo!");
    gtk_widget_set_parent (self->clip, GTK_WIDGET (self));
    gtk_widget_add_css_class (self->clip, "clip");

    GtkGesture *drag_gesture = gtk_gesture_drag_new ();
    gtk_widget_add_controller (self->clip, GTK_EVENT_CONTROLLER (drag_gesture));

    g_signal_connect (drag_gesture, "drag-begin", G_CALLBACK (cb_drag_begin), self);
    g_signal_connect (drag_gesture, "drag-update", G_CALLBACK (cb_drag_update), self);
    g_signal_connect (drag_gesture, "drag-end", G_CALLBACK (cb_drag_end), self);

    GtkLayoutManager *fixed = gtk_widget_get_layout_manager (GTK_WIDGET (self));
    GtkFixedLayoutChild *fixed_child = GTK_FIXED_LAYOUT_CHILD (gtk_layout_manager_get_layout_child (fixed, self->clip));

    GskTransform *transform = gsk_transform_new ();
    transform = gsk_transform_translate (transform, &GRAPHENE_POINT_INIT (50, 0));
    gtk_fixed_layout_child_set_transform (fixed_child, transform);
}
