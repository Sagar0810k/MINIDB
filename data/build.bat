@echo off
setlocal

REM ---- Toolchain ----
set CC=gcc
set CFLAGS=-Wall -g

REM ---- Project paths ----
set SRC=..\src
set OBJ=..\obj

REM ---- Fix for GnuWin32 Bison + m4 path issue ----
set BISON_PKGDATADIR=C:\GnuWin32\share\bison

REM ---- Create obj dir if missing ----
if not exist "%OBJ%" mkdir "%OBJ%"

echo === Generating parser (Bison) ===
bison -d -o "%SRC%\parser.tab.c" "%SRC%\parser.y"
if errorlevel 1 goto :error

echo === Generating lexer (Flex) ===
flex -o"%SRC%\lex.yy.c" "%SRC%\lexer.l"
if errorlevel 1 goto :error

echo === Compiling sources ===
%CC% %CFLAGS% -c "%SRC%\ast.c"        -o "%OBJ%\ast.o"
%CC% %CFLAGS% -c "%SRC%\utils.c"      -o "%OBJ%\utils.o"
%CC% %CFLAGS% -c "%SRC%\catalog.c"    -o "%OBJ%\catalog.o"
%CC% %CFLAGS% -c "%SRC%\semantic.c"   -o "%OBJ%\semantic.o"
%CC% %CFLAGS% -c "%SRC%\storage.c"    -o "%OBJ%\storage.o"
%CC% %CFLAGS% -c "%SRC%\executor.c"   -o "%OBJ%\executor.o"
%CC% %CFLAGS% -c "%SRC%\parser.tab.c" -o "%OBJ%\parser.tab.o"
%CC% %CFLAGS% -c "%SRC%\lex.yy.c"     -o "%OBJ%\lex.yy.o"
%CC% %CFLAGS% -c "%SRC%\main.c"       -o "%OBJ%\main.o"
if errorlevel 1 goto :error

echo === Linking ===
%CC% %CFLAGS% -o minidb.exe "%OBJ%\*.o"
if errorlevel 1 goto :error

echo.
echo ✅ Build successful: minidb.exe
goto :eof

:error
echo.
echo ❌ Build failed.
exit /b 1
