
/**
* @file  log.h
* @brief 常见类型定义
* @author       Vincent
* @date     2015-5-24
* @version  A001
* @copyright Vincent
*/
#pragma  once
#ifndef LOG_H  
#define LOG_H  



/** log文件路径*/
#define LOG_FILE_NAME "D:\\log.txt"

/** 启用开关*/
#define LOG_ENABLE

#include <fstream>  
#include <string>  
#include <ctime>  
#include <windows.h>  

using namespace std;
/**
* @brief 用于输出log文件的类1TYPENUM3.
*/
class CLog
{
public:
	/**
	* @brief 获取Log文件的路径
	* @param[in] szPath 路径指针
	* @return 无返回值
	*/
	static void GetLogFilePath(char* szPath)
	{
		/**
		* @brief getmoduleFileName.
		*/
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		ZeroMemory(strrchr(szPath, ('\\')), strlen(strrchr(szPath, ('\\')))*sizeof(char));
		//strcat(szPath, "\\");
		memset(szPath, 0, sizeof(szPath));
		strcat(szPath,LOG_FILE_NAME);
	}
	/**
	* @brief 输出一个内容，可以是字符串(ascii)、整数、浮点数、布尔、枚举
	* 格式为：[2011-11-11 11:11:11] aaaaaaa并换行
	* @param[in] x 要写入log的内容
	* @return 无返回值
	*/
	template <class T>
	static void WriteLog(T x)
	{
		//if (!StartOrNot)
		//{
		//	return;
		//}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << x << endl;
		fout.close();
	}
	/**
	* @brief 输出2个内容，以等号连接。一般用于前面是一个变量的描述字符串，后面接这个变量的值
	* @param[in] x1 要写入log的内容
	* @param[in] x2 要写入log的内容  
	* @return 无返回值
	*/
	template<class T1, class T2>
	static void WriteLog2(T1 x1, T2 x2)
	{
		if (!StartOrNot)
		{
			return;
		}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << x1 << " = " << x2 << endl;
		fout.close();
	}
	/**
	* @brief 输出一行当前函数开始的标志,宏传入__FUNCTION__
	* @param[in] x要写入log的内容
	* @return 无返回值
	*/
	template <class T>
	static void WriteFuncBegin(T x)
	{
		if (!StartOrNot)
		{
			return;
		}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);	
		remove(szPath);
			//DeleteFile(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << "	--------------------" << x << "  Begin--------------------" << endl;
		fout.close();
	}
	/**
	* @brief 输出一行当前函数结束的标志，宏传入__FUNCTION__
	* @param[in] x要写入log的内容
	* @return 无返回值
	*/
	template <class T>
	static void WriteFuncEnd(T x)
	{
		if (!StartOrNot)
		{
			return;
		}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << "--------------------" << x << "  End  --------------------" << endl;
		fout.close();
	}
	/**
	* @brief 打开log输出开关
	*/
	static void Open()
	{
		StartOrNot = true;
	}
	/**
	* @brief 关闭log输出开关
	*/
	static void End()
	{
		StartOrNot = false;
	}
	/** 定义开关变量 StartOrNot*/
	static bool StartOrNot;
private:
	/**
	* @brief 获取本地时间，格式如"[2011-11-11 11:11:11] 
	*/
	static string GetSystemTime()
	{
		time_t tNowTime;
		time(&tNowTime);
		tm tLocalTime ;
		localtime_s(&tLocalTime, &tNowTime);
		char szTime[30] = { '\0' };
		strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", &tLocalTime);
		string strTime = szTime;
		return strTime;
	}

};

#ifdef LOG_ENABLE

/** 输出Log的宏定义1，括号内可以是字符串(ascii)、整数、浮点数、bool等*/
#define LOG(x)			 CLog::WriteLog(x);		
/** 输出Log的宏定义2，x1,x2可以是字符串(ascii)、整数、浮点数、bool等*/
#define LOG2(x1,x2)		 CLog::WriteLog2(x1,x2);
/** 输出当前所在函数名的宏定义*/
#define LOG_FUNC		 LOG(__FUNCTION__)	
/** 输出当前行号的宏定义*/
#define LOG_LINE		 LOG(__LINE__)			
/** 开始输出标志的宏定义形式如：[时间]"------------FuncName  Begin------------"*/
#define LOG_FUNC_BEGIN   CLog::WriteFuncBegin(__FUNCTION__);	
/** 开始输出标志的宏定义形式如：[时间]"------------FuncName  End------------"*/
#define LOG_FUNC_END     CLog::WriteFuncEnd(__FUNCTION__);		
/** 打开Log输出开关宏定义*/
#define LOG_OPEN         CLog::Open();
/** 关闭Log输出开关宏定义*/
#define LOG_END          CLog::End();
#else

#define LOG(x)				
#define LOG2(x1,x2)		
#define LOG_FUNC		
#define LOG_LINE		
#define LOG_FUNC_BEGIN  
#define LOG_FUNC_END  
#define LOG_OPEN
#define LOG_END 
#endif

#endif 
/** @}*/ // 自动注释文档范例