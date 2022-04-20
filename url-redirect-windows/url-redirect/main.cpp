#include <Windows.h>
#include <future>
#include <strsafe.h>

#define USE_CLIPBOARD_FOR_SHARING_DATA 0

int wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int cmdShow) {

	UINT messageIdentifier = RegisterWindowMessage(L"com.4D.Protocol");

	if (messageIdentifier) {

		std::wstring URL = std::wstring(lpCmdLine, wcslen(lpCmdLine));

		if (wcslen(lpCmdLine)) {

			size_t len = URL.length();

			HWND hwnd = FindWindow(L"PROTOCOL_MESSAGE_WINDOW", NULL);

			if (hwnd) {

				WPARAM wparam = 0;
				wparam = (len + 1) * sizeof(wchar_t);
				wchar_t end = (wchar_t)0;

#if USE_CLIPBOARD_FOR_SHARING_DATA

				if (OpenClipboard(NULL)) {

					UINT fmt = RegisterClipboardFormat(L"com.4D.Protocol");
					if (fmt) {

						HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, wparam);

						if (hglbCopy) {

							wchar_t *lptstrCopy = (wchar_t *)GlobalLock(hglbCopy);
							if (lptstrCopy) {
								memcpy(lptstrCopy,
									URL.data(),
									len * sizeof(wchar_t));
								lptstrCopy[len] = end;
								GlobalUnlock(hglbCopy);

								SetClipboardData(fmt, hglbCopy);
							}

						}

					}
					CloseClipboard();
				}
#endif

				HANDLE fmIn = CreateFileMapping(
					INVALID_HANDLE_VALUE,
					NULL,
					PAGE_READWRITE,
					0, 
					wparam,
					L"com.4D.Protocol");

				if (fmIn)
				{

					LPVOID bufIn = MapViewOfFile(fmIn, FILE_MAP_WRITE|FILE_MAP_COPY, 0, 0, 0);

					if (bufIn) {
						try
						{
							unsigned char *p = (unsigned char *)bufIn;

							CopyMemory(p, URL.data(), len * sizeof(wchar_t));
							p += (len * sizeof(wchar_t));
							CopyMemory(p, &end, sizeof(wchar_t));
							p += sizeof(wchar_t);

						}
						catch (...)
						{

						}

						LRESULT result = SendMessageTimeout(hwnd,
							messageIdentifier,
							wparam,
							0,
							SMTO_ABORTIFHUNG,
							5000, NULL);

						UnmapViewOfFile(bufIn);

					}

					CloseHandle(fmIn);
				}
				
			}

		}

	}

	return 0;
}