#ifndef MESSAGEQUEUE_F77_H
#define MESSAGEQUEUE_F77_H


#include <wx/msgqueue.h> //include the wx source

enum messageQueue_F77_Error
{
    MSGQUEUE_F77_NO_ERROR= 0,	// operation completed successfully
    MSGQUEUE_F77_TIMEOUT,		// no messages received before timeout expired
    MSGQUEUE_F77_MISC_ERROR,	// some unexpected (and fatal) error has occurred
    MSGQUEUE_F77_QUEUE_EMPTY,		// queue is empty
};

template <typename T>
class messageQueue_F77
{
 public:
	 /// The type of the messages transported by this queue
	typedef T Message;
	 /// Default ctor creates an initially empty queue
	messageQueue_F77()
	   : m_conditionNotEmpty(m_mutex)
	{}

	 /// Add a message to this queue and signal the threads waiting for messages.
	 /// This method is safe to call from multiple threads in parallel.
	messageQueue_F77_Error Post(const Message& msg)
	{
		wxMutexLocker locker(m_mutex);
		wxCHECK( locker.IsOk(), MSGQUEUE_F77_MISC_ERROR );
		m_messages.push(msg);
		m_conditionNotEmpty.Signal();
		return MSGQUEUE_F77_NO_ERROR;
	}

	 ///use this to signify an exit attempt, this will work only if queue() is empty
	messageQueue_F77_Error Broadcast_exit()
	{
		wxMutexLocker locker(m_mutex);
		wxCHECK( locker.IsOk(), MSGQUEUE_F77_MISC_ERROR );
		if(m_messages.size()) return MSGQUEUE_F77_MISC_ERROR; //cant exit if there are still items in
		_exiting= true;
		m_conditionNotEmpty.Broadcast();
		return MSGQUEUE_F77_NO_ERROR;
	}
	 ///use this to forcefully exit, this will Clear() the queue()
	messageQueue_F77_Error Broadcast_exitForced()
	{
		wxMutexLocker locker(m_mutex);
		wxCHECK( locker.IsOk(), MSGQUEUE_F77_MISC_ERROR );
		if(m_messages.empty()){ //check first if empty
			std::queue<T> empty;
			std::swap(m_messages, empty);
		}
		_exiting= true;
		m_conditionNotEmpty.Broadcast();
		return MSGQUEUE_F77_NO_ERROR;
	}

	 /// Remove all messages from the queue.
	 /// This method is meant to be called from the same thread(s) that call
	 /// Post() to discard any still pending requests if they became unnecessary.
	messageQueue_F77_Error Clear()
	{
		wxCHECK( IsOk(), MSGQUEUE_F77_MISC_ERROR );
		wxMutexLocker locker(m_mutex);
		std::queue<T> empty;
		std::swap(m_messages, empty);
		return MSGQUEUE_F77_NO_ERROR;
	}
	size_t size() const
	{
		wxCHECK( IsOk(), MSGQUEUE_F77_MISC_ERROR );
		wxMutexLocker locker(m_mutex);
		return m_messages.size();
	}
	inline bool isEmpty() const { return !size(); }
	// Same as ReceiveTimeout() but waits for as long as it takes for a message
	// to become available (so it can't return wxMSGQUEUE_TIMEOUT)
	messageQueue_F77_Error Receive(T& msg)
	{
		wxCHECK( IsOk(), MSGQUEUE_F77_MISC_ERROR );
		wxMutexLocker locker(m_mutex);
		wxCHECK( locker.IsOk(), MSGQUEUE_F77_MISC_ERROR );
		while ( m_messages.empty() )
		{
			wxCondError result= m_conditionNotEmpty.Wait();
			if(_exiting){
				Message emptyMsg;
				msg= emptyMsg;
				return MSGQUEUE_F77_QUEUE_EMPTY;
			}
			wxCHECK( result == wxCOND_NO_ERROR, MSGQUEUE_F77_MISC_ERROR );
		}
		if(m_messages.size()){
			msg= m_messages.front();
			m_messages.pop();
			return MSGQUEUE_F77_NO_ERROR;
		}
		Message emptyMsg;
		msg= emptyMsg;
		return MSGQUEUE_F77_QUEUE_EMPTY;
	}
	// Wait no more than timeout milliseconds until a message becomes available.
	//
	// Setting timeout to 0 is equivalent to an infinite timeout. See Receive().
	messageQueue_F77_Error ReceiveTimeout(long timeout, T& msg)
	{
		wxCHECK( IsOk(), MSGQUEUE_F77_MISC_ERROR );
		wxMutexLocker locker(m_mutex);
		wxCHECK( locker.IsOk(), MSGQUEUE_F77_MISC_ERROR );
		const wxMilliClock_t waitUntil = wxGetLocalTimeMillis() + timeout;
		while ( m_messages.empty() )
		{
			wxCondError result= m_conditionNotEmpty.WaitTimeout(timeout);
			if(_exiting){
				Message emptyMsg;
				msg= emptyMsg;
				return MSGQUEUE_F77_QUEUE_EMPTY;
			}
			if ( result == wxCOND_NO_ERROR )
				continue;
			wxCHECK( result == wxCOND_TIMEOUT, MSGQUEUE_F77_MISC_ERROR );
			const wxMilliClock_t now = wxGetLocalTimeMillis();
			if ( now >= waitUntil )
				return MSGQUEUE_F77_TIMEOUT;
			timeout = (waitUntil - now).GetValue();
			wxASSERT(timeout > 0);
		}
		if(m_messages.size()){
			msg= m_messages.front();
			m_messages.pop();
			return MSGQUEUE_F77_NO_ERROR;
		}
		Message emptyMsg;
		msg= emptyMsg;
		return MSGQUEUE_F77_QUEUE_EMPTY;
	}

	// Return false only if there was a fatal error in ctor
	bool IsOk() const
	{
		return m_conditionNotEmpty.IsOk();
	}

 protected:
	// Disable copy ctor and assignment operator
	messageQueue_F77(const messageQueue_F77<T>& rhs);
	messageQueue_F77<T>& operator=(const messageQueue_F77<T>& rhs);

	bool _exiting= false;
	mutable wxMutex m_mutex;
	wxCondition     m_conditionNotEmpty;

	std::queue<T>   m_messages;
};

#endif // MESSAGEQUEUE_F77_H
