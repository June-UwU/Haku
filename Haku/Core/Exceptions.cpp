#include "Exceptions.hpp"
#include <sstream>
#include <winerror.h>
namespace Haku
{
namespace Errors
{
Error::Error(std::string message) noexcept
{
	std::ostringstream str;

	str << "FILE : " << __FILE__ << "\n"
		<< "FUNCION : " << __FUNCTION__ << "\n"
		<< "LINE :" << __LINE__ << "\n"
		<< "MESSAGE : " << message << "\n";
	m_formatted_message = str.str();
}

const char* Error::what() const noexcept
{
	return m_formatted_message.c_str();
}
WinError::WinError(HRESULT code)
{
	char* buffer{};
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		code,
		LANG_SYSTEM_DEFAULT,
		LPSTR(&buffer),
		0,
		0);
	std::ostringstream str;

	str << "FILE : " << __FILE__ << "\n"
		<< "FUNCION : " << __FUNCTION__ << "\n"
		<< "LINE :" << __LINE__ << "\n"
		<< "CODE : " << code << "\n";
	if (buffer)
	{
		str << buffer;
	}
	else
	{
		str << "unknown";
	}
	m_formatted_message = str.str();
}
const char* WinError::what() const noexcept
{
	return m_formatted_message.c_str();
}
}; // namespace Errors
}; // namespace Haku
