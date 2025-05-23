// AJM: Added this file in
#include "csg.h"
#include "..\common\cmdlib.h"

#ifdef HLCSG_AUTOWAD

#define MAX_AUTOWAD_TEXNAME 32	// 32 char limit in array size in brush_texture_t struct

int g_numUsedTextures = 0;

typedef struct autowad_texname_s	// non-dupicate list of textures used in map
	{
	char name[MAX_AUTOWAD_TEXNAME];
	autowad_texname_s *next;
	} autowad_texname_t;

autowad_texname_t *g_autowad_texname;

// ESHQ: ������� ��������, �.�. ��������� ��� ������������ � cmdlib
#if 0
// =====================================================================================
// Extract File stuff (ExtractFile | ExtractFilePath | ExtractFileBase)
//
// With VS 2005 - and the 64 bit build, i had to pull 3 classes over from
// cmdlib.cpp even with the proper includes to get rid of the lnk2001 error
//
// amckern - amckern@yahoo.com
// =====================================================================================

#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')

void ExtractFileBase (const char *const path, char *dest)
	{
	hlassert (path != dest);

	const char *src;
	src = path + strlen (path) - 1;

	// back up until a \ or the start
	while (src != path && !PATHSEPARATOR (*(src - 1)))
		src--;

	while (*src && *src != '.')
		{
		*dest++ = *src++;
		}
	*dest = 0;
	}

void ExtractFilePath (const char *const path, char *dest)
	{
	hlassert (path != dest);

	const char *src;

	src = path + strlen (path) - 1;

	// back up until a \ or the start
	while (src != path && !PATHSEPARATOR (*(src - 1)))
		src--;

	memcpy (dest, path, src - path);
	dest[src - path] = 0;
	}

void ExtractFile (const char *const path, char *dest)
	{
	hlassert (path != dest);

	const char *src;

	src = path + strlen (path) - 1;

	while (src != path && !PATHSEPARATOR (*(src - 1)))
		src--;

	while (*src)
		{
		*dest++ = *src++;
		}
	*dest = 0;
	}
#endif

// =====================================================================================
// autowad_PushName
// adds passed texname as an entry to autowad_wadnames list, without duplicates
// =====================================================================================
void autowad_PushName (const char *const texname)
	{
	autowad_texname_t *tex;

	if (!g_autowad_texname)
		{
		// first texture, make an entry
		tex = (autowad_texname_t *)malloc (sizeof (autowad_texname_t));
		tex->next = NULL;
		strcpy_s (tex->name, texname);

		g_autowad_texname = tex;
		g_numUsedTextures++;

		Log ("[dbg] Used texture: %i[%s]\n", g_numUsedTextures, texname);
		
		return;
		}

	// otherwise, see if texname isnt already in the list
	for (tex = g_autowad_texname; ; tex = tex->next)
		{
		if (!strcmp (tex->name, texname))
			return;		// dont bother adding it again

		if (!tex->next)
			break;		// end of list
		}

	// unique texname
	g_numUsedTextures++;
	autowad_texname_t *last;
	last = tex;
	tex = (autowad_texname_t *)malloc (sizeof (autowad_texname_t));
	strcpy_s (tex->name, texname);
	tex->next = NULL;
	last->next = tex;

	Log ("[dbg] Used texture: %i[%s]\n", g_numUsedTextures, texname);
	}

// =====================================================================================
// autowad_PurgeName
// =====================================================================================
void autowad_PurgeName (const char *const texname)
	{
	autowad_texname_t *prev;
	autowad_texname_t *current;

	if (!g_autowad_texname)
		return;

	current = g_autowad_texname;
	prev = NULL;
	for (; ;)
		{
		if (!strcmp (current->name, texname))
			{
			if (!prev)	// start of the list
				{
				g_autowad_texname = current->next;
				}
			else	// middle of list
				{
				prev->next = current->next;
				}

			free (current);
			return;
			}

		if (!current->next)
			{
			return; // end of list
			}

		// move along
		prev = current;
		current = current->next;
		}
	}

// =====================================================================================
// autowad_PopName
// removes a name from the autowad list, puts it in memory and retuns pointer (which 
// shoudl be destructed by the calling function later)
// =====================================================================================
char *autowad_PopName ()
	{
	// todo: code
	return NULL;
	}

// =====================================================================================
// autowad_cleanup
// frees memory used by autowad procedure
// =====================================================================================
void autowad_cleanup ()
	{
	if (g_autowad_texname != NULL)
		{
		autowad_texname_t *current;
		autowad_texname_t *next;

		for (current = g_autowad_texname; ; current = next)
			{
			if (!current)
				break;

			next = current->next;
			free (current);
			}
		}
	}

// =====================================================================================
// autowad_IsUsedTexture
// is this texture in the autowad list?
// =====================================================================================
bool autowad_IsUsedTexture (const char *const texname)
	{
	autowad_texname_t *current;

	if (!g_autowad_texname)
		return false;

	for (current = g_autowad_texname; ; current = current->next)
		{
		if (!strcmp (current->name, texname))
			return true;

		if (!current->next)
			return false; // reached end of list
		}

	return false;
	}

