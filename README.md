#binproto
##A library for parse and serialize binary transfer protocol based on bytes order written in C++

Binproto is very easy to define packet structs in C/C++ Macro based on existing binary protocol document.
You can use it by including a single file conveniently.

##Feature
* It supports on WIN/UNIX, written in C/C++
* Only depend on ANSI C++, portable
* Easy to use it by including binproto.hpp

##Example

Define packet type Teacher has 3 members:

* the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
* the type of the 2nd member age is a unsigned int,which is 8bit,0-255
* the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.
* the type of the 4th member subject is a string that its length is variable, first 1 byte hold the length, and the content follows it
 Teacher

  |-----------------|-----------|------------------------|

  |   name length   |  1 byte   |count of bytes name need|

  |-----------------|-----------|------------------------|

  |   name string   |  n bytes  |     n = name length    |

  |-----------------|-----------|------------------------|

  |       age       |  1 byte   |      student age       |

  |-----------------|-----------|------------------------|

  |    sex string   | 10 bytes  |  fixed length string   |

  |-----------------|-----------|------------------------|

  | subject length  |  1 byte   |  count of subject need |

  |-----------------|-----------|------------------------|

  |     subject     |  n bytes  |   n = subject length   |

  |-----------------|-----------|------------------------|


BINPROTO_DEFINE_PACKET_P04(Teacher

	,binproto::variable_len_string<1>,name

	,binproto::uint8_obj,age

	,binproto::fixed_len_string<10>,sex

	,binproto::variable_len_string<1>,subject);





Define packet type Student has 3 members

* the type of the 1st member name is a string that its length is variable, first 1 byte hold the length, and the content follows it
* the type of the 2nd member age is a unsigned int,which is 8bit,0-255
* the type of the 3rd member sex is a string that its length is fixed,the length is 10 bytes.

 Student

  |-----------------|-----------|------------------------|

  |   name length   |  1 byte   |count of bytes name need|

  |-----------------|-----------|------------------------|

  |   name string   |  n bytes  |     n = name length    |

  |-----------------|-----------|------------------------|

  |       age       |  1 byte   |      student age       |

  |-----------------|-----------|------------------------|


  |    sex string   | 10 bytes  |  fixed length string   |

  |-----------------|-----------|------------------------|


BINPROTO_DEFINE_PACKET_P03(Student

	,binproto::variable_len_string<1>,name

	,binproto::uint8_obj,age

	,binproto::fixed_len_string<10>,sex);




Define packet type SchoolClass has 3 members

* the type of the 1st member class number is a unsigned int,which is 8bit,0-255
* the type of the 2nd member teacher.
* the type of the 3rd member student list, and the count of students is 256 at most, so the length size of this list is 1 byte.

 SchoolClass

  |-----------------|-----------|------------------------|

  |   class number  |  1 byte   |    like 1 or 2 or 3    |

  |-----------------|-----------|------------------------|

  |     teacher     |  .......  |     teacher struct     |

  |-----------------|-----------|------------------------|

  |   student list  |  .......  |      student list      |

  |-----------------|-----------|------------------------|


BINPROTO_DEFINE_PACKET_P03(SchoolClass

	,binproto::uint8_obj,classnum

	,Teacher,teacher

	,(binproto::binary_obj_list<Student,1>),stulist);

