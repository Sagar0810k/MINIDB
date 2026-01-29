@echo off
REM ===============================================
REM Mini DBMS Build Script for Windows
REM ===============================================

echo.
echo ========================================
echo Building Mini DBMS
echo ========================================
echo.

REM Set compiler and tools
set CC=gcc
set BISON=bison
set FLEX=flex

REM Check if build directory exists
if not exist build mkdir build

REM Clean old generated files to prevent conflicts
echo [0/6] Cleaning old generated files...
if exist build\lex.yy.c del /F /Q build\lex.yy.c
if exist build\parser.tab.c del /F /Q build\parser.tab.c
if exist build\parser.tab.h del /F /Q build\parser.tab.h
if exist src\lex.yy.c del /F /Q src\lex.yy.c
echo       - Cleanup complete

REM Change to source directory
cd src

echo.
echo [1/6] Generating parser with Bison...
%BISON% -d parser.y -o ..\build\parser.tab.c
if %errorlevel% neq 0 (
    echo ERROR: Bison failed!
    cd ..
    exit /b 1
)
echo       - parser.tab.c generated
echo       - parser.tab.h generated

echo.
echo [2/6] Generating lexer with Flex...
REM Generate in current directory (src) first
%FLEX% lexer.l
if %errorlevel% neq 0 (
    echo ERROR: Flex failed!
    cd ..
    exit /b 1
)
REM Move the generated file to build directory
move /Y lex.yy.c ..\build\lex.yy.c >nul
if %errorlevel% neq 0 (
    echo ERROR: Failed to move lex.yy.c to build directory!
    cd ..
    exit /b 1
)
echo       - lex.yy.c generated

echo.
echo [3/6] Compiling parser...
%CC% -c ..\build\parser.tab.c -o ..\build\parser.tab.o -I..\include
if %errorlevel% neq 0 (
    echo ERROR: Parser compilation failed!
    cd ..
    exit /b 1
)

echo.
echo [4/6] Compiling lexer...
%CC% -c ..\build\lex.yy.c -o ..\build\lex.yy.o -I..\include
if %errorlevel% neq 0 (
    echo ERROR: Lexer compilation failed!
    cd ..
    exit /b 1
)

echo.
echo [5/6] Compiling source files...
%CC% -c utils.c -o ..\build\utils.o -I..\include
%CC% -c table.c -o ..\build\table.o -I..\include
%CC% -c storage.c -o ..\build\storage.o -I..\include
%CC% -c db.c -o ..\build\db.o -I..\include
%CC% -c executor.c -o ..\build\executor.o -I..\include
%CC% -c main.c -o ..\build\main.o -I..\include
if %errorlevel% neq 0 (
    echo ERROR: Source compilation failed!
    cd ..
    exit /b 1
)

echo.
echo [6/6] Linking executable...
%CC% ..\build\parser.tab.o ..\build\lex.yy.o ..\build\utils.o ..\build\table.o ..\build\storage.o ..\build\db.o ..\build\executor.o ..\build\main.o -o ..\build\minidb.exe
if %errorlevel% neq 0 (
    echo ERROR: Linking failed!
    cd ..
    exit /b 1
)

cd ..

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable: build\minidb.exe
echo.
echo To run: build\minidb.exe
echo.

exit /b 0