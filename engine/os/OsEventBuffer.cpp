#include "OsEventBuffer.h"
#include "Log.h"

namespace crown
{

//-----------------------------------------------------------------------------
OsEventBuffer::OsEventBuffer() : m_size(0)
{
}

//-----------------------------------------------------------------------------
void OsEventBuffer::push_event(OsEventType event_type, void* event_data, size_t event_size)
{
	if (m_size + sizeof(event_type) + sizeof(event_size) + event_size > MAX_OS_EVENT_BUFFER_SIZE)
	{
		flush();
	}

	char* cur = m_buffer + m_size;

	*(uint32_t*) cur = event_type;
	*(size_t*) (cur + sizeof(event_type)) = event_size;
	memcpy(cur + sizeof(event_type) + sizeof(event_size), event_data, event_size);

	m_size = sizeof(event_type) + sizeof(event_size) + event_size;

	Log::i("event type: %d", event_type);
}

//-----------------------------------------------------------------------------
void OsEventBuffer::push_event_buffer(char* event_buffer, size_t buffer_size)
{
	if (m_size + buffer_size > MAX_OS_EVENT_BUFFER_SIZE)
	{
		flush();
	}

	char* cur = m_buffer + m_size;

	memcpy(cur, event_buffer, buffer_size);

	m_size += buffer_size;
}


//-----------------------------------------------------------------------------
void* OsEventBuffer::get_next_event(uint32_t& event_type, size_t& event_size)
{
	static size_t read = 0;

	if (read < m_size)
	{
		char* cur = m_buffer + read;

		// Saves type
		event_type = *(uint32_t*) cur;
		// Saves size
		event_size = *(size_t*) cur + sizeof(uint32_t);

		// Set read to next event
		read += sizeof(size_t) + sizeof(uint32_t) + event_size;

		return cur + sizeof(size_t) + sizeof(uint32_t);
	}

	read = 0;

	return NULL;
}

//-----------------------------------------------------------------------------
void OsEventBuffer::clear()
{
	m_size = 0;
}

//-----------------------------------------------------------------------------
void OsEventBuffer::flush()
{
	m_size = 0;
}

//-----------------------------------------------------------------------------
size_t OsEventBuffer::size() const
{
	return m_size;
}

//-----------------------------------------------------------------------------
char* OsEventBuffer::buffer()
{
	return m_buffer;
}

//-----------------------------------------------------------------------------
OsEventBuffer g_os_event_buffer;

OsEventBuffer* os_event_buffer()
{
	return &g_os_event_buffer;
}

} // namespace crown