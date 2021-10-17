// httplib.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

#include <pthread.h>
#include <unistd.h>

#include "select.h"
#include "ctb-0.16/ctb.h"
#include "httplib.h"

#define __LINUX__

using namespace std;

vector<unsigned char> encode(const string s)
{
    string str = s;
    vector<unsigned char> buff;

    int len = str.length();
    buff.resize(len);
    memcpy(&buff[0], str.c_str(), len);
    return buff;
}





//Form Cloud, String format is LPCWSTR(wchar_t*).
//Function: const char string to wchar_t string
//param: const char string
//return: wchar_t string pointer

#ifdef __LINUX__

std::string UnicodeToUTF8(wchar_t* lpszWideStr)
{
    int nlen = wcslen(lpszWideStr)+1;
    
	char* buffer = new char[nlen + 1];
	
	wcstombs(buffer, lpszWideStr, nlen);
	
	printf("wctmb over\n");
	
	std::string multStr = buffer;
	
	delete buffer;
    return multStr;
}



wstring UTF82WCS(const char* szU8)
{
	int wcsLen;
	
	wcsLen = strlen(szU8);
	
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	printf("UTF82WCS 01 over\n");
	mbstowcs(wszString, szU8, 2*wcsLen);
	printf("UTF82WCS 02 over\n");
    wstring unicodeString(wszString);
    printf("UTF82WCS 03 over\n");
	
	delete wszString;
    return unicodeString;
}


#else
	
std::string UnicodeToUTF8(LPCWSTR lpszWideStr)
{
    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpszWideStr, -1,
        nullptr, 0, nullptr, nullptr);
    printf("wctmb over\n");
    char* buffer = new char[nLen + 1];
    ::ZeroMemory(buffer, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, lpszWideStr, -1,
        buffer, nLen, nullptr, nullptr);

    std::string multStr = buffer;
    delete[] buffer;
    return multStr;
}



wstring UTF82WCS(const char* szU8)
{
    //预转换，得到所需空间的大小;
    int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), NULL, 0);
	
    //分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
    wchar_t* wszString = new wchar_t[wcsLen + 1];
    printf("UTF82WCS 1 over\n");
    //转换
    ::MultiByteToWideChar(CP_UTF8, NULL, szU8, strlen(szU8), wszString, wcsLen);
    printf("UTF82WCS 2 over\n");
    //最后加上'\0'
    wszString[wcsLen] = '\0';

    wstring unicodeString(wszString);
    printf("UTF82WCS 3 over\n");
	
	delete wszString;
	
    return unicodeString;
}

#endif



//GateWay Body
int main(int argc,char* argv[])
{
	char* tmp = "Wellocom!\n";
	//Cloud login
	httplib::Client cli("http://139.9.40.127:5000");
    printf("%s\n", tmp);
	printf("Name:15280019206\n");
	printf("PassWord:******\n");
	//end of Cloud login
	
	//uart init
    int baudrate = 115200;
#ifdef __LINUX__
	string devname = "/dev/ttyACM0";
	//string devname = "/dev/ttyS0";
#else
	string devname = ctb::COM1;
#endif
    string eos = "\r\n";
    string protocol = "8N1";
	int timeout = 100;
    bool showAvailablePorts = false;
	bool uart_status = false;
	
	ctb::SerialPort* serialPort = new ctb::SerialPort();
	
	if( serialPort->Open( devname.c_str(), baudrate, 
					protocol.c_str(),
					ctb::SerialPort::NoFlowControl ) >= 0 ) {
	  uart_status = true;
	}
	
    if( ! uart_status ) {
	   cout << "Cannot open " << devname.c_str() << endl;
	   return -1;
    }
	//end of uart init
	
	//var init
    char receiveBuf[ 512 ];
	string str_A = UnicodeToUTF8(L"{\"head\":\"mind.in\",\"value\":\"{\\\"mind_in_val\\\":\\\"PTJieFeng_Test\\\",\\\"mind_in_detail\\\":{\\\"命令\\\":\\\"增\\\",\\\"内容\\\":[\\\"EVENT\\\",\\\"OPEN\\\",\\\"Sat Oct 16 11:07:48 2021\\\"],\\\"async\\\":1}}\",\"sid\":128128}");
	string str_B = UnicodeToUTF8(L"{\"head\":\"mind.in\",\"value\":\"{\\\"mind_in_val\\\":\\\"PTJieFeng_Test\\\",\\\"mind_in_detail\\\":{\\\"命令\\\":\\\"显示\\\",\\\"每页数据量\\\":20,\\\"页数\\\":1,\\\"async\\\":1,\\\"timeout\\\":10}}\",\"sid\":128128}");
	
	//loop task
    for(;;) {
			
#ifdef __LINUX__
		sleep(2);
#else
		Sleep(2000);
#endif

		// clear buffer
		for(int i=0; i<128; i++) {
			receiveBuf[i] = '\0';
		}
		
		//do uart rx data read
		int readed = 0;
		string line_get;
		do {
			//ctb::sleepms(100);
			// in case of an event driven GUI you better use a non blocking
			// Read(...) in your idle function. Here we have to wait for the 
			// response before we send another user command...
			readed = serialPort->Readv( receiveBuf, 
							 sizeof( receiveBuf ) - 1, 
							 timeout);
							 
			// something received?
			if( readed > 0 ) {
				line_get = receiveBuf;		//long string problem!!!
			}
		} while( readed > 0 );
		/*
		//display back to uart(or store to file)
		if( (line_get.empty() == 0) ) {
			line_get += eos;
			// send data throughout the connected device independent of the typ
			if( serialPort->Writev( (char*)line_get.c_str(),
							line_get.size(),
							timeout) != line_get.size() ) {
			  cout << "Incomplete data transmission" << endl;
			}
		}
		*/
		
		int sent_flag = 0xFE;
		string line_set;
		sent_flag = string_select((char*)line_get.c_str());
		
		
		if( sent_flag == 0 ) {
			
			string_gen((char*)str_A.c_str());
			/*
			if( (str_A.empty() == 0) ) {
					str_A += eos;
					// send data throughout the connected device independent of the typ
					if( serialPort->Writev( (char*)str_A.c_str(),
									str_A.size(),
									timeout) != str_A.size() ) {
					  cout << "Incomplete data transmission" << endl;
					}
				}
			*/
			printf("utf8 over\n");
			
			if (auto res = cli.Post("/ui_routine", str_A.c_str(), "application/json;")) {
				printf("post over\n");
				std::cout << res->status << std::endl;
				if (res->status == 200) {
					string rbody = res->body;
					
					line_set = rbody;
					
					wstring out_s3 = UTF82WCS(rbody.c_str());//string(.c_str())
					//std::wcout.imbue(locale("CHS"));
					setlocale(LC_ALL,"");
					printf("imbue over\n");
					wprintf(L"%ls\n", out_s3.c_str());//debug
					//std::cout << str_tx << std::endl;
				}
				/*
				if( (line_set.empty() == 0) ) {
					line_set += eos;
					// send data throughout the connected device independent of the typ
					if( serialPort->Writev( (char*)line_set.c_str(),
									line_set.size(),
									timeout) != line_set.size() ) {
					  cout << "Incomplete data transmission" << endl;
					}
				}*/
			}
			else {
				auto err = res.error();
			}
		}
		else {
			cout << "Fog Sleeping!\n" << endl; //debug
		}
		
		//cout << endl << "Your input >";
		//getline( cin, line ); // read the string to send
		// add some defined EOS (end of string sequence or character)

	} // while( true )

	serialPort->Close();
	delete serialPort;
	//end of uart part
	
    return 0;
}
