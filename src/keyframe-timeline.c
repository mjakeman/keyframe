#include "keyframe-timeline.h"

typedef struct
{
    // For dispose
    GtkWidget *sw;
} KeyframeTimelinePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeTimeline, keyframe_timeline, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_timeline_new:
 *
 * Create a new #KeyframeTimeline.
 *
 * Returns: (transfer full): a newly created #KeyframeTimeline
 */
KeyframeTimeline *
keyframe_timeline_new (void)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE, NULL);
}

static void
keyframe_timeline_finalize (GObject *object)
{
    KeyframeTimeline *self = (KeyframeTimeline *)object;
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    gtk_widget_unparent (priv->sw);

    G_OBJECT_CLASS (keyframe_timeline_parent_class)->finalize (object);
}

static void
keyframe_timeline_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
    KeyframeTimeline *self = KEYFRAME_TIMELINE (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    KeyframeTimeline *self = KEYFRAME_TIMELINE (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_class_init (KeyframeTimelineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_finalize;
    object_class->get_property = keyframe_timeline_get_property;
    object_class->set_property = keyframe_timeline_set_property;
}

static GListModel *
create_layers_model (gpointer layer_ptr, gpointer unused)
{
    if (layer_ptr == NULL)
    {
        GListModel *model = g_list_store_new (G_TYPE_OBJECT);
        KeyframeLayer *layer1 = keyframe_layer_new ("layer1");
        KeyframeLayer *layer2 = keyframe_layer_new ("layer2");
        KeyframeLayer *layer3 = keyframe_layer_new ("layer3");
        KeyframeLayer *layer4 = keyframe_layer_new ("layer4");
        g_list_store_append (model, layer1);
        g_list_store_append (model, layer2);
        g_list_store_append (model, layer3);
        g_list_store_append (model, layer4);

        return G_LIST_MODEL (model);
    }

    return NULL;
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item)
{
    GtkWidget *label = gtk_label_new ("");
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_list_item_set_child (list_item, label);
}

static void
bind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item)
{
    GtkWidget *label;
    KeyframeLayer *obj;
    label = gtk_list_item_get_child (list_item);
    obj = gtk_tree_list_row_get_item (gtk_list_item_get_item (list_item));

    char *layer_name;
    g_object_get (obj,
                  "name", &layer_name,
                  NULL);

    gtk_label_set_label (GTK_LABEL (label), layer_name);
}


static void
keyframe_timeline_init (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    priv->sw = gtk_scrolled_window_new ();
    GtkColumnView *col_view = GTK_COLUMN_VIEW (gtk_column_view_new (NULL));

    gtk_widget_set_layout_manager (GTK_WIDGET (self), gtk_bin_layout_new ());
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (priv->sw), GTK_WIDGET (col_view));
    gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (priv->sw), 200);
    gtk_widget_set_parent (priv->sw, GTK_WIDGET (self));

    gtk_column_view_set_reorderable (col_view, false);
    gtk_column_view_set_show_column_separators (col_view, true);

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (factory, "bind", bind_listitem_cb, NULL);

    GtkColumnViewColumn* col1 = gtk_column_view_column_new("Layer", factory);
    gtk_column_view_append_column (col_view, col1);

    GtkColumnViewColumn* col2 = gtk_column_view_column_new("Text", factory);
    gtk_column_view_append_column (col_view, col2);

    GtkColumnViewColumn* col3 = gtk_column_view_column_new("Graph", factory);
    gtk_column_view_append_column (col_view, col3);
    gtk_column_view_column_set_expand (col3, true);

    GListModel *model = create_layers_model (NULL, NULL);
    GtkTreeListModel *treemodel = gtk_tree_list_model_new (model,
                                       FALSE,
                                       TRUE,
                                       create_layers_model,
                                       NULL,
                                       NULL);

    GtkSingleSelection *selection = gtk_single_selection_new (G_LIST_MODEL (treemodel));

    gtk_column_view_set_model (GTK_COLUMN_VIEW (col_view), GTK_SELECTION_MODEL (selection));

}
