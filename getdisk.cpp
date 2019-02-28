#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <cstring>
#include <iostream>
#include <typeinfo>
#include <regex>
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

struct udiskRecord
{
	TCHAR udiskSubkeysName[255];
	DWORD subkeysCount;
	//int udiskSubkeysNum;
	TCHAR serialNum[255];
	//FILETIME ftLastWriteTime;
	SYSTEMTIME ymdhms;
	TCHAR FriendlyName[255];
};

using namespace std;

void getSerialNum(TCHAR udiskName[100],udiskRecord *udr) //ȥ�����к�ǰ���ַ���������
{
	
	wregex pattern(_T("(\\w*(?=&0$))"));  //������ʽ,ƥ��0&���ַ�����ĩβ������ĸ�����»��ߵ��ַ�
	wcmatch match;  //���յĶ���
	std::regex_constants::match_flag_type m = std::regex_constants::match_prev_avail;  //flags
	if(regex_search(udiskName,match,pattern,m))
		setlocale(LC_CTYPE, "");    //û��������������ַ�
		wcscpy_s((udr->serialNum),(match.str().c_str())); 
	}
 
//void GetChildKey(HKEY hKey)
void GetChildKey(HKEY hKey,udiskRecord *udr) //���ؽṹ��
{ 
  
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
    DWORD    cbName;                   // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 
	SYSTEMTIME lsTime;             //ת��ΪSYSTEM��Ĵ�Žṹ��
	DWORD i, retCode; 
    TCHAR achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
	//struct udiskRecord udr[30];  //����ṹ������ָ��
	retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 
     // Enumerate the subkeys, until RegEnumKeyEx fails.
        if (cSubKeys)
   		{
			udr->subkeysCount=cSubKeys;   //���Ӽ��������ṹ��
		
		for (i=0; i<cSubKeys; i++) 
        { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
				 /**wcscpy(_tcscpy)��������TCHAR���ַ�����ֵ
				����strcpy����������char�ַ�����ֵ�����������(=)ֻ�ܸ���
				�ַ���������ĸ�ĵ�ַ����strcpy�Ǹ��������ַ���**/
				//printf("%s",ftLastWriteTime);
				//FileTimeToSystemTime(&ftLastWriteTime,&udr->ymdhms);
								
				wcscpy_s((udr->udiskSubkeysName),achKey); 
				udr++;
				
			}
		}
	}
}

void __cdecl _tmain(void)
{
   long ret;
   HKEY hTestKey;   //���USBSTOR�ľ��
   HKEY     hKey2;   //USBSTOR+��ȡ�Ӽ��ľ��
   HKEY hKey3;  // //USBSTOR+��ȡ�Ӽ����Ӽ��ľ��
   TCHAR tchData[1024];  //����ֵ������
   DWORD dwType = REG_SZ;  //����װ��ȡ���������͵�һ������
   DWORD dwSize;  //�������ߴ�
   dwSize=sizeof(tchData);
   struct udiskRecord *udr1;
   udr1=new udiskRecord[30];  //ָ����ʹ��ǰ��Ӧ�ø�����ʼ��һ���ڴ�飬��Ȼ�ᱨ�����ҳ�ΪҰָ�룬��C������MALLOC,��C++������new
   struct udiskRecord *udr2; 
   udr2=new udiskRecord[30]; 
   struct udiskRecord *udr3;
   udr3=new udiskRecord[30]; 
   LPTSTR openkey=TEXT("SYSTEM\\ControlSet001\\Enum\\USBSTOR\\");  

	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Enum\\USBSTOR"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS)
	  {
		GetChildKey(hTestKey,udr1);
		printf("���ĵ�����%d��U�̼�¼\n",udr1->subkeysCount);
		DWORD m=udr1->subkeysCount;  //USBSTOR�Ӽ���������������m

		for(DWORD i=0;i<m;i++)
		 {
			LPTSTR s=(udr1->udiskSubkeysName);   //USBSTOR���Ӽ�
			LPTSTR sbig=new TCHAR[2000];   //���뽨��һ���µ��ڴ����򣬲ſ��������ַ���
			wcscat(wcscpy(sbig,openkey),s);  //�����ַ���
	
			if (RegOpenKeyEx( HKEY_LOCAL_MACHINE,sbig,0,KEY_READ,&hKey2) == ERROR_SUCCESS)
				{
					GetChildKey(hKey2,udr2);
					getSerialNum(udr2->udiskSubkeysName,udr2);
					DWORD n=udr2->subkeysCount;  //USBSTOR�µ��Ӽ����Ӽ���U��ͬһ�ͺŲ�ͬ���кŵ��Ӽ���������������K���������ͬһ�ͺŵ�U�̣����Ի�ȡͬһ�ͺŲ�ͬ���кŵ�U��
					for(DWORD j=0;j<n;j++)
						{
							LPTSTR t=udr2->udiskSubkeysName;
							wcscat(wcscat(sbig,_TEXT("\\")),t);   //�����Ӽ����Ӽ����ַ���
							if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,sbig,0,KEY_READ,&hKey3) == ERROR_SUCCESS)
								{
									ret=RegQueryValueEx(hKey3,_T("FriendlyName"),NULL,NULL,(LPBYTE)tchData,&dwSize);
				   
									if (ret==ERROR_SUCCESS)
										{		
											 wcscpy_s((udr2->FriendlyName),tchData); 
										}
										else 
										{
											printf("error");
										}
								}
							
						
						}
				}
			 _tprintf(TEXT("[%d]%s\n"),i+1,udr1->udiskSubkeysName);
			setlocale(LC_CTYPE, ".936");   //��ӡ���ġ����кš�Ҫ�������������
			wprintf(_T("%ws %s\t"),_T("���к�:"),udr2->serialNum);
			wprintf(_T("%ws %s\n"),_T("��������:"),udr2->FriendlyName);
			wprintf(_T("%ws %s\t"),_T("�״�ʹ��ʱ��:"),"");
			wprintf(_T("%ws %s\n"),_T("���ʹ��ʱ��:"),"");
			printf("--------------------------------------------------------------------\n");
		    udr1++;
		    udr2++;  
		   
		  }
	  RegCloseKey(hTestKey);
   }
   //delete[] udr1;
   system("PAUSE");
}