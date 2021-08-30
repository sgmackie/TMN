" Build
map <F2> :Dispatch build_windows.bat<cr>

" Run
map <F3> :Start -dir="build/tmne.exe"<cr>

" Debug
map <F4> :!start remedybg open-session debug/tmne_debug.rdbg<cr>
