/*
 * Binary Protocol Serialize and Parse Library, Version 1.0.7,
 * Copyright (C) 2012-2013, Ren Bin (ayrb13@gmail.com)
 * 
 * This library is free software. Permission to use, copy, modify,
 * and/or distribute this software for any purpose with or without fee
 * is hereby granted, provided that the above copyright notice and
 * this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * a.k.a. as Open BSD license
 * (http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/share/misc/license.template
 */

#ifndef __BINPROTO_HPP__
#define __BINPROTO_HPP__

#include <string>
#include <stdint.h>
#include <assert.h>
#include <list>

#if defined(_WIN32) || defined(WIN32)

#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
//warning C4244: 'argument' : conversion from 'const uint64_t' to 'u_long', possible loss of data
#pragma warning(disable : 4290)
//warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable : 4244)

#else
#include <arpa/inet.h>
#include <string.h>
#endif


//static assert;
#define BINPROTO_STATIC_ASSERT(expr,comment) \
	typedef char _binproto_static_assert_type##__LINE__[((expr)?1:-1)];

//is same type
template<typename A,typename B>
struct _binproto_is_same_type
{
	static const int value = 0;
};
template<typename A>
struct _binproto_is_same_type<A,A>
{
	static const int value = 1;
};
#define BINPROTO_IS_SAME_TYPE(A,B) _binproto_is_same_type<A,B>::value

//max value traits
template<typename num_type>
struct _binproto_num_type_max_traits
{
	BINPROTO_STATIC_ASSERT(
		((BINPROTO_IS_SAME_TYPE(num_type,uint8_t))||
		(BINPROTO_IS_SAME_TYPE(num_type,uint16_t))||
		(BINPROTO_IS_SAME_TYPE(num_type,uint32_t))||
		(BINPROTO_IS_SAME_TYPE(num_type,uint64_t))),
		"typename must be uint8_t or uint16_t or uint32_t or uint64_t");
};
template<> struct _binproto_num_type_max_traits<uint8_t>{
	static const uint8_t value = UINT8_MAX;
};
template<> struct _binproto_num_type_max_traits<uint16_t>{
	static const uint16_t value = UINT16_MAX;
};
template<> struct _binproto_num_type_max_traits<uint32_t>{
	static const uint32_t value = UINT32_MAX;
};
template<> struct _binproto_num_type_max_traits<uint64_t>{
	static const uint64_t value = UINT64_MAX;
};
#define BINPROTO_UINT_MAX_VALUE(uint_type) (_binproto_num_type_max_traits<uint_type>::value)

//assert;
#ifdef BINPROTO_DISABLE_ASSERT
#	define BINPROTO_ASSERT(expr,err_msg) assert(expr)
#else
#	define BINPROTO_ASSERT(expr,err_msg) ((void)0)
#endif

//throw exception;
#define BINPROTO_THROW(err_msg) throw(binproto::exception(err_msg))

//must use try catch mode when parse;
#define BINPROTO_PARSE_ENSURE(expr,err_msg) \
	if(!(expr)){throw(exception(err_msg));}
#define BINPROTO_PARSE_TRY \
	try{
#define BINPROTO_PARSE_CATCH(levelname) \
	}catch(const binproto::exception& ex){ex.throw_to_high_level(levelname);return 0;}

namespace binproto
{
	class exception
	{
	public:
		exception(const std::string& comment)
			:_comment(comment)
		{
		}
		exception(const char* comment)
			:_comment(comment)
		{
		}
		const std::string& what() const
		{
			return _comment;
		}
		void throw_to_high_level(const char* levelname) const
		{
			BINPROTO_THROW(_comment + " in " + levelname);
		}
	private:
		std::string _comment;
	};
	
