/*  $Id$
 *
 *  Copyright (C) 2024 Kevin Stensberg <kstensberg@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <gtk/gtk.h>
#include <libxfce4util/libxfce4util.h>
#include <libxfce4panel/libxfce4panel.h>

#include "mediaremote.h"
#include "mediaremote-dialogs.h"

/* default settings */
#define DEFAULT_SETTING1 NULL
#define DEFAULT_SETTING2 1
#define DEFAULT_SETTING3 FALSE



/* prototypes */
static void
mediaremote_construct (XfcePanelPlugin *plugin);


/* register the plugin */
XFCE_PANEL_PLUGIN_REGISTER (mediaremote_construct);



void
mediaremote_save (XfcePanelPlugin *plugin,
             MediaremotePlugin    *mediaremote)
{
  XfceRc *rc;
  gchar  *file;

  /* get the config file location */
  file = xfce_panel_plugin_save_location (plugin, TRUE);

  if (G_UNLIKELY (file == NULL))
    {
       DBG ("Failed to open config file");
       return;
    }

  /* open the config file, read/write */
  rc = xfce_rc_simple_open (file, FALSE);
  g_free (file);

  if (G_LIKELY (rc != NULL))
    {
      /* save the settings */
      DBG(".");
      if (mediaremote->setting1)
        xfce_rc_write_entry    (rc, "setting1", mediaremote->setting1);

      xfce_rc_write_int_entry  (rc, "setting2", mediaremote->setting2);
      xfce_rc_write_bool_entry (rc, "setting3", mediaremote->setting3);

      /* close the rc file */
      xfce_rc_close (rc);
    }
}



static void
mediaremote_read (MediaremotePlugin *mediaremote)
{
  XfceRc      *rc;
  gchar       *file;
  const gchar *value;

  /* get the plugin config file location */
  file = xfce_panel_plugin_save_location (mediaremote->plugin, TRUE);

  if (G_LIKELY (file != NULL))
    {
      /* open the config file, readonly */
      rc = xfce_rc_simple_open (file, TRUE);

      /* cleanup */
      g_free (file);

      if (G_LIKELY (rc != NULL))
        {
          /* read the settings */
          value = xfce_rc_read_entry (rc, "setting1", DEFAULT_SETTING1);
          mediaremote->setting1 = g_strdup (value);

          mediaremote->setting2 = xfce_rc_read_int_entry (rc, "setting2", DEFAULT_SETTING2);
          mediaremote->setting3 = xfce_rc_read_bool_entry (rc, "setting3", DEFAULT_SETTING3);

          /* cleanup */
          xfce_rc_close (rc);

          /* leave the function, everything went well */
          return;
        }
    }

  /* something went wrong, apply default values */
  DBG ("Applying default settings");

  mediaremote->setting1 = g_strdup (DEFAULT_SETTING1);
  mediaremote->setting2 = DEFAULT_SETTING2;
  mediaremote->setting3 = DEFAULT_SETTING3;
}



static MediaremotePlugin *
mediaremote_new (XfcePanelPlugin *plugin)
{
  MediaremotePlugin   *mediaremote;
  GtkOrientation  orientation;
  GtkWidget      *label;

  /* allocate memory for the plugin structure */
  mediaremote = g_slice_new0 (MediaremotePlugin);

  /* pointer to plugin */
  mediaremote->plugin = plugin;

  /* read the user settings */
  mediaremote_read (mediaremote);

  /* get the current orientation */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* create some panel widgets */
  mediaremote->ebox = gtk_event_box_new ();
  gtk_widget_show (mediaremote->ebox);

  mediaremote->hvbox = gtk_box_new (orientation, 2);
  gtk_widget_show (mediaremote->hvbox);
  gtk_container_add (GTK_CONTAINER (mediaremote->ebox), mediaremote->hvbox);

  /* some mediaremote widgets */
  label = gtk_label_new (_("Mediaremote"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (mediaremote->hvbox), label, FALSE, FALSE, 0);

  label = gtk_label_new (_("Plugin"));
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (mediaremote->hvbox), label, FALSE, FALSE, 0);

  return mediaremote;
}



static void
mediaremote_free (XfcePanelPlugin *plugin,
             MediaremotePlugin    *mediaremote)
{
  GtkWidget *dialog;

  /* check if the dialog is still open. if so, destroy it */
  dialog = g_object_get_data (G_OBJECT (plugin), "dialog");
  if (G_UNLIKELY (dialog != NULL))
    gtk_widget_destroy (dialog);

  /* destroy the panel widgets */
  gtk_widget_destroy (mediaremote->hvbox);

  /* cleanup the settings */
  if (G_LIKELY (mediaremote->setting1 != NULL))
    g_free (mediaremote->setting1);

  /* free the plugin structure */
  g_slice_free (MediaremotePlugin, mediaremote);
}



static void
mediaremote_orientation_changed (XfcePanelPlugin *plugin,
                            GtkOrientation   orientation,
                            MediaremotePlugin    *mediaremote)
{
  /* change the orientation of the box */
  gtk_orientable_set_orientation(GTK_ORIENTABLE(mediaremote->hvbox), orientation);
}



static gboolean
mediaremote_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     MediaremotePlugin    *mediaremote)
{
  GtkOrientation orientation;

  /* get the orientation of the plugin */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* set the widget size */
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
  else
    gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);

  /* we handled the orientation */
  return TRUE;
}



static void
mediaremote_construct (XfcePanelPlugin *plugin)
{
  MediaremotePlugin *mediaremote;

  /* setup transation domain */
  xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

  /* create the plugin */
  mediaremote = mediaremote_new (plugin);

  /* add the ebox to the panel */
  gtk_container_add (GTK_CONTAINER (plugin), mediaremote->ebox);

  /* show the panel's right-click menu on this ebox */
  xfce_panel_plugin_add_action_widget (plugin, mediaremote->ebox);

  /* connect plugin signals */
  g_signal_connect (G_OBJECT (plugin), "free-data",
                    G_CALLBACK (mediaremote_free), mediaremote);

  g_signal_connect (G_OBJECT (plugin), "save",
                    G_CALLBACK (mediaremote_save), mediaremote);

  g_signal_connect (G_OBJECT (plugin), "size-changed",
                    G_CALLBACK (mediaremote_size_changed), mediaremote);

  g_signal_connect (G_OBJECT (plugin), "orientation-changed",
                    G_CALLBACK (mediaremote_orientation_changed), mediaremote);

  /* show the configure menu item and connect signal */
  xfce_panel_plugin_menu_show_configure (plugin);
  g_signal_connect (G_OBJECT (plugin), "configure-plugin",
                    G_CALLBACK (mediaremote_configure), mediaremote);

  /* show the about menu item and connect signal */
  xfce_panel_plugin_menu_show_about (plugin);
  g_signal_connect (G_OBJECT (plugin), "about",
                    G_CALLBACK (mediaremote_about), NULL);
}
