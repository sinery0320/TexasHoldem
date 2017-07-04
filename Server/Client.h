#pragma once
namespace TCP{ class CClientTcp; }

namespace Game{
	class IClient
	{
	private:
		CString m_strName;
		int m_nID;
		TCP::CClientTcp *m_pTcp;
		CString m_strOutput;

	public:
		IClient() : m_pTcp(nullptr){ ; }
		virtual ~IClient();
		
		void SetID(int id){ m_nID = id; }
		void SetName(CString name){ m_strName = name; }
		void SetOutput(CString out){ m_strOutput = out; }

		TCP::CClientTcp* GetTcp() { return m_pTcp; }
		CString GetName(){ return m_strName; }
		int GetID(){ return m_nID; }
		CString GetOutput(){ return m_strOutput; }

		virtual CString GetInfoStr(){ return _T("empty"); }
		virtual void InitOneGame(){ ; }
		virtual void WinTheGame(int money){ UNREFERENCED_PARAMETER(money); }

	public:
		void SetTcp(TCP::CClientTcp *pTcp);
		void InitHead(byte* pData, short cmdid, short datalen);
		void SendData(byte* pData, int count);
		void ResponseFun(byte *pData, int count);
		virtual void OnCmdRespond(byte *pData, int count) = 0;
		virtual int SendInitRequest(){ return false; }
		virtual bool IsClientGameOver(){ return false; }
	};
}
