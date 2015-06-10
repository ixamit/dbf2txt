#!/bin/sh
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

install=false
remove=false
help=false
case $1 in
    -h|--help)
      help=true
      shift;;
    -i|--install)
      install=true
      shift;;
    -r|--remove)
      remove=true
      shift;;
    *)
      echo "invalid or missing argv"
      exit 1
      ;;
esac

###########
# h e l p #
###########
if [ -z "$1" ] || $help ; then
    echo ""
    echo "Script which install or remove plugin"
    echo "usage: $0 <-i|r> <plugin>"
    echo ""
    exit 0
fi

plugin="$1"
foo_plugin="foo"
plugin_upper=`echo "$plugin" | tr '[:lower:]' '[:upper:]'`
foo_plugin_upper=`echo "$foo_plugin" | tr '[:lower:]' '[:upper:]'`

# save current directory and move to source package root
dir_sav=`pwd`
dir=`dirname $0`
cd "$dir/.."

# check directory existance
as_directory=false
if [ -d "plugin/$plugin" ]; then
    as_directory=true
fi

# check configure.in 
as_configure=false
grep "plugin/$plugin/Makefile" configure.in > /dev/null 2>&1
if [ $? -eq 0 ]; then
    as_configure=true
fi

#check Makefile.am
as_makefile=false
grep -w "$plugin" plugin/Makefile.am > /dev/null 2>&1
if [ $? -eq 0 ]; then
    as_makefile=true
fi

#################
# i n s t a l l #
#################
if $install; then
    # clone foo plugin to the new plugin 
    if ! $as_directory; then
        echo 'cloning foo plugin'
        mkdir plugin/"$plugin"
        find plugin/"$foo_plugin" -type f ! -name Makefile.in -exec cp {} plugin/"$plugin" \;
        mv plugin/"$plugin/$foo_plugin.c" plugin/"$plugin/$plugin.c"
        sed -i "s/$foo_plugin_upper/$plugin_upper/Ig" plugin/"$plugin/$plugin.c"
        sed -i "s/$foo_plugin/$plugin/g" plugin/"$plugin"/Makefile.am
    fi
    # add to end of SUBDIRS line
    if ! $as_makefile; then
        echo 'add plugin to Makefile.am'
        sed -i "/^SUBDIRS/ s/$/ $plugin/" plugin/Makefile.am
    fi
    # append line to AC_OUTPUT macro 
    if ! $as_configure; then
        echo 'add plugin to configure.in'
        sed -i "/plugin\/Makefile/a plugin/$plugin/Makefile" configure.in
    fi
fi
###############
# r e m o v e #
###############
if $remove; then
    if $as_makefile; then
        echo 'remove plugin from Makefile.am'
        sed -i "s/ $plugin//" plugin/Makefile.am
    fi
    if $as_configure; then
        echo 'remove plugin from configure.in'
        sed -i "/plugin\/$plugin\/Makefile/d" configure.in 
    fi
    if $as_directory; then
        echo "the directory plugin/$plugin" exist. You can remove it manually
    fi
fi
cd "$dir_sav"

