#include "BitmapDump.h"
#include <BitmapStream.h>
#include <stdio.h>
#include <Message.h>
#include <Application.h>
#include "MrPeeps.h"
#include <TranslatorFormats.h>
#include <Alert.h>

BitmapDumper::BitmapDumper(void)
{
	roster=BTranslatorRoster::Default();
}

BitmapDumper::~BitmapDumper(void)
{
}

status_t BitmapDumper::DumpToJPEG(BBitmap *bmp, BPositionIO *dest)
{
	if(!bmp || !dest)
		return B_BAD_VALUE;

	BBitmapStream bstream(bmp);
	status_t status=roster->Translate(&bstream,NULL,NULL,dest,B_JPEG_FORMAT);
	
	if(status!=B_OK)
	{
		if(status!=B_OK)
		{
			BAlert *alert=new BAlert("Mr. Peeps!","Mr. Peeps! was not able "
			"to save the photo. This is actually an "
			"issue with the translators installed on your system. The translators "
			"written by the Haiku project do not have this issue.\n\n"
			"Would you like to:\n\n1) Go to the Haiku website\n2) Save the photo "
			"in another way which takes up more space\n 3) Do nothing\n",
			"Haiku website","Bigger File", "Do Nothing");
			int32 value=alert->Go();
			
			if(value==0)
			{
				BMessage *msg=new BMessage(M_LAUNCH_BROWSER);
				msg->AddString("address","http://www.haiku-os.org/");
				be_app->PostMessage(msg);
			}
			else
			if(value==1)
			{
				printf("Couldn't save as a JPEG. Hmmmm... Trying PNG\n");
				status=roster->Translate(&bstream,NULL,NULL,dest,B_PNG_FORMAT);
		
				if(status!=B_OK)
				{
					printf("Couldn't save as a PNG. Hmmmm... Trying TIFF\n");
					status=roster->Translate(&bstream,NULL,NULL,dest,B_TIFF_FORMAT);
					
					if(status!=B_OK)
					{
						BAlert *failalert=new BAlert("Mr. Peeps!","Mr. Peeps! has "
						"tried very hard to save your photo, but simply could not "
						"do so. You have my sincere apologies.","OK");
						failalert->Go();
					}
				}
			}
			// else do nothing
		}
	}
	
	BBitmap *detached;
	bstream.DetachBitmap(&detached);
	
	if(detached!=bmp)
		debugger("Uh-oh -- detachment problem!");
		
	return status;
}
