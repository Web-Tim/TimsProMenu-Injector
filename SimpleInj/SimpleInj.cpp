#include <iostream>
#include <windows.h>
#include <thread>
#include <string>
#include <libloaderapi.h>

using namespace std;

void GetProcID(const char* windowTitle, DWORD& procID)
{
	GetWindowThreadProcessId(FindWindow(NULL, windowTitle), &procID);
}

void err(const char* errTitle, const char* errMsg)
{
	MessageBox(NULL, errMsg, errTitle, NULL);
	exit(-1);
}

bool fileExists(string fileName)
{
	struct stat buffer;
	return (stat(fileName.c_str(), &buffer) == 0);
}

const char* paint = "Unbenannt - paint.net v4.2.15";   //Type: x64
const char* battleNet = "Blizzard Battle.net";		   //Type: x86	
const char* sauerbraten = "Cube 2: Sauerbraten";       //Type: x64
const char* csgo = "Counter-Strike: Global Offensive"; //Type: x86
const char* mc = "Minecraft 1.8.8"; //Type: x64
const char* badlionClient = "Badlion Minecraft Client v2.17.2-7dabf62-PRODUCTION (1.8.9)";
const char* pvpLounge = "Lounge Client 1.8.9";
const char* gta = "Grand Theft Auto V";

int main()
{
	DWORD procID = NULL;
	char dllPath[MAX_PATH];
	const char* dllName = "client.dll";
	const char* windowTitle = gta;

	if (!fileExists(dllName))
	{
		err("File Error", "File doesn´t exist");
	}

	if (!GetFullPathName(dllName, MAX_PATH, dllPath, nullptr))
	{
		err("File Error", "Failed to get full Path");
	}

	GetProcID(windowTitle, procID);
	if (procID == NULL)
	{
		err("Process Error", "Failed to get the Process ID");
	}

	HANDLE h_proc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);
	if (!h_proc)
	{
		err("Process Error", "Failed to open a handle to the Process");
	}

	void* allocMem = VirtualAllocEx(h_proc, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocMem)
	{
		err("Allocate Memory", "Failed to allocate Memory");
	}

	if (!WriteProcessMemory(h_proc, allocMem, dllPath, MAX_PATH, nullptr))
	{
		err("Writing", "Writing to Process failed");
	}

	HANDLE h_thread = CreateRemoteThread(h_proc, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocMem, NULL, nullptr);
	if (!h_thread)
	{
		err("Remote Thread", "Failed to create Remote Thread");
	}

	CloseHandle(h_proc);
	VirtualFreeEx(h_proc, allocMem, NULL, MEM_RELEASE);
	MessageBox(0, "Successfully Injected your dll!", "Success", 0);

}