	template<typename numtype>
	class num_obj
	{
	public:
		typedef numtype uint_type;
	public:
		num_obj()
			:_num(0)
		{
		}
		num_obj(numtype num)
			:_num(num)
		{
		}
		num_obj(const num_obj& num)
			:_num(num._num)
		{
		}
		num_obj& operator=(numtype num)
		{
			_num = num;
			return *this;
		}
		num_obj& operator=(const num_obj& num)
		{
			_num = num._num;
			return *this;
		}
		numtype to_int() const
		{
			return _num;
		}
		bool operator<(const num_obj& other) const
		{
			return _num < other._num;
		}
		bool operator==(const num_obj& other) const
		{
			return _num == other._num;
		}
		bool operator>(const num_obj& other) const
		{
			return _num > other._num;
		}
		bool operator!=(const num_obj& other) const
		{
			return _num != other._num;
		}
		inline uint32_t serialize_to_buffer(char* buffer,uint32_t bufflen) const ;
		inline uint32_t parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception);
		uint32_t get_binary_len() const
		{
			return sizeof(numtype);
		}
	private:
		numtype _num;
	};

	typedef num_obj<uint8_t> uint8_obj;
	typedef num_obj<uint16_t> uint16_obj;
	typedef num_obj<uint32_t> uint32_obj;
	typedef num_obj<uint64_t> uint64_obj;

	template<>
	uint32_t uint8_obj::parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
	{
		BINPROTO_PARSE_ENSURE(bufflen >= sizeof(_num),"uint8_obj parse error");
		_num = buffer[0];
		return get_binary_len();
	}
	template<>
	uint32_t uint8_obj::serialize_to_buffer(char* buffer,uint32_t bufflen) const 
	{
		BINPROTO_ASSERT(bufflen >= sizeof(_num),"uint8_obj serialize error");
		buffer[0] = _num;
		return get_binary_len();
	}
	template<>
	uint32_t uint16_obj::parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
	{
		BINPROTO_PARSE_ENSURE(bufflen >= sizeof(_num),"uint16_obj parse error");
		memcpy(&_num,buffer,sizeof(_num));
		_num = ntohs(_num);
		return get_binary_len();
	}
	template<>
	uint32_t uint16_obj::serialize_to_buffer(char* buffer,uint32_t bufflen) const 
	{
		BINPROTO_ASSERT(bufflen >= sizeof(_num),"uint16_obj serialize error");
		uint16_t net_uint = htons(_num);
		memcpy(buffer,&net_uint,sizeof(net_uint));
		return get_binary_len();
	}
	template<>
	uint32_t uint32_obj::parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
	{
		BINPROTO_PARSE_ENSURE(bufflen >= sizeof(_num),"uint32_obj parse error");
		memcpy(&_num,buffer,sizeof(_num));
		_num = ntohl(_num);
		return get_binary_len();
	}
	template<>
	uint32_t uint32_obj::serialize_to_buffer(char* buffer,uint32_t bufflen) const 
	{
		BINPROTO_ASSERT(bufflen >= sizeof(_num),"uint32_obj serialize error");
		uint32_t net_uint = htonl(_num);
		memcpy(buffer,&net_uint,sizeof(net_uint));
		return get_binary_len();
	}
	template<>
	uint32_t uint64_obj::parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
	{
		BINPROTO_PARSE_ENSURE(bufflen >= sizeof(_num),"uint64_obj parse error");
		uint32_t net_uint;
		memcpy(&net_uint,buffer,sizeof(net_uint));
		_num = ntohl(net_uint);
		_num = _num << 32;
		memcpy(&net_uint,buffer+4,sizeof(net_uint));
		_num += ntohl(net_uint);
		return get_binary_len();
	}
	template<>
	uint32_t uint64_obj::serialize_to_buffer(char* buffer,uint32_t bufflen) const 
	{
		BINPROTO_ASSERT(bufflen >= sizeof(_num),"uint64_obj serialize error");
		uint32_t net_uint = htonl(_num>>32);
		memcpy(buffer,&net_uint,sizeof(net_uint));
		net_uint = htonl(_num);
		memcpy(buffer+4,&net_uint,sizeof(net_uint));
		return get_binary_len();
	}

	template<int len_size>
	struct len_size_type_traits{BINPROTO_STATIC_ASSERT(len_size==1||len_size==2||len_size==4,"len_size must be 1,2,4");};

	template<> struct len_size_type_traits<1>{typedef uint8_obj len_type;};
	template<> struct len_size_type_traits<2>{typedef uint16_obj len_type;};
	template<> struct len_size_type_traits<4>{typedef uint32_obj len_type;};

	template<int len_size>
	class variable_len_string
	{
	public:
		typedef typename len_size_type_traits<len_size>::len_type len_type;
	public:
		variable_len_string(){}
		variable_len_string(const char* str)
		{
			BINPROTO_ASSERT(strlen(str) <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type),"str len must not larger than len_size max value");
			_str = str;
		}
		variable_len_string(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type),"str len must not larger than len_size max value");
			_str.assign(str,size);
		}
		variable_len_string(const std::string& str):_str(str){
			BINPROTO_ASSERT(str.size() <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type),"str len must not larger than len_size max value");
		}
		variable_len_string(const variable_len_string& str):_str(str._str){}
		variable_len_string& operator=(const std::string& str)
		{
			BINPROTO_ASSERT(str.size() <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type),"str len must not larger than len_size max value");
			_str = str;
			return *this;
		}
		variable_len_string& operator=(const char* str)
		{
			BINPROTO_ASSERT(strlen(str) <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type),"str len must not larger than len_size max value");
			_str = str;
			return *this;
		}
		variable_len_string& operator=(const variable_len_string& str)
		{
			_str = str._str;
			return *this;
		}
		variable_len_string& assign(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type),"str len must not larger than len_size max value");
			_str.assign(str,size);
			return *this;
		}
		const std::string& to_string() const
		{
			return _str;
		}
		const char* c_str() const
		{
			return _str.c_str();
		}
		bool operator<(const variable_len_string& other) const
		{
			return _str < other._str;
		}
		bool operator==(const variable_len_string& other) const
		{
			return _str == other._str;
		}
		bool operator>(const variable_len_string& other) const
		{
			return _str > other._str;
		}
		bool operator!=(const variable_len_string& other) const
		{
			return _str != other._str;
		}
		uint32_t serialize_to_buffer(char* buffer,uint32_t bufflen) const 
		{
			BINPROTO_ASSERT(get_binary_len() <= bufflen,"variable_len_string serialize error");
			uint32_t temp_len = 0;
			temp_len+=len_type(_str.length()).serialize_to_buffer(buffer,bufflen);
			memcpy(buffer+temp_len,_str.c_str(),_str.length());
			temp_len += _str.size();
			return temp_len;
		}
		uint32_t parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
		{
			uint32_t temp_len = 0;
			len_type temp;
			BINPROTO_PARSE_ENSURE(bufflen >= temp.get_binary_len(),"variable_len_string parse error");
			temp_len += temp.parse_from_buffer(buffer,bufflen);
			BINPROTO_PARSE_ENSURE(bufflen >= temp_len + temp.to_int(),"variable_len_string parse error");
			_str.assign(buffer + temp_len,temp.to_int());
			temp_len += temp.to_int();
			return temp_len;
		}
		uint32_t get_binary_len() const
		{
			return len_size	+ _str.length();
		}
	private:
		std::string _str;
	};

	template<int str_len>
	class fixed_len_string
	{
	public:
		fixed_len_string():_str("")
		{
			_str.resize(str_len);
		}
		fixed_len_string(const char* str)
		{
			BINPROTO_ASSERT(strlen(str) <= str_len,"str len must not larger than template str_len value");
			_str.assign(str,strlen(str));
			_str.resize(str_len);
		}
		fixed_len_string(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= str_len,"str len must not larger than template str_len value");
			_str.assign(str,size);
			_str.resize(str_len);
		}
		fixed_len_string(const std::string& str) : _str(str)
		{
			BINPROTO_ASSERT(str.size() <= str_len,"str len must not larger than template str_len value");
			_str.resize(str_len);
		}
		fixed_len_string(const fixed_len_string& str) : _str(str._str)
		{
			_str.resize(str_len);
		}
		fixed_len_string& operator=(const std::string& str)
		{
			BINPROTO_ASSERT(str.size() <= str_len,"str len must not larger than template str_len value");
			_str = str;
			_str.resize(str_len);
			return *this;
		}
		fixed_len_string& operator=(const char* str)
		{
			BINPROTO_ASSERT(strlen(str) <= str_len,"str len must not larger than template str_len value");
			_str.assign(str,strlen(str));
			_str.resize(str_len);
			return *this;
		}
		fixed_len_string& operator=(const fixed_len_string& packstring)
		{
			_str = packstring._str;
			return *this;
		}
		fixed_len_string& assign(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= str_len,"str len must not larger than template str_len value");
			_str.assign(str,size);
			_str.resize(str_len);
			return *this;
		}
		const std::string& to_string() const
		{
			return _str;
		}
		const char* c_str() const
		{
			return _str.c_str();
		}
		bool operator<(const variable_len_string& other) const
		{
			return _str < other._str;
		}
		bool operator==(const variable_len_string& other) const
		{
			return _str == other._str;
		}
		bool operator>(const variable_len_string& other) const
		{
			return _str > other._str;
		}
		bool operator!=(const variable_len_string& other) const
		{
			return _str != other._str;
		}
		uint32_t serialize_to_buffer(char* buffer,uint32_t bufflen) const 
		{
			BINPROTO_ASSERT(str_len <= bufflen,"fixed_len_string serialize error");
			if(_str.size() > str_len)
			{
				memcpy(buffer,_str.c_str(),str_len);
			}
			else
			{
				memcpy(buffer,_str.c_str(),_str.size());
				memset(buffer+_str.size(),0,str_len-_str.size());
			}
			return str_len;
		}
		uint32_t parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
		{
			BINPROTO_PARSE_ENSURE(str_len <= bufflen,"fixed_len_string parse error");
			_str.assign(buffer, str_len);
			_str.resize(str_len);
			return str_len;
		}
		uint32_t get_binary_len() const
		{
			return str_len;
		}
	private:
		std::string _str;
	};

	template<typename obj_type, int list_len_size>
	class binary_obj_list
	{
	public:
		typedef typename len_size_type_traits<list_len_size>::len_type list_size_type;
		typedef typename std::list<obj_type> container;
	public:
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::reverse_iterator reverse_iterator;
		typedef typename container::const_reverse_iterator const_reverse_iterator;
	public:
		binary_obj_list<obj_type,list_len_size>():_list()
		{
		}
		iterator begin()
		{
			return _list.begin();
		}
		iterator end()
		{
			return _list.end();
		}
		const_iterator begin() const
		{
			return _list.begin();
		}
		const_iterator end() const
		{
			return _list.end();
		}
		reverse_iterator rbegin()
		{
			return _list.rbegin();
		}
		reverse_iterator rend()
		{
			return _list.rend();
		}
		const_reverse_iterator rbegin() const
		{
			return _list.rbegin();
		}
		const_reverse_iterator rend() const
		{
			return _list.rend();
		}
		void push_back(const obj_type& _Val)
		{
			_list.push_back(_Val);
		}
		void push_front(const obj_type& _Val)
		{
			_list.push_front(_Val);
		}
		void pop_back()
		{
			_list.pop_back();
		}
		void pop_front()
		{
			_list.pop_front();
		}
		iterator erase(iterator _Where)
		{
			return _list.erase(_Where);
		}
		iterator erase(iterator _First_arg, iterator _Last_arg)
		{
			return _list.erase(_First_arg,_Last_arg);
		}
		obj_type& front()
		{
			return _list.front();
		}
		obj_type& back()
		{
			return _list.back();
		}
		const obj_type&  front() const
		{
			return _list.front();
		}
		const obj_type&  back() const
		{
			return _list.back();
		}
		iterator insert(iterator _Where, const obj_type& _Val)
		{
			return _list.insert(_Where,_Val);
		}
		uint32_t size() const{return _list.size();}
		void clear(){_list.clear();}
		template<typename key_value>
		iterator find(key_value obj_type::*member, const key_value& key) const
		{
			for(iterator it = begin(); it != end(); ++it)
			{
				if(((&*it)->*member) == key)
					return it;
			}
			return end();
		}
	public:
		uint32_t serialize_to_buffer(char* buffer,uint32_t bufflen) const 
		{
			uint32_t temp_len = 0;
			BINPROTO_ASSERT(list_len_size <= bufflen,"binary_obj_list length serialize error");
			BINPROTO_ASSERT(_list.size() <= BINPROTO_UINT_MAX_VALUE(typename list_size_type::uint_type),"binary_obj_list serialize error,len size overflow");
			temp_len += list_size_type(size()).serialize_to_buffer(buffer, bufflen);
			for(const_iterator it = begin(); it != end(); ++it)
			{
				temp_len += it->serialize_to_buffer(buffer+temp_len, bufflen-temp_len);
			}
			return temp_len;
		}
		uint32_t parse_from_buffer(const char* buffer,uint32_t bufflen) throw(exception)
		{
			clear();
			uint32_t temp_len = 0;
			list_size_type temp;
			BINPROTO_PARSE_ENSURE(list_len_size <= bufflen,"binary_obj_list length parse error");
			temp_len += temp.parse_from_buffer(buffer, bufflen);

			BINPROTO_PARSE_TRY;
			for(uint32_t i = 0; i < temp.to_int(); i++)
			{
				push_back(obj_type());
				temp_len += _list.back().parse_from_buffer(buffer + temp_len, bufflen - temp_len);
			}
			return temp_len;
			BINPROTO_PARSE_CATCH("binary_obj_list");
		}
		uint32_t get_binary_len() const
		{
			size_t iLength = 0;
			iLength += list_size_type().get_binary_len();
			for(const_iterator it = begin(); it != end(); ++it)
			{
				iLength += it->get_binary_len();
			}
			return iLength;
		}
	private:
		container _list;
	};
}
//macro defination

