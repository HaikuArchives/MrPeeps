#ifndef _DEBUGTOOLS_H_
#define _DEBUGTOOLS_H_

#include <SupportDefs.h>
#include <GraphicsDefs.h>

void DebugAlert(const char *msg, const char *title="Debug");
void PrintStatusToStream(status_t value);
void PrintStatus(status_t value);
void PrintColorSpaceToStream(color_space value);
void PrintMessageCode(int32 code);
void TranslateMessageCodeToStream(int32 code);

#endif
