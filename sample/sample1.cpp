//simple packet define sample

//disable the assert
#define BINPROTO_DISABLE_ASSERT

//serialize not throw exception
#define BINPROTO_SERIALIZE_NO_THROW
#include "binproto.hpp"

//define packet type student has 3 members
//the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
//the type of the 2nd member age is a unsigned int,which is 8bit,0-255
//the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.
BINPROTO_DEFINE_PACKET_P03(Student
	,binproto::variable_len_string<1>,name
	,binproto::uint8_obj,age
	,binproto::fixed_len_string<10>,sex);

#define BUFF_SIZE 1024

int main()
{
	try
	{
		char *buffer = new char[BUFF_SIZE];
		{
			Student sd;
			sd.name = "Ren Bin";//Yeah, That's my name, HAHA!!
			sd.age = 27;
			sd.sex = "male";//I want a chinese girl,:)
			sd.serialize_to_buffer(buffer,BUFF_SIZE);
		}
		{
			Student sd;
			sd.parse_from_buffer(buffer,BUFF_SIZE);
			printf("this %s named %s is %d years old.\n",sd.sex.to_string().c_str(),sd.name.to_string().c_str(),sd.age.to_int());
		}
	}
	catch(const binproto::exception& ex)
	{
		printf("%s.\n",ex.what().c_str());
	}
	return 0;
}