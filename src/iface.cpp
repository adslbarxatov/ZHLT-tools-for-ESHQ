#include "iface.h"

// ESHQ: подменная точка входа
#if AS_DLL
B_API(sint) BuildMapEx (const char *MapPath, uchar Step)
#else
int main (int argc, char **argv)
#endif
	{
	char	path[_MAX_PATH];
	uchar	esrm = 0;

	// Приглашение и контроль параметров
#if !AS_DLL
#if CON_UI
	fprintf (stdout, "\n < %s >\n\n", B_PRODUCT);
#endif
	if (argc < 3)
		{
#if CON_UI
		fprintf (stdout, " > Usage: RandomazeZHLT <mapfile> <step>\n");
		fprintf (stdout, "   Step 1: CSG\n");
		fprintf (stdout, "   Step 2: BSP\n");
		fprintf (stdout, "   Step 3: VIS\n");
		fprintf (stdout, "   Step 4: RAD\n");
#endif

		return 1;
		}
#endif

	// Подготовка параметров
	path[0] = '\0';
#if AS_DLL
	strcpy (path, MapPath);
#else
	strcpy (path, argv[1]);
#endif

	if (strstr (path, "esrm") != NULL)
		esrm = 1;

	/*// TEST //
	MakeCSG (path, esrm);
	MakeBSP (path, esrm);
	MakeVIS (path, esrm);
	MakeRAD (path, esrm);
	//      //*/

	// Запуск
#if AS_DLL
	switch (Step + 0x30)
#else
	switch (argv[2][0])
#endif
		{
		case '1':
			return MakeCSG (path, esrm);

		case '2':
			return MakeBSP (path, esrm);

		case '3':
			return MakeVIS (path, esrm);

		case '4':
			return MakeRAD (path, esrm);

		default:
#if CON_UI
			fprintf (stdout, " ! Specify the correct step number and try again\n");
#endif
			return -1;
		}
	}

B_API(char *) LibVersionEx ()
	{
	return B_VERSION_S;
	}

B_API(uint) StepsCountEx ()
	{
	return 4;
	}

#include "common\log.h"

B_API (char *) BannerEx ()
	{
	return GetBanner ();
	}
