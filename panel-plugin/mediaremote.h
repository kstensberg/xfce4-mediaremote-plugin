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

#ifndef __MEDIAREMOTE_H__
#define __MEDIAREMOTE_H__

G_BEGIN_DECLS

/* plugin structure */
typedef struct
{
    XfcePanelPlugin *plugin;

    /* panel widgets */
    GtkWidget       *ebox;
    GtkWidget       *hvbox;
    GtkWidget       *label;

    /* mediaremote settings */
    gchar           *setting1;
    gint             setting2;
    gboolean         setting3;
}
MediaremotePlugin;



void
mediaremote_save (XfcePanelPlugin *plugin,
             MediaremotePlugin    *mediaremote);

G_END_DECLS

#endif /* !__MEDIAREMOTE_H__ */
