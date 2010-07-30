#include "stdafx.h"

#include "StyleSheetUtils.h"
#include "nsIDOMStyleSheetList.h"
#include "nsIDOMMediaList.h"
#include "nsIDOMDocumentStyle.h"
#include "nsIDOMWindowCollection.h"

bool IsValidMedia(nsIDOMStyleSheet * styleSheet)
{
	nsCOMPtr<nsIDOMMediaList> medias;
	styleSheet->GetMedia(getter_AddRefs(medias));
	nsEmbedString mediaText;
	medias->GetMediaText(mediaText);
	CString strMedia(mediaText.get());
	strMedia.MakeLower();
	return (strMedia.IsEmpty() || 
	        strMedia.Find(_T("all")) > 0 ||
	        strMedia.Find(_T("screen")) > 0);
}

bool IsAlternate(nsIDOMStyleSheet *styleSheet)
{
	nsCOMPtr<nsIDOMNode> node;
	styleSheet->GetOwnerNode(getter_AddRefs(node));
	if(!node) return false;
	nsEmbedString nodeName;
	node->GetNodeName(nodeName);
	CString tmp(nodeName.get());
	if(tmp != _T("link")) return false;
	nsCOMPtr<nsIDOMElement> elm = do_QueryInterface(node);
	if(!elm) return false;
	nsEmbedString value;
	elm->GetAttribute(nsEmbedString(_T("rel")), value);
	if(CString(value.get()) == "alternate stylesheet") return true;
	else return false;
}

bool IsPersistent(nsIDOMStyleSheet *styleSheet)
{
	nsEmbedString title;
	styleSheet->GetTitle(title);
	return (!IsAlternate(styleSheet) && !CString(title.get()).IsEmpty());
}

bool IsPreferred(nsIDOMStyleSheet *styleSheet)
{
	nsEmbedString title;
	styleSheet->GetTitle(title);
	return (!IsAlternate(styleSheet) && CString(title.get()).IsEmpty());
}

void SetStyleSheetDisabled(nsIDOMStyleSheet *styleSheet, bool isDisabled)
{
	styleSheet->SetDisabled(isDisabled);
}

bool IsValidForFrame(nsIDOMWindow *frame, const CString& title)
{
	nsCOMPtr<nsIDOMDocument> doc;
	frame->GetDocument(getter_AddRefs(doc));
	nsCOMPtr<nsIDOMDocumentStyle> docst = do_QueryInterface(doc);
	nsCOMPtr<nsIDOMStyleSheetList> sheets;
	docst->GetStyleSheets(getter_AddRefs(sheets));

  if( title=="_nostyle" ) return true;
  else if( title=="_default" ) return true;
  else
  {
	  PRUint32 c;
	  sheets->GetLength(&c);
	  for(int i=0; i<c; i++)
	  {
		  nsCOMPtr<nsIDOMStyleSheet> ss;
		  sheets->Item(i, getter_AddRefs(ss));
		  nsEmbedString str;
		  ss->GetTitle(str);
		  if(CString(str.get()) == title) return true;
		  else return false;
	  }
  }
  return false;
}

void SwitchStyleSheetInFrame(nsIDOMWindow *frame, const CString& title)
{
	if(!IsValidForFrame(frame, title)) return;
	nsCOMPtr<nsIDOMDocument> doc;
	frame->GetDocument(getter_AddRefs(doc));
	nsCOMPtr<nsIDOMDocumentStyle> docst = do_QueryInterface(doc);
	nsCOMPtr<nsIDOMStyleSheetList> sheets;
	docst->GetStyleSheets(getter_AddRefs(sheets));

	PRUint32 c;
	sheets->GetLength(&c);
	for(int i=0; i<c; i++)
	{
		nsCOMPtr<nsIDOMStyleSheet> styleSheet;
		sheets->Item(i, getter_AddRefs(styleSheet));
		if(IsValidMedia(styleSheet)){
			if(title == "_nostyle"){
				SetStyleSheetDisabled(styleSheet, true);
			}else if(title == "_default"){
				if(IsPersistent(styleSheet) || IsPreferred(styleSheet))
				{
					SetStyleSheetDisabled(styleSheet, false);
				}
				else
				{
					SetStyleSheetDisabled(styleSheet, true);
				}
			}
			else
			{
				nsEmbedString strTitle;
				styleSheet->GetTitle(strTitle);

				if(IsPersistent(styleSheet))
				{
					SetStyleSheetDisabled(styleSheet, false);
				}
				else
				{
					SetStyleSheetDisabled(styleSheet, CString(strTitle.get())!=title);
				}
			}
		}
	}
	nsCOMPtr<nsIDOMWindowCollection> collection;
	frame->GetFrames(getter_AddRefs(collection));
	PRUint32 l;
	collection->GetLength(&l);
	for(int i=0;i<l;i++){
		nsCOMPtr<nsIDOMWindow> win;
		collection->Item(i,getter_AddRefs(win));
		SwitchStyleSheetInFrame(win, title);
	}
}
