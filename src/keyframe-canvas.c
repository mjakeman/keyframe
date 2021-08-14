#include "keyframe-canvas.h"

#include "keyframe-layer.h"
#include "keyframe-layer-geometry.h"

#include "keyframe-renderer.h"

typedef struct
{
    cairo_surface_t *surface;
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

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_set_source_surface (cr, priv->surface, 0, 0);
    cairo_paint (cr);

    cairo_set_source_rgb (cr, 0.6, 0.6, 0.6);
    cairo_rectangle (cr, 0, 0, allocation.width, allocation.height);
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
keyframe_canvas_init (KeyframeCanvas *self)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (self);

    KeyframeLayer *layer1 = keyframe_layer_geometry_new ("layer1");
    KeyframeLayer *layer2 = keyframe_layer_geometry_new ("layer2");
    KeyframeLayer *layer3 = keyframe_layer_geometry_new ("layer3");
    KeyframeLayer *layer4 = keyframe_layer_geometry_new ("layer4");

    KeyframeRenderer *renderer = keyframe_renderer_new ();

    keyframe_renderer_begin_frame (renderer);
    keyframe_layer_fill_command_buffer (layer1, renderer);
    priv->surface = keyframe_renderer_end_frame (renderer);
}
