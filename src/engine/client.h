/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H
#include "kernel.h"

#include "message.h"
#include "graphics.h"
#include <engine/shared/config.h>

enum
{
	CLIENT_MAIN,
	CLIENT_DUMMY,
	NUM_CLIENTS,
};

class IClient : public IInterface
{
	MACRO_INTERFACE("client", 0)
protected:
	// quick access to state of the client
	int m_State;

	// quick access to time variables
	int m_PrevGameTick[NUM_CLIENTS];
	int m_CurGameTick[NUM_CLIENTS];
	float m_GameIntraTick[NUM_CLIENTS];
	float m_GameTickTime[NUM_CLIENTS];

	int m_CurMenuTick;
	int64 m_MenuStartTime;

	int m_PredTick[NUM_CLIENTS];
	float m_PredIntraTick[NUM_CLIENTS];

	float m_LocalTime;
	float m_RenderFrameTime;

	int m_GameTickSpeed;

	class CConfig *m_pConfig;

public:

	class CSnapItem
	{
	public:
		int m_Type;
		int m_ID;
		int m_DataSize;
	};

	/* Constants: Client States
		STATE_OFFLINE - The client is offline.
		STATE_CONNECTING - The client is trying to connect to a server.
		STATE_LOADING - The client has connected to a server and is loading resources.
		STATE_ONLINE - The client is connected to a server and running the game.
		STATE_DEMOPLAYBACK - The client is playing a demo
		STATE_QUITING - The client is quitting.
	*/

	enum
	{
		STATE_OFFLINE=0,
		STATE_CONNECTING,
		STATE_LOADING,
		STATE_ONLINE,
		STATE_DEMOPLAYBACK,
		STATE_QUITING,
	};

	//
	inline int State() const { return m_State; }

	void InitConfig(CConfig *pConfig) { m_pConfig = pConfig; }

	// tick time access
	inline int PrevGameTick() { return m_PrevGameTick[m_pConfig->m_ClDummy]; }
	inline int GameTick() { return m_CurGameTick[m_pConfig->m_ClDummy]; }
	inline int PredGameTick() { return m_PredTick[m_pConfig->m_ClDummy]; }
	inline float IntraGameTick() { return m_GameIntraTick[m_pConfig->m_ClDummy]; }
	inline float PredIntraGameTick() { return m_PredIntraTick[m_pConfig->m_ClDummy]; }
	inline float GameTickTime() { return m_GameTickTime[m_pConfig->m_ClDummy]; }

	inline int MenuTick() const { return m_CurMenuTick; }
	inline int GameTickSpeed() const { return m_GameTickSpeed; }

	// other time access
	inline float RenderFrameTime() const { return m_RenderFrameTime; }
	inline float LocalTime() const { return m_LocalTime; }

	// actions
	virtual void Connect(const char *pAddress) = 0;
	virtual void Disconnect() = 0;
	virtual void Quit() = 0;
	virtual const char *DemoPlayer_Play(const char *pFilename, int StorageType) = 0;
	virtual void DemoRecorder_Start(const char *pFilename, bool WithTimestamp) = 0;
	virtual void DemoRecorder_HandleAutoStart() = 0;
	virtual void DemoRecorder_Stop() = 0;
	virtual void RecordGameMessage(bool State) = 0;
	virtual void AutoStatScreenshot_Start() = 0;
	virtual void AutoScreenshot_Start() = 0;
	virtual void ServerBrowserUpdate() = 0;

	// dummy
	virtual void DummyDisconnect(const char *pReason) = 0;
	virtual void DummyConnect() = 0;
	virtual bool DummyConnected() = 0;
	virtual bool DummyConnecting() = 0;

	// timeout
	virtual void GenerateTimeoutSeed() = 0;

	// gfx
	virtual void SwitchWindowScreen(int Index) = 0;
	virtual bool ToggleFullscreen() = 0;
	virtual void ToggleWindowBordered() = 0;
	virtual void ToggleWindowVSync() = 0;

	// networking
	virtual void EnterGame() = 0;

	// network stats
	virtual int GetInputtimeMarginStabilityScore() = 0;

	//
	virtual const char *GetCurrentMapName() const = 0;
	virtual const char *GetCurrentMapPath() const = 0;
	virtual const char *MapDownloadName() const = 0;
	virtual int MapDownloadAmount() const = 0;
	virtual int MapDownloadTotalsize() const = 0;

	// input
	virtual const int *GetInput(int Tick) = 0;

	// remote console
	virtual void RconAuth(const char *pUsername, const char *pPassword) = 0;
	virtual bool RconAuthed() = 0;
	virtual bool UseTempRconCommands() const = 0;
	virtual void Rcon(const char *pLine) = 0;

	// server info
	virtual void GetServerInfo(class CServerInfo *pServerInfo) = 0;

	// snapshot interface

	enum
	{
		SNAP_CURRENT=0,
		SNAP_PREV=1
	};

	// TODO: Refactor: should redo this a bit i think, too many virtual calls
	virtual int SnapNumItems(int SnapID) = 0;
	virtual const void *SnapFindItem(int SnapID, int Type, int ID) = 0;
	virtual const void *SnapGetItem(int SnapID, int Index, CSnapItem *pItem) = 0;
	virtual int SnapItemSize(int SnapID, int Index) = 0;
	virtual void SnapInvalidateItem(int SnapID, int Index) = 0;

	virtual void *SnapNewItem(int Type, int ID, int Size) = 0;

	virtual void SnapSetStaticsize(int ItemType, int Size) = 0;

	virtual int SendMsg(CMsgPacker *pMsg, int Flags, int NetClient = -1) = 0;

	template<class T>
	int SendPackMsg(T *pMsg, int Flags, int NetClient = -1)
	{
		CMsgPacker Packer(pMsg->MsgID(), false);
		if(pMsg->Pack(&Packer))
			return -1;
		return SendMsg(&Packer, Flags, NetClient);
	}

	//
	virtual const char *ServerAddress() const = 0;
	virtual const char *ErrorString() const = 0;
	virtual const char *LatestVersion() const = 0;
	virtual bool ConnectionProblems() = 0;

	virtual bool SoundInitFailed() const = 0;
};

class IGameClient : public IInterface
{
	MACRO_INTERFACE("gameclient", 0)
protected:
public:
	virtual void OnConsoleInit() = 0;

	virtual void OnRconLine(const char *pLine) = 0;
	virtual void OnInit() = 0;
	virtual void OnNewSnapshot() = 0;
	virtual void OnDemoRecSnap() = 0;
	virtual void OnEnterGame() = 0;
	virtual void OnShutdown() = 0;
	virtual void OnRender() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnStateChange(int NewState, int OldState) = 0;
	virtual void OnConnected() = 0;
	virtual void OnMessage(int MsgID, CUnpacker *pUnpacker, bool IsDummy = false) = 0;
	virtual void OnPredict() = 0;
	virtual void OnActivateEditor() = 0;

	virtual int OnSnapInput(int *pData, bool Dummy, bool Force) = 0;
	virtual void OnDummySwap() = 0;
	virtual void OnDummyDisconnect() = 0;
	virtual void SendStartInfo(int Dummy) = 0;

	virtual const char *GetItemName(int Type) const = 0;
	virtual const char *Version() const = 0;
	virtual const char *NetVersion() const = 0;
	virtual int ClientVersion() const = 0;

};

extern IGameClient *CreateGameClient();
#endif
