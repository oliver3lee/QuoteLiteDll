# QuoteLiteDll- QuoteLite API
1. 簡易讀檔報價系統API
2. 需搭配QuoteLite專案使用
3. API介面如下

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
	bool GetBasicInfoPackage(string &strSymbol, CBasicInfo &classPackage);					// 回傳基本欄位所需資訊
	bool GetSimilarCommodityPackage(string &strSymbol, vector<CSimilarSymbolInfo> &vecPackage);		// 相似商品包
	bool GetPriceAndVolumeInfoPackage(string &strSymbol, map<double, int> &package);			// 分價量明細包
	bool CQuoteManager::GetPerTradeDetailInfoPackage(string &strSymbol, vector<CTickInfo> &vecPackage);	// 成交明細包

public:
	CQuoteManager();
	~CQuoteManager();

	void QuoteInintialize(const string &strPath);				// 計算路徑下檔案數
	int ReadSymbolsInfo();							// 讀檔
	int GetFileNum();							// 得到資料夾下檔案數
	int GetCurrentReadFileProgress();					// 得到目前讀檔進度
	bool GetReadFilestatus();						// 檔案是否全部讀取完畢
	bool GetReadBasicInfostatus();						// 基本資訊是否全部取完畢
	bool GetSymbolStatus(string &strSymbol);				// 檔案讀取成功或失敗
	std::unordered_map<string, shared_ptr<CSymbolInfo>> m_umapFileList;	// 檔案陣列