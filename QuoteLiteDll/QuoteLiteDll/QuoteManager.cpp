#include "QuoteManager.h"
#include <algorithm>

CQuoteManager::CQuoteManager() :m_iFileNum(-1), m_ReadFileFinish(false), m_bReadFileBasicInfofinish(false)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_iThreadTotal = sysInfo.dwNumberOfProcessors;
}

CQuoteManager::~CQuoteManager()
{
	//std::unordered_map<string, shared_ptr<CSymbolInfo>>::iterator iter;

	//for (iter = m_umapFileList.begin(); iter != m_umapFileList.end(); ++iter)
	//{
	//	delete iter->second;
	//}

	//m_umapFileList.clear();
}

void CQuoteManager::QuoteInintialize(const string &strPath)
{
	m_path = strPath;
	m_path = m_path.substr(0, m_path.length() - 5);

	long handle;
	struct _finddata_t fileinfo;
	handle = _findfirst(strPath.c_str(), &fileinfo);
	char szSymbolName[260];
	char* pszNext;
	if (-1 != handle)
	{
		m_iFileNum = 0;
		do
		{
			strncpy_s(szSymbolName, sizeof(szSymbolName), fileinfo.name, strlen(fileinfo.name) - 4);
			//m_umapFileList[szSymbolName] = new CSymbolInfo();
			m_umapFileList[szSymbolName] = shared_ptr<CSymbolInfo>(new CSymbolInfo());
			m_iFileNum++;

		} while (!_findnext(handle, &fileinfo));
	}
	else
	{
		m_iFileNum = -1;
	}
	_findclose(handle);
}

int CQuoteManager::ReadSymbolsInfo()
{
	// 取得目前 CPU frequency
	QueryPerformanceFrequency(&m_liPerfFreq);
	// 取得執行前時間
	QueryPerformanceCounter(&m_liPerfRreadFileStart);

	if (0 < m_iFileNum)
	{
		ReadFileBasicInfo();
	}
	return m_iFileNum;
}

void CQuoteManager::ReadFileBasicInfo()	// 開讀檔Thread
{
	m_iterMapIndex = m_umapFileList.begin();
	if (m_iThreadTotal < m_iFileNum)
	{
		for (int i = 0; i < m_iThreadTotal; ++i)
		{
			m_ReadFileThreads.push_back(thread(&CQuoteManager::ReadingBasicInfoManager, this));
		}
	}
	else
	{
		m_ReadFileThreads.push_back(thread(&CQuoteManager::ReadingBasicInfoManager, this));
	}
}


void CQuoteManager::ReadingBasicInfoManager()	// 讀基本資訊管理
{
	std::unordered_map<string, shared_ptr<CSymbolInfo>>::iterator iterCurrentlyIndex;
	bool bWhetherEnd;
	while (1)
	{
		bWhetherEnd = AddReadBasicIndex(iterCurrentlyIndex);
		if (false == bWhetherEnd)
		{
			iterCurrentlyIndex->second->FileToArrayAndGetSimbleInformation(iterCurrentlyIndex->first, m_path);
		}
		else
		{
			CountFinishThreadNum();
			break;
		}
	}
}


bool CQuoteManager::AddReadBasicIndex(unordered_map<string, shared_ptr<CSymbolInfo>>::iterator &iterCurrentlyIndex)	// 讀基本資訊iIndex++
{
	lock_guard<mutex> mLock(m_MutexFileIndex);
	if (m_umapFileList.end() != m_iterMapIndex)
	{
		iterCurrentlyIndex = m_iterMapIndex++;
		m_iReadBasicInfoIndex++;
		return false;
	}
	else
	{
		return true;
	}
}

