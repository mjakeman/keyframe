#include "keyframe-timeline.h"

#include "keyframe-composition-manager.h"
#include "keyframe-layer-properties-dialog.h"
#include "model/keyframe-composition.h"
#include "model/keyframe-layers.h"

#include "keyframe-timeline-menu.h"
#include "keyframe-timeline-column-view.h"

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

static KeyframeLayer *
get_current_layer (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return NULL;

    GtkSelectionModel *selection = gtk_column_view_get_model (priv->col_view);
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
    GListModel *model = create_layers_model_from_composition (priv->composition);

    GtkTreeListModel *treemodel = gtk_tree_list_model_new (model, FALSE, TRUE, create_layers_tree, NULL, NULL);
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

        GtkAdjustment *adjustment = gtk_range_get_adjustment (priv->slider);

        priv->composition = g_object_ref (new_comp);
        priv->comp_update_id = g_signal_connect (priv->composition, "changed", keyframe_timeline_composition_changed, self);
        priv->timeline_binding = g_object_bind_property (priv->composition, "current-time", adjustment, "value", G_BINDING_BIDIRECTIONAL);
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

    GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_layout_manager (GTK_WIDGET (self), gtk_bin_layout_new ());
    gtk_widget_set_parent (box, GTK_WIDGET (self));

    // Toolbar
    GtkWidget *menu = keyframe_timeline_menu_new ();
    gtk_box_append (GTK_BOX (box), menu);

    GtkWidget *colview = keyframe_timeline_column_view_new ();
    gtk_widget_set_vexpand (colview, TRUE);
    gtk_widget_set_hexpand (colview, TRUE);
    gtk_box_append (GTK_BOX (box), colview);
    priv->col_view = colview;

    /*// Vertical Layout
    GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand (vbox, TRUE);
    gtk_box_append (GTK_BOX (box), vbox);

    GtkWidget *header_paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    GtkWidget *contents_paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
    g_object_bind_property (header_paned, "position", contents_paned, "position", G_BINDING_BIDIRECTIONAL);
    gtk_paned_set_position (GTK_PANED (header_paned), 200);

    GtkWidget *contents_scroll_area = gtk_scrolled_window_new ();
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (contents_scroll_area), contents_paned);
    gtk_widget_set_vexpand (contents_scroll_area, TRUE);

    gtk_box_append (GTK_BOX (vbox), header_paned);
    gtk_box_append (GTK_BOX (vbox), contents_scroll_area);

    // HEADER
    GtkWidget *header_label = gtk_label_new ("Layers");
    gtk_paned_set_start_child (GTK_PANED (header_paned), header_label);

    priv->slider = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0, 1000.0f, 0.1f);
    gtk_paned_set_end_child (GTK_PANED (header_paned), priv->slider);

    // CONTENTS
    // Split between layer list and clip editor

    // Timeline
    priv->col_view = GTK_COLUMN_VIEW (gtk_column_view_new (NULL));
    // gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (contents_scroll_area), GTK_WIDGET (priv->col_view));
    gtk_paned_set_start_child (GTK_PANED (contents_paned), priv->col_view);
    gtk_paned_set_end_child (GTK_PANED (contents_paned), gtk_text_view_new ());
    // gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (contents_scroll_area), 200);

    gtk_column_view_set_reorderable (priv->col_view, false);
    gtk_column_view_set_show_column_separators (priv->col_view, true);
    g_signal_connect (priv->col_view, "activate", layer_activate_cb, self);

    GtkListItemFactory *name_factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (name_factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (name_factory, "bind", bind_listitem_cb, "name");

    GtkListItemFactory *type_factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (type_factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (type_factory, "bind", bind_listitem_cb, "type");

    GtkColumnViewColumn* col1 = gtk_column_view_column_new("Layer Name", name_factory);
    gtk_column_view_column_set_resizable (col1, true);
    gtk_column_view_append_column (priv->col_view, col1);
    gtk_column_view_column_set_expand (col1, true);

    GtkColumnViewColumn* col2 = gtk_column_view_column_new("Type", type_factory);
    gtk_column_view_column_set_resizable (col2, true);
    gtk_column_view_append_column (priv->col_view, col2);*/
}
