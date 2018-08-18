#!/bin/sh

path=$(dirname $(readlink -f $0))
cd $path
ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++  ~/workspace/imatch2cn_plugin
mv tags libimatch2plugin
#ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++  ~/workspace/engine/
#mv tags mycpp
#ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++  ~/workspace/libevent-2.0.21-stable
#mv tags libevent
#ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q --language-force=C++  ~/workspace/tools/cpp_src/
#mv tags libstdcpp
cd -
