SETLOCAL EnableDelayedExpansion

Rem Package information
set PKG_TITLE="OpenOrbis Hello World Sample"
set PKG_VERSION="1.00"
set PKG_ASSETS="assets"
set PKG_TITLE_ID="BREW00083"
set PKG_CONTENT_ID="IV0000-BREW00083_00-HELLOWORLDLUA000"

Rem Libraries to link in
set libraries=-lc -lkernel -lc++

set extra_flags=-I"..\\lua"

Rem Read the script arguments into local vars
set intdir=%1
set targetname=%~2
set outputPath=%3

set outputElf=%intdir%\%targetname%.elf
set outputOelf=%intdir%\%targetname%.oelf

@mkdir %intdir%

echo asdfasdfasdf

Rem Compile object files for all the source files
for %%f in (*.c) do (
    echo clang --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.c
    clang --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.c
)

for %%f in (*.cpp) do (
    echo clang++ --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.cpp
    clang++ --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.cpp
)

Rem Get a list of object files for linking
set obj_files=

Rem Lua obj files
for %%f in (..\lua\%1*.o) do set obj_files=!obj_files! .\%%f
@REM set obj_files=!obj_files! ..\lua\%1\lauxlib.o
@REM set obj_files=!obj_files! ..\lua\%1\lua.o
@REM set obj_files=!obj_files! ..\lua\%1\ldebug.o
@REM set obj_files=!obj_files! ..\lua\%1\lapi.o
@REM set obj_files=!obj_files! ..\lua\%1\lstate.o
@REM set obj_files=!obj_files! ..\lua\%1\linit.o
@REM set obj_files=!obj_files! ..\lua\%1\lfunc.o
@REM set obj_files=!obj_files! ..\lua\%1\ltable.o
@REM set obj_files=!obj_files! ..\lua\%1\ltm.o
@REM set obj_files=!obj_files! ..\lua\%1\lgc.o
@REM set obj_files=!obj_files! ..\lua\%1\lobject.o
@REM set obj_files=!obj_files! ..\lua\%1\lvm.o
@REM set obj_files=!obj_files! ..\lua\%1\ldo.o
@REM set obj_files=!obj_files! ..\lua\%1\lopcodes.o
@REM set obj_files=!obj_files! ..\lua\%1\lstring.o
@REM set obj_files=!obj_files! ..\lua\%1\lzio.o
@REM set obj_files=!obj_files! ..\lua\%1\ldump.o
@REM set obj_files=!obj_files! ..\lua\%1\lmem.o
@REM set obj_files=!obj_files! ..\lua\%1\llex.o
@REM set obj_files=!obj_files! ..\lua\%1\lbaselib.o
@REM set obj_files=!obj_files! ..\lua\%1\loadlib.o
@REM set obj_files=!obj_files! ..\lua\%1\lcorolib.o
@REM set obj_files=!obj_files! ..\lua\%1\ltablib.o
@REM set obj_files=!obj_files! ..\lua\%1\liolib.o
@REM set obj_files=!obj_files! ..\lua\%1\loslib.o

for %%f in (%1\\*.o) do set obj_files=!obj_files! .\%%f


Rem Link the input ELF
ld.lld -error-limit=0 -m elf_x86_64 -pie --script "%OO_PS4_TOOLCHAIN%\link.x" --eh-frame-hdr -o "%outputElf%" "-L%OO_PS4_TOOLCHAIN%\\lib" %libraries% --verbose "%OO_PS4_TOOLCHAIN%\lib\crt1.o" %obj_files%

Rem Create the eboot
%OO_PS4_TOOLCHAIN%\bin\windows\create-fself.exe -in "%outputElf%" --out "%outputOelf%" --eboot "eboot.bin" --paid 0x3800000000000011

Rem Eboot cleanup
copy "eboot.bin" %outputPath%\eboot.bin
del "eboot.bin"

Rem Create param.sfo
cd ..
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_new sce_sys/param.sfo
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo APP_TYPE --type Integer --maxsize 4 --value 1
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo APP_VER --type Utf8 --maxsize 8 --value %PKG_VERSION%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo ATTRIBUTE --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo CATEGORY --type Utf8 --maxsize 4 --value "gd"
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo CONTENT_ID --type Utf8 --maxsize 48 --value %PKG_CONTENT_ID%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo SYSTEM_VER --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo TITLE --type Utf8 --maxsize 128 --value %PKG_TITLE%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo TITLE_ID --type Utf8 --maxsize 12 --value %PKG_TITLE_ID%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo VERSION --type Utf8 --maxsize 8 --value %PKG_VERSION%

Rem Get a list of assets for packaging
Rem Get a list of assets for packaging
set module_files=
for %%f in (sce_module\\*) do set module_files=!module_files! sce_module/%%~nxf

set asset_audio_files=
for %%f in (assets\\audio\\*) do set asset_audio_files=!asset_audio_files! assets/audio/%%~nxf

set asset_fonts_files=
for %%f in (assets\\fonts\\*) do set asset_fonts_files=!asset_fonts_files! assets/fonts/%%~nxf

set asset_images_files=
for %%f in (assets\\images\\*) do set asset_images_files=!asset_images_files! assets/images/%%~nxf

set asset_misc_files=
for %%f in (assets\\misc\\*) do set asset_misc_files=!asset_misc_files! assets/misc/%%~nxf

set asset_videos_files=
for %%f in (assets\\videos\\*) do set asset_videos_files=!asset_videos_files! assets/videos/%%~nxf

Rem Create gp4
%OO_PS4_TOOLCHAIN%\bin\windows\create-gp4.exe -out pkg.gp4 --content-id=%PKG_CONTENT_ID% --files "eboot.bin sce_sys/about/right.sprx sce_sys/param.sfo sce_sys/icon0.png %module_files% %asset_audio_files% %asset_fonts_files% %asset_images_files% %asset_misc_files% %asset_videos_files%"

Rem Create pkg
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe pkg_build pkg.gp4 .
