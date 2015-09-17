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
  "$iar_build" "$lib-$target.ewp" -build $opt -log info || exit 1
  mkdir -p "$lib_dir" || exit 1
  mv -f -t "$lib_dir" "$lib-$target-$opt.r82" || exit 1
  rm -Rf "$opt" || exit 1
}

build_dir="`pwd`"

target=`echo "$build_dir" | sed 's,.*/\([^/]*\)/[^/]*$,\L\1,'`
target_dir=`toupper $target`
arch=`echo "$target" | sed -e 's,^at32\([a-z]\{2\}r[0-9]\+\).*,\1,' -e 's,^at32\([a-z]\{2\}\).*,\1,'`
compiler=`echo "$build_dir" | sed 's,.*/\([^/]*\)$,\L\1,'`
compiler_dir=`toupper $compiler`

libs_dir=`echo $build_dir | sed 's,^\(.*\)/\(APPLICATIONS\|BOARDS\|COMPONENTS\|DRIVERS\|SERVICES\|UTILS\)/.*,\1/UTILS/LIBS,'`
lib=`find "$build_dir" -mindepth 1 -maxdepth 1 -type f -name '*.eww' | sed "s,.*/\([^/]*\)-$target\.eww\$,\1,"`
install_dir="$libs_dir/$lib_name"
lib_dir="$install_dir/$target_dir/$compiler_dir"
include_dir="$install_dir/INCLUDE"

iar_build="`regtool get '\HKEY_LOCAL_MACHINE\SOFTWARE\IAR Systems\Embedded Workbench\3.1\EWAVR32\InstallPath'`/common/bin/iarbuild.exe"

for cur_opt in size_opt balanced_opt speed_opt
do
  build $cur_opt
done
rm -f "$lib-$target.dep"
