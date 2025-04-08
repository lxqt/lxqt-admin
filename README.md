# lxqt-admin

## Overview

This repository provides two GUI tools to adjust settings of the operating system LXQt
is running on.

Both are using [polkit](https://www.freedesktop.org/wiki/Software/polkit/) to handle
permissions.

### Time and Date Configuration

Adjusts the time and date. Binary is `lxqt-admin-time`.

![lxqt-admin-time](lxqt-admin-time.png)

It is using `systemd-timedated` as its backend, which is accessed by its D-Bus interface.
As such, this means the option to sync the system time by NTP is relying on
`systemd-timesyncd` as a backend.

### User and Group Settings

Management of users and groups. Binary is `lxqt-admin-user`.

![lxqt-admin-user](lxqt-admin-user.png)

The backend is a script, `lxqt-admin-user-helper`. As such, it is using the shadow tools
to do the actual work. However, the script can still be modified to use different tools.

## Installing

### Compiling Sources

Runtime dependencies consist of polkit and [liblxqt](https://github.com/lxqt/liblxqt).
A polkit agent should be available with
[lxqt-policykit](https://github.com/lxqt/lxqt-policykit/), representing the first choice
in LXQt. Additional build dependencies are CMake and optionally Git, to pull the latest
VCS checkouts.

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX` will
normally have to be set to `/usr`.

To build, run `make`. To install, run `make install` which accepts variable `DESTDIR`
as usual.

### Binary Packages

Package `lxqt-admin` is available in the official repositories of all major
distributions.

#### openSUSE

openSUSE does not ship with lxqt-admin in it's standard repositories since the
functionality is covered by openSUSE's [YaST](http://yast.github.io/). It is still
possible to install & use it on openSUSE.

## Usage

Much like similar tools provided by [lxqt-config](https://github.com/lxqt/lxqt-config),
the tools of lxqt-admin can be launched from the
[Configuration Center](https://github.com/lxqt/lxqt-config#configuration-center), as
well as from the panel's menu.

The actual usage should be self-explanatory. To apply settings the GUI of the polkit
authentication agent that's in use is launched to acquire the password.


### Translations

Translations can be done in 
[LXQt-Weblate/admin-user](https://translate.lxqt-project.org/projects/lxqt-configuration/lxqt-admin-user/)
and in [LXQt-weblate/admin-time](https://translate.lxqt-project.org/projects/lxqt-configuration/lxqt-admin-time/).

<a href="https://translate.lxqt-project.org/projects/lxqt-configuration/lxqt-admin-user/">
<img src="https://translate.lxqt-project.org/widgets/lxqt-configuration/-/lxqt-admin-user/multi-auto.svg" alt="Translation status" />
</a>
