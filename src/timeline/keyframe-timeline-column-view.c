#include "keyframe-timeline-column-view.h"

#include "../model/keyframe-layers.h"
#include "../model/keyframe-value-float.h"

#include "keyframe-timeline-property.h"
#include "keyframe-timeline-header.h"
#include "keyframe-timeline-channel.h"
#include "tracks/keyframe-tracks.h"

typedef struct
{
    GtkListView *list_view;
    GtkWidget *header;

    GtkAdjustment *hadjustment;
    float start_pos;
} KeyframeTimelineColumnViewPrivate;

static void
keyframe_timeline_column_view_scrollable_init (GtkScrollableInterface *iface);

G_DEFINE_TYPE_WITH_CODE (KeyframeTimelineColumnView, keyframe_timeline_column_view, GTK_TYPE_WIDGET,
                         G_ADD_PRIVATE (KeyframeTimelineColumnView)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_SCROLLABLE,
                                                keyframe_timeline_column_view_scrollable_init))

static gboolean
keyframe_timeline_column_view_scrollable_get_border (GtkScrollable *scrollable,
                                                     GtkBorder     *border)
{
    KeyframeTimelineColumnView *self = KEYFRAME_TIMELINE_COLUMN_VIEW (scrollable);
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    border->top = gtk_widget_get_height (priv->header);
    border->left = 400; // TODO: Don't hardcode

    return TRUE;
}

static void
keyframe_timeline_column_view_scrollable_init (GtkScrollableInterface *iface)
{
    iface->get_border = keyframe_timeline_column_view_scrollable_get_border;
}

