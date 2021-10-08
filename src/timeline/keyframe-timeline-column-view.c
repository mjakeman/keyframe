#include "keyframe-timeline-column-view.h"

#include "../model/keyframe-layers.h"
#include "keyframe-timeline-property.h"
#include "keyframe-timeline-channel.h"

typedef struct
{
    GtkListView *list_view;
} KeyframeTimelineColumnViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeTimelineColumnView, keyframe_timeline_column_view, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_MODEL,
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

    g_clear_pointer (&priv->list_view, gtk_widget_unparent);

    G_OBJECT_CLASS (keyframe_timeline_column_view_parent_class)->dispose (object);
}

static void
keyframe_timeline_column_view_get_property (GObject    *object,
                                            guint       prop_id,
                                            GValue     *value,
                                            GParamSpec *pspec)
{
    KeyframeTimelineColumnView *self = KEYFRAME_TIMELINE_COLUMN_VIEW (object);

    switch (prop_id)
      {
      case PROP_MODEL:
          return keyframe_timeline_column_view_get_model (self);
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_column_view_set_property (GObject      *object,
                                            guint         prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
    KeyframeTimelineColumnView *self = KEYFRAME_TIMELINE_COLUMN_VIEW (object);

    switch (prop_id)
      {
      case PROP_MODEL:
          GtkSelectionModel *model = g_value_get_object (value);
          keyframe_timeline_column_view_set_model (self, model);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
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

    properties [PROP_MODEL] = g_param_spec_object ("model", "Model", "Model", GTK_TYPE_SELECTION_MODEL, G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item,
                   gpointer            user_data)
{
    GtkWidget *channel = keyframe_timeline_channel_new ();
    gtk_box_set_spacing (GTK_BOX (channel), 5);

    GtkWidget *checkbox = gtk_check_button_new ();
    gtk_box_append (GTK_BOX (channel), checkbox);

    GtkWidget *expander = gtk_tree_expander_new ();
    GtkWidget *label = gtk_label_new ("");
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_tree_expander_set_child (GTK_TREE_EXPANDER (expander), label);
    gtk_box_append (GTK_BOX (channel), expander);

    gtk_list_item_set_child (list_item, channel);
}

static void
bind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item,
                  gpointer            user_data)
{
    GtkWidget *channel = gtk_list_item_get_child (list_item);
    GtkWidget *checkbox = gtk_widget_get_first_child (channel);
    // TODO: This is really ugly -> Create an API for KeyframeTimelineChannel
    GtkTreeExpander *expander = GTK_TREE_EXPANDER (gtk_widget_get_next_sibling (checkbox));
    GtkTreeListRow *row = GTK_TREE_LIST_ROW (gtk_list_item_get_item (list_item));
    gtk_tree_expander_set_list_row (expander, row);

    GtkWidget *label = gtk_tree_expander_get_child (expander);
    GObject *bind_obj = gtk_tree_list_row_get_item (row);


    if (KEYFRAME_IS_LAYER (bind_obj))
    {
        char *layer_prop;
        g_object_get (bind_obj,
                      "name", &layer_prop,
                      NULL);

        gtk_label_set_label (GTK_LABEL (label), layer_prop);
        g_object_bind_property (bind_obj, "visible", checkbox, "active",
                                G_BINDING_BIDIRECTIONAL|G_BINDING_SYNC_CREATE);
    }
    else if (KEYFRAME_IS_TIMELINE_PROPERTY (bind_obj))
    {
        GParamSpec *param;
        g_object_get (bind_obj,
                      "param-spec", &param,
                      NULL);

        gtk_label_set_label (GTK_LABEL (label), g_param_spec_get_nick (param));
        gtk_widget_set_tooltip_text (label, g_param_spec_get_blurb (param));
    }

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
    g_signal_connect (factory, "setup", setup_listitem_cb, self);
    g_signal_connect (factory, "bind", bind_listitem_cb, self);

    GtkWidget *scroll_area = gtk_scrolled_window_new ();
    gtk_widget_set_parent (scroll_area, GTK_WIDGET (self));
    gtk_widget_set_layout_manager (GTK_WIDGET (self), gtk_bin_layout_new ());

    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scroll_area), vbox);

    GtkWidget *list_view = gtk_list_view_new (NULL, factory);
    gtk_widget_add_css_class (list_view, "timeline");
    // gtk_list_view_set_show_separators (list_view, TRUE);
    gtk_widget_set_vexpand (list_view, TRUE);
    gtk_widget_set_hexpand (list_view, TRUE);
    gtk_box_append (GTK_BOX (vbox), list_view);

    priv->list_view = list_view;

    // TODO: Accessibility Support
    // Since we're using out own columnview-like implementation, accessibility
    // support needs to be added manually. Figure out what level of
    // accessibility is practical for an application of this type.

    // g_object_ (self, "model", list_view, "model", G_BINDING_DEFAULT);
}
