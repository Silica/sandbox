@echo off
if "%Platform%" == "x64" (
	cl /Fesandbox64 /MTd /EHsc sandbox.cpp
	cl /Fesandbox64 /LDd /EHsc sandbox.cpp
) else if "%1" == "release" (
	cl /Ox /EHsc /MT sandbox.cpp
	cl /Ox /EHsc /LD sandbox.cpp
) else (
	cl /MTd /EHsc sandbox.cpp
	cl /LDd /EHsc sandbox.cpp
)
