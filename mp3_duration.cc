#include "mp3_duration.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <mciapi.h>
#include <memory>

namespace
{

inline auto mcisendcommanda_impl(::std::uint_least32_t IDDevice,::std::uint_least32_t uMsg,::std::uintptr_t fdwCommand,::std::uintptr_t dwParam)
{
	return mciSendCommandA(IDDevice,uMsg,fdwCommand,dwParam);
}

inline auto mcisendcommandw_impl(::std::uint_least32_t IDDevice,::std::uint_least32_t uMsg,::std::uintptr_t fdwCommand,::std::uintptr_t dwParam)
{
	return mciSendCommandW(IDDevice,uMsg,fdwCommand,dwParam);
}

template<::fast_io::win32_family family>
inline auto mcisendcommand(::std::uint_least32_t IDDevice,::std::uint_least32_t uMsg,::std::uintptr_t fdwCommand,::std::uintptr_t dwParam) noexcept
{
	if constexpr(family==::fast_io::win32_family::ansi_9x)
	{
		return ::fast_io::noexcept_call(mcisendcommanda_impl,IDDevice,uMsg,fdwCommand,dwParam);
	}
	else
	{
		return ::fast_io::noexcept_call(mcisendcommandw_impl,IDDevice,uMsg,fdwCommand,dwParam);
	}
}

template<::fast_io::win32_family family>
struct mci_close_wrapper
{
	::std::uint_least32_t IDDevice{};
	explicit mci_close_wrapper(::std::uint_least32_t dv) noexcept:IDDevice(dv)
	{

	}
	mci_close_wrapper& operator=(mci_close_wrapper const&)=delete;
	mci_close_wrapper(mci_close_wrapper const&)=delete;
	~mci_close_wrapper()
	{
		mcisendcommand<family>(IDDevice,MCI_CLOSE,0,0);
	}
};


}

#define TRY_DURATION_RESULT(x) if(x) {return {0,true};}

duration_result get_mp3_duration(char const *filename) noexcept
{
	::std::uint_least32_t deviceid{};
	if constexpr(::fast_io::win32_family::native==::fast_io::win32_family::ansi_9x)
	{
		MCI_OPEN_PARMSA params{.lpstrDeviceType=nullptr,
		.lpstrElementName=reinterpret_cast<char const*>(filename)};
		TRY_DURATION_RESULT(mcisendcommand<::fast_io::win32_family::ansi_9x>(0,MCI_OPEN,MCI_OPEN_ELEMENT,(uintptr_t)std::addressof(params)));
		deviceid=params.wDeviceID;
	}
	else
	{
		MCI_OPEN_PARMSW params{.lpstrDeviceType=L"mpegaudio"};
		TRY_DURATION_RESULT(::fast_io::win32_api_common(::fast_io::mnp::os_c_str(filename),[&](char16_t const* u16filename) noexcept
		{
			params.lpstrElementName=reinterpret_cast<wchar_t const*>(u16filename);
			return mcisendcommand<::fast_io::win32_family::wide_nt>(0,MCI_OPEN,MCI_OPEN_ELEMENT,(uintptr_t)std::addressof(params));
		}));
		deviceid=params.wDeviceID;
	}
	mci_close_wrapper<::fast_io::win32_family::native> clwrap(deviceid);
	MCI_STATUS_PARMS statusparam{.dwItem=MCI_STATUS_LENGTH};
	TRY_DURATION_RESULT(mcisendcommand<::fast_io::win32_family::native>(deviceid,MCI_STATUS,MCI_STATUS_ITEM,(uintptr_t)std::addressof(statusparam)));
	return {statusparam.dwReturn};
}
