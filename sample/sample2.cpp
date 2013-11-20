//nested packet define sample

//disable the assert
#define BINPROTO_DISABLE_ASSERT
#include "binproto.hpp"

//define packet type teacher has 3 members
//the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
//the type of the 2nd member age is a unsigned int,which is 8bit,0-255
//the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.
//the type of the 4th member subject is a string that its length is variable, first 1 byte hold the length, and the content follows it
// teacher
//  |-----------------|-----------|------------------------|
//  |   name length   |  1 byte   |count of bytes name need|
//  |-----------------|-----------|------------------------|
//  |   name string   |  n bytes  |     n = name length    |
//  |-----------------|-----------|------------------------|
//  |       age       |  1 byte   |      student age       |
//  |-----------------|-----------|------------------------|
//  |    sex string   | 10 bytes  |  fixed length string   |
//  |-----------------|-----------|------------------------|
//  | subject length  |  1 byte   |  count of subject need |
//  |-----------------|-----------|------------------------|
//  |     subject     |  n bytes  |   n = subject length   |
//  |-----------------|-----------|------------------------|


//define packet type student has 3 members
//the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
//the type of the 2nd member age is a unsigned int,which is 8bit,0-255
//the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.
// student
//  |-----------------|-----------|------------------------|
//  |   name length   |  1 byte   |count of bytes name need|
//  |-----------------|-----------|------------------------|
//  |   name string   |  n bytes  |     n = name length    |
//  |-----------------|-----------|------------------------|
//  |       age       |  1 byte   |      student age       |
//  |-----------------|-----------|------------------------|
//  |    sex string   | 10 bytes  |  fixed length string   |
//  |-----------------|-----------|------------------------|



//define packet type class has 3 members
//the type of the 1st member class number is a unsigned int,which is 8bit,0-255
//the type of the 2nd member teacher.
//the type of the 3rd member student list, and the count of students is 256 at most, so the length size of this list is 1 byte.
// class
//  |-----------------|-----------|------------------------|
//  |   class number  |  1 byte   |    like 1 or 2 or 3    |
//  |-----------------|-----------|------------------------|
//  |     teacher     |  .......  |     teacher struct     |
//  |-----------------|-----------|------------------------|
//  |   student list  |  .......  |      student list      |
//  |-----------------|-----------|------------------------|


BINPROTO_DEFINE_PACKET_P04(Teacher
	,binproto::variable_len_string<1>,name
	,binproto::uint8_obj,age
	,binproto::fixed_len_string<10>,sex
	,binproto::variable_len_string<1>,subject);

BINPROTO_DEFINE_PACKET_P03(Student
	,binproto::variable_len_string<1>,name
	,binproto::uint8_obj,age
	,binproto::fixed_len_string<10>,sex);

BINPROTO_DEFINE_PACKET_P03(SchoolClass
	,binproto::uint8_obj,classnum
	,Teacher,teacher
	,(binproto::binary_obj_list<Student,1>),stulist);

#define BUFF_SIZE 1024*1024

int main2()
{
	try
	{
		char *buffer = new char[BUFF_SIZE];
		{
			SchoolClass schoolclass;
			schoolclass.classnum = 1;
			schoolclass.teacher.name = "Ren Bin";
			schoolclass.teacher.age = 27;
			schoolclass.teacher.sex = "male";
			schoolclass.teacher.subject = "maths";

			Student sd;
			sd.name = "Li Ming";
			sd.age = 13;
			sd.sex = "male";
			schoolclass.stulist.push_back(sd);

			sd.name = "Han Meimei";
			sd.age = 13;
			sd.sex = "female";
			schoolclass.stulist.push_back(sd);

			sd.name = "Jim";
			sd.age = 14;
			sd.sex = "male";
			schoolclass.stulist.push_back(sd);

			int binlen = schoolclass.get_binary_len();
			schoolclass.serialize_to_buffer(buffer,BUFF_SIZE);
		}
		{
			SchoolClass schoolclass;
			schoolclass.parse_from_buffer(buffer,BUFF_SIZE);
			int binlen = schoolclass.get_binary_len();
			printf("Class number is %d\n",schoolclass.classnum.to_int());
			printf("Class teacher named %s teaches %s, and %s age is %d\n"
				,schoolclass.teacher.name.c_str()
				,schoolclass.teacher.subject.c_str()
				,strcmp(schoolclass.teacher.sex.c_str(), "male") == 0 ? "his" : "her"
				,schoolclass.teacher.age.to_int());
			printf("Here comes the students\n");
			for(binproto::binary_obj_list<Student,1>::iterator it = schoolclass.stulist.begin();
				it != schoolclass.stulist.end(); ++it)
			{
				printf("a student named %s, and %s age is %d\n"
					,it->name.c_str()
					,strcmp(it->sex.c_str(), "male") == 0 ? "his" : "her"
					,it->age.to_int());
			}
		}
	}
	catch(const binproto::exception& ex)
	{
		printf("%s.\n",ex.what().c_str());
	}
	return 0;
}
