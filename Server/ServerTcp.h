#pragma once

/*
========================
Protocol Information
Min byte len = 16
Max byte len = 1024
========================
(require)
2 byte  read-1 / write-2 require
2 byte  require-0
4 byte  reserve
4 byte  address
4 byte  value byte length
n byte  values
------------------------
(response error)
2 byte  read - 1 / write - 2
2 byte  response error - 2
4 byte  reserve
4 byte  error type
4 byte  reserve
------------------------
(response read / write)
2 byte  read - 1 / write - 2
2 byte  response data - 1
4 byte  reserve
4 byte  address
4 byte  value byte length
n byte  values
        2 byte  byte size (=32 so far)
        2 byte  object id (0-sun, 1-earth, 2-moon)
        1 byte  visible (0-hide, 1-show)
        3 byte  reserve
        8 byte  x
        8 byte  y
        8 byte  r
========================
*/

// CServerTcp command target
//class CServerTcp;
//class CServerTcpClient : public CSocket
//{
//private:
//    CServerTcp *m_Server;
//    CString m_strClientInfo;
//    byte* m_pData;
//    int m_nLen;
//
//private:
//    void ResponseError(int nErrorID);
//    void ResponseRead();
//
//public:
//    CServerTcpClient(CServerTcp *server);
//    ~CServerTcpClient();
//    BOOL InitInfo();
//    virtual void OnReceive(int nErrorCode);
//    virtual void OnClose(int nErrorCode);
//};
//class IClient;
class CGameSevDlg;
namespace TCP{
	class CClientTcp;
	class CServerTcp : public CSocket
	{
	public:
		enum{
			CLIENT_MAX_COUNT = 24,

			DATA_MIN_SIZE = 8,
			DATA_MAX_SIZE = 1024,
			DATA_BUF_SIZE = 20480,

			// require type (2 byte)
			DATA_SEND = 0x0001,
			DATA_BACK = 0x0002,

			// response type (2 byte)
			DATA_ERROR = 0x0002,

			// error type (2 byte)
			DATA_ERR_BUISY = 0x00000001,    // Server is busy
			DATE_ERR_REQUEST,               // Request type is wrong
			DATA_ERR_ADDR,                  // Address is wrong
			DATA_ERR_SIZE,                  // Buffer size < min or > max
			DATA_ERR_NOTSUPPORT,            // This is not support
			DATA_ERR_OTHERS = 0xFFFFFFFF,   // Other unknow error
		};

	private:
		//std::list<std::shared_ptr<CClientTcp>> m_ltClient;
		CGameSevDlg *m_Dlg;

	public:
		CServerTcp(CGameSevDlg *dlg);
		virtual ~CServerTcp();

		virtual void OnConnect(int nErrorCode);
		virtual void OnAccept(int nErrorCode);
		void OnClientDisconnect(CClientTcp *pClientTcp);
		int CreateTcp(unsigned short wPort);    // return 0 success, others failed
	};
}

