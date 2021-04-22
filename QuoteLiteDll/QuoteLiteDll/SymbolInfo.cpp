#include "SymbolInfo.h"
#include<iostream>
#include <algorithm>

void CSymbolInfo::PopToken(char* pszToken, int iTokenBufLen, char* pszData)	// �C�C�ɮפ��e����
{
	char* pszTokenCut;
	char* pszNext;

	pszTokenCut = strtok_s(pszData, ",", &pszNext);
	if (NULL != pszTokenCut)
	{
		strcpy_s(pszToken, iTokenBufLen, pszTokenCut);
	}
	memmove(pszData, pszNext, strlen(pszNext) + 1); //�]���q�r���ҥH+1
}

void CSymbolInfo::FileToArrayAndGetSimbleInformation(const string &strFileName, const string &strPath)	// Ū�ɦs�}�C����o�Ĥ@������T
{
	m_strSymbolID = strFileName;
	const string filepath = strPath + m_strSymbolID + ".csv";
	ifstream pFile(filepath, ios::in);

	if (pFile)
	{
		string strline;
		double dData;
		int iData;
		int iTime = 0;		// �ɶ�������ɮצ�m
		int iPrice = 0;		// ����������ɮצ�m
		int iVolume = 0;	// ����q������ɮצ�m
		int iColNum = 0;
		int iIndex = 0;
		char szTemp[50];


		while (getline(pFile, strline)) //getline(inFile, strline)��ܫ���Ū��CSV�ɮפ������   
		{
			TickInfo Info;
			const char* c_pszLineData = strline.c_str();

			while (0 != strlen(c_pszLineData))
			{

				PopToken(szTemp, sizeof(szTemp), const_cast<char *>(c_pszLineData));

				if (1 == iColNum)	// ��o(�ɶ��B�����B����q)�����ɮצ�m
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

				if ((iTime == iIndex) && (0 != iColNum && 1 != iColNum))	// �ɶ����
				{
					iData = atoi(szTemp);
					Info.iTime = iData;
				}
				else if ((iPrice == iIndex) && (0 != iColNum && 1 != iColNum))	// �������
				{
					dData = atof(szTemp);
					Info.dPrice = dData;

					if (dData > m_dPriceHigh)	// �̰����P�_
					{
						m_dPriceHigh = dData;
					}

					if (0 == m_dPriceLow)	// �̧C���P�_
					{
						m_dPriceLow = dData;
					}
					else if (dData < m_dPriceLow)
					{
						m_dPriceLow = dData;
					}
				}
				else if ((iVolume == iIndex) && (0 != iColNum && 1 != iColNum))	// ����q���
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

		CountAvgPrice();			// �����p��
		CountPriceOfValume();		// ��������
		m_bReadFileStatus = true;	// Ū�����\

		pFile.close();
	}
	else	// Ū�ɥ��� 
	{
		m_bReadFileStatus = false;	// Ū������
		m_csErrorMessage = m_strSymbolID.c_str();
		m_csErrorMessage ="Read " + m_csErrorMessage + " error";
		HWND hwnd = ::FindWindow(NULL, _T("QuoteLite"));
		::PostMessage(hwnd, WM_READFILEERRORMESSAGE, 0, (LPARAM)&m_csErrorMessage);	//�ǰe�����N����
	}
}

void CSymbolInfo::CountAvgPrice()	// �����p��
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


void CSymbolInfo::CountPriceOfValume()	// �p������q����
{
	map<double, int>::iterator iter;

	for (int i = 0; i < m_vecPerTransaction.size(); i++)
	{
		iter = m_mapPriceAndVolumeArray.find(m_vecPerTransaction[i].dPrice);	// �M��map�����S�����������q���¸��
		if (iter != m_mapPriceAndVolumeArray.end()) // ���N�A���֥[
		{
			m_mapPriceAndVolumeArray[m_vecPerTransaction[i].dPrice] += m_vecPerTransaction[i].iVolume;
		}
		else	// �䤣��N�[�J�s��
		{
			m_mapPriceAndVolumeArray[m_vecPerTransaction[i].dPrice] = m_vecPerTransaction[i].iVolume;
		}
	}
}

void CSymbolInfo::FindSimilarItem(std::unordered_map<string, shared_ptr<CSymbolInfo>>& File)	// �M��ۦ��ӫ~
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