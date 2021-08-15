#include "keyframe-timeline.h"

#include "keyframe-composition-manager.h"
#include "model/keyframe-composition.h"
#include "model/keyframe-layers.h"

typedef struct
{
    // For dispose
    GtkWidget *sw;

    GtkWidget *col_view;
    KeyframeCompositionManager *manager;
    KeyframeComposition *composition;

    ulong signal_id;
} KeyframeTimelinePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeTimeline, keyframe_timeline, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_MANAGER,
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
keyframe_timeline_new (KeyframeCompositionManager *manager)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE,
                         "manager", manager,
                         NULL);
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
keyframe_timeline_update_composition (KeyframeTimeline *self);

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
cb_current_changed (KeyframeCompositionManager *manager,
                    KeyframeTimeline           *self)
{
    keyframe_timeline_update_composition (self);
}

static void
keyframe_timeline_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    KeyframeTimeline *self = KEYFRAME_TIMELINE (object);
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    switch (prop_id)
      {
      case PROP_MANAGER:
          if (priv->manager)
              g_signal_handler_disconnect (priv->manager, priv->signal_id);
          priv->manager = g_value_get_object (value);
          priv->signal_id = g_signal_connect (priv->manager, "current-changed", cb_current_changed, self);
          keyframe_timeline_update_composition (self);
          break;
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

    properties [PROP_MANAGER] =
        g_param_spec_object ("manager", "Manager", "Manager",
                             KEYFRAME_TYPE_COMPOSITION_MANAGER,
                             G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static GListModel *
create_layers_model_from_composition (KeyframeComposition *composition)
{
    if (composition == NULL)
        return NULL;

    g_return_if_fail (KEYFRAME_IS_COMPOSITION (composition));

    GListModel *model = g_list_store_new (KEYFRAME_TYPE_LAYER);

    GSList *layers = keyframe_composition_get_layers (composition);
    for (GSList *l = layers; l != NULL; l = l->next)
    {
        KeyframeLayer *layer = l->data;
        g_assert (KEYFRAME_IS_LAYER (layer));

        g_list_store_append (model, layer);
    }

    return G_LIST_MODEL (model);
}

static GListModel *
create_layers_tree (gpointer layer_ptr, gpointer unused)
{
    // TODO: Add support for sublayers at some point
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
                  GtkListItem        *list_item,
                  const char         *prop)
{
    GtkWidget *label;
    KeyframeLayer *obj;
    label = gtk_list_item_get_child (list_item);
    obj = gtk_tree_list_row_get_item (gtk_list_item_get_item (list_item));

    char *layer_prop;
    g_object_get (obj,
                  prop, &layer_prop,
                  NULL);

    gtk_label_set_label (GTK_LABEL (label), layer_prop);
}

static void
keyframe_timeline_update_composition (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (priv->manager) {
        priv->composition = keyframe_composition_manager_get_current (priv->manager);
        g_print ("Updated!\n");
    }

    if (priv->composition == NULL)
    {
        gtk_column_view_set_model (GTK_COLUMN_VIEW (priv->col_view), NULL);
        return;
    }

    GListModel *model = create_layers_model_from_composition (priv->composition);

    GtkTreeListModel *treemodel = gtk_tree_list_model_new (model, FALSE, TRUE, create_layers_tree, NULL, NULL);
    GtkSingleSelection *selection = gtk_single_selection_new (G_LIST_MODEL (treemodel));

    gtk_column_view_set_model (GTK_COLUMN_VIEW (priv->col_view), GTK_SELECTION_MODEL (selection));
}

static void
keyframe_timeline_init (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    priv->sw = gtk_scrolled_window_new ();
    priv->col_view = GTK_COLUMN_VIEW (gtk_column_view_new (NULL));

    gtk_widget_set_layout_manager (GTK_WIDGET (self), gtk_bin_layout_new ());
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (priv->sw), GTK_WIDGET (priv->col_view));
    gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (priv->sw), 200);
    gtk_widget_set_parent (priv->sw, GTK_WIDGET (self));

    gtk_column_view_set_reorderable (priv->col_view, false);
    gtk_column_view_set_show_column_separators (priv->col_view, true);

    GtkListItemFactory *name_factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (name_factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (name_factory, "bind", bind_listitem_cb, "name");

    GtkListItemFactory *type_factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (type_factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (type_factory, "bind", bind_listitem_cb, "type");

    GtkColumnViewColumn* col1 = gtk_column_view_column_new("Layer Name", name_factory);
    gtk_column_view_column_set_resizable (col1, true);
    gtk_column_view_append_column (priv->col_view, col1);

    GtkColumnViewColumn* col2 = gtk_column_view_column_new("Type", type_factory);
    gtk_column_view_column_set_resizable (col2, true);
    gtk_column_view_append_column (priv->col_view, col2);

    GtkColumnViewColumn* col3 = gtk_column_view_column_new("Graph", name_factory);
    gtk_column_view_column_set_resizable (col3, true);
    gtk_column_view_append_column (priv->col_view, col3);
    gtk_column_view_column_set_expand (col3, true);
}
