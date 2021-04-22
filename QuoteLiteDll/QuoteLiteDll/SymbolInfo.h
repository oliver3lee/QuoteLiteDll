#pragma once
#include <iostream>
#include <map>
#include <atlstr.h>
#include <thread>
#include <vector> 
#include <string>
#include <fstream>
#include <io.h>
#include <mutex>
#include <unordered_map>
#include <memory>

#ifdef CLASS_DECLSPEC_EXPORTS
#define CLASS_DECLSPEC __declspec(dllexport)
#else
#define CLASS_DECLSPEC __declspec(dllimport)
#endif

#define WM_READFILEFINISHONE WM_USER + 102
#define WM_READFILEFINISHTWO WM_USER + 103
#define WM_READFILEERRORMESSAGE WM_USER + 104

using namespace std;

class  CLASS_DECLSPEC CSymbolInfo
{
private:
	friend class CQuoteManager;
	typedef struct TickInfoStruct	// 檔案儲存陣列結構
	{
		int iTime;		// 時間
		double dPrice;	// 價錢
		int	iVolume;	// 成交量
	}TickInfo;

	vector<TickInfo> m_vecPerTransaction;				// 每個時段交易資訊
	vector<shared_ptr<CSymbolInfo>> m_vecSimilarSymbol;	// 相似商品
	map<double, int> m_mapPriceAndVolumeArray;			// 分價量明細

	string m_strSymbolID;	// 商品ID
	double m_dPriceHigh;	// 最高價
	double m_dPriceLow;		// 最低價
	int m_iTotalVolume;		// 總量
	double m_dAvgPrice;		// 均價
	double m_dPriceTotal;	// 總價
	int m_iLikeSymbolCnt;	// 相似商品數
	bool m_bReadFileStatus;	// 檔案是否讀取成功

	void FileToArrayAndGetSimbleInformation(const string &strFileName, const string &strPath);	// 讀檔存陣列
	void FindSimilarItem(std::unordered_map<string, shared_ptr<CSymbolInfo>>& File);			// 尋找相似商品
	void PopToken(char* pszToken, int iTokenBufLen, char* const pszData);						// 每列檔案內容切割
	void CountAvgPrice();																		// 計算平均價
	void CountPriceOfValume();																	// 計算分價量明細

	CString m_csErrorMessage;	// 讀檔失敗錯誤訊息

	CSymbolInfo();
public:
	~CSymbolInfo();
	//void(*pDFHookCallbackFunc)(void* pVoid) = NULL;	// Callback function回傳檔案讀取進度
};