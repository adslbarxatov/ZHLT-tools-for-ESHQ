#pragma warning(disable: 4267) // 'size_t' to 'unsigned int', possible loss of data

#include "csg.h"

#ifdef HAVE_UNISTD_E
#include <unistd.h>
#endif

void LoadWadincludeFile (const char *const filename)
	{
	char	*fname;
	int		i, x;
	char	*pData = NULL;
	char	*pszData;
	unsigned	len = strlen (filename) + 5;

	fname = (char *)Alloc (len);
	safe_snprintf (fname, len, "%s.wic", filename);

	if (q_exists (fname))
		{
		i = LoadFile (fname, &pData);

		if (i == 0)
			{
			goto LoadWadincludeFileReturn;
			}
		}
	else
		{
		Warning ("WadInclude file %s does not exist", fname);
		goto LoadWadincludeFileReturn;
		}

	for (pszData = pData, x = 0; x < i; x++)
		{
		if (pData[x] == ';')
			{
			pData[x] = 0;
			g_WadInclude.push_back (pszData);
			pszData = pData + x + 1;
			}
		}

LoadWadincludeFileReturn:;
	Free (fname);
	if (pData)
		{
		Free (pData);
		}
	}

void SaveWadincludeFile (const char *const filename)
	{
	char	*fname;
	FILE	*file;
	int		x;
	unsigned	len = strlen (filename) + 5;

	fname = (char *)Alloc (len);
	safe_snprintf (fname, len, "%s.wic", filename);

	_unlink (fname);

	file = SafeOpenWrite (fname);

	WadInclude_i it;
	for (it = g_WadInclude.begin (); it != g_WadInclude.end (); it++)
		{
		x = it->size ();
		if (x)
			{
			SafeWrite (file, it->c_str (), x);
			SafeWrite (file, ";", 1);
			}
		}

	Free (fname);
	fclose (file);
	}

// this function is called in place of tex_initfromwad for onlyents compiles
void HandleWadinclude ()
	{
	int		i;
	char	szTmpWad[1024];		// arbitrary, but needs to be large
	char	*pszWadFile;
	wadpath_t	*currentwad;

	Log ("\n"); // looks cleaner

	szTmpWad[0] = 0;

#ifdef HLCSG_AUTOWAD
	if (g_bWadAutoDetect)
		{
		autowad_UpdateUsedWads ();
		}
#endif

	// for eachwadpath
	for (i = 0; i < g_iNumWadPaths; i++)
		{
		bool	bExcludeThisWad = false;

		currentwad = g_pWadPaths[i];
		pszWadFile = currentwad->path;

#ifdef HLCSG_AUTOWAD
		if (g_bWadAutoDetect && !currentwad->usedtextures)
			continue;
#endif

	// look and see if we're supposed to include the textures from this WAD in the bsp.
		WadInclude_i it;
		for (it = g_WadInclude.begin (); it != g_WadInclude.end (); it++)
			{
			if (stristr (pszWadFile, it->c_str ()))
				{
				Log ("Including Wadfile: %s\n", pszWadFile);
				bExcludeThisWad = true;		// wadincluding this one
				}
			}

		if (!bExcludeThisWad)
			{
			Log ("Using Wadfile: %s\n", pszWadFile);
			safe_snprintf (szTmpWad, 1024, "%s%s;", szTmpWad, pszWadFile);
			}
		}

	Log ("\"wad\" is \"%s\"\n", szTmpWad);

	SetKeyValue (&g_entities[0], "wad", szTmpWad);

	Log ("\n");
	CheckFatal ();
	}
