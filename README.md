# litos v. 2.1

**litos** aims to be an easy-to-use and fast editor. My target is an
editor for quickly editing text files, providing shortcuts for inserting HTML tags. I try to use the latest
GTK features available, which means that if GTK adds something new in a major
release that is useful for the editor, we will likely bump the GTK dependency
and integrate this new feature in **litos**.

### Required Packages 

Mousepad depends on the following packages:

* gcc
* make
* pkg-config
* libgtk-3-dev [GTK](https://www.gtk.org) >=3.20.0
* [GLib](https://wiki.gnome.org/Projects/GLib) >=2.42.0
* libgtksourceview-3-dev [GtkSourceView](https://wiki.gnome.org/Projects/GtkSourceView) >=3.0

### Installation

From source code repository: 

    % cd litos
    % make

### Usage

Ctrl-b To tranform "string" into "<b>string<</b>"
Ctrl-i To tranform "string" into "<i>string<</i>"
Ctrl-p To tranform "string" into "<sup>string<</sup>"

To create file from Templates make sure you've the "Templates" dir in you home direcotry and the file $HOME/.config/user-dirs.dirs exists (see https://wiki.archlinux.org/title/XDG_user_directories)

###Authors

Giovanni Resta - giovannirestadev@gmail.com

### Reporting Bugs

giovannirestadev@gmail.com
