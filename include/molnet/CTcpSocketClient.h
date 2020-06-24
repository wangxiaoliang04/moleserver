#ifndef _C_TCP_SOCKET_CLIENT_H_INCLUDE_
#define _C_TCP_SOCKET_CLIENT_H_INCLUDE_

#include "common.h"
#include "Singleton.h"
#include "AtomicBoolean.h"
#include "Mutex.h"
#include "ThreadStarter.h"
#include "MolNetMessage.h"
#include <string>
#include <list>

#define REV_SIZE      30000                     // �������ݵĻ����С

class CMolMessageIn;
class CMolMessageOut;
class CircularBuffer;

/**
* ��ǰsocket������״̬
*/
enum ConnectState
{
	NOCONNECT = 0,     // û������
	CONNECTTING,       // ������
	CONNECTED,         // ������
	MESPROCESS         // ������Ϣ����
};

class CMolTcpSocketClient : public ThreadBase
{
public:
	/// ���캯��
	CMolTcpSocketClient();
	/// ��������
	~CMolTcpSocketClient(void);

	/// �ر�����
	void CloseConnect(bool isShow=false);
	/// ����ָ���ķ�����
	bool Connect(std::string ipaddress,int port);
	/// �������ӷ�����
	bool Reconnect(void);
		/// ���ص�ǰ����״̬
	inline ConnectState GetConnectState(void) { return m_bConnectState; }
	/// ��������
	int Send(CMolMessageOut &msg);
	int Send(char *msg,uint32 len);
	int Sendhtml5(char *Bytes,uint32 len);
	/// ��⵱ǰ�Ƿ���������
	inline bool IsConnected(void)
	{
		return m_bConnectState > NOCONNECT ? true : false;
	}

	int GetNetMessage(NetMessage & mes);
	void ExitWorkingThread(void);

private:
	virtual bool run();
	/// �õ���Ϣ�б�
	inline std::list<MessageStru>* GetMesList(void)
	{
		return &_MesList;
	}
	/// ���һ����Ϣ���б���
	inline void PushMessage(MessageStru mes)
	{
		_mesLock.Acquire();
		_MesList.push_back(mes);
		//++_mesLock_count;
		_mesLock.Release();
	}
	/// �õ���ǰ��Ϣ����
	inline int GetMesCount(void)
	{
		return (int)_MesList.size();
	}
	/// �����Ϣ�б�
	void ClearMesList(void);

private:
	void ProcessSelect(void);
	void GameMainLoop(void);	
	

private:
	int m_Socket;                 /**< ��ǰ�ؼ���socket��� */
	ConnectState m_bConnectState;    /**< ��ǰ������״̬ */
	CircularBuffer *m_ReadBuffer;  /**< �������ݻ����� */

	std::list<MessageStru> _MesList;
	Mutex _mesLock,_sendLock,m_ReadBufferLock;

	fd_set m_readableSet,m_writeableSet;
	fd_set m_exceptionSet;
	struct timeval lostHeartHintTime;
	struct timeval reconnectHintTime;
	volatile int sendedhearthintcount;

	volatile bool m_mainlooprunning;
	unsigned int remaining;
	unsigned short opcode;
	uint16 compress;
	uint32 mchecksum;
	char m_ipaddress[32];
	int m_port;

    // html5���Ƿ��������ӳɹ�
    AtomicBoolean m_html5connected;	
    char m_buffer[MOL_REV_BUFFER_SIZE_TWO];               
    unsigned long m_buffer_pos;
    packetheard m_packetheard;
    AtomicBoolean m_htmlMsgProcessed;  
    int masksOffset;
    int64 payloadSize;  

    AtomicULong             m_readTimer;
    AtomicCounter           m_readMsgCount;
    AtomicBoolean           m_readMsgBool;        
};

class CTcpSocketClientManager : public Singleton<CTcpSocketClientManager>
{
public:
	CTcpSocketClientManager();
	~CTcpSocketClientManager();

	int addTcpSocketClient(CMolTcpSocketClient *pClient);
	bool delTcpSocketClient(CMolTcpSocketClient *pClient);
	void ExitWorkingThread(void);
	void Update(void);
	void Sendhtml5(int serverindex,char *Bytes,uint32 len);
	void deleteAllTcpSocketClient(void);

private:
	std::vector<CMolTcpSocketClient*> m_TcpSocketClients;
};

#define MolTcpSocketClientManager CTcpSocketClientManager::getSingleton()

#endif
