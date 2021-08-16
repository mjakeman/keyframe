#include "keyframe-canvas.h"

#include "model/keyframe-layers.h"
#include "keyframe-renderer.h"

typedef struct
{
    KeyframeComposition *composition;

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

#include <math.h>

#define MAX_ZOOM 3
#define MIN_ZOOM 0.25
#define CLAMP(x, lower, upper) (MIN(upper, MAX(x, lower)))

G_DEFINE_FINAL_TYPE_WITH_PRIVATE (KeyframeCanvas, keyframe_canvas, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_COMPOSITION,
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
GtkWidget *
keyframe_canvas_new (KeyframeComposition *composition)
{
    return GTK_WIDGET (g_object_new (KEYFRAME_TYPE_CANVAS,
                                     "composition", composition,
                                     NULL));
}

static void
keyframe_canvas_finalize (GObject *object)
{
    KeyframeCanvas *self = (KeyframeCanvas *)object;
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (self);

    g_object_unref (priv->composition);

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
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (self);

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

    int comp_width, comp_height;
    g_object_get (priv->composition, "width", &comp_width, "height", &comp_height, NULL);

    int canvas_width = comp_width * priv->zoom;
    int canvas_height = comp_height * priv->zoom;

    int canvas_x = (allocation.width/2)-(canvas_width/2) + priv->x;
    int canvas_y = (allocation.height/2)-(canvas_height/2) + priv->y;

    cairo_rectangle (cr, canvas_x, canvas_y, canvas_width, canvas_height);
    cairo_clip (cr);

    // Draw checkerboard background (TODO: Save?)
    for (int i = 0; i < allocation.width/10; i++)
    {
        for (int j = 0; j < allocation.height/10; j++)
        {
            if ((i % 2 || j % 2) && !(i % 2 && j % 2))
                cairo_set_source_rgb (cr, 0.75, 0.75, 0.75);
            else
                cairo_set_source_rgb (cr, 1, 1, 1);

            cairo_rectangle (cr, i*10, j*10, 10, 10);
            cairo_fill (cr);
        }
    }

    // Whether the canvas should be clipped
    if (!priv->clip)
        cairo_reset_clip (cr);

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
    scale = gtk_gesture_zoom_get_scale_delta (GTK_GESTURE_ZOOM (gesture));

    double delta_x = x - priv->start_x;
    double delta_y = y - priv->start_y;

    // TODO: CLAMP based on widget dimensions rather than source dimensions
    // This becomes a problem with large image sizes (e.g. 4K, etc)
    priv->zoom = CLAMP (priv->old_zoom * scale, MIN_ZOOM, MAX_ZOOM);
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
keyframe_canvas_constructed (GObject *self)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (KEYFRAME_CANVAS (self));
    g_assert (priv->composition != NULL);

    // Render Frame
    KeyframeRenderer *renderer = keyframe_renderer_new ();

    {
        int width, height;
        g_object_get (priv->composition, "width", &width, "height", &height, NULL);

        keyframe_renderer_begin_frame (renderer,
                                   width * priv->resolution_factor,
                                   height * priv->resolution_factor);

        GSList *layers = keyframe_composition_get_layers (priv->composition);
        for (GSList *l = layers; l != NULL; l = l->next)
        {
            KeyframeLayer *layer = l->data;
            g_assert (KEYFRAME_IS_LAYER (layer));

            keyframe_layer_fill_command_buffer (layer, renderer);
        }

        priv->surface = keyframe_renderer_end_frame (renderer);
    }

    /* Always chain up to the parent constructed function to complete object
    * initialisation. */
    G_OBJECT_CLASS (keyframe_canvas_parent_class)->constructed (self);
}

static void
keyframe_canvas_class_init (KeyframeCanvasClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_canvas_finalize;
    object_class->get_property = keyframe_canvas_get_property;
    object_class->set_property = keyframe_canvas_set_property;
    object_class->constructed = keyframe_canvas_constructed;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    widget_class->snapshot = keyframe_canvas_snapshot;

    properties [PROP_COMPOSITION] =
        g_param_spec_object ("composition", "Composition", "Composition",
                             KEYFRAME_TYPE_COMPOSITION, G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_canvas_init (KeyframeCanvas *self)
{
    KeyframeCanvasPrivate *priv = keyframe_canvas_get_instance_private (self);

    // Setup Widget Signals
    priv->zoom_gesture = gtk_gesture_zoom_new ();
    g_signal_connect (priv->zoom_gesture, "begin", G_CALLBACK (cb_gesture_begin), self);
    g_signal_connect (priv->zoom_gesture, "update", G_CALLBACK (cb_gesture_update), self);
    g_signal_connect (priv->zoom_gesture, "end", G_CALLBACK (cb_gesture_end), self);

    gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (priv->zoom_gesture),
                                                GTK_PHASE_BUBBLE);
    gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (priv->zoom_gesture));

    // Set Canvas Properties
    // TODO: Only apply resolution factor to bitmap layers? (e.g. exclude text)
    priv->resolution_factor = 1; //0.25;
    priv->zoom = 0.5f; //1.0f;
    priv->x = 0;
    priv->y = 0;
    priv->clip = false;

    // For debugging missing fonts
    // GtkWidget *dlg = gtk_font_chooser_dialog_new ("Fonts", NULL);
    // gtk_window_present (GTK_WINDOW (dlg));
}
