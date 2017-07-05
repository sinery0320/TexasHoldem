// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLAYER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLAYER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PLAYER_EXPORTS
#define PLAYER_API __declspec(dllexport)
#else
#define PLAYER_API __declspec(dllimport)
#endif

// This class is exported from the Player.dll
class PLAYER_API CPlayer {
private:
	CString m_strName;

public:
	CString GetName(){ return m_strName; }

public:
	CPlayer(void);
};

//extern PLAYER_API int nPlayer;

//PLAYER_API int fnPlayer(void);
