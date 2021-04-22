#pragma once
#include "SymbolInfo.h"
#include <atomic> 

using namespace std;
class  CLASS_DECLSPEC CQuoteManager
{
public:

	class CTickInfo	// 成交明細規格包
	{
	public:
		CTickInfo(int iTime, double dPrice, int iVolume)
			: m_iTime(iTime)
			, m_dPrice(dPrice)
			, m_iVolume(iVolume)
		{
		}
		~CTickInfo(){}

		int m_iTime;
		double m_dPrice;
		int	m_iVolume;
	private:
		CTickInfo(){}
	};

	class CBasicInfo	// 基本資訊規格包
	{
	public:
		~CBasicInfo(){}
		CBasicInfo(){}

		string m_strSymbolID;
		int m_iTime;
		double m_iPrice;
		int m_iVolume;
		double m_dAvgPrice;
		int m_iTotalVolume;
		double m_dPriceHigh;
		double m_dPriceLow;
		int m_iLikeSymbolCnt;
	};

	class CSimilarSymbolInfo	// 相似商品規格包
	{
	public:
		CSimilarSymbolInfo(string &strSymbolID, double dAvgPrice, int iTotalVolume)
			: m_strSymbolID(strSymbolID)
			, m_dAvgPrice(dAvgPrice)
			, m_iTotalVolume(iTotalVolume)
		{
		}
		~CSimilarSymbolInfo(){}

		string m_strSymbolID;
		double m_dAvgPrice;
		int	m_iTotalVolume;
	private:
		CSimilarSymbolInfo(){}
	};

public:
	int GetFileNameList(vector<string> &vecPackage);
	bool GetBasicInfoPackage(string &strSymbol, CBasicInfo &classPackage);								// 回傳基本欄位所需資訊
	bool GetSimilarCommodityPackage(string &strSymbol, vector<CSimilarSymbolInfo> &vecPackage);			// 相似商品包
	bool GetPriceAndVolumeInfoPackage(string &strSymbol, map<double, int> &package);					// 分價量明細包
	bool CQuoteManager::GetPerTradeDetailInfoPackage(string &strSymbol, vector<CTickInfo> &vecPackage);	// 成交明細包

public:
	CQuoteManager();
	~CQuoteManager();

	void QuoteInintialize(const string &strPath);	// 計算路徑下檔案數
	int ReadSymbolsInfo();							// 讀檔
	int GetFileNum();								// 得到資料夾下檔案數
	int GetCurrentReadFileProgress();				// 得到目前讀檔進度
	bool GetReadFilestatus();						// 檔案是否全部讀取完畢
	bool GetReadBasicInfostatus();					// 基本資訊是否全部取完畢
	bool GetSymbolStatus(string &strSymbol);		// 檔案讀取成功或失敗

	std::unordered_map<string, shared_ptr<CSymbolInfo>> m_umapFileList;		// 檔案陣列

private:
	string m_path;	// 檔案路徑
	int m_iFileNum;	//檔案數量
	std::vector<std::thread> m_ReadFileThreads;	// 讀檔thread
	
private:
	void ReadFileBasicInfo();						// 讀基本資訊
	void ReadFileExtendInfo();						// 讀相似商品

	std::unordered_map<string, shared_ptr<CSymbolInfo>>::iterator m_iterMapIndex;	// map iIndex
	std::atomic<int>m_iReadBasicInfoIndex{ 0 };		// 第一部分讀檔檔案iIndex
	std::atomic<int>m_iReadExtendInfoIndex{ 0 };	// 第二部分讀檔檔案iIndex
	std::atomic<int>m_iThreadFinishNum{ 0 };		// thread 完成數量

private:
	void ReadingBasicInfoManager();				// 讀檔程序管理
	void ReadingLikeSymbolManager();			// 尋找相似商品程序管理
	void CountFinishThreadNum();				// thread 完成數量計算
	bool AddReadBasicIndex(unordered_map<string, shared_ptr<CSymbolInfo>>::iterator &iterCurrentlyIndex);	// 第一部份讀檔iIndex增加
	bool AddReadExtendIndex(unordered_map<string, shared_ptr<CSymbolInfo>>::iterator &iterCurrentlyIndex);	// 第二部份讀檔iIndex增加

private:
	LARGE_INTEGER m_liPerfFreq;					// CPU頻率
	LARGE_INTEGER m_liPerfRreadFileStart;		// 讀檔到array開始時間
	LARGE_INTEGER m_liPerfRreadSingleFileStart;	// 讀單一文件第二部分資訊開始時間
	LARGE_INTEGER m_liPerfEnd;					// 結束時間
	double m_dCostTimePartOne;					// 第一部份完成時間
	double m_dCostTimePartTwo;					// 第二部分完成時間
	CString m_csUseTime;						// 總花費時間
	bool m_ReadFileFinish;
	mutex m_MutexFileIndex;
	int m_iThreadTotal;
	bool m_bReadFileBasicInfofinish;
};