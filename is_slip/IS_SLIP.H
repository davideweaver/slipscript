// IS_SLIP.CPP - Implementation file for your Internet Server
//    is_slip Filter and is_slip Extension

class CIs_slipExtension : public CHttpServer
{
public:
	CIs_slipExtension();
	~CIs_slipExtension();

	BOOL GetExtensionVersion(HSE_VERSION_INFO* pVer);

	// TODO: Add handlers for your commands here.
	// For example:

	void Default(CHttpServerContext* pCtxt);

	DECLARE_PARSE_MAP()
};

class CIs_slipFilter : public CHttpFilter
{
public:
	CIs_slipFilter();
	~CIs_slipFilter();

	BOOL GetFilterVersion(PHTTP_FILTER_VERSION pVer);

	DWORD OnUrlMap(CHttpFilterContext* pCtxt,
		PHTTP_FILTER_URL_MAP pMapInfo);
	DWORD OnSendRawData(CHttpFilterContext* pCtxt,
		PHTTP_FILTER_RAW_DATA pRawData);
	DWORD OnEndOfNetSession(CHttpFilterContext* pCtxt);

	// TODO: Add your own overrides here
};
