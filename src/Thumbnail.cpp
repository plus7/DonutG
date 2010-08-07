#include "stdafx.h"

#include "Donut.h"
#include "Thumbnail.h"
#include <atlenc.h> //for Base64Decode()

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define __ATLTYPES_H__	// Use the WTL types
#include <atlimage.h> //for CImage
#include <nsContentCID.h>
#include <nsIDOMHTMLDocument.h>
#include <nsIDOMHTMLCanvasElement.h>
#include <nsIDOMCanvasRenderingContext2D.h>

void CThumbnailDlg::_Function_EnumChild_MakeThumbnail::operator ()(HWND hWnd) {
	nsresult rv;
	nsCOMPtr<nsIWebBrowser> wb;
	wb = DonutGetNsIWebBrowser(hWnd);
	nsCOMPtr<nsIDOMWindow> win;
	wb->GetContentDOMWindow(getter_AddRefs(win));

	if (win) {
		//ÉeÉìÉ|ÉâÉäÇ»HTMLDocumentÇê∂ê¨
		const nsCID hoge = NS_HTMLDOCUMENT_CID;
		nsCOMPtr<nsIDOMHTMLDocument> htmlDocument(do_CreateInstance(hoge, &rv));
		nsCOMPtr<nsIDOMElement> elm;
		htmlDocument->CreateElement(NS_LITERAL_STRING("canvas"), getter_AddRefs(elm));
		nsCOMPtr<nsIDOMHTMLCanvasElement> canvas = do_QueryInterface(elm);
		if(!canvas) return;

		nsCOMPtr<nsISupports> suppCtx;
		canvas->GetContext(NS_LITERAL_STRING("2d"), getter_AddRefs(suppCtx));

		nsCOMPtr<nsIDOMCanvasRenderingContext2D> ctx = do_QueryInterface(suppCtx);
		if(!ctx) return;

		ctx->DrawWindow(win, 0, 0, m_nWidth, m_nHeight, NS_LITERAL_STRING("rgb(255,255,255)"), 0);

		nsEmbedString data;
		nsresult rv = canvas->ToDataURLAs(NS_LITERAL_STRING("image/png"),
			                              NS_LITERAL_STRING("transparency=none"), 
										  data);
		//::MessageBox(NULL, data.get(), _T(""), MB_OK);
		nsEmbedCString cdata(NS_LossyConvertUTF16toASCII(data.get()));
		cdata.Cut(0, strlen("data:image/png;base64,"));
		BYTE *buf = new BYTE[Base64DecodeGetRequiredLength(cdata.Length())];
		int buflen = Base64DecodeGetRequiredLength(cdata.Length());
		Base64Decode(cdata.get(), cdata.Length(), buf, &buflen);
		if(buflen <= 0) { delete buf; return; }
		IStreamPtr spStream;
		if(!CreateOnHGlobal(&spStream)){ delete buf; return; }
		HRESULT hr = spStream->Write(buf, buflen, NULL);
		if (FAILED(hr)) {
			delete buf;
			return;
		}

		CImage img;
		hr = img.Load(spStream);

		/*CClientDC 	tmpClientDc( ::GetDesktopWindow() );
		CDC 		hdc 	= ::CreateCompatibleDC(tmpClientDc);
		CBitmap 	bmp		= ::CreateCompatibleBitmap(tmpClientDc, m_nWidth, m_nHeight);*/

		m_ImgList.Add(img);

		delete buf;
		//spStream->Release();
	}
}