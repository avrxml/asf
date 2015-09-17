#!/bin/sh
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

lib_name=LIBMAD

source_dir="../../.."

tolower()
{
  echo `echo "$1" | sed 's,.*,\L\0,'`
}

toupper()
{
  echo `echo "$1" | sed 's,.*,\U\0,'`
}

build()
{
  opt="$1"
  opt_level=${!opt}
  build_options="$2"
  conf_options="$3"
  CFLAGS="-march=$arch -Wall -O$opt_level $build_options" "$source_dir/configure" --prefix="$install_dir" --libdir="$lib_dir" --includedir="$include_dir" --host=$host $conf_options || exit 1
  make || exit 1
  make install || exit 1
  make clean distclean || exit 1
  find "$lib_dir" -mindepth 1 -maxdepth 1 -type f \! -name '*.a' | xargs rm -f || exit 1
  mv -f "$lib_dir/$lib.a" "$lib_dir/$lib-$target-$opt.a" || exit 1
  find "$include_dir" -name '*.h' | xargs unix2dos || exit 1
}

build_dir="`pwd`"

host=avr32
target=`echo "$build_dir" | sed 's,.*/\([^/]*\)/[^/]*$,\L\1,'`
target_dir=`toupper $target`
arch=`echo "$target" | sed -e 's,^at32\([a-z]\{2\}r[0-9]\+\).*,\1,' -e 's,^at32\([a-z]\{2\}\).*,\1,'`
compiler=`echo "$build_dir" | sed 's,.*/\([^/]*\)$,\L\1,'`
compiler_dir=`toupper $compiler`

libs_dir=`echo $build_dir | sed 's,^\(.*\)/\(APPLICATIONS\|BOARDS\|COMPONENTS\|DRIVERS\|SERVICES\|UTILS\)/.*,\1/UTILS/LIBS,'`
lib=`tolower $lib_name`
install_dir="$libs_dir/$lib_name"
lib_dir="$install_dir/$target_dir/$compiler_dir"
include_dir="$install_dir/INCLUDE"

size_opt=s
balanced_opt=2
speed_opt=3

default_build_options="-ffunction-sections"
default_conf_options="--disable-shared --enable-sso --disable-debugging"

for cur_opt in size_opt balanced_opt speed_opt
do
  build $cur_opt "$default_build_options" "$default_conf_options"
done
rm -Rf "msvc++"