enum {
    PROP_0,
    PROP_MODEL,
    PROP_START_POSITION,
    PROP_HADJUSTMENT,
    PROP_VADJUSTMENT,
    PROP_HSCROLL_POLICY,
    PROP_VSCROLL_POLICY,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_timeline_column_view_new:
 *
 * Create a new #KeyframeTimelineColumnView.
 *
 * Returns: (transfer full): a newly created #KeyframeTimelineColumnView as
 * a #GtkWidget
 */
GtkWidget *
keyframe_timeline_column_view_new (void)
{
    return GTK_WIDGET (g_object_new (KEYFRAME_TYPE_TIMELINE_COLUMN_VIEW, NULL));
}

static void
keyframe_timeline_column_view_finalize (GObject *object)
{
    KeyframeTimelineColumnView *self = (KeyframeTimelineColumnView *)object;
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    G_OBJECT_CLASS (keyframe_timeline_column_view_parent_class)->finalize (object);
}

static void
keyframe_timeline_column_view_dispose (GObject *object)
{
    KeyframeTimelineColumnView *self = (KeyframeTimelineColumnView *)object;
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    g_clear_pointer (&priv->hadjustment, g_object_unref);
    g_clear_pointer (&priv->list_view, gtk_widget_unparent);
    g_clear_pointer (&priv->header, gtk_widget_unparent);

    G_OBJECT_CLASS (keyframe_timeline_column_view_parent_class)->dispose (object);
}

static void
keyframe_timeline_column_view_get_property (GObject    *object,
                                            guint       prop_id,
                                            GValue     *value,
                                            GParamSpec *pspec)
{
    KeyframeTimelineColumnView *self = KEYFRAME_TIMELINE_COLUMN_VIEW (object);
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_MODEL:
            g_value_set_object (value, keyframe_timeline_column_view_get_model (self));
            break;
        case PROP_START_POSITION:
            // TODO: This might end up being too slow
            // Look at alternate means e.g. sharing an hadjustment
            g_value_set_float (value, priv->start_pos);
            break;
        case PROP_HADJUSTMENT:
            g_value_set_object (value, priv->hadjustment);
            break;
        case PROP_HSCROLL_POLICY:
            // TODO: Should this be never?
            g_value_set_enum (value, GTK_SCROLL_NATURAL);
            break;
        case PROP_VADJUSTMENT:
            GtkAdjustment *vadj = gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (priv->list_view));
            g_value_set_object (value, vadj);
            break;
        case PROP_VSCROLL_POLICY:
            GtkScrollablePolicy scroll_policy = gtk_scrollable_get_vscroll_policy (GTK_SCROLLABLE (priv->list_view));
            g_value_set_enum (value, scroll_policy);
            break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
cb_adjustment_changed (KeyframeTimelineColumnView *self,
                       GtkAdjustment              *adj);

static void
keyframe_timeline_column_view_set_property (GObject      *object,
                                            guint         prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
    KeyframeTimelineColumnView *self = KEYFRAME_TIMELINE_COLUMN_VIEW (object);
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    switch (prop_id)
    {
        case PROP_MODEL:
            GtkSelectionModel *model = g_value_get_object (value);
            keyframe_timeline_column_view_set_model (self, model);
            break;
        case PROP_HADJUSTMENT:
            priv->hadjustment = g_value_get_object (value);
            if (priv->hadjustment)
            {
                g_object_ref_sink (priv->hadjustment);

                // TODO: Update/Assign these values properly
                g_object_set (priv->hadjustment,
                              "lower", 0,
                              "upper", 5000,
                              "step-increment", 10,
                              "page-increment", 50,
                              "page-size", 50,
                              NULL);

                g_signal_connect_swapped (priv->hadjustment, "value-changed",
                                          G_CALLBACK (cb_adjustment_changed), self);
            }
            break;
        case PROP_HSCROLL_POLICY:
            // Do nothing
            break;
        case PROP_VADJUSTMENT:
            GtkAdjustment *vadj = g_value_get_object (value);
            gtk_scrollable_set_vadjustment (GTK_SCROLLABLE (priv->list_view), vadj);
            break;
        case PROP_VSCROLL_POLICY:
            GtkScrollablePolicy scroll_policy = g_value_get_enum (value);
            gtk_scrollable_set_vscroll_policy (GTK_SCROLLABLE (priv->list_view), scroll_policy);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_timeline_column_view_allocate (GtkWidget *widget,
                                        int        width,
                                        int        height,
                                        int        baseline)
{
    KeyframeTimelineColumnView *self = KEYFRAME_TIMELINE_COLUMN_VIEW (widget);
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    // Split allocation between header and listview widget
    int preferred_header_height;
    gtk_widget_measure (priv->header, GTK_ORIENTATION_VERTICAL, width,
                        NULL, &preferred_header_height, NULL, NULL);

    gtk_widget_size_allocate (priv->header,
                              &(const GtkAllocation){ 0, 0, width, preferred_header_height},
                              -1);

    gtk_widget_size_allocate (GTK_WIDGET (priv->list_view),
                              &(const GtkAllocation){ 0, preferred_header_height, width, height - preferred_header_height},
                              -1);

    // Update hadjustment
    if (priv->hadjustment)
    {
        gtk_adjustment_set_upper (priv->hadjustment, 5000);
        gtk_adjustment_set_step_increment (priv->hadjustment, 10);
        gtk_adjustment_set_page_increment (priv->hadjustment, 50);
        gtk_adjustment_set_page_size (priv->hadjustment, width);
    }
}

static void
keyframe_timeline_column_view_class_init (KeyframeTimelineColumnViewClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = keyframe_timeline_column_view_dispose;
    object_class->finalize = keyframe_timeline_column_view_finalize;
    object_class->get_property = keyframe_timeline_column_view_get_property;
    object_class->set_property = keyframe_timeline_column_view_set_property;

    gpointer iface = g_type_default_interface_peek (GTK_TYPE_SCROLLABLE);

    properties[PROP_VADJUSTMENT] =
        g_param_spec_override ("vadjustment",
                               g_object_interface_find_property (iface, "vadjustment"));

    properties[PROP_HADJUSTMENT] =
        g_param_spec_override ("hadjustment",
                               g_object_interface_find_property (iface, "hadjustment"));

    properties[PROP_VSCROLL_POLICY] =
        g_param_spec_override ("vscroll-policy",
                               g_object_interface_find_property (iface, "vscroll-policy"));

    properties[PROP_HSCROLL_POLICY] =
        g_param_spec_override ("hscroll-policy",
                               g_object_interface_find_property (iface, "hscroll-policy"));


    properties [PROP_MODEL] =
        g_param_spec_object ("model",
                             "Model",
                             "Model",
                             GTK_TYPE_SELECTION_MODEL,
                             G_PARAM_READWRITE);

    properties [PROP_START_POSITION] =
        g_param_spec_float ("start-position",
                            "Start Position",
                            "Start Position",
                            0, G_MAXFLOAT, 0,
                            G_PARAM_READABLE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    widget_class->size_allocate = keyframe_timeline_column_view_allocate;

    gtk_widget_class_set_css_name (widget_class, "timeline");
    // gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item,
                   gpointer            user_data)
{
    GtkWidget *channel = keyframe_timeline_channel_new ();
    GtkWidget *info_box = keyframe_timeline_channel_get_info_box (KEYFRAME_TIMELINE_CHANNEL (channel));

    GtkWidget *checkbox = gtk_check_button_new ();
    gtk_box_append (GTK_BOX (info_box), checkbox);

    GtkWidget *expander = gtk_tree_expander_new ();
    GtkWidget *label = gtk_label_new ("");
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_label_set_ellipsize (GTK_LABEL (label), PANGO_ELLIPSIZE_MIDDLE);
    gtk_widget_set_size_request (label, 200, -1);
    gtk_tree_expander_set_child (GTK_TREE_EXPANDER (expander), label);
    gtk_box_append (GTK_BOX (info_box), expander);

    gtk_list_item_set_child (list_item, channel);
}

static void
bind_listitem_cb (GtkListItemFactory         *factory,
                  GtkListItem                *list_item,
                  KeyframeTimelineColumnView *self)
{
    GtkWidget *channel = gtk_list_item_get_child (list_item);
    GtkTreeListRow *row = GTK_TREE_LIST_ROW (gtk_list_item_get_item (list_item));
    GObject *bind_obj = gtk_tree_list_row_get_item (row);

    // TODO: This is really ugly -> Create an API for KeyframeTimelineChannel
    GtkWidget *info_box = keyframe_timeline_channel_get_info_box (KEYFRAME_TIMELINE_CHANNEL (channel));
    GtkWidget *checkbox = gtk_widget_get_first_child (info_box);
    GtkTreeExpander *expander = GTK_TREE_EXPANDER (gtk_widget_get_next_sibling (checkbox));
    gtk_tree_expander_set_list_row (expander, row);

    GtkWidget *label = gtk_tree_expander_get_child (expander);

    GtkWidget *track;
    if (KEYFRAME_IS_LAYER (bind_obj))
    {
        g_object_bind_property (bind_obj, "name", label, "label",
                                G_BINDING_DEFAULT|G_BINDING_SYNC_CREATE);
        g_object_bind_property (bind_obj, "visible", checkbox, "active",
                                G_BINDING_BIDIRECTIONAL|G_BINDING_SYNC_CREATE);

        track = keyframe_track_clip_new ();
        g_object_bind_property (bind_obj, "start-time", track, "start-time",
                                G_BINDING_BIDIRECTIONAL|G_BINDING_SYNC_CREATE);
        g_object_bind_property (bind_obj, "end-time", track, "end-time",
                                G_BINDING_BIDIRECTIONAL|G_BINDING_SYNC_CREATE);

        char *layer_type;
        g_object_get (bind_obj,
                      "type", &layer_type,
                      NULL);

        gtk_widget_set_tooltip_text (label, layer_type);
        g_free (layer_type);
    }
    else if (KEYFRAME_IS_TIMELINE_PROPERTY (bind_obj))
    {
        KeyframeLayer *layer;

        GParamSpec *param;
        g_object_get (bind_obj,
                      "param-spec", &param,
                      "layer", &layer,
                      NULL);

        gtk_label_set_label (GTK_LABEL (label), g_param_spec_get_nick (param));
        gtk_widget_set_tooltip_text (label, g_param_spec_get_blurb (param));

        // Setup Track
        if (param->value_type == KEYFRAME_TYPE_VALUE_FLOAT)
        {
            track = keyframe_track_frame_new ();
            g_object_bind_property (layer, g_param_spec_get_name (param),
                                    track, "float-value",
                                    G_BINDING_BIDIRECTIONAL|G_BINDING_SYNC_CREATE);
        }
        else
        {
            track = keyframe_track_new ();
        }
    }

    g_object_bind_property (self, "hadjustment", track, "adjustment",
                                G_BINDING_DEFAULT|G_BINDING_SYNC_CREATE);
    keyframe_timeline_channel_set_track (KEYFRAME_TIMELINE_CHANNEL (channel),
                                         KEYFRAME_TRACK (track));

    int depth = gtk_tree_list_row_get_depth (row);
    gtk_widget_remove_css_class (GTK_WIDGET (channel), "channel-depth-0");
    gtk_widget_remove_css_class (GTK_WIDGET (channel), "channel-depth-1");
    gtk_widget_remove_css_class (GTK_WIDGET (channel), "channel-depth-2");
    gtk_widget_remove_css_class (GTK_WIDGET (channel), "channel-depth-3");

    char *style_class = g_strdup_printf ("channel-depth-%d", depth);
    gtk_widget_add_css_class (GTK_WIDGET (channel), style_class);
    g_free (style_class);
}

void
keyframe_timeline_column_view_set_model (KeyframeTimelineColumnView *self,
                                         GtkSelectionModel *model)
{
    g_return_if_fail (GTK_IS_SELECTION_MODEL (model));

    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);
    gtk_list_view_set_model (priv->list_view, model);

    g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_MODEL]);
}


static void
cb_adjustment_changed (KeyframeTimelineColumnView *self,
                       GtkAdjustment              *adj)
{
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);
    // TODO: Maybe centre position instead of start?
    priv->start_pos = gtk_adjustment_get_value (adj);
    g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_START_POSITION]);
    g_print ("%f\n", priv->start_pos);
}

GtkSelectionModel *
keyframe_timeline_column_view_get_model (KeyframeTimelineColumnView *self)
{
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);
    return gtk_list_view_get_model (priv->list_view);
}

static void
keyframe_timeline_column_view_init (KeyframeTimelineColumnView *self)
{
    KeyframeTimelineColumnViewPrivate *priv = keyframe_timeline_column_view_get_instance_private (self);

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (factory, "setup", G_CALLBACK (setup_listitem_cb), self);
    g_signal_connect (factory, "bind", G_CALLBACK (bind_listitem_cb), self);

    GtkWidget *header = keyframe_timeline_header_new ();
    gtk_widget_set_parent (header, GTK_WIDGET (self));
    priv->header = header;

    GtkWidget *list_view = gtk_list_view_new (NULL, factory);
    gtk_widget_set_vexpand (list_view, TRUE);
    gtk_widget_set_hexpand (list_view, TRUE);
    gtk_widget_set_parent (list_view, GTK_WIDGET (self));
    priv->list_view = GTK_LIST_VIEW (list_view);

    // TODO: Accessibility Support
    // Since we're using out own columnview-like implementation, accessibility
    // support needs to be added manually. Figure out what level of
    // accessibility is practical for an application of this type.
}
