// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

//////////////////////////////////////////////////////////////////////////
// NOTE: INTERNAL HEADER NOT FOR PUBLIC USE
// This header should only be include by CrySocks.h
#if !defined(INCLUDED_FROM_CRY_SOCKS_H)
	#error "CRYTEK INTERNAL HEADER. Only include from CrySocks.h."
#endif
//////////////////////////////////////////////////////////////////////////

#include <np.h>
#include <net.h>
#include <sys/socket.h>
#include <libnet/socket.h>
#include <arpa/inet.h>

typedef SceNetSockaddr   CRYSOCKADDR;
typedef SceNetSockaddrIn CRYSOCKADDR_IN;
typedef SceNetSocklen_t  CRYSOCKLEN_T;
typedef int              CRYSOCKET;
typedef SceNetInAddr     CRYINADDR;
typedef SceNetInAddr_t   CRYINADDR_T;
// Note:
// SCE uses SceNetId for SOCKET as well as the genuine return type int, that is returned by most Win32 socket functions

// Type wrappers for sockets
typedef fd_set  CRYFD_SET;
typedef timeval CRYTIMEVAL;

#define SD_SEND                    SCE_NET_SHUT_WR
#define SD_RECEIVE                 SCE_NET_SHUT_RD
#define SD_BOTH                    SCE_NET_SHUT_RDWR
#define SO_NBIO                    SCE_NET_SO_NBIO

#define CRY_INVALID_SOCKET         CRYSOCKET(-1)
#define CRY_SOCKET_ERROR           -1

#define sockaddr_in_p2p            SceNetSockaddrIn

#define CRY_PSN_PORT               SCE_NP_PORT
#define CRY_ORBIS_SOCKET_USECRYPTO SCE_NET_SO_USECRYPTO

#if CRY_PLATFORM_ORBIS
	#define CRY_ORBIS_SOCKET_DGRAM SCE_NET_SOCK_DGRAM_P2P
#endif

extern in_addr_t inet_addr(const char*);
extern char*     inet_ntoa(struct in_addr);

