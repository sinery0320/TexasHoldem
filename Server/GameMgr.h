#pragma once
class CGameSevDlg;
namespace TCP{
	class CClientTcp;
	class CServerTcp;
}
namespace Game{
	class IClient;
	class IGameMgr
	{
	public:
		enum SEVSTATE{
			SEV_IDEL,
			SEV_LISTEN,
			SEV_START,
		};
		enum REQUESTSTATE{
			REQ_IDLE,
			REQ_WAIT,
			REQ_DONE,
			REQ_OVERTIME,
			REQ_GAMEOVER,
		};

	private:
		CGameSevDlg *m_Dlg;
		std::shared_ptr<TCP::CServerTcp> m_pServerTcp;

		int m_nServerState;
		bool m_bInTimer;

	public:
		std::list<std::shared_ptr<IClient>> m_ltClient;
		int GetServerState(){ return m_nServerState; }

	public:
		void OnTimer100MSec();

	public:
		IGameMgr(CGameSevDlg *dlg);
		virtual ~IGameMgr();

		virtual bool StartListen(unsigned short wPort);
		virtual bool StopListen();
		virtual bool StartGame();
		virtual void StopGame();
		virtual void OnGameOver();

		virtual void CreateClient(TCP::CClientTcp* tcp) = 0;
		virtual void OnClientDisconnect(TCP::CClientTcp* tcp) = 0;
		virtual void OnTimer100MillSec() = 0;
		virtual void FillGrid(CListCtrl& lcClient, CListCtrl& lcGame){
			UNREFERENCED_PARAMETER(lcClient);
			UNREFERENCED_PARAMETER(lcGame);
		}
		virtual void ShowGameInfo(int gameID){
			UNREFERENCED_PARAMETER(gameID);
		}
	};
}
