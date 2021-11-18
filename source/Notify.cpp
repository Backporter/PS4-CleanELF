#include <stdint.h>
#include <stdarg.h>
#include "../include/types.h"
#include "../include/imports.h"
#include "../include/syscall.h"
#include "../include/detour.h"
#include "../include/utility.h"

enum NotifyType
{
	NotificationRequest = 0,
	SystemNotification = 1,
	SystemNotificationWithUserId = 2,
	SystemNotificationWithDeviceId = 3,
	SystemNotificationWithDeviceIdRelatedToUser = 4,
	SystemNotificationWithText = 5,
	SystemNotificationWithTextRelatedToUser = 6,
	SystemNotificationWithErrorCode = 7,
	SystemNotificationWithAppId = 8,
	SystemNotificationWithAppName = 9,
	SystemNotificationWithAppInfo = 9,
	SystemNotificationWithAppNameRelatedToUser = 10,
	SystemNotificationWithParams = 11,
	SendSystemNotificationWithUserName = 12,
	SystemNotificationWithUserNameInfo = 13,
	SendAddressingSystemNotification = 14,
	AddressingSystemNotificationWithDeviceId = 15,
	AddressingSystemNotificationWithUserName = 16,
	AddressingSystemNotificationWithUserId = 17,

	UNK_1 = 100,
	TrcCheckNotificationRequest = 101,
	NpDebugNotificationRequest = 102,
	UNK_2 = 102,
};

struct NotifyBuffer
{ //Naming may be incorrect.
	NotifyType Type;		//0x00 
	int ReqId;				//0x04
	int Priority;			//0x08
	int MsgId;				//0x0C
	int TargetId;			//0x10
	int UserId;				//0x14
	int unk1;				//0x18
	int unk2;				//0x1C
	int AppId;				//0x20
	int ErrorNum;			//0x24
	int unk3;				//0x28
	char UseIconImageUri; 	//0x2C
	char Message[1024]; 	//0x2D
	char Uri[1024]; 		//0x42D
	char unkstr[1024];		//0x82D
}; //Size = 0xC30

//Calling from userland
void Notify(char* MessageFMT, ...)
{
	NotifyBuffer Buffer;

	//Create full string from va list.
	va_list args;
	va_start(args, MessageFMT);
	vsprintf(Buffer.Message, MessageFMT, args);
	va_end(args);

	//Populate the notify buffer.
	Buffer.Type = NotifyType::NotificationRequest; //this one is just a standard one and will print what ever is stored at the buffer.Message.
	Buffer.unk3 = 0;
	Buffer.UseIconImageUri = 1; //Bool to use a custom uri.
	Buffer.TargetId = -1; //Not sure if name is correct but is always set to -1.
	strcpy(Buffer.Uri, "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png"); //Copy the uri to the buffer.

	// From user land we can call int64_t sceKernelSendNotificationRequest(int64_t unk1, char* Buffer, size_t size, int64_t unk2) which is a libkernel import.
	sceKernelSendNotificationRequest(0, (char*)&Buffer, 3120, 0);

	// What sceKernelSendNotificationRequest is doing is opening the device "/dev/notification0" or "/dev/notification1"
	// and writing the NotifyBuffer we created to it. Somewhere in ShellUI it is read and parsed into a json which is where
	// I found some clues on how to build the buffer.
}

void MiraPrint(const char * format, ...)
{
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	// perror(buffer);
	va_end(args);
}