#define BINPROTO_FUNCTION_PARSE_START uint32_t parse_from_buffer(const char* buffer,uint32_t bufflen) throw(binproto::exception) {uint32_t temp_len = 0;
#define BINPROTO_FUNCTION_PARSE(object) temp_len += (object).parse_from_buffer(buffer + temp_len, bufflen - temp_len);
#define BINPROTO_FUNCTION_PARSE_END return temp_len;}

#define BINPROTO_FUNCTION_SERIALIZE_START uint32_t serialize_to_buffer(char* buffer,uint32_t bufflen) const {uint32_t temp_len = 0;
#define BINPROTO_FUNCTION_SERIALIZE(object) temp_len += (object).serialize_to_buffer(buffer + temp_len, bufflen - temp_len);
#define BINPROTO_FUNCTION_SERIALIZE_END return temp_len;}

#define BINPROTO_FUNCTION_GETLEN_START uint32_t get_binary_len() const{uint32_t temp_len = 0;
#define BINPROTO_FUNCTION_GETLEN(object) temp_len += (object).get_binary_len();
#define BINPROTO_FUNCTION_GETLEN_END return temp_len;}

#define BINPROTO_PACKET_DEFINE_START(classname) struct classname{
#define BINPROTO_PACKET_DEFINE_MEMBER(type,name) type name;
#define BINPROTO_PACKET_DEFINE_END };

