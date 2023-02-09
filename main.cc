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
	fast_io::native_file_loader loader(fast_io::mnp::os_c_str(argv[1]));
	auto v{get_mp3_duration(loader.data(),loader.data()+loader.size())};
	println(fast_io::out(),fast_io::mnp::os_c_str(argv[1])," duration:",v.duration
	," invalid:",v.invalid);
}
