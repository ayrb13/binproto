#binproto
##A library for parse and serialize binary transfer protocol based on bytes order written in C++

Binproto is very easy to define packet structs in C/C++ Macro based on existing binary protocol document.
You can use it by including a single file conveniently.

##Feature
* It supports on WIN/UNIX, written in C/C++
* Only depend on ANSI C++, portable
* Easy to use it by including binproto.hpp

##Example

###Define packet type Teacher has 3 members:

* the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
* the type of the 2nd member age is a unsigned int,which is 8bit,0-255
* the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.
* the type of the 4th member subject is a string that its length is variable, first 1 byte hold the length, and the content follows it

Teacher
<table>
<tr><td><em>Index</em></td><td><em>Name</em></td><td><em>Size (byte)</em></td><td><em>Description</em></td></tr>
<tr><td>1</td><td>size of teacher's name</td><td>1</td><td>number of size</td></tr>
<tr><td>2</td><td>teacher's name</td><td>n</td><td>string of name</td></tr>
<tr><td>3</td><td>age</td><td>1</td><td>number of teacher's age</td></tr>
<tr><td>4</td><td>sex</td><td>10</td><td>fixed length string,left-justified,fill empty \0</td></tr>
<tr><td>5</td><td>size of teacher's subject</td><td>1</td><td>number of size</td></tr>
<tr><td>6</td><td>teacher's subject</td><td>n</td><td>string of subject</td></tr>
</table>

	BINPROTO_DEFINE_PACKET_P04(Teacher
		,binproto::variable_len_string<1>,name
		,binproto::uint8_obj,age
		,binproto::fixed_len_string<10>,sex
		,binproto::variable_len_string<1>,subject);
 
###Define packet type Student has 3 members

* the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
* the type of the 2nd member age is a unsigned int,which is 8bit,0-255
* the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.

Student
<table>
<tr><td><em>Index</em></td><td><em>Name</em></td><td><em>Size (byte)</em></td><td><em>Description</em></td></tr>
<tr><td>1</td><td>size of student's name</td><td>1</td><td>number of size</td></tr>
<tr><td>2</td><td>student's name</td><td>n</td><td>string of name</td></tr>
<tr><td>3</td><td>age</td><td>1</td><td>number of student's age</td></tr>
<tr><td>4</td><td>sex</td><td>10</td><td>fixed length string,left-justified,fill empty \0</td></tr>
</table>

	BINPROTO_DEFINE_PACKET_P03(Student
		,binproto::variable_len_string<1>,name
		,binproto::uint8_obj,age
		,binproto::fixed_len_string<10>,sex);

###Define packet type SchoolClass has 3 members

* the type of the 1st member class number is a unsigned int,which is 8bit,0-255
* the type of the 2nd member teacher.
* the type of the 3rd member student list, and the count of students is 256 at most, so the length size of this list is 1 byte.

SchoolClass
<table>
<tr><td><em>Index</em></td><td><em>Name</em></td><td><em>Size (byte)</em></td><td><em>Description</em></td></tr>
<tr><td>1</td><td>classnum</td><td>1</td><td>number of classnum</td></tr>
<tr><td>2</td><td>size of teacher's name</td><td>1</td><td>number of size</td></tr>
<tr><td>3</td><td>teacher's name</td><td>n</td><td>string of name</td></tr>
<tr><td>4</td><td>age</td><td>1</td><td>number of teacher's age</td></tr>
<tr><td>5</td><td>sex</td><td>10</td><td>fixed length string,left-justified,fill empty \0</td></tr>
<tr><td>6</td><td>size of teacher's subject</td><td>1</td><td>number of size</td></tr>
<tr><td>7</td><td>teacher's subject</td><td>n</td><td>string of subject</td></tr>
<tr><td>8</td><td>student count</td><td>1</td><td>number of student count</td></tr>
<tr><td>9</td><td>size of student's name</td><td>1</td><td>number of size</td></tr>
<tr><td>10</td><td>student's name</td><td>n</td><td>string of name</td></tr>
<tr><td>11</td><td>age</td><td>1</td><td>number of student's age</td></tr>
<tr><td>12</td><td>sex</td><td>10</td><td>fixed length string,left-justified,fill empty \0</td></tr>
<tr><td>13</td><td>next student</td><td>...</td><td>repeat 9 - 12</td></tr>
</table>

	BINPROTO_DEFINE_PACKET_P03(SchoolClass
	
		,binproto::uint8_obj,classnum
	
		,Teacher,teacher
	
		,binproto::binary_obj_list<Student BINPROTO_COMMA 1>,stulist);

