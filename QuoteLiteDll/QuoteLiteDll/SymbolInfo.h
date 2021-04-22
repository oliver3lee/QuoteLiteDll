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
	typedef struct TickInfoStruct	// �ɮ��x�s�}�C���c
	{
		int iTime;		// �ɶ�
		double dPrice;	// ����
		int	iVolume;	// ����q
	}TickInfo;

	vector<TickInfo> m_vecPerTransaction;				// �C�Ӯɬq�����T
	vector<shared_ptr<CSymbolInfo>> m_vecSimilarSymbol;	// �ۦ��ӫ~
	map<double, int> m_mapPriceAndVolumeArray;			// �����q����

	string m_strSymbolID;	// �ӫ~ID
	double m_dPriceHigh;	// �̰���
	double m_dPriceLow;		// �̧C��
	int m_iTotalVolume;		// �`�q
	double m_dAvgPrice;		// ����
	double m_dPriceTotal;	// �`��
	int m_iLikeSymbolCnt;	// �ۦ��ӫ~��
	bool m_bReadFileStatus;	// �ɮ׬O�_Ū�����\

	void FileToArrayAndGetSimbleInformation(const string &strFileName, const string &strPath);	// Ū�ɦs�}�C
	void FindSimilarItem(std::unordered_map<string, shared_ptr<CSymbolInfo>>& File);			// �M��ۦ��ӫ~
	void PopToken(char* pszToken, int iTokenBufLen, char* const pszData);						// �C�C�ɮפ��e����
	void CountAvgPrice();																		// �p�⥭����
	void CountPriceOfValume();																	// �p������q����

	CString m_csErrorMessage;	// Ū�ɥ��ѿ��~�T��

	CSymbolInfo();
public:
	~CSymbolInfo();
	//void(*pDFHookCallbackFunc)(void* pVoid) = NULL;	// Callback function�^���ɮ�Ū���i��
};