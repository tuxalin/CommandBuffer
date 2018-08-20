// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2014 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.

#include "windows/NsWindowsInclude.h"
#include "NsSocket.h"
#include "NsArray.h"
#include "NsIntrinsics.h"
#include "NvIntrinsics.h"

#pragma warning(disable : 4061 4062 4350 4555 4571 4625 4626 4946)
#include "ppltasks.h"

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;

namespace nvidia
{
namespace shdfnd
{

const uint32_t Socket::DEFAULT_BUFFER_SIZE = 32768;

class SocketImpl
{
  public:
	SocketImpl(bool isBlocking);
	virtual ~SocketImpl();

	bool connect(const char* host, uint16_t port, uint32_t timeout);
	bool listen(uint16_t port);
	void disconnect();

	void setBlocking(bool blocking);

	virtual uint32_t write(const uint8_t* data, uint32_t length);
	virtual bool flush();

	uint32_t read(uint8_t* data, uint32_t length);

	NV_FORCE_INLINE bool isBlocking() const
	{
		return mIsBlocking;
	}
	NV_FORCE_INLINE bool isConnected() const
	{
		return mIsConnected;
	}
	NV_FORCE_INLINE const char* getHost() const
	{
		return mHost;
	}
	NV_FORCE_INLINE uint16_t getPort() const
	{
		return mPort;
	}

