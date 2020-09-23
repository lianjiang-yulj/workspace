" let g:python3_host_prog = '/Library/Frameworks/Python.framework/Versions/3.8/bin/python3.8'
let g:python3_host_prog = '/opt/anaconda3/bin/python3.8'
set runtimepath^=~/.vim runtimepath+=~/.vim/after
let &packpath = &runtimepath
source ~/.vimrc


call plug#begin()
" 前面可能有东西　下面一行是重要的
" Plug 'altercation/vim-colors-solarized'
" 前面可能有东西
call plug#end()

if has('gui_running')
    set background=light
else
    set background=dark
endif
syntax enable
" colorscheme solarized
colorscheme default

set shada='50,<1000,s100,:0,n~/.local/share/nvim/shada/main.shada
