#include "keyframe-canvas.h"

#include "keyframe-layer.h"
#include "keyframe-layer-geometry.h"

#include "keyframe-renderer.h"

typedef struct
{
    float x;
    float y;
    float zoom;
    float resolution_factor;
    gboolean clip;
    cairo_surface_t *surface;

    GtkGesture *zoom_gesture;
    float old_zoom;
    double start_x, start_y;
    double old_x, old_y;
} KeyframeCanvasPrivate;

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (KeyframeCanvas, keyframe_canvas, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_canvas_new:
 *
 * Create a new #KeyframeCanvas.
 *
 * Returns: (transfer full): a newly created #KeyframeCanvas
 */
KeyframeCanvas *
keyframe_canvas_new (void)
{
    return g_object_new (KEYFRAME_TYPE_CANVAS, NULL);
}

static void
keyframe_canvas_finalize (GObject *object)
{
    KeyframeCanvas *self = (KeyframeCanvas *)object;
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_canvas_parent_class)->finalize (object);
}

static void
keyframe_canvas_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
    KeyframeCanvas *self = KEYFRAME_CANVAS (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_canvas_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
    KeyframeCanvas *self = KEYFRAME_CANVAS (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_canvas_snapshot (GtkWidget *widget, GtkSnapshot *snapshot)
{
    KeyframeCanvas *canvas = (KeyframeCanvas *) widget;
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (canvas);

    GtkAllocation allocation;
    cairo_t *cr;

    gtk_widget_get_allocation (widget, &allocation);
    cr = gtk_snapshot_append_cairo (snapshot,
                                    &GRAPHENE_RECT_INIT (
                                        0, 0,
			                            allocation.width,
			                            allocation.height
                                    ));

    int canvas_width = 1920 * priv->zoom;
    int canvas_height = 1080 * priv->zoom;

    int canvas_x = (allocation.width/2)-(canvas_width/2) + priv->x;
    int canvas_y = (allocation.height/2)-(canvas_height/2) + priv->y;

    cairo_rectangle (cr, canvas_x, canvas_y, canvas_width, canvas_height);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_fill (cr);

    // Whether the canvas should be clipped
    if (priv->clip)
        cairo_clip (cr);

    // Blit surface to canvas area
    {
        cairo_save (cr);

        // Work relative to canvas position
        cairo_translate (cr, canvas_x, canvas_y);

        // Scale image to real canvas size (since we might be rendering at partial resolution)
        float width_ratio = (float)canvas_width / (float)cairo_image_surface_get_width (priv->surface);
        float height_ratio = (float)canvas_height / (float)cairo_image_surface_get_height (priv->surface);
        cairo_scale (cr, width_ratio, height_ratio);

        // Do Painting
        cairo_set_source_surface (cr, priv->surface, 0, 0);
        cairo_paint (cr);

        cairo_restore (cr);
    }


    cairo_set_source_rgb (cr, 0.6, 0.6, 0.6);
    cairo_rectangle (cr, canvas_x, canvas_y, canvas_width, canvas_height);
    cairo_stroke (cr);

    cairo_destroy (cr);
}

static void
keyframe_canvas_class_init (KeyframeCanvasClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_canvas_finalize;
    object_class->get_property = keyframe_canvas_get_property;
    object_class->set_property = keyframe_canvas_set_property;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    widget_class->snapshot = keyframe_canvas_snapshot;
}

static void
cb_gesture_begin (GtkGesture       *gesture,
                  GdkEventSequence *sequence,
                  KeyframeCanvas   *canvas)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (canvas);

    gtk_gesture_get_bounding_box_center (gesture, &priv->start_x, &priv->start_y);

    priv->old_zoom = priv->zoom;
    priv->old_x = priv->x;
    priv->old_y = priv->y;
}

static void
cb_gesture_update (GtkGesture       *gesture,
                   GdkEventSequence *sequence,
                   KeyframeCanvas   *canvas)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (canvas);

    double x, y, scale;
    gtk_gesture_get_bounding_box_center (gesture, &x, &y);
    scale = gtk_gesture_zoom_get_scale_delta (gesture);

    double delta_x = x - priv->start_x;
    double delta_y = y - priv->start_y;

    priv->zoom = priv->old_zoom * scale;
    priv->x = priv->old_x + delta_x;
    priv->y = priv->old_y + delta_y;

    gtk_widget_queue_draw (GTK_WIDGET (canvas));
}

static void
cb_gesture_end (GtkGesture       *gesture,
                GdkEventSequence *sequence,
                KeyframeCanvas   *canvas)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (canvas);
    priv->old_zoom = 0;
    priv->old_x = 0;
    priv->old_y = 0;
    priv->start_x = 0;
    priv->start_y = 0;
}

static void
keyframe_canvas_init (KeyframeCanvas *self)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (self);

    KeyframeLayer *layer1 = keyframe_layer_geometry_new ("layer1");
    KeyframeLayer *layer2 = keyframe_layer_geometry_new ("layer2");
    KeyframeLayer *layer3 = keyframe_layer_geometry_new ("layer3");
    KeyframeLayer *layer4 = keyframe_layer_geometry_new ("layer4");

    int width = 1920;
    int height = 1080;

    priv->resolution_factor = 0.25;
    priv->zoom = 0.5f; //1.0f;
    priv->x = 0;
    priv->y = 0;
    priv->clip = false;

    KeyframeRenderer *renderer = keyframe_renderer_new ();

    keyframe_renderer_begin_frame (renderer,
                                   width * priv->resolution_factor,
                                   height * priv->resolution_factor);
    keyframe_layer_fill_command_buffer (layer1, renderer);
    priv->surface = keyframe_renderer_end_frame (renderer);


    // Setup Widget Signals
    priv->zoom_gesture = gtk_gesture_zoom_new ();
    g_signal_connect (priv->zoom_gesture, "begin", cb_gesture_begin, self);
    g_signal_connect (priv->zoom_gesture, "update", cb_gesture_update, self);
    g_signal_connect (priv->zoom_gesture, "end", cb_gesture_end, self);

    gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (priv->zoom_gesture),
                                                GTK_PHASE_BUBBLE);
    gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (priv->zoom_gesture));
}