  protected:
	StreamSocket ^ mSocket;
	StreamSocketListener ^ mListenSocket;
	DataWriter ^ mWriter;
	DataReader ^ mReader;
	DataReaderLoadOperation ^ mAsyncReadTask;
	DataReaderLoadOperation ^ mBlockingReadTask;
	const char* mHost;
	uint16_t mPort;
	bool mIsConnected;
	bool mIsBlocking;
};

SocketImpl::SocketImpl(bool isBlocking)
: mSocket(nullptr)
, mListenSocket(nullptr)
, mWriter(nullptr)
, mReader(nullptr)
, mAsyncReadTask(nullptr)
, mBlockingReadTask(nullptr)
, mPort(0)
, mHost(NULL)
, mIsConnected(false)
, mIsBlocking(isBlocking)
{
}

SocketImpl::~SocketImpl()
{
	if(mSocket)
		delete mSocket;
	if(mListenSocket)
		delete mListenSocket;
}

void SocketImpl::setBlocking(bool blocking)
{
	if(blocking != mIsBlocking)
	{
		mIsBlocking = blocking;
	}
}

bool SocketImpl::flush()
{
	try
	{
		if(mWriter)
		{
			auto flush = mWriter->FlushAsync();
			task<bool>(flush).wait();
			return flush->GetResults();
		}
	}
	catch(Exception ^ )
	{
	}
	return false;
};

static void NvConvertToWchar(const char* inString, WCHAR* outString, int outSize)
{
	// convert to unicode
	int succ = MultiByteToWideChar(CP_ACP, 0, inString, -1, outString, outSize);

	// validate
	if(succ < 0)
		succ = 0;
	if(succ < outSize)
		outString[succ] = 0;
	else if(outString[outSize - 1])
		outString[0] = 0;
}

bool SocketImpl::connect(const char* host, uint16_t port, uint32_t timeout)
{
	try
	{
		// convert host name char to wchar/String
		WCHAR hostNameWchar[256];
		NvConvertToWchar(host, hostNameWchar, 256);
		String ^ hostNameString = ref new String(hostNameWchar);
		HostName ^ hostName = ref new HostName(hostNameString);

		// signal to handle timeout
		HANDLE signal = CreateEventEx(NULL, NULL, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);

		auto socket = ref new StreamSocket();

		// onComplete lambda
		auto completionDelegate = [signal, this](IAsyncAction ^ action, AsyncStatus /*status*/)
		{
			switch(action->Status)
			{
			case AsyncStatus::Started:
			case AsyncStatus::Canceled:
				break;
			case AsyncStatus::Completed:
				mIsConnected = true;
			case AsyncStatus::Error:
				SetEvent(signal);
				break;
			}
		};

		mIsConnected = false;
		auto async = socket->ConnectAsync(hostName, port.ToString());
		async->Completed = ref new AsyncActionCompletedHandler(completionDelegate, Platform::CallbackContext::Any);
		WaitForSingleObjectEx(signal, timeout, false);
		async->Cancel(); // ensures that onComplete callback is triggered before closing the handle.
		CloseHandle(signal);

		if(mIsConnected)
		{
			mPort = port;
			mHost = host;
			mSocket = socket;
			mWriter = ref new DataWriter(mSocket->OutputStream);
			mReader = ref new DataReader(mSocket->InputStream);
			mReader->InputStreamOptions = InputStreamOptions::Partial;
		}
		else
		{
			delete socket;
			mPort = 0;
			mHost = NULL;
		}

		return mIsConnected;
	}
	catch(Exception ^ /*e*/)
	{
		return false;
	}
}

bool SocketImpl::listen(uint16_t port)
{
	// onConnect Lambda
	auto onConnectDelegate = [this](StreamSocketListener ^ listener,
	                                StreamSocketListenerConnectionReceivedEventArgs ^ object)
	{
		mSocket = object->Socket; // first client we have encountered
		mWriter = ref new DataWriter(mSocket->OutputStream);
		mReader = ref new DataReader(mSocket->InputStream);
		mReader->InputStreamOptions = InputStreamOptions::Partial;
		mIsConnected = true;
	};

	try
	{
		auto listenSocket = ref new StreamSocketListener();
		listenSocket->ConnectionReceived +=
		    ref new TypedEventHandler<StreamSocketListener ^, StreamSocketListenerConnectionReceivedEventArgs ^ >(
		        onConnectDelegate);

		task<void>(listenSocket->BindServiceNameAsync(port.ToString())).then([this, port](task<void> previousTask)
		{
			try
			{
				// Try getting all exceptions from the continuation chain above this point.
				previousTask.get();
				mPort = port;
			}
			catch(Exception ^ /*exception*/)
			{
			}
		});
		mListenSocket = listenSocket;
		return true;
	}
	catch(Exception ^ )
	{
		return false;
	}
}

void SocketImpl::disconnect()
{
	if(mBlockingReadTask)
		mBlockingReadTask->Cancel();
	if(mAsyncReadTask)
		mAsyncReadTask->Cancel();

	if(mSocket)
		delete mSocket;
	if(mListenSocket)
		delete mListenSocket;

	mListenSocket = nullptr;
	mSocket = nullptr;
	mReader = nullptr;
	mWriter = nullptr;
	mIsConnected = false;
	mPort = 0;
	mHost = NULL;
}

uint32_t SocketImpl::write(const uint8_t* data, uint32_t length)
{
	if(length == 0 || !mSocket)
		return 0;

	try
	{
		auto tempData = ref new Platform::Array<uint8>(const_cast<uint8*>(data), length);
		mWriter->WriteBytes(tempData);

		auto storeAsync = mWriter->StoreAsync();
		auto t = task<unsigned int>(storeAsync);

		if(mIsBlocking)
		{
			t.wait();
			return storeAsync->GetResults();
		}
		else
			return length;
	}
	catch(Exception ^ )
	{
		mIsConnected = false;
		return 0;
	}
}

uint32_t SocketImpl::read(uint8_t* data, uint32_t length)
{
	if(length == 0)
	{
		return 0;
	}

	uint32_t len = 0;

	if(mIsBlocking)
	{
		try
		{
			mBlockingReadTask = mReader->LoadAsync(length);
			auto t = task<unsigned int>(mBlockingReadTask);
			t.wait();

			auto lReader = mReader;
			mBlockingReadTask = nullptr;

			if(!lReader)
				return 0;

			len = lReader->UnconsumedBufferLength;
			if(len)
			{
				NV_ASSERT(len <= length);
				auto tempData = ref new Platform::Array<uint8>(len);
				lReader->ReadBytes(tempData);
				memcpy(data, tempData->begin(), __min(len, length));
			}
			else
				mIsConnected = false;
			return len;
		}
		catch(Exception ^ )
		{
			mIsConnected = false;
			return 0;
		}
	}
	else
	{
		if(mAsyncReadTask)
		{
			if(mAsyncReadTask->Status == AsyncStatus::Completed)
			{
				auto lReader = mReader;
				if(!lReader)
					return 0;

				len = mReader->UnconsumedBufferLength;
				if(len)
				{
					NV_ASSERT(len <= length);
					auto tempData = ref new Platform::Array<uint8>(len);
					mReader->ReadBytes(tempData);
					memcpy(data, tempData->begin(), __min(len, length));
				}

				mAsyncReadTask = mReader->LoadAsync(length);
			}
		}
		else
		{
			mAsyncReadTask = mReader->LoadAsync(length);
		}
	}
	return len;
}

class BufferedSocketImpl : public SocketImpl
{
  public:
	BufferedSocketImpl(bool isBlocking) : SocketImpl(isBlocking), mBufferPos(0)
	{
	}
	virtual ~BufferedSocketImpl()
	{
	}
	bool flush();
	uint32_t write(const uint8_t* data, uint32_t length);

