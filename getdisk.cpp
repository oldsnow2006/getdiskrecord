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

void getSerialNum(TCHAR udiskName[100],udiskRecord *udr) //去除序列号前后字符的正则函数
{
	
	wregex pattern(_T("(\\w*(?=&0$))"));  //正则表达式,匹配0&在字符串最末尾，带字母数字下划线的字符
	wcmatch match;  //接收的对象
	std::regex_constants::match_flag_type m = std::regex_constants::match_prev_avail;  //flags
	if(regex_search(udiskName,match,pattern,m))
		setlocale(LC_CTYPE, "");    //没有输出不了中文字符
		wcscpy_s((udr->serialNum),(match.str().c_str())); 
	}
 
//void GetChildKey(HKEY hKey)
void GetChildKey(HKEY hKey,udiskRecord *udr) //返回结构体
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
	SYSTEMTIME lsTime;             //转换为SYSTEM后的存放结构体
	DWORD i, retCode; 
    TCHAR achValue[MAX_VALUE_NAME]; 
    DWORD cchValue = MAX_VALUE_NAME; 
	//struct udiskRecord udr[30];  //定义结构体数组指针
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
			udr->subkeysCount=cSubKeys;   //把子键数赋给结构体
		
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
				 /**wcscpy(_tcscpy)用来复制TCHAR等字符串的值
				，而strcpy是用来复制char字符串的值。复制运算符(=)只能复制
				字符串的首字母的地址，而strcpy是复制整个字符串**/
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
   HKEY hTestKey;   //获得USBSTOR的句柄
   HKEY     hKey2;   //USBSTOR+获取子键的句柄
   HKEY hKey3;  // //USBSTOR+获取子键的子键的句柄
   TCHAR tchData[1024];  //返回值缓冲区
   DWORD dwType = REG_SZ;  //用于装载取回数据类型的一个变量
   DWORD dwSize;  //缓冲区尺寸
   dwSize=sizeof(tchData);
   struct udiskRecord *udr1;
   udr1=new udiskRecord[30];  //指针在使用前都应该给他初始化一个内存块，不然会报错，并且成为野指针，在C里面用MALLOC,在C++里面用new
   struct udiskRecord *udr2; 
   udr2=new udiskRecord[30]; 
   struct udiskRecord *udr3;
   udr3=new udiskRecord[30]; 
   LPTSTR openkey=TEXT("SYSTEM\\ControlSet001\\Enum\\USBSTOR\\");  

	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Enum\\USBSTOR"),0,KEY_READ,&hTestKey) == ERROR_SUCCESS)
	  {
		GetChildKey(hTestKey,udr1);
		printf("您的电脑有%d个U盘记录\n",udr1->subkeysCount);
		DWORD m=udr1->subkeysCount;  //USBSTOR子键的数量赋给变量m

		for(DWORD i=0;i<m;i++)
		 {
			LPTSTR s=(udr1->udiskSubkeysName);   //USBSTOR的子键
			LPTSTR sbig=new TCHAR[2000];   //必须建立一个新的内存区域，才可以连接字符串
			wcscat(wcscpy(sbig,openkey),s);  //连接字符串
	
			if (RegOpenKeyEx( HKEY_LOCAL_MACHINE,sbig,0,KEY_READ,&hKey2) == ERROR_SUCCESS)
				{
					GetChildKey(hKey2,udr2);
					getSerialNum(udr2->udiskSubkeysName,udr2);
					DWORD n=udr2->subkeysCount;  //USBSTOR下的子键的子键，U盘同一型号不同序列号的子键的数量赋给变量K，如果存在同一型号的U盘，可以获取同一型号不同序列号的U盘
					for(DWORD j=0;j<n;j++)
						{
							LPTSTR t=udr2->udiskSubkeysName;
							wcscat(wcscat(sbig,_TEXT("\\")),t);   //连接子键的子键的字符串
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
			setlocale(LC_CTYPE, ".936");   //打印中文“序列号”要设置这个，不懂
			wprintf(_T("%ws %s\t"),_T("序列号:"),udr2->serialNum);
			wprintf(_T("%ws %s\n"),_T("生产厂商:"),udr2->FriendlyName);
			wprintf(_T("%ws %s\t"),_T("首次使用时间:"),"");
			wprintf(_T("%ws %s\n"),_T("最后使用时间:"),"");
			printf("--------------------------------------------------------------------\n");
		    udr1++;
		    udr2++;  
		   
		  }
	  RegCloseKey(hTestKey);
   }
   //delete[] udr1;
   system("PAUSE");
}