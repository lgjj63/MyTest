//g++ test_file_mapping.cpp -lboost_system -lboost_date_time

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp> 
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <fstream>
#define STATUS_FILE_NAME "test_file"
int main()
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	ptime cleartime(second_clock::local_time());
	ptime reporttime(second_clock::local_time());

	//create a file map
	using namespace boost::interprocess;
	file_mapping::remove(STATUS_FILE_NAME);
	const std::size_t FileSize = 1024 * mapped_region::get_page_size() / 100;
	{  
		//Create a file
		std::filebuf fbuf;
		fbuf.open(STATUS_FILE_NAME, std::ios_base::in | std::ios_base::out
			| std::ios_base::trunc | std::ios_base::binary);
		//Set the size
		fbuf.pubseekoff(FileSize-1, std::ios_base::beg);
		fbuf.sputc(0);
	}
	//Remove file on exit
	struct file_remove
	{
		~file_remove (){  file_mapping::remove(STATUS_FILE_NAME); }
	} destroy_on_exit;

	//Create a file mapping
	file_mapping m_file(STATUS_FILE_NAME, read_write);
	mapped_region region(m_file, read_write);
	void* addr       = region.get_address();/*notic*/
	std::size_t size  = region.get_size();
	//Write all the memory to 1
	std::memset(addr, 0, size);
	int i = 0;
	while(1)
	{
		char *pStart = (char *)addr;
		int start = 0;
		std::string s = "";
		s = s + "Report Time       ==> " + boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time()) + "\n";
		s = s + "###################################### Overview ###############################################\n";
		s = s + "Now Session       ==> " + boost::lexical_cast<std::string>(i) + "\n";
		s = s + "####################################### Socket ################################################\n";
		memcpy(&pStart[0]+start, s.c_str(), s.length());
		start+=s.length();
		i++;
		if(i>1000000)
			break;
	}
	return 0;		
}


