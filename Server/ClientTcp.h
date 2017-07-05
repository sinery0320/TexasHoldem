#pragma once

namespace Game{ class IClient; }
// CClientTcp command target
namespace TCP{
	class CServerTcp;
	class CClientTcp : public CSocket
	{
	private:
		CServerTcp *m_Server;
		CString m_strClientInfo;
		byte* m_pData;
		int m_nLen;
		bool m_bConnect;
		std::function<void(byte*, int)> m_ResponseFun;
		std::list<CString> m_ltPackage;

	private:
		void ResponseError(int nErrorID);

	public:
		CClientTcp(CServerTcp *server);
		virtual ~CClientTcp();
		virtual BOOL InitInfo();
		virtual void OnReceive(int nErrorCode);
		virtual void OnClose(int nErrorCode);
		void SendData(byte *pData, int len);
		void InitResponseFun(Game::IClient *pClient);
	};
}

