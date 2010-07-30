#pragma once

#include "nsIDOMStyleSheet.h"

bool IsValidMedia(nsIDOMStyleSheet * styleSheet);
bool IsAlternate(nsIDOMStyleSheet *styleSheet);
bool IsPersistent(nsIDOMStyleSheet *styleSheet);
bool IsPreferred(nsIDOMStyleSheet *styleSheet);
void SetStyleSheetDisabled(nsIDOMStyleSheet *styleSheet, bool isDisabled);
bool IsValidForFrame(nsIDOMWindow *frame, const CString& title);
void SwitchStyleSheetInFrame(nsIDOMWindow *frame, const CString& title);