#define BINPROTO_DEFINE_PACKET_P00(classname) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P01(classname,type01,name01) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P02(classname,type01,name01,type02,name02) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P03(classname,type01,name01,type02,name02,type03,name03) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P04(classname,type01,name01,type02,name02,type03,name03,type04,name04) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P05(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P06(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P07(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P08(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P09(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P10(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P11(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P12(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P13(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P14(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P15(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14,type15,name15) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_PACKET_DEFINE_MEMBER(type15,name15) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_FUNCTION_PARSE(name15) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE(name15) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN(name15) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P16(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14,type15,name15,type16,name16) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_PACKET_DEFINE_MEMBER(type15,name15) \
BINPROTO_PACKET_DEFINE_MEMBER(type16,name16) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_FUNCTION_PARSE(name15) \
BINPROTO_FUNCTION_PARSE(name16) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE(name15) \
BINPROTO_FUNCTION_SERIALIZE(name16) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN(name15) \
BINPROTO_FUNCTION_GETLEN(name16) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P17(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14,type15,name15,type16,name16,type17,name17) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_PACKET_DEFINE_MEMBER(type15,name15) \
BINPROTO_PACKET_DEFINE_MEMBER(type16,name16) \
BINPROTO_PACKET_DEFINE_MEMBER(type17,name17) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_FUNCTION_PARSE(name15) \
BINPROTO_FUNCTION_PARSE(name16) \
BINPROTO_FUNCTION_PARSE(name17) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE(name15) \
BINPROTO_FUNCTION_SERIALIZE(name16) \
BINPROTO_FUNCTION_SERIALIZE(name17) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN(name15) \
BINPROTO_FUNCTION_GETLEN(name16) \
BINPROTO_FUNCTION_GETLEN(name17) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P18(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14,type15,name15,type16,name16,type17,name17,type18,name18) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_PACKET_DEFINE_MEMBER(type15,name15) \
BINPROTO_PACKET_DEFINE_MEMBER(type16,name16) \
BINPROTO_PACKET_DEFINE_MEMBER(type17,name17) \
BINPROTO_PACKET_DEFINE_MEMBER(type18,name18) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_FUNCTION_PARSE(name15) \
BINPROTO_FUNCTION_PARSE(name16) \
BINPROTO_FUNCTION_PARSE(name17) \
BINPROTO_FUNCTION_PARSE(name18) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE(name15) \
BINPROTO_FUNCTION_SERIALIZE(name16) \
BINPROTO_FUNCTION_SERIALIZE(name17) \
BINPROTO_FUNCTION_SERIALIZE(name18) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN(name15) \
BINPROTO_FUNCTION_GETLEN(name16) \
BINPROTO_FUNCTION_GETLEN(name17) \
BINPROTO_FUNCTION_GETLEN(name18) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P19(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14,type15,name15,type16,name16,type17,name17,type18,name18,type19,name19) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_PACKET_DEFINE_MEMBER(type15,name15) \
BINPROTO_PACKET_DEFINE_MEMBER(type16,name16) \
BINPROTO_PACKET_DEFINE_MEMBER(type17,name17) \
BINPROTO_PACKET_DEFINE_MEMBER(type18,name18) \
BINPROTO_PACKET_DEFINE_MEMBER(type19,name19) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_FUNCTION_PARSE(name15) \
BINPROTO_FUNCTION_PARSE(name16) \
BINPROTO_FUNCTION_PARSE(name17) \
BINPROTO_FUNCTION_PARSE(name18) \
BINPROTO_FUNCTION_PARSE(name19) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE(name15) \
BINPROTO_FUNCTION_SERIALIZE(name16) \
BINPROTO_FUNCTION_SERIALIZE(name17) \
BINPROTO_FUNCTION_SERIALIZE(name18) \
BINPROTO_FUNCTION_SERIALIZE(name19) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN(name15) \
BINPROTO_FUNCTION_GETLEN(name16) \
BINPROTO_FUNCTION_GETLEN(name17) \
BINPROTO_FUNCTION_GETLEN(name18) \
BINPROTO_FUNCTION_GETLEN(name19) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END 

