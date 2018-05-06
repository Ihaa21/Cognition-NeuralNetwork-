@echo off

set CodeDir=..\code
set OutputDir=..\build_win32

set CommonCompilerFlags=-O2 -MTd -nologo -fp:precise -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4127 -wd4201 -wd4100 -wd4189 -wd4505 -Z7 -FC
REM set CommonCompilerFlags=-DROSEMARY_DEBUG=1 -DROSEMARY_INTERNAL=1 -DROSEMARY_WINDOWS=1 %CommonCompilerFlags%
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib Winmm.lib opengl32.lib DbgHelp.lib 

IF NOT EXIST %OutputDir% mkdir %OutputDir%

pushd %OutputDir%

del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% %CodeDir%\win32_cognition.cpp -Fmwin32_cognition.map /link %CommonLinkerFlags%

popd
