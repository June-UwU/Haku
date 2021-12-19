#pragma once
#include "../hakupch.hpp"
#include "../macros.hpp"
#include <exception>
namespace Haku
{
namespace Errors
{
class HAKU_API Error : public std::exception
{
public:
	Error(std::string message) noexcept;
	~Error() = default;
	const char* what() const noexcept;

private:
	std::string m_formatted_message;
};

class HAKU_API WinError : public std::exception
{
public:
	WinError(HRESULT code);
	const char* what() const noexcept;

private:
	std::string m_formatted_message;
};
}; // namespace Errors
}; // namespace Haku
