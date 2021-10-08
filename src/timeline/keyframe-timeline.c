#include "keyframe-timeline.h"

#include "keyframe-composition-manager.h"
#include "keyframe-layer-properties-dialog.h"
#include "model/keyframe-composition.h"
#include "model/keyframe-layers.h"

#include "keyframe-timeline-menu.h"
#include "keyframe-timeline-column-view.h"
#include "keyframe-timeline-property.h"

typedef struct
{
    // For dispose
    // GtkWidget *sw;

    GtkWidget *slider;
    GtkWidget *col_view;
    KeyframeCompositionManager *manager;
    KeyframeComposition *composition;

    ulong signal_id;
    ulong comp_update_id;
    GBinding *timeline_binding;
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

    // gtk_widget_unparent (priv->sw);

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

static GListModel *
create_layers_tree (gpointer layer_ptr, gpointer unused)
{
    // For now, only support a depth of one level (i.e. must
    // be immediate child of a layer)
    if (!KEYFRAME_IS_LAYER (layer_ptr))
        return NULL;

    KeyframeLayer *layer = KEYFRAME_LAYER (layer_ptr);

    guint n_props;
    GParamSpec **params = g_object_class_list_properties (G_OBJECT_GET_CLASS (layer), &n_props);

    GListModel *model = G_LIST_MODEL (g_list_store_new (KEYFRAME_TYPE_TIMELINE_PROPERTY));

    for (guint i = 0; i < n_props; i++)
    {
        KeyframeTimelineProperty *prop = keyframe_timeline_property_new (params[i]);
        g_list_store_append (G_LIST_STORE (model), prop);
    }

    // TODO: Add support for sublayers at some point
    return model;
}

static KeyframeLayer *
get_current_layer (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return NULL;

    GtkSelectionModel *selection = keyframe_timeline_column_view_get_model (priv->col_view);
    GtkTreeListRow *tree_row = GTK_TREE_LIST_ROW (gtk_single_selection_get_selected_item (GTK_SINGLE_SELECTION (selection)));
    KeyframeLayer *layer = KEYFRAME_LAYER (gtk_tree_list_row_get_item (tree_row));

    if (!(KEYFRAME_IS_LAYER (layer)))
        return NULL;

    return layer;
}

static void
layer_activate_cb (GtkColumnView    *col_view,
                   guint             position,
                   KeyframeTimeline *self)
{
    gtk_widget_activate_action (GTK_WIDGET (self), "layer.edit", NULL);
}


static void
keyframe_timeline_composition_changed (KeyframeComposition *composition,
                                       KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);
    g_assert (composition == priv->composition);

    // Update List model
    GListModel *model = G_LIST_MODEL (composition);

    GtkTreeListModel *treemodel = gtk_tree_list_model_new (model, FALSE, FALSE, create_layers_tree, NULL, NULL);
    GtkSingleSelection *selection = gtk_single_selection_new (G_LIST_MODEL (treemodel));

    // This currently gets regenerated every time we move the playhead
    // We should differentiate between 'frame-change' and 'invalidate'
    keyframe_timeline_column_view_set_model (priv->col_view, GTK_SELECTION_MODEL (selection));
}

static void
keyframe_timeline_update_composition (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (priv->manager) {
        KeyframeComposition *new_comp = keyframe_composition_manager_get_current (priv->manager);

        // No change
        if (new_comp == priv->composition)
            return;

        if (priv->composition)
        {
            g_signal_handler_disconnect (priv->composition, priv->comp_update_id);
            g_object_unref (priv->composition);
            g_object_unref (priv->timeline_binding);
        }

        // GtkAdjustment *adjustment = gtk_range_get_adjustment (priv->slider);

        priv->composition = g_object_ref (new_comp);
        // priv->comp_update_id = g_signal_connect (priv->composition, "changed", keyframe_timeline_composition_changed, self);
        // priv->timeline_binding = g_object_ (priv->composition, "current-time", adjustment, "value", G_BINDING_BIDIRECTIONAL);
    }

    if (priv->composition == NULL)
    {
        gtk_column_view_set_model (GTK_COLUMN_VIEW (priv->col_view), NULL);
        return;
    }

    // Regenerate List Model
    keyframe_timeline_composition_changed (priv->composition, self);
}

