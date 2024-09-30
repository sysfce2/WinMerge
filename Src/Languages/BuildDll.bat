@echo off
rem BuildDll.bat
rem
rem This batch file is used to build the Resource DLL
rem Use it like so: BuildDll Bulgarian
rem It returns 1 if there is an error
rem
rem RCS ID line follows -- this is updated by CVS
rem $Id: BuildDll.bat 3273 2006-05-17 16:44:12Z kimmov $

if '%1==' echo Syntax: BuildDll language
if '%1==' goto end

echo Building %1
set rcScript=.\%1\Merge%1.rc
set outputdir=..\..\Build\Languages

.\MakeResDll -v -b -o %outputdir% %rcScript%

if errorlevel==1 goto _failed
goto end

:_failed
rem This causes console to close!
rem exit 1

:end
