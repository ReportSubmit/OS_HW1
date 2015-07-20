// ConsoleApplication6.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ConsoleApplication6.h"
#include "stdint.h"
#include "Strsafe.h"
#include "OleAuto.h"
#include <atlstr.h>


///Unicode and ANSI names StringCbVPrintfW (Unicode) and StringCbVPrintfA (ANSI)


/**
* @brief
* @param      
* @see        
* @remarks    
* @code               
* @endcode    
* @return     
**/
void print(_In_ const char* fmt, _In_ ...)
{
	char log_buffer[2048];
	va_list args;

	va_start(args,fmt);
	HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
	if (S_OK != hRes)
	{
		fprintf(
			stderr,
			"%s, StringCbVPrintfA() failed. res = 0x%08x",
			__FUNCTION__,
			hRes
			);
		return;
	}

	OutputDebugStringA(log_buffer);
	fprintf(stdout, "%s \n", log_buffer);
}


/**
* @brief
* @param      
* @see        
* @remarks    
* @code               
* @endcode    
* @return     
**/

CStringW UTF8toUTF16(const CStringA& utf8)
{
   CStringW utf16;
   int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
   if (len>1)
   { 
      wchar_t *ptr = utf16.GetBuffer(len-1);
      if (ptr) MultiByteToWideChar(CP_UTF8, 0, utf8, -1, ptr, len);
      utf16.ReleaseBuffer();
   }
   return utf16;
}

bool AsciiToUnicode(const char * szAscii, wchar_t * szUnicode)
{
	int len, i;
	if((szUnicode == NULL) || (szAscii == NULL))
		return false;
	len = strlen(szAscii);
	for(i=0;i<len+1;i++)
		*szUnicode++ = static_cast<wchar_t>(*szAscii++);
	return true;
}

bool UnicodeToAscii(const wchar_t * szUnicode, char * szAscii)
{
	int len, i;
	if((szUnicode == NULL) || (szAscii == NULL))
		return false;
	len = wcslen(szUnicode);
	for(i=0;i<len+1;i++)
		*szAscii++ = static_cast<char>(*szUnicode++);
	return true;
}

int UnicodeToUtf8(TCHAR* pUnicode, char** pUtf8)
{
	int len = ::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pUnicode, -1, NULL, 0, NULL, NULL); 
	*pUtf8 = new char[len];
	::WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pUnicode, -1, *pUtf8, len, NULL, NULL);

	return len;
}

char* ANSIToUTF8(const char * pszCode)
{
	int		nLength, nLength2;
	BSTR	bstrCode; 
	char*	pszUTFCode = NULL;

	nLength = MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen((LPCWSTR)pszCode), NULL, NULL); 
	bstrCode = SysAllocStringLen(NULL, nLength); 
	MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen((LPCWSTR)pszCode), bstrCode, nLength);

	nLength2 = WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, 0, NULL, NULL); 
	pszUTFCode = (char*)malloc(nLength2+1); 
	WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, nLength2, NULL, NULL); 

	return pszUTFCode;
}


char* UTF8ToANSI(const char *pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;

	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen((LPCWSTR)pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen((LPCWSTR)pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];

	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);

	return pszAnsi;
}


int is_file_existsW(TCHAR * file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData) ;
	int found = handle != INVALID_HANDLE_VALUE;
	if(found) 
	{
		//FindClose(&handle); this will crash
		FindClose(handle);
	}
	return found;
}


