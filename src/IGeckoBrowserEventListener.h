#ifndef I_GECKOBROWSEREVENTLISTENER
#define I_GECKOBROWSEREVENTLISTENER
class nsIWebBrowser;
class nsIContextMenuInfo;
class IGeckoBrowserEventListener
{
public:
	virtual void OnHoge() = 0;
	virtual void OnStatusTextChange(const CString &strText) = 0;
	virtual void OnProgressChange(long progress, long progressMax) = 0;
	virtual void OnDownloadBegin() = 0;
	virtual void OnDownloadComplete() = 0;
	virtual void OnTitleChange(const CString &strTitle) = 0;
	virtual void OnNavigateComplete2(nsISupports *pSupp, const CString &strURL) = 0;
	virtual void OnBeforeNavigate(const CString& uri, bool &cancel) = 0;
	virtual void OnDocumentComplete(nsISupports *pSupp, const CString &strURL) = 0;
	virtual void OnCommandStateChange(long Command, bool bEnable) = 0;
	virtual void OnNewWindow(nsIWebBrowser **ppWb, bool *bCancel) = 0;
	virtual void OnLocationChange(const CString& strURL) = 0;
	virtual void OnShowContextMenu(PRUint32 aContextFlags, nsIContextMenuInfo *aUtils) = 0;
};

#define DECL_GECKOEVENTLISTENER void OnHoge();                                      \
                                void OnStatusTextChange(const CString &strText);    \
                                void OnProgressChange(long progress, long progressMax); \
                                void OnDownloadBegin();                             \
                                void OnDownloadComplete();                          \
                                void OnTitleChange(const CString &strTitle);        \
                                void OnNavigateComplete2(nsISupports *pSupp, const CString &strURL); \
                                void OnBeforeNavigate(const CString& uri, bool &cancel); \
                                void OnDocumentComplete(nsISupports *pSupp, const CString &strURL); \
								void OnCommandStateChange(long Command, bool bEnable); \
								void OnNewWindow(nsIWebBrowser **ppWb, bool *bCancel); \
								void OnLocationChange(const CString& strURL);\
                                void OnShowContextMenu(PRUint32 aContextFlags, nsIContextMenuInfo *aUtils);


#endif