  private:
	uint32_t mBufferPos;
	uint8_t mBuffer[Socket::DEFAULT_BUFFER_SIZE];
};

bool BufferedSocketImpl::flush()
{
	uint32_t totalBytesWritten = 0;

	while(totalBytesWritten < mBufferPos && mIsConnected)
		totalBytesWritten += (int32_t)SocketImpl::write(mBuffer + totalBytesWritten, mBufferPos - totalBytesWritten);

	bool ret = (totalBytesWritten == mBufferPos);
	mBufferPos = 0;
	return ret;
}

uint32_t BufferedSocketImpl::write(const uint8_t* data, uint32_t length)
{
	uint32_t bytesWritten = 0;
	while(mBufferPos + length >= Socket::DEFAULT_BUFFER_SIZE)
	{
		uint32_t currentChunk = Socket::DEFAULT_BUFFER_SIZE - mBufferPos;
		intrinsics::memCopy(mBuffer + mBufferPos, data + bytesWritten, currentChunk);
		bytesWritten += (uint32_t)currentChunk; // for the user, this is consumed even if we fail to shove it down a
		// non-blocking socket

		uint32_t sent = SocketImpl::write(mBuffer, Socket::DEFAULT_BUFFER_SIZE);
		mBufferPos = Socket::DEFAULT_BUFFER_SIZE - sent;

		if(sent < Socket::DEFAULT_BUFFER_SIZE) // non-blocking or error
		{
			if(sent) // we can reasonably hope this is rare
				intrinsics::memMove(mBuffer, mBuffer + sent, mBufferPos);

			return bytesWritten;
		}
		length -= currentChunk;
	}

	if(length > 0)
	{
		intrinsics::memCopy(mBuffer + mBufferPos, data + bytesWritten, length);
		bytesWritten += length;
		mBufferPos += length;
	}

	return bytesWritten;
}

Socket::Socket(bool inIsBuffering, bool isBlocking)
{
	if(inIsBuffering)
	{
		void* mem = NV_ALLOC(sizeof(BufferedSocketImpl), "BufferedSocketImpl");
		mImpl = NV_PLACEMENT_NEW(mem, BufferedSocketImpl)(isBlocking);
	}
	else
	{
		void* mem = NV_ALLOC(sizeof(SocketImpl), "SocketImpl");
		mImpl = NV_PLACEMENT_NEW(mem, SocketImpl)(isBlocking);
	}
}

Socket::~Socket()
{
	mImpl->flush();
	mImpl->disconnect();
	mImpl->~SocketImpl();
	NV_FREE(mImpl);
}

bool Socket::connect(const char* host, uint16_t port, uint32_t timeout)
{
	return mImpl->connect(host, port, timeout);
}

bool Socket::listen(uint16_t port)
{
	return mImpl->listen(port);
}

void Socket::disconnect()
{
	mImpl->disconnect();
}

bool Socket::isConnected() const
{
	return mImpl->isConnected();
}

const char* Socket::getHost() const
{
	return mImpl->getHost();
}

uint16_t Socket::getPort() const
{
	return mImpl->getPort();
}

bool Socket::flush()
{
	if(!mImpl->isConnected())
		return false;
	return mImpl->flush();
}

uint32_t Socket::write(const uint8_t* data, uint32_t length)
{
	if(!mImpl->isConnected())
		return 0;
	return mImpl->write(data, length);
}

uint32_t Socket::read(uint8_t* data, uint32_t length)
{
	if(!mImpl->isConnected())
		return 0;
	return mImpl->read(data, length);
}

void Socket::setBlocking(bool blocking)
{
	mImpl->setBlocking(blocking);
}

bool Socket::isBlocking() const
{
	return mImpl->isBlocking();
}

bool Socket::accept(bool)
{
	return false;
}

} // namespace shdfnd
} // namespace nvidia
