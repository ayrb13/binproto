#binproto
##一个顺序的C++序列化反序列化库

Binproto可以方便的使用C++宏来定义所需要进行序列化反序列化的结构体，你只需要include它就够了

##特性
* C++编写，支持windows和unix系统
* 使用标准C++编写，便于移植
* 使用方便，只需要#include “binproto.hpp”

##例子

###定义Teacher这个结构体，有三个成员

* 第一个成员name是一个变长字符串，可以使用第一个字节保存长度，后续字节为name的内容；
* 第二个成员是age，取值范围可为0-255，使用一个字节表示；
* 第三个成员是sex，使用10字节的定长字符串来表示；
* 第四个成员是subject是一个变长字符串，可以使用第一个字节保存长度，后续字节为subject的内容；

Teacher
<table>
<tr><td><em>序号</em></td><td><em>字段名</em></td><td><em>字节数</em></td><td><em>描述</em></td></tr>
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
 
###定义Student这个结构体，有三个成员

* 第一个成员name是一个变长字符串，可以使用第一个字节保存长度，后续字节为name的内容；
* 第二个成员是age，取值范围可为0-255，使用一个字节表示；
* 第三个成员是sex，使用10字节的定长字符串来表示；

Student
<table>
<tr><td><em>序号</em></td><td><em>字段名</em></td><td><em>字节数</em></td><td><em>描述</em></td></tr>
<tr><td>1</td><td>size of student's name</td><td>1</td><td>number of size</td></tr>
<tr><td>2</td><td>student's name</td><td>n</td><td>string of name</td></tr>
<tr><td>3</td><td>age</td><td>1</td><td>number of student's age</td></tr>
<tr><td>4</td><td>sex</td><td>10</td><td>fixed length string,left-justified,fill empty \0</td></tr>
</table>

	BINPROTO_DEFINE_PACKET_P03(Student
		,binproto::variable_len_string<1>,name
		,binproto::uint8_obj,age
		,binproto::fixed_len_string<10>,sex);

###定义SchoolClass这个结构体，有三个成员

* 第一个成员class number是一个正整数，取值范围可为0-255，用一个字节表示；
* 第二个成员teacher用上面定义的teacher结构体来表示；
* 第三个成员student list, 学生的数量最多255个,所以用第一个字节表示长度，后续的字节循环表示student结构体

SchoolClass
<table>
<tr><td><em>序号</em></td><td><em>字段名</em></td><td><em>字节数</em></td><td><em>描述</em></td></tr>
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
