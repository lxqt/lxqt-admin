## lxqt-admin

This repository is providing two tools to adjust settings of the operating system LXQt is running on. Both can
be launched from GUI "Configuration Center" of [lxqt-config](https://github.com/lxde/lxqt-config).

GUI "Time and date configuration", binary `lxqt-admin-time`, can be used to adjust the system time of the operating
system as well as the timezone.

![lxqt-admin-time](lxqt-admin-time.png)

In "User and Group Settings", binary `lxqt-admin-user`, users and groups of the operating system can be modified.

![lxqt-admin-user](lxqt-admin-user.png)

Technical note:   
Currently the binaries summarized in this repository are relying on
[system-tools-backends](http://system-tools-backends.freedesktop.org) and their wrapper
[liboobs](https://github.com/GNOME/liboobs). These aren't maintained any longer and hence lack in many Linux
distributions which obviously prevents the usage of lxqt-admin on these distributions. Switching to different and common
backends is work in progress, see https://github.com/lxde/lxqt/issues/495, so this situation will hopefully change in
the not too distant future.
