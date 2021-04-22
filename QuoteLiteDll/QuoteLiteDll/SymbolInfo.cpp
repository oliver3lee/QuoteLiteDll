#include "SymbolInfo.h"
#include<iostream>
#include <algorithm>

void CSymbolInfo::PopToken(char* pszToken, int iTokenBufLen, char* pszData)	// 每列檔案內容切割
{
	char* pszTokenCut;
	char* pszNext;

	pszTokenCut = strtok_s(pszData, ",", &pszNext);
	if (NULL != pszTokenCut)
	{
		strcpy_s(pszToken, iTokenBufLen, pszTokenCut);
	}
	memmove(pszData, pszNext, strlen(pszNext) + 1); //因分段字元所以+1
}

void CSymbolInfo::FileToArrayAndGetSimbleInformation(const string &strFileName, const string &strPath)	// 讀檔存陣列並獲得第一部份資訊
{
	m_strSymbolID = strFileName;
	const string filepath = strPath + m_strSymbolID + ".csv";
	ifstream pFile(filepath, ios::in);

	if (pFile)
	{
		string strline;
		double dData;
		int iData;
		int iTime = 0;		// 時間欄位位於檔案位置
		int iPrice = 0;		// 價錢欄位位於檔案位置
		int iVolume = 0;	// 成交量欄位位於檔案位置
		int iColNum = 0;
		int iIndex = 0;
		char szTemp[50];


		while (getline(pFile, strline)) //getline(inFile, strline)表示按行讀取CSV檔案中的資料   
		{
			TickInfo Info;
			const char* c_pszLineData = strline.c_str();

			while (0 != strlen(c_pszLineData))
			{

				PopToken(szTemp, sizeof(szTemp), const_cast<char *>(c_pszLineData));

				if (1 == iColNum)	// 獲得(時間、價錢、成交量)欄位於檔案位置
				{
					if (0 == strcmp("143", szTemp))
					{
						iTime = iIndex;
					}
					else if (0 == strcmp("505", szTemp) || 0 == strcmp("125", szTemp))
					{
						iPrice = iIndex;
					}
					else if (0 == strcmp("413", szTemp))
					{
						iVolume = iIndex;
					}
				}

				if ((iTime == iIndex) && (0 != iColNum && 1 != iColNum))	// 時間欄位
				{
					iData = atoi(szTemp);
					Info.iTime = iData;
				}
				else if ((iPrice == iIndex) && (0 != iColNum && 1 != iColNum))	// 價錢欄位
				{
					dData = atof(szTemp);
					Info.dPrice = dData;

					if (dData > m_dPriceHigh)	// 最高價判斷
					{
						m_dPriceHigh = dData;
					}

					if (0 == m_dPriceLow)	// 最低價判斷
					{
						m_dPriceLow = dData;
					}
					else if (dData < m_dPriceLow)
					{
						m_dPriceLow = dData;
					}
				}
				else if ((iVolume == iIndex) && (0 != iColNum && 1 != iColNum))	// 成交量欄位
				{
					iData = atoi(szTemp);
					Info.iVolume = iData;
					m_iTotalVolume += Info.iVolume;
				}
				++iIndex;
			}
			if (0 != iColNum && 1 != iColNum)
			{
				m_vecPerTransaction.push_back(Info);
			}
			
			++iColNum;
			iIndex = 0;
		}

		CountAvgPrice();			// 均價計算
		CountPriceOfValume();		// 分價明細
		m_bReadFileStatus = true;	// 讀取成功

		pFile.close();
	}
	else	// 讀檔失敗 
	{
		m_bReadFileStatus = false;	// 讀取失敗
		m_csErrorMessage = m_strSymbolID.c_str();
		m_csErrorMessage ="Read " + m_csErrorMessage + " error";
		HWND hwnd = ::FindWindow(NULL, _T("QuoteLite"));
		::PostMessage(hwnd, WM_READFILEERRORMESSAGE, 0, (LPARAM)&m_csErrorMessage);	//傳送給任意視窗
	}
}

void CSymbolInfo::CountAvgPrice()	// 均價計算
{
	if (0 != m_iTotalVolume)
	{
		for (int i = 0; i < m_vecPerTransaction.size(); ++i)
		{
			double a = m_vecPerTransaction.at(i).dPrice;
			int b = m_vecPerTransaction.at(i).iVolume;
			m_dPriceTotal += m_vecPerTransaction[i].dPrice * m_vecPerTransaction[i].iVolume;
		}
		m_dAvgPrice = m_dPriceTotal / m_iTotalVolume;
	}
	else
	{
		m_dAvgPrice = -1;
	}
}


void CSymbolInfo::CountPriceOfValume()	// 計算分價量明細
{
	map<double, int>::iterator iter;

	for (int i = 0; i < m_vecPerTransaction.size(); i++)
	{
		iter = m_mapPriceAndVolumeArray.find(m_vecPerTransaction[i].dPrice);	// 尋找map中有沒有此筆分價量的舊資料
		if (iter != m_mapPriceAndVolumeArray.end()) // 找到就再做累加
		{
			m_mapPriceAndVolumeArray[m_vecPerTransaction[i].dPrice] += m_vecPerTransaction[i].iVolume;
		}
		else	// 找不到就加入新的
		{
			m_mapPriceAndVolumeArray[m_vecPerTransaction[i].dPrice] = m_vecPerTransaction[i].iVolume;
		}
	}
}

void CSymbolInfo::FindSimilarItem(std::unordered_map<string, shared_ptr<CSymbolInfo>>& File)	// 尋找相似商品
{
	if ((-1 != this->m_dAvgPrice) && (false != m_bReadFileStatus))
	{
		double iMin = (double)0.8 *this->m_dAvgPrice;
		double iMax = (double)1.2 * this->m_dAvgPrice;
		std::unordered_map<string, shared_ptr<CSymbolInfo>>::iterator iter;
		for (iter = File.begin(); iter != File.end(); ++iter)
		{
			if ((iMin <= iter->second->m_dAvgPrice) && (iMax >= iter->second->m_dAvgPrice))
			{
				if (m_strSymbolID != iter->first)
				{
					m_vecSimilarSymbol.push_back(iter->second);
					m_iLikeSymbolCnt++;
				}
			}
		}
	}
}


CSymbolInfo::CSymbolInfo() :m_dPriceHigh(0), m_dPriceLow(0), m_iTotalVolume(0), m_dAvgPrice(0), m_dPriceTotal(0), m_bReadFileStatus(false), m_iLikeSymbolCnt(0)
{
}

CSymbolInfo::~CSymbolInfo()
{
}