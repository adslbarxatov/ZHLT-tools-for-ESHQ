#define _CRT_SECURE_NO_WARNINGS

// Подключение заголовков
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <windows.h>
#include "..\Generics\CSTypes.h"

#define B_API(t)	extern "C" __declspec(dllexport) t

#define B_VERSION			7,0,0,0
#define B_VERSION_S			"7.0.0.0"
#define B_PRODUCT			"ZHLT tools adaptation for ESHQ"
#define B_COMPANY			FDL_COMPANY

// Сводные интерфейсы модулей
//
// Mode 0: финальная компиляция для ESHQ
// Mode 1: быстрая компиляция для ES:RM
//
// Ожидаемые параметры компиляции:
// CSG full: [-nonullifytrigger] -cliptype precise -wadinclude eshq68 $file
// BSP full: <empty>
// VIS full: -full
// RAD full: -extra [-limiter -1] -colourgamma 0.45 0.45 0.45 $file
// CSG esrm: -low
// BSP esrm: -low
// VIS esrm: -low
// RAD esrm: -low -sparse

int MakeCSG (const char *map, uchar Mode);
int MakeBSP (const char *map, uchar Mode);
int MakeVIS (const char *map, uchar Mode);
int MakeRAD (const char *map, uchar Mode);

// Интерфейс DLL
#if AS_DLL
B_API(sint) BuildMapEx (const char *MapPath, uchar Step);
#endif

B_API(uint) StepsCountEx ();
B_API(char *) BannerTextEx ();