// =====================================================================================
// GetUsedTextures
// =====================================================================================
void GetUsedTextures ()
	{
	int		i;
	side_t	*bs;

	for (i = 0; i < g_numbrushsides; i++)
		{
		bs = &g_brushsides[i];
		autowad_PushName (bs->td.name);
		}
	}

// =====================================================================================
// autowad_UpdateUsedWads
// =====================================================================================

// yes, these should be in a header/common lib, but i cant be bothered

#define MAXWADNAME 16

typedef struct
	{
	char	identification[4];		// should be WAD2/WAD3
	int		numlumps;
	int		infotableofs;
	} wadinfo_t;

typedef struct
	{
	int		filepos;
	int		disksize;
	int		size;				// uncompressed
	char	type;
	char	compression;
	char	pad1, pad2;
	char	name[MAXWADNAME];	// must be null terminated

	int		iTexFile;			// index of the wad this texture is located in
	} lumpinfo_t;

static void CleanupName (const char *const in, char *out)
	{
	int		i;

	for (i = 0; i < MAXWADNAME; i++)
		{
		if (!in[i])
			{
			break;
			}

		out[i] = toupper (in[i]);
		}

	for (; i < MAXWADNAME; i++)
		{
		out[i] = 0;
		}
	}

void autowad_UpdateUsedWads ()
	{
	// see which wadfiles are needed
	// code for this wad loop somewhat copied from below
	wadinfo_t	thiswad;
	lumpinfo_t	*thislump = NULL;
	wadpath_t	*currentwad;
	char		*pszWadFile;
	FILE		*texfile;
	const char	*pszWadroot = getenv ("WADROOT");
	int			i, j;
	int			nTexLumps = 0;

	// open each wadpath and sort though its contents
	for (i = 0; i < g_iNumWadPaths; i++)
		{
		currentwad = g_pWadPaths[i];
		pszWadFile = currentwad->path;
		currentwad->usedbymap = false;	// guilty until proven innocent

		texfile = fopen (pszWadFile, "rb");

#ifdef SYSTEM_WIN32
		if (!texfile)
			{
			// cant find it, maybe this wad file has a hard code drive
			if (pszWadFile[1] == ':')
				{
				pszWadFile += 2;		// skip past the drive
				texfile = fopen (pszWadFile, "rb");
				}
			}
#endif

		char	szTmp[_MAX_PATH];
		if (!texfile && pszWadroot)
			{
			char	szFile[_MAX_PATH];
			char	szSubdir[_MAX_PATH];

			ExtractFile (pszWadFile, szFile);

			ExtractFilePath (pszWadFile, szTmp);
			ExtractFile (szTmp, szSubdir);

			// szSubdir will have a trailing separator
			safe_snprintf (szTmp, _MAX_PATH, "%s" SYSTEM_SLASH_STR "%s%s", pszWadroot, szSubdir, szFile);
			texfile = fopen (szTmp, "rb");

#ifdef SYSTEM_POSIX
			if (!texfile)
				{
				// cant find it, Convert to lower case and try again
				strlwr (szTmp);
				texfile = fopen (szTmp, "rb");
				}
#endif
			}

		if (!texfile) // still cant find it, skip this one
			{
			if (pszWadroot)
				{
				Warning ("Wad file '%s' not found, also tried '%s'", pszWadFile, szTmp);
				}
			else
				{
				Warning ("Wad file '%s' not found", pszWadFile);
				}
			continue;
			}

		// look and see if we're supposed to include the textures from this WAD in the bsp.
		{
		WadInclude_i	it;
		bool			including = false;
		for (it = g_WadInclude.begin (); it != g_WadInclude.end (); it++)
			{
			if (stristr (pszWadFile, it->c_str ()))
				{
				including = true;
				currentwad->usedbymap = true;
				break;
				}
			}
		}

		// read in this wadfiles information
		SafeRead (texfile, &thiswad, sizeof (thiswad));

		// make sure its a valid format
		if (strncmp (thiswad.identification, "WAD2", 4) && strncmp (thiswad.identification, "WAD3", 4))
			{
			fclose (texfile);
			continue;
			}

		thiswad.numlumps = LittleLong (thiswad.numlumps);
		thiswad.infotableofs = LittleLong (thiswad.infotableofs);

		// read in lump
		if (fseek (texfile, thiswad.infotableofs, SEEK_SET))
			{
			fclose (texfile);
			continue;	// had trouble reading, skip this wad
			}

		// memalloc for this lump
		thislump = (lumpinfo_t *)realloc (thislump, (nTexLumps + thiswad.numlumps) * sizeof (lumpinfo_t));
		// BUGBUG: is this destructed?

		// for each texlump
		for (j = 0; j < thiswad.numlumps; j++, nTexLumps++)
			{
			SafeRead (texfile, &thislump[nTexLumps], (sizeof (lumpinfo_t) - sizeof (int)));
			// iTexFile is NOT read from file

			CleanupName (thislump[nTexLumps].name, thislump[nTexLumps].name);

			if (autowad_IsUsedTexture (thislump[nTexLumps].name))
				{
				currentwad->usedbymap = true;
				currentwad->usedtextures++;

				Log ("    - Used wadfile: [%s]\n", thislump[nTexLumps].name);
				autowad_PurgeName (thislump[nTexLumps].name);
				}
			}

		fclose (texfile);
		}

	return;
	}

#endif