void CQuoteManager::CountFinishThreadNum()	// 計算讀基本資訊Thread是否結束
{
	m_iThreadFinishNum++;

	if ((m_ReadFileThreads.size() == m_iThreadFinishNum) || (0 == m_ReadFileThreads.size()))
	{
		// 取得執行後的時間
		QueryPerformanceCounter(&m_liPerfEnd);
		m_dCostTimePartOne = (double)(m_liPerfEnd.QuadPart - m_liPerfRreadFileStart.QuadPart) / (double)m_liPerfFreq.QuadPart;

		m_csUseTime.Format(_T("%.6f"), m_dCostTimePartOne);
		HWND hwnd = ::FindWindow(NULL, _T("QuoteLite"));
		::PostMessage(hwnd, WM_READFILEFINISHONE, 0, (LPARAM)&m_csUseTime);//傳送給任意視窗

		m_bReadFileBasicInfofinish = true;
		ReadFileExtendInfo();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CQuoteManager::ReadFileExtendInfo()
{
	// 取得目前 CPU frequency
	QueryPerformanceFrequency(&m_liPerfFreq);
	// 取得執行前時間
	QueryPerformanceCounter(&m_liPerfRreadFileStart);
	m_iterMapIndex = m_umapFileList.begin();

	if (m_iThreadTotal < m_iFileNum)
	{
		for (int i = 0; i < m_iThreadTotal; ++i)
		{
			m_ReadFileThreads.push_back(thread(&CQuoteManager::ReadingLikeSymbolManager, this));
		}
	}
	else
	{
		m_ReadFileThreads.push_back(thread(&CQuoteManager::ReadingLikeSymbolManager, this));
	}
}


void CQuoteManager::ReadingLikeSymbolManager()	// 讀延伸資訊管理
{
	std::unordered_map<string, shared_ptr<CSymbolInfo>>::iterator iterCurrentlyIndex;
	bool bWhetherEnd;
	while (1)
	{
		bWhetherEnd = AddReadExtendIndex(iterCurrentlyIndex);
		if (false == bWhetherEnd)
		{
			iterCurrentlyIndex->second->FindSimilarItem(m_umapFileList);
		}
		else
		{
			break;
		}
	}
}

bool CQuoteManager::AddReadExtendIndex(unordered_map<string, shared_ptr<CSymbolInfo>>::iterator &iterCurrentlyIndex)	// 讀延伸資訊iIndex++
{
	lock_guard<mutex> mLock(m_MutexFileIndex);
	if (m_umapFileList.end() != m_iterMapIndex)
	{
		iterCurrentlyIndex = m_iterMapIndex++;
		m_iReadExtendInfoIndex++;
		return false;
	}
	else
	{
		m_ReadFileFinish = true;

		// 取得執行後的時間
		QueryPerformanceCounter(&m_liPerfEnd);
		m_dCostTimePartTwo = (double)(m_liPerfEnd.QuadPart - m_liPerfRreadFileStart.QuadPart) / (double)m_liPerfFreq.QuadPart;

		m_csUseTime.Format(_T("%.6f = %.6f + %.6f"), (m_dCostTimePartOne + m_dCostTimePartTwo), m_dCostTimePartOne, m_dCostTimePartTwo);
		HWND hwnd = ::FindWindow(NULL, _T("QuoteLite"));
		::PostMessage(hwnd, WM_READFILEFINISHTWO, 0, (LPARAM)&m_csUseTime);//傳送給任意視窗
		return true;
	}
}

int CQuoteManager::GetCurrentReadFileProgress()	// 提供目前進度
{
	if (false == m_bReadFileBasicInfofinish)
	{
		int iCurrentProgress = m_iReadBasicInfoIndex + 1;
		return iCurrentProgress;
	}
	else
	{
		int iCurrentProgress = m_iReadExtendInfoIndex + 1;
		return iCurrentProgress;
	}
}

int CQuoteManager::GetFileNum()
{
	int iFileNum = m_iFileNum;
	return iFileNum;
}

bool CQuoteManager::GetReadFilestatus()
{
	return m_ReadFileFinish;
}
bool CQuoteManager::GetReadBasicInfostatus()
{
	return m_bReadFileBasicInfofinish;
}

bool CQuoteManager::GetSymbolStatus(string &strSymbol)
{
	return m_umapFileList[strSymbol]->m_bReadFileStatus;
}

int CQuoteManager::GetFileNameList(vector<string> &vecPackage)
{
	unordered_map<string, shared_ptr<CSymbolInfo>>::iterator iter;
	for (iter = m_umapFileList.begin(); iter != m_umapFileList.end(); ++iter)
	{
		vecPackage.push_back(iter->second->m_strSymbolID);
	}
	sort(vecPackage.begin(), vecPackage.end());
	return m_iFileNum;
}

bool CQuoteManager::GetBasicInfoPackage(string &strSymbol, CBasicInfo &classPackage)	// 回傳基本欄位所需資訊
{
	if (true == m_umapFileList[strSymbol]->m_bReadFileStatus)
	{
		classPackage.m_strSymbolID = m_umapFileList[strSymbol]->m_strSymbolID;
		if (0 != m_umapFileList[strSymbol]->m_vecPerTransaction.size())
		{
			classPackage.m_iTime = m_umapFileList[strSymbol]->m_vecPerTransaction.back().iTime;
			classPackage.m_iPrice = m_umapFileList[strSymbol]->m_vecPerTransaction.back().dPrice;
			classPackage.m_iVolume = m_umapFileList[strSymbol]->m_vecPerTransaction.back().iVolume;
			classPackage.m_dAvgPrice = m_umapFileList[strSymbol]->m_dAvgPrice;
			classPackage.m_iTotalVolume = m_umapFileList[strSymbol]->m_iTotalVolume;
			classPackage.m_dPriceHigh = m_umapFileList[strSymbol]->m_dPriceHigh;
			classPackage.m_dPriceLow = m_umapFileList[strSymbol]->m_dPriceLow;
		}
		else
		{
			classPackage.m_iTime = -1;
			classPackage.m_iPrice = -1;
			classPackage.m_iVolume = -1;
			classPackage.m_dAvgPrice = -1;
			classPackage.m_iTotalVolume = -1;
			classPackage.m_dPriceHigh = -1;
			classPackage.m_dPriceLow = -1;
		}

		classPackage.m_iLikeSymbolCnt = m_umapFileList[strSymbol]->m_iLikeSymbolCnt;
		return true;
	}
	else
	{
		return false;
	}
}

bool CQuoteManager::GetPriceAndVolumeInfoPackage(string &strSymbol, map<double, int> &mapPackage)
{
	if (true == m_umapFileList[strSymbol]->m_bReadFileStatus)
	{
		mapPackage = m_umapFileList[strSymbol]->m_mapPriceAndVolumeArray;
		return true;
	}
	else
	{
		return false;
	}
}

bool CQuoteManager::GetPerTradeDetailInfoPackage(string &strSymbol, vector<CTickInfo> &vecPackage)
{
	if (true == m_umapFileList[strSymbol]->m_bReadFileStatus)
	{
		for (int i = 0; i < m_umapFileList[strSymbol]->m_vecPerTransaction.size(); ++i)
		{
			CTickInfo data(m_umapFileList[strSymbol]->m_vecPerTransaction.at(i).iTime, m_umapFileList[strSymbol]->m_vecPerTransaction.at(i).dPrice, m_umapFileList[strSymbol]->m_vecPerTransaction.at(i).iVolume);
			vecPackage.emplace_back(data);
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CQuoteManager::GetSimilarCommodityPackage(string &strSymbol, vector<CSimilarSymbolInfo> &vecPackage)
{
	if (true == m_umapFileList[strSymbol]->m_bReadFileStatus)
	{
		for (int i = 0; i < m_umapFileList[strSymbol]->m_vecSimilarSymbol.size(); ++i)
		{
			CSimilarSymbolInfo data(m_umapFileList[strSymbol]->m_vecSimilarSymbol.at(i)->m_strSymbolID, m_umapFileList[strSymbol]->m_vecSimilarSymbol.at(i)->m_dAvgPrice, m_umapFileList[strSymbol]->m_vecSimilarSymbol.at(i)->m_iTotalVolume);
			vecPackage.emplace_back(data);
		}
		return true;
	}
	else
	{
		return false;
	}
}