bool create_bob_txt()
{
	// current directory 를 구한다.
	wchar_t *buf=NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);
	if (0 == buflen)
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		return false;
	}

	buf = (PWSTR) malloc(sizeof(WCHAR) * buflen);
	if (0 == GetCurrentDirectoryW(buflen, buf))
	{
		print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
		free(buf);
		return false;
	}

	// current dir \\ bob.txt 파일명 생성
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\bob.txt",
		buf)))
	{  
		print("err, can not create file name");
		free(buf);
		return false;
	}


	if (true == is_file_existsW(file_name))
	{
		::DeleteFileW(file_name);
	}

	// 파일 생성
	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL); 

	if(file_handle == INVALID_HANDLE_VALUE)
	{                                              
		print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
		return false;
	}


	// 파일에 데이터 쓰기
	DWORD bytes_written = 0;
	wchar_t string_buf[1024];
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}
	char *pUtf8 =NULL;
	int len = UnicodeToUtf8(string_buf,&pUtf8);

	if (!WriteFile(file_handle, pUtf8, len, &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// 영어로 쓰기
	if (!SUCCEEDED(StringCbPrintfW(
		string_buf,
		sizeof(string_buf),
		L"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	free(pUtf8);

	*pUtf8 =NULL;
	len = UnicodeToUtf8(string_buf,&pUtf8);

	if (!WriteFile(file_handle, string_buf, wcslen(string_buf), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}




	char string_bufa[1024];


	if (!SUCCEEDED(StringCbPrintfA(
		string_bufa,
		sizeof(string_bufa),
		"동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라만세")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	free(pUtf8);

	pUtf8 =NULL;
	pUtf8 =ANSIToUTF8(string_bufa);
	len = strlen(pUtf8);


	if (!WriteFile(file_handle, pUtf8, len, &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	// 영어로 쓰기
	if (!SUCCEEDED(StringCbPrintfA(
		string_bufa,
		sizeof(string_bufa),
		"All work and no play makes jack a dull boy.")))
	{
		print("err, can not create data to write.");
		CloseHandle(file_handle);
		return false;
	}

	free(pUtf8);

	pUtf8 =NULL;
	pUtf8 =ANSIToUTF8(string_bufa);
	len = strlen(pUtf8);


	if (!WriteFile(file_handle, string_bufa, strlen(string_bufa), &bytes_written, NULL))
	{
		print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
		CloseHandle(file_handle);
		return false;
	}

	free(pUtf8);

	// 파일 닫기
	CloseHandle(file_handle);

	wchar_t copy_file_name[260];

	if (!SUCCEEDED(StringCbPrintfW(
		copy_file_name,
		sizeof(copy_file_name),
		L"%ws\\bob2.txt",
		buf)))
	{  
		print("err, can not copy file name");
		free(buf);
		return false;
	}
	free(buf); buf = NULL;

	/* BOB.txt -> BOB2.txt 파일 복사 */
	if(!CopyFileW(file_name,copy_file_name,false)){
		print("CopyFileW fail");
		return false;
	}


	/* ReadFile()을 통한 읽기 후 출력*/

	HANDLE read_file_handle = CreateFileW(
		copy_file_name,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL); 

	if(read_file_handle == INVALID_HANDLE_VALUE)
	{                                              
		print("err, CreateFile(path=%ws), gle=0x%08x", read_file_handle, GetLastError());
		return false;
	}

	char read_buf[512];
	char buf_noNull[512];
	char* ansi_buf;

	DWORD result=0;
	
	ReadFile(read_file_handle,read_buf,sizeof(char)*512,&result,NULL);
	
	// read_buf의 글자 사이사이에 있는 NULL값 제거 ( UTF8ToANSI 에서  strlen을 사용하기 때문)
	int j=0;
	for(int i=0;i<256;i++){
		if(read_buf[i] != '\0'){
			buf_noNull[j++] = read_buf[i]; 
		}
	}
	buf_noNull[j]='\0';
	
	// 출력
	ansi_buf=UTF8ToANSI(buf_noNull);
	printf("%s\n",ansi_buf);

	CloseHandle(read_file_handle);


	/* MMIO()을 통한 읽기 후 출력*/

	read_file_handle = CreateFileW(
		copy_file_name,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL); 

	if(read_file_handle == INVALID_HANDLE_VALUE)
	{                                              
		print("err, CreateFile(path=%ws), gle=0x%08x", read_file_handle, GetLastError());
		return false;
	}


	HANDLE file_map = CreateFileMapping(
                            read_file_handle,
                            NULL,
                            PAGE_READONLY,
                            0,
                            0,
                            NULL
                            );
    if (NULL == file_map)
    {
        print("err, CreateFileMapping(%ws) failed, gle = %u", file_name, GetLastError());
        CloseHandle(read_file_handle);
        return false;
    }
 
    PCHAR file_view = (PCHAR) MapViewOfFile(
                                    file_map,
                                    FILE_MAP_READ,
                                    0,
                                    0,
                                    0
                                    );
    if(file_view == NULL)
    {
        print("err, MapViewOfFile(%ws) failed, gle = %u", file_name, GetLastError());
       
        CloseHandle(file_map);
        CloseHandle(read_file_handle);
        return false;
    }

	// read_buf의 글자 사이사이에 있는 NULL값 제거 ( UTF8ToANSI 에서  strlen을 사용하기 때문)
	
	j=0;
	for(int i=0;i<256;i++){
		if(read_buf[i] != '\0'){
			buf_noNull[j++] = file_view[i]; 
		}
	}
	buf_noNull[j]='\0';
	
	// 출력
	ansi_buf=UTF8ToANSI(buf_noNull);
	printf("%s\n",ansi_buf);


	UnmapViewOfFile(file_view);
    CloseHandle(file_map);
	CloseHandle(read_file_handle);

	/* BOB.txt 파일 삭제*/



	::DeleteFileW(file_name);

	return true;

}


int main(int argc, char **argv) {

	create_bob_txt();
	return 0;
}