static void
new_layer_action (KeyframeTimeline *self,
                  const char       *action_name,
                  GVariant         *param)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return;

    KeyframeLayer *layer;

    GType typeid = g_variant_get_uint64 (param);

    if (typeid == KEYFRAME_TYPE_LAYER_TEXT)
    {
        layer = g_object_new (KEYFRAME_TYPE_LAYER_TEXT,
                              "name", "Untitled Layer",
                              "markup", "Hello World",
                              NULL);
    }
    else if (typeid == KEYFRAME_TYPE_LAYER_COOL)
    {
        layer = g_object_new (KEYFRAME_TYPE_LAYER_COOL,
                              "name", "Untitled Layer",
                              NULL);
    }
    else if (typeid == KEYFRAME_TYPE_LAYER_GEOMETRY)
    {
        int w, h;
        g_object_get (priv->composition, "width", &w, "height", &h, NULL);

        layer = g_object_new (KEYFRAME_TYPE_LAYER_GEOMETRY,
                              "name", "Untitled Layer",
                              "width", (float)w,
                              "height", (float)h,
                              NULL);
    }
    else
    {
        g_warning ("Cannot create layer of unrecognised typeid: %llu\n", typeid);
        return;
    }

    keyframe_composition_push_layer (priv->composition, layer);
}

static void
delete_layer_action (KeyframeTimeline *self,
                     const char       *action_name,
                     GVariant         *param)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);
    KeyframeLayer *layer = get_current_layer (self);

    if (!layer)
        return;

    keyframe_composition_delete_layer (priv->composition, layer);

    // self->composition ... delete?
    // layer should probably have a reference to composition
    //   -> e.g. composition owns layers and is responsible for freeing them
}

static void
edit_layer_action (KeyframeTimeline *self,
                   const char       *action_name,
                   GVariant         *param)
{
    KeyframeLayer *layer = get_current_layer (self);

    if (!layer)
        return;

    GtkWidget *dlg = keyframe_layer_properties_dialog_new (layer);
    gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (gtk_widget_get_root (self)));
    // gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);
    g_signal_connect_swapped (dlg, "response", keyframe_composition_invalidate, priv->composition);

    gtk_widget_show (dlg);
}

static void
keyframe_timeline_class_init (KeyframeTimelineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_finalize;
    object_class->get_property = keyframe_timeline_get_property;
    object_class->set_property = keyframe_timeline_set_property;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_install_action (widget_class, "layer.new", NULL, new_layer_action);
    gtk_widget_class_install_action (widget_class, "layer.edit", NULL, edit_layer_action);
    gtk_widget_class_install_action (widget_class, "layer.delete", NULL, delete_layer_action);

    properties [PROP_MANAGER] =
        g_param_spec_object ("manager", "Manager", "Manager",
                             KEYFRAME_TYPE_COMPOSITION_MANAGER,
                             G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
keyframe_timeline_init (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    GtkLayoutManager *layout = gtk_box_layout_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_layout_manager (GTK_WIDGET (self), layout);

    // Toolbar
    GtkWidget *menu = keyframe_timeline_menu_new ();
    gtk_widget_set_parent (menu, GTK_WIDGET (self));

    // Data Table + Time Track
    GtkWidget *colview = keyframe_timeline_column_view_new ();
    gtk_widget_set_vexpand (colview, TRUE);
    gtk_widget_set_hexpand (colview, TRUE);
    gtk_widget_set_parent (colview, GTK_WIDGET (self));
    priv->col_view = colview;
}
