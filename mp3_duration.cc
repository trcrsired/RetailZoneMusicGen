#include "mp3_duration.h"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

#if 0

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
mp3_header
{
	char8_t syncword[3];//must be ID3
	::std::uint_least8_t version;
	::std::uint_least8_t layer;
	struct : ::std::uint_least8_t
	{
		version:2;
		layer:4;
		error_protection:2;
	}vle;
	::std::uint_least8_t bitrate;
	struct : ::std::uint_least8_t
	{
		frequency:4;
		padding:2;
		privbit:2;
	}fpp;
};
#endif


struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
#if __has_cpp_attribute(__gnu__::__packed__)
[[__gnu__::__packed__]]
#endif
id3_header
{
char header[3]; /*必须为“ID3”否则认为标签不存在*/
::std::uint_least8_t ver; /*版本号ID3V2.3 就记录3*/
::std::uint_least8_t revision; /*副版本号此版本记录为0*/
::std::uint_least8_t flag; /*标志字节，只使用高三位，其它位为0 */
::std::uint_least32_t size; /*标签大小*/
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
#if __has_cpp_attribute(__gnu__::__packed__)
[[__gnu__::__packed__]]
#endif
frame_header
{
::std::uint_least32_t sync:11; //同步信息
::std::uint_least32_t version:2; //版本
::std::uint_least32_t layer: 2; //层
::std::uint_least32_t errorprotection:1; // CRC校验
::std::uint_least32_t bitrate_index:4; //位率
::std::uint_least32_t sampling_frequency:2; //采样频率
::std::uint_least32_t padding:1; //帧长调节
::std::uint_least32_t privatebit:1; //保留字
::std::uint_least32_t mode:2; //声道模式
::std::uint_least32_t modeextension:2; //扩充模式
::std::uint_least32_t copyright:1; // 版权
::std::uint_least32_t original:1; //原版标志
::std::uint_least32_t emphasis:2; //强调模式
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
#if __has_cpp_attribute(__gnu__::__packed__)
[[__gnu__::__packed__]]
#endif
tagged_frame
{
char id[4]; /*标识帧，说明其内容，例如作者/标题等*/
::std::uint_least32_t size; /*帧内容的大小，不包括帧头，不得小于1*/
::std::uint_least8_t flags[2]; /*标志帧，只定义了6 位*/
};

inline constexpr ::std::uint_least32_t mp3_safe_size(::std::uint_least32_t sz) noexcept
{
	if constexpr(::std::endian::native==::std::endian::big)
	{
		return ((sz)&UINT32_C(0x0F))+(((sz>>8)&UINT32_C(0x0F))<<7)
			+(((sz>>16)&UINT32_C(0x0F))<<14)+
			(((sz>>24)&UINT32_C(0x0F))<<21);
	}
	else
	{
		return ((sz>>24)&UINT32_C(0x0F))+(((sz>>16)&UINT32_C(0x0F))<<7)
			+(((sz>>8)&UINT32_C(0x0F))<<14)+
			(((sz)&UINT32_C(0x0F))<<21);
	}
}

duration_result get_mp3_duration(void const* firstptr, void const* lastptr) noexcept
{
	char const* first{reinterpret_cast<char const*>(firstptr)};
	char const* last{reinterpret_cast<char const*>(lastptr)};
	char const* start{first};
	#define PROTECTED_ADVANCE(diff) if(static_cast<std::size_t>(last-start)<static_cast<std::make_unsigned_t<decltype(diff)>>(diff)) {return {0,true};}

	PROTECTED_ADVANCE(10);
	id3_header const& hd{*reinterpret_cast<id3_header const*>(start)};
	if(memcmp(hd.header,u8"ID3",3)!=0)
	{
		return {0,true};
	}
	::std::uint_least32_t sz{mp3_safe_size(hd.size)};
	println(fast_io::out(),"Version:",hd.ver,"\tmp3_safe_size:",sz);
	start+=10;
//	println(fast_io::out(),"Version:",hd.ver,"\tmp3_safe_size:",sz);
//	start+=sz;
//	println(fast_io::out(),"Version:",hd.ver,"\tmp3_safe_size:",sz," ",last-first," ",last-start);
	for(;start<last;)
	{
		PROTECTED_ADVANCE(10)
		tagged_frame const& tgf{*reinterpret_cast<tagged_frame const*>(start)};
//		memcpy(std::addressof(tgf),start,4);
//		start+pos;

//		println(fast_io::mnp::addrvw(h));
		::std::uint_least32_t framesize{tgf.size};
		if constexpr(std::endian::native!=::std::endian::big)
		{
			framesize=::std::byteswap(framesize);
		}
		start+=10;
		PROTECTED_ADVANCE(framesize);
		if(memcmp(tgf.id,u8"TLEN",4)==0)
		{
			println(fast_io::out(),"Find Tlen: ",framesize);
		}
		else if(memcmp(tgf.id,u8"\xFF\xFB",2)==0)
		{
			frame_header h;
			memcpy(std::addressof(h),start,sizeof(4));

			auto bitrate_index{h.bitrate_index};
			auto sampling_frequency{h.sampling_frequency};
			println(fast_io::out(),"bitrate_index:",bitrate_index,"\tsampling_frequency:",sampling_frequency);
			
			break;
		}
		start+=framesize;

//		return {0,true};
	}
	return {0,true};
}