#define BINPROTO_DEFINE_PACKET_P20(classname,type01,name01,type02,name02,type03,name03,type04,name04,type05,name05,type06,name06,type07,name07,type08,name08,type09,name09,type10,name10,type11,name11,type12,name12,type13,name13,type14,name14,type15,name15,type16,name16,type17,name17,type18,name18,type19,name19,type20,name20) \
BINPROTO_PACKET_DEFINE_START(classname) \
BINPROTO_PACKET_DEFINE_MEMBER(type01,name01) \
BINPROTO_PACKET_DEFINE_MEMBER(type02,name02) \
BINPROTO_PACKET_DEFINE_MEMBER(type03,name03) \
BINPROTO_PACKET_DEFINE_MEMBER(type04,name04) \
BINPROTO_PACKET_DEFINE_MEMBER(type05,name05) \
BINPROTO_PACKET_DEFINE_MEMBER(type06,name06) \
BINPROTO_PACKET_DEFINE_MEMBER(type07,name07) \
BINPROTO_PACKET_DEFINE_MEMBER(type08,name08) \
BINPROTO_PACKET_DEFINE_MEMBER(type09,name09) \
BINPROTO_PACKET_DEFINE_MEMBER(type10,name10) \
BINPROTO_PACKET_DEFINE_MEMBER(type11,name11) \
BINPROTO_PACKET_DEFINE_MEMBER(type12,name12) \
BINPROTO_PACKET_DEFINE_MEMBER(type13,name13) \
BINPROTO_PACKET_DEFINE_MEMBER(type14,name14) \
BINPROTO_PACKET_DEFINE_MEMBER(type15,name15) \
BINPROTO_PACKET_DEFINE_MEMBER(type16,name16) \
BINPROTO_PACKET_DEFINE_MEMBER(type17,name17) \
BINPROTO_PACKET_DEFINE_MEMBER(type18,name18) \
BINPROTO_PACKET_DEFINE_MEMBER(type19,name19) \
BINPROTO_PACKET_DEFINE_MEMBER(type20,name20) \
BINPROTO_FUNCTION_PARSE_START \
BINPROTO_PARSE_TRY \
BINPROTO_FUNCTION_PARSE(name01) \
BINPROTO_FUNCTION_PARSE(name02) \
BINPROTO_FUNCTION_PARSE(name03) \
BINPROTO_FUNCTION_PARSE(name04) \
BINPROTO_FUNCTION_PARSE(name05) \
BINPROTO_FUNCTION_PARSE(name06) \
BINPROTO_FUNCTION_PARSE(name07) \
BINPROTO_FUNCTION_PARSE(name08) \
BINPROTO_FUNCTION_PARSE(name09) \
BINPROTO_FUNCTION_PARSE(name10) \
BINPROTO_FUNCTION_PARSE(name11) \
BINPROTO_FUNCTION_PARSE(name12) \
BINPROTO_FUNCTION_PARSE(name13) \
BINPROTO_FUNCTION_PARSE(name14) \
BINPROTO_FUNCTION_PARSE(name15) \
BINPROTO_FUNCTION_PARSE(name16) \
BINPROTO_FUNCTION_PARSE(name17) \
BINPROTO_FUNCTION_PARSE(name18) \
BINPROTO_FUNCTION_PARSE(name19) \
BINPROTO_FUNCTION_PARSE(name20) \
BINPROTO_PARSE_CATCH(#classname) \
BINPROTO_FUNCTION_PARSE_END \
BINPROTO_FUNCTION_SERIALIZE_START \
BINPROTO_FUNCTION_SERIALIZE(name01) \
BINPROTO_FUNCTION_SERIALIZE(name02) \
BINPROTO_FUNCTION_SERIALIZE(name03) \
BINPROTO_FUNCTION_SERIALIZE(name04) \
BINPROTO_FUNCTION_SERIALIZE(name05) \
BINPROTO_FUNCTION_SERIALIZE(name06) \
BINPROTO_FUNCTION_SERIALIZE(name07) \
BINPROTO_FUNCTION_SERIALIZE(name08) \
BINPROTO_FUNCTION_SERIALIZE(name09) \
BINPROTO_FUNCTION_SERIALIZE(name10) \
BINPROTO_FUNCTION_SERIALIZE(name11) \
BINPROTO_FUNCTION_SERIALIZE(name12) \
BINPROTO_FUNCTION_SERIALIZE(name13) \
BINPROTO_FUNCTION_SERIALIZE(name14) \
BINPROTO_FUNCTION_SERIALIZE(name15) \
BINPROTO_FUNCTION_SERIALIZE(name16) \
BINPROTO_FUNCTION_SERIALIZE(name17) \
BINPROTO_FUNCTION_SERIALIZE(name18) \
BINPROTO_FUNCTION_SERIALIZE(name19) \
BINPROTO_FUNCTION_SERIALIZE(name20) \
BINPROTO_FUNCTION_SERIALIZE_END \
BINPROTO_FUNCTION_GETLEN_START \
BINPROTO_FUNCTION_GETLEN(name01) \
BINPROTO_FUNCTION_GETLEN(name02) \
BINPROTO_FUNCTION_GETLEN(name03) \
BINPROTO_FUNCTION_GETLEN(name04) \
BINPROTO_FUNCTION_GETLEN(name05) \
BINPROTO_FUNCTION_GETLEN(name06) \
BINPROTO_FUNCTION_GETLEN(name07) \
BINPROTO_FUNCTION_GETLEN(name08) \
BINPROTO_FUNCTION_GETLEN(name09) \
BINPROTO_FUNCTION_GETLEN(name10) \
BINPROTO_FUNCTION_GETLEN(name11) \
BINPROTO_FUNCTION_GETLEN(name12) \
BINPROTO_FUNCTION_GETLEN(name13) \
BINPROTO_FUNCTION_GETLEN(name14) \
BINPROTO_FUNCTION_GETLEN(name15) \
BINPROTO_FUNCTION_GETLEN(name16) \
BINPROTO_FUNCTION_GETLEN(name17) \
BINPROTO_FUNCTION_GETLEN(name18) \
BINPROTO_FUNCTION_GETLEN(name19) \
BINPROTO_FUNCTION_GETLEN(name20) \
BINPROTO_FUNCTION_GETLEN_END \
BINPROTO_PACKET_DEFINE_END

#endif//__BINPROTO_HPP__
