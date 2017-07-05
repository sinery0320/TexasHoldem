#pragma once

class CGameMgr;
class CClientTcp : public CSocket
{
public:
	enum{
		DATA_MIN_SIZE = 8,
		DATA_MAX_SIZE = 1024,
		DATA_BUF_SIZE = 2048,
	};

private:
	BOOL m_bConnect;
	CGameMgr *m_Mgr;

private:
	void OnClientInit(byte *data, int len);
	void OnReceivePoker(byte *data, int len);
	void OnReceiveBetRequest(byte *data, int len);
	void OnReceiveResult(byte *data, int len);
	void OnGameOver(byte *data, int len);
	void SendData(byte *data, int len);

public:
	CClientTcp(CGameMgr *mgr);
	virtual ~CClientTcp();
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	BOOL ConnectTo(CString strIP, UINT uiPort);
};

