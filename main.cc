int main(int argc,char** argv)
{
	if(argc < 2)
	{
		if(argc == 0)
		{
			return 1;
		}
		perr(fast_io::mnp::os_c_str(*argv)," <path>\n");
		return 1;
	}
	auto v{get_mp3_duration(argv[1])};

	println(fast_io::mnp::os_c_str(argv[1])," duration:",v.duration);
}
