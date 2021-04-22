#pragma once
#include "SymbolInfo.h"
#include <atomic> 

using namespace std;
class  CLASS_DECLSPEC CQuoteManager
{
public:

	class CTickInfo	// ������ӳW��]
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

	class CBasicInfo	// �򥻸�T�W��]
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

	class CSimilarSymbolInfo	// �ۦ��ӫ~�W��]
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
	bool GetBasicInfoPackage(string &strSymbol, CBasicInfo &classPackage);								// �^�ǰ����һݸ�T
	bool GetSimilarCommodityPackage(string &strSymbol, vector<CSimilarSymbolInfo> &vecPackage);			// �ۦ��ӫ~�]
	bool GetPriceAndVolumeInfoPackage(string &strSymbol, map<double, int> &package);					// �����q���ӥ]
	bool CQuoteManager::GetPerTradeDetailInfoPackage(string &strSymbol, vector<CTickInfo> &vecPackage);	// ������ӥ]

public:
	CQuoteManager();
	~CQuoteManager();

	void QuoteInintialize(const string &strPath);	// �p����|�U�ɮ׼�
	int ReadSymbolsInfo();							// Ū��
	int GetFileNum();								// �o���Ƨ��U�ɮ׼�
	int GetCurrentReadFileProgress();				// �o��ثeŪ�ɶi��
	bool GetReadFilestatus();						// �ɮ׬O�_����Ū������
	bool GetReadBasicInfostatus();					// �򥻸�T�O�_����������
	bool GetSymbolStatus(string &strSymbol);		// �ɮ�Ū�����\�Υ���

	std::unordered_map<string, shared_ptr<CSymbolInfo>> m_umapFileList;		// �ɮװ}�C

private:
	string m_path;	// �ɮ׸��|
	int m_iFileNum;	//�ɮ׼ƶq
	std::vector<std::thread> m_ReadFileThreads;	// Ū��thread
	
private:
	void ReadFileBasicInfo();						// Ū�򥻸�T
	void ReadFileExtendInfo();						// Ū�ۦ��ӫ~

	std::unordered_map<string, shared_ptr<CSymbolInfo>>::iterator m_iterMapIndex;	// map iIndex
	std::atomic<int>m_iReadBasicInfoIndex{ 0 };		// �Ĥ@����Ū���ɮ�iIndex
	std::atomic<int>m_iReadExtendInfoIndex{ 0 };	// �ĤG����Ū���ɮ�iIndex
	std::atomic<int>m_iThreadFinishNum{ 0 };		// thread �����ƶq

private:
	void ReadingBasicInfoManager();				// Ū�ɵ{�Ǻ޲z
	void ReadingLikeSymbolManager();			// �M��ۦ��ӫ~�{�Ǻ޲z
	void CountFinishThreadNum();				// thread �����ƶq�p��
	bool AddReadBasicIndex(unordered_map<string, shared_ptr<CSymbolInfo>>::iterator &iterCurrentlyIndex);	// �Ĥ@����Ū��iIndex�W�[
	bool AddReadExtendIndex(unordered_map<string, shared_ptr<CSymbolInfo>>::iterator &iterCurrentlyIndex);	// �ĤG����Ū��iIndex�W�[

private:
	LARGE_INTEGER m_liPerfFreq;					// CPU�W�v
	LARGE_INTEGER m_liPerfRreadFileStart;		// Ū�ɨ�array�}�l�ɶ�
	LARGE_INTEGER m_liPerfRreadSingleFileStart;	// Ū��@���ĤG������T�}�l�ɶ�
	LARGE_INTEGER m_liPerfEnd;					// �����ɶ�
	double m_dCostTimePartOne;					// �Ĥ@���������ɶ�
	double m_dCostTimePartTwo;					// �ĤG���������ɶ�
	CString m_csUseTime;						// �`��O�ɶ�
	bool m_ReadFileFinish;
	mutex m_MutexFileIndex;
	int m_iThreadTotal;
	bool m_bReadFileBasicInfofinish;
};