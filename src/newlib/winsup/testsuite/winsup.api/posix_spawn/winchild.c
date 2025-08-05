#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int wmain (int argc, wchar_t **argv)
{
  if (argc != 3)
    {
      fwprintf (stderr, L"Usage: %ls handle expected\n", argv[0]);
      return 1;
    }

  if (!wcscmp (argv[1], L"CWD"))
    {
      LPWSTR buffer;
      DWORD len = GetCurrentDirectoryW (0, NULL);
      if (len == 0)
        {
	  fwprintf (stderr, L"%ls: GetCurrentDirectory failed with error %lu\n",
		    argv[0], GetLastError ());
	  return 2;
	}
      buffer = malloc (len * sizeof (WCHAR));
      if (GetCurrentDirectoryW (len, buffer) != len - 1)
        {
	  fwprintf (stderr, L"%ls: GetCurrentDirectory failed with error %lu\n",
		    argv[0], GetLastError ());
	  return 2;
	}
      if (wcscmp (argv[2], buffer))
        {
	  fwprintf (stderr, L"%ls: CWD '%ls' != expected '%ls'\n",
		    argv[0], buffer, argv[2]);
	  free (buffer);
	  return 4;
	}
      free (buffer);
    }
  else if (iswdigit (argv[1][0]) && !argv[1][1])
    {
      HANDLE stdhandle;
      DWORD nStdHandle;
      switch (argv[1][0])
      {
	case L'0':
	  nStdHandle = STD_INPUT_HANDLE;
	  break;
	case L'1':
	  nStdHandle = STD_OUTPUT_HANDLE;
	  break;
	case L'2':
	  nStdHandle = STD_ERROR_HANDLE;
	  break;
	default:
	  fwprintf (stderr, L"%ls: Unknown handle '%ls'\n", argv[0], argv[1]);
	  return 1;
      }

      stdhandle = GetStdHandle (nStdHandle);
      if (stdhandle == INVALID_HANDLE_VALUE)
        {
	  fwprintf (stderr, L"%ls: Failed getting standard handle %ls: %lu\n",
	      argv[0], argv[1], GetLastError ());
	  return 2;
	}
      else if (stdhandle == NULL)
	{
	  if (wcscmp (argv[2], L"<CLOSED>"))
	    {
	      fwprintf (stderr,
			L"%ls: Handle %ls name '%ls' != expected '%ls'\n",
			argv[0], argv[1], L"<CLOSED>", argv[2]);
	      return 4;
	    }
	}
      else
        {
	  LPWSTR buf, win32path;
	  buf = malloc (65536);
	  if (!GetFinalPathNameByHandleW (stdhandle, buf,
					  65536 / sizeof (WCHAR),
					  FILE_NAME_OPENED|VOLUME_NAME_DOS))
	    {
	      POBJECT_NAME_INFORMATION pinfo = (POBJECT_NAME_INFORMATION) buf;
	      ULONG len;
	      NTSTATUS status = NtQueryObject (stdhandle, ObjectNameInformation,
					       pinfo, 65536, &len);
	      if (!NT_SUCCESS (status))
		{
		  fwprintf (stderr,
		      L"%ls: NtQueryObject for handle %ls failed: 0x%08x\n",
		      argv[0], argv[1], status);
		  free (buf);
		  return 3;
		}

	      pinfo->Name.Buffer[pinfo->Name.Length / sizeof (WCHAR)] = L'\0';
	      win32path = pinfo->Name.Buffer;
	    }
	  else
	    {
	      static const WCHAR prefix[] = L"\\\\?\\";
	      win32path = buf;
	      if (!wcsncmp (win32path, prefix,
			    sizeof (prefix) / sizeof (WCHAR) - 1))
		win32path += sizeof (prefix) / sizeof (WCHAR) - 1;
	    }

	  if (wcscmp (win32path, argv[2]))
	    {
	      fwprintf (stderr,
			L"%ls: Handle %ls name '%ls' != expected '%ls'\n",
			argv[0], argv[1], win32path, argv[2]);
	      free (buf);
	      return 4;
	    }
	  free (buf);
	}
    }
  else
    {
      fwprintf (stderr, L"%ls: Unknown handle '%ls'\n", argv[0], argv[1]);
      return 1;
    }
  return 0;
}