namespace CrySock
{

///////////////////////////////////////////////////////////////////////////////
// Forward declarations
static eCrySockError TranslateLastSocketError();
static int           TranslateToSocketError(eCrySockError socketError);

static eCrySockError TranslateInvalidSocket(CRYSOCKET s);
static eCrySockError TranslateSocketError(int socketError);

///////////////////////////////////////////////////////////////////////////////

static int GetLastSocketError()
{
	return sce_net_errno;
}

///////////////////////////////////////////////////////////////////////////////

static CRYSOCKET socket(int af, int type, int protocol, const char* dbg_socket_name = "")
{
	SceNetId res = sceNetSocket(dbg_socket_name, af, type, protocol);
	if (res < 0)
	{
		return CRY_INVALID_SOCKET;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

//! Create default INET socket.
static CRYSOCKET socketinet(const char* dbg_socket_name = "")
{
	SceNetId res = sceNetSocket(dbg_socket_name, AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (res < 0)
	{
		return CRY_INVALID_SOCKET;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int setsockopt(CRYSOCKET s, int level, int optname, const char* optval, CRYSOCKLEN_T optlen)
{
	SceNetId res = sceNetSetsockopt(s, level, optname, optval, optlen);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int shutdown(CRYSOCKET s, int how)
{
	SceNetId res = sceNetShutdown(s, how);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int getsockname(CRYSOCKET s, CRYSOCKADDR* addr, CRYSOCKLEN_T* addrlen)
{
	SceNetId res = sceNetGetsockname(s, addr, addrlen);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int listen(CRYSOCKET s, int backlog)
{
	SceNetId res = sceNetListen(s, backlog);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int recv(CRYSOCKET s, char* buf, int len, int flags)
{
	SceNetId res = sceNetRecv(s, buf, len, flags);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int recvfrom(CRYSOCKET s, char* buf, int len, int flags, CRYSOCKADDR* addr, CRYSOCKLEN_T* addrLen)
{
	SceNetId res = sceNetRecvfrom(s, buf, len, flags, addr, addrLen);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int sendto(CRYSOCKET s, char* buf, int len, int flags, CRYSOCKADDR* addr, CRYSOCKLEN_T addrLen)
{
	SceNetId res = sceNetSendto(s, buf, len, flags, addr, addrLen);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int closesocket(CRYSOCKET s)
{
	SceNetId res = sceNetSocketClose(s);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int connect(CRYSOCKET s, const CRYSOCKADDR* addr, CRYSOCKLEN_T addrlen)
{
	SceNetId res = sceNetConnect(s, addr, addrlen);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static CRYSOCKET accept(CRYSOCKET s, CRYSOCKADDR* addr, CRYSOCKLEN_T* addrlen)
{
	SceNetId res = sceNetAccept((SceNetId)s, addr, addrlen);
	if (res < 0)
	{
		return CRY_INVALID_SOCKET;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static int send(CRYSOCKET s, const char* buf, int len, int flags)
{
	int msgNoSignal = MSG_NOSIGNAL;
	SceNetId res = sceNetSend(s, buf, len, flags | msgNoSignal);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////

static bool MakeSocketNonBlocking(CRYSOCKET sock)
{
	int nonblocking = 1;
	if (sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &nonblocking, sizeof(int)) < 0)
	{
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

static bool MakeSocketBlocking(CRYSOCKET sock)
{
	int nonblocking = 0;
	if (sceNetSetsockopt(sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &nonblocking, sizeof(int)) < 0)
	{
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////

struct CRYHOSTENT
{
};
static struct CRYHOSTENT* gethostbyname(const char* hostname)
{
	CryFatalError("(SCE) gethostbyname is not supported. Use sceNetResolver*() function family instead. ");
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////

static int gethostname(char* name, int namelen)
{
	CryFatalError("(SCE) gethostname is not supported. Use sceNetResolver*() function family instead. ");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

static CRYINADDR_T GetAddrForHostOrIP(const char* hostnameOrIP, uint32 timeoutMilliseconds = 0)
{
	uint32 ret = 0;
	bool bSuccess = false;
	SceNetInAddr sceaddr;
	SceNetId poolId = sceNetPoolCreate(__FUNCTION__, 4 * 1024, 0);
	if (poolId >= 0)
	{
		SceNetId rid = sceNetResolverCreate("CrySock resolver", poolId, 0);
		if (rid >= 0)
		{
			if (sceNetResolverStartNtoa(rid, hostnameOrIP, &sceaddr, 0, 0, 0) < 0)
			{
				CryWarning(VALIDATOR_MODULE_NETWORK, VALIDATOR_ERROR, "Failed DNS lookup of '%s'\n", hostnameOrIP);
			}
			else
			{
				bSuccess = true;
			}
			sceNetResolverDestroy(rid);
		}
		else
		{
			CryWarning(VALIDATOR_MODULE_NETWORK, VALIDATOR_ERROR, "Failed to create DNS resolver\n");
		}
		sceNetPoolDestroy(poolId);
	}
	else
	{
		CryWarning(VALIDATOR_MODULE_NETWORK, VALIDATOR_ERROR, "Failed to create DNS resolver pool\n");
	}
	if (bSuccess)
		ret = sceaddr.s_addr;
	else
		ret = inet_addr(hostnameOrIP);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////

static int SetRecvTimeout(CRYSOCKET s, const int seconds, const int microseconds)
{
	struct timeval timeout;
	timeout.tv_sec = seconds;
	timeout.tv_usec = microseconds;
	return CrySock::setsockopt(s, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
}

///////////////////////////////////////////////////////////////////////////////

static int SetSendTimeout(CRYSOCKET s, const int seconds, const int microseconds)
{
	struct timeval timeout;
	timeout.tv_sec = seconds;
	timeout.tv_usec = microseconds;
	return CrySock::setsockopt(s, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
}

///////////////////////////////////c////////////////////////////////////////////

static int bind(CRYSOCKET s, const CRYSOCKADDR* addr, CRYSOCKLEN_T addrlen)
{
	SceNetId res = sceNetBind(s, addr, addrlen);
	if (res < 0)
	{
		return CRY_SOCKET_ERROR;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////
//! \retval 1   Readable - use recv to get the data.
//! \retval 0   Timed out (exceeded passed timeout value).
//! \retval <0  An error occurred - see eCrySocketError.
static int IsRecvPending(CRYSOCKET s, CRYTIMEVAL* timeout)
{
	CRYFD_SET emptySet;
	FD_ZERO(&emptySet);

	CRYFD_SET readSet;
	FD_ZERO(&readSet);
	FD_SET(s, &readSet);

	int ret = ::select(s + 1, &readSet, &emptySet, &emptySet, timeout);

	if (ret >= 0)
	{
		ret = FD_ISSET(s, &readSet);
		if (ret != 0)
		{
			ret = 1;
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
//! \retval 1   Connected.
//! \retval 0   Timed out (exceeded passed timeout value).
//! \retval <0  An error occurred - see eCrySocketError.
static int WaitForWritableSocket(CRYSOCKET s, CRYTIMEVAL* timeout)
{
	CRYFD_SET emptySet;
	FD_ZERO(&emptySet);

	CRYFD_SET writeSet;
	FD_ZERO(&writeSet);
	FD_SET(s, &writeSet);

	int ret = ::select(s + 1, &emptySet, &writeSet, &emptySet, timeout);

	if (ret >= 0)
	{
		ret = FD_ISSET(s, &writeSet);
		if (ret != 0)
		{
			ret = 1;
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
//! IsRecvPending should be sufficient for most applications.
//! The correct value for the first parameter 'nfds' should be the highest
//! numbered socket (as passed to FD_SET) +1.
//! Only use this if you know what you're doing! Passing nfds as 0 on anything
//! other than WinSock is wrong!.
static int select(int nfds, CRYFD_SET* readfds, CRYFD_SET* writefds, CRYFD_SET* exceptfds, CRYTIMEVAL* timeout)
{
#if !defined(_RELEASE)
	if (nfds == 0)
	{
		CryFatalError("CrySock select detected first parameter is 0!  This *MUST* be fixed!");
	}
#endif

	return ::select(nfds, readfds, writefds, exceptfds, timeout);
}

///////////////////////////////////////////////////////////////////////////////
static uint32 DNSLookup(const char* hostname, uint32 timeoutMilliseconds = 200)
{
	CRYINADDR_T ret = CrySock::GetAddrForHostOrIP(hostname, timeoutMilliseconds);

	if (ret == 0 || ret == ~0)
	{
		char host[256];

		size_t hostlen = strlen(hostname);
		size_t domainlen = sizeof(LOCAL_FALLBACK_DOMAIN);
		if (hostlen + domainlen > sizeof(host) - 1)
		{
			CryWarning(VALIDATOR_MODULE_NETWORK, VALIDATOR_ERROR, "hostname too long to fallback to local domain: '%s'", hostname);
			return 0;
		}

		cry_strcpy(host, hostname);
		cry_strcat(host, LOCAL_FALLBACK_DOMAIN);

		host[hostlen + domainlen - 1] = 0;

		ret = CrySock::GetAddrForHostOrIP(host, timeoutMilliseconds);
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////

static CRYINADDR_T inet_addr(const char* cp)
{
	//Concider using sceNetInetPton()
	return ::inet_addr(cp);
}

///////////////////////////////////////////////////////////////////////////////

static char* inet_ntoa(CRYINADDR in)
{
	in_addr tmp = { 0 };
	tmp.s_addr = in.s_addr;
	// Concider using sceNetInetNtop()
	return ::inet_ntoa(tmp);
}

///////////////////////////////////////////////////////////////////////////////

static eCrySockError TranslateInvalidSocket(CRYSOCKET s)
{
	if (s < 0)
	{
		return TranslateLastSocketError();
	}
	return eCSE_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////

static eCrySockError TranslateSocketError(int socketError)
{
	if (socketError < 0)
	{
		return TranslateLastSocketError();
	}
	return eCSE_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////

static eCrySockError TranslateLastSocketError()
{
#define TRANSLATE(_from, _to) case (_from): \
  socketError = (_to);  break;
	eCrySockError socketError = eCSE_NO_ERROR;
	int error = GetLastSocketError();
	switch (error)
	{
		TRANSLATE(0, eCSE_NO_ERROR);
		TRANSLATE(SCE_NET_ERROR_EACCES, eCSE_EACCES);
		TRANSLATE(SCE_NET_ERROR_EADDRINUSE, eCSE_EADDRINUSE);
		TRANSLATE(SCE_NET_ERROR_EADDRNOTAVAIL, eCSE_EADDRNOTAVAIL);
		TRANSLATE(SCE_NET_ERROR_EAFNOSUPPORT, eCSE_EAFNOSUPPORT);
		TRANSLATE(SCE_NET_ERROR_EALREADY, eCSE_EALREADY);
		TRANSLATE(SCE_NET_ERROR_EBADF, eCSE_EBADF);
		TRANSLATE(SCE_NET_ERROR_ECONNABORTED, eCSE_ECONNABORTED);
		TRANSLATE(SCE_NET_ERROR_ECONNREFUSED, eCSE_ECONNREFUSED);
		TRANSLATE(SCE_NET_ERROR_ECONNRESET, eCSE_ECONNRESET);
		TRANSLATE(SCE_NET_ERROR_EFAULT, eCSE_EFAULT);
		TRANSLATE(SCE_NET_ERROR_EHOSTDOWN, eCSE_EHOSTDOWN);
		TRANSLATE(SCE_NET_ERROR_EINPROGRESS, eCSE_EINPROGRESS);
		TRANSLATE(SCE_NET_ERROR_EINTR, eCSE_EINTR);
		TRANSLATE(SCE_NET_ERROR_EINVAL, eCSE_EINVAL);
		TRANSLATE(SCE_NET_ERROR_EISCONN, eCSE_EISCONN);
		TRANSLATE(SCE_NET_ERROR_EMFILE, eCSE_EMFILE);
		TRANSLATE(SCE_NET_ERROR_EMSGSIZE, eCSE_EMSGSIZE);
		TRANSLATE(SCE_NET_ERROR_ENETUNREACH, eCSE_ENETUNREACH);
		TRANSLATE(SCE_NET_ERROR_ENOBUFS, eCSE_ENOBUFS);
		TRANSLATE(SCE_NET_ERROR_ENOPROTOOPT, eCSE_ENOPROTOOPT);
		TRANSLATE(SCE_NET_ERROR_ENOTCONN, eCSE_ENOTCONN);
		TRANSLATE(SCE_NET_ERROR_EOPNOTSUPP, eCSE_EOPNOTSUPP);
		TRANSLATE(SCE_NET_ERROR_EPROTONOSUPPORT, eCSE_EPROTONOSUPPORT);
		TRANSLATE(SCE_NET_ERROR_ETIMEDOUT, eCSE_ETIMEDOUT);
		TRANSLATE(SCE_NET_ERROR_ETOOMANYREFS, eCSE_ETOOMANYREFS);
		TRANSLATE(SCE_NET_ERROR_EWOULDBLOCK, eCSE_EWOULDBLOCK);
	default:
		CryLog("CrySock could not translate OS error code %x, treating as miscellaneous", error);
		socketError = eCSE_MISC_ERROR;
		break;
	}
#undef TRANSLATE

	return socketError;
}

///////////////////////////////////////////////////////////////////////////////

static int TranslateToSocketError(eCrySockError socketError)
{
#define TRANSLATE(_to, _from) case (_from): \
  error = (_to); break;                                            // reverse order of inputs (_to, _from) instead of (_from, _to) compared to TranslateLastSocketError
	int error = 0;
	switch (socketError)
	{
		TRANSLATE(0, eCSE_NO_ERROR);
		TRANSLATE(SCE_NET_ERROR_EACCES, eCSE_EACCES);
		TRANSLATE(SCE_NET_ERROR_EADDRINUSE, eCSE_EADDRINUSE);
		TRANSLATE(SCE_NET_ERROR_EADDRNOTAVAIL, eCSE_EADDRNOTAVAIL);
		TRANSLATE(SCE_NET_ERROR_EAFNOSUPPORT, eCSE_EAFNOSUPPORT);
		TRANSLATE(SCE_NET_ERROR_EALREADY, eCSE_EALREADY);
		TRANSLATE(SCE_NET_ERROR_EBADF, eCSE_EBADF);
		TRANSLATE(SCE_NET_ERROR_ECONNABORTED, eCSE_ECONNABORTED);
		TRANSLATE(SCE_NET_ERROR_ECONNREFUSED, eCSE_ECONNREFUSED);
		TRANSLATE(SCE_NET_ERROR_ECONNRESET, eCSE_ECONNRESET);
		TRANSLATE(SCE_NET_ERROR_EFAULT, eCSE_EFAULT);
		TRANSLATE(SCE_NET_ERROR_EHOSTDOWN, eCSE_EHOSTDOWN);
		TRANSLATE(SCE_NET_ERROR_EINPROGRESS, eCSE_EINPROGRESS);
		TRANSLATE(SCE_NET_ERROR_EINTR, eCSE_EINTR);
		TRANSLATE(SCE_NET_ERROR_EINVAL, eCSE_EINVAL);
		TRANSLATE(SCE_NET_ERROR_EISCONN, eCSE_EISCONN);
		TRANSLATE(SCE_NET_ERROR_EMFILE, eCSE_EMFILE);
		TRANSLATE(SCE_NET_ERROR_EMSGSIZE, eCSE_EMSGSIZE);
		TRANSLATE(SCE_NET_ERROR_ENETUNREACH, eCSE_ENETUNREACH);
		TRANSLATE(SCE_NET_ERROR_ENOBUFS, eCSE_ENOBUFS);
		TRANSLATE(SCE_NET_ERROR_ENOPROTOOPT, eCSE_ENOPROTOOPT);
		TRANSLATE(SCE_NET_ERROR_ENOTCONN, eCSE_ENOTCONN);
		TRANSLATE(SCE_NET_ERROR_EOPNOTSUPP, eCSE_EOPNOTSUPP);
		TRANSLATE(SCE_NET_ERROR_EPROTONOSUPPORT, eCSE_EPROTONOSUPPORT);
		TRANSLATE(SCE_NET_ERROR_ETIMEDOUT, eCSE_ETIMEDOUT);
		TRANSLATE(SCE_NET_ERROR_ETOOMANYREFS, eCSE_ETOOMANYREFS);
		TRANSLATE(SCE_NET_ERROR_EWOULDBLOCK, eCSE_EWOULDBLOCK);
	default:
		CRY_ASSERT_MESSAGE(false, string().Format("CrySock could not translate eCrySockError error code %x, treating as miscellaneous", socketError));
		break;
	}
#undef TRANSLATE

	return error;
}

} // CrySock
