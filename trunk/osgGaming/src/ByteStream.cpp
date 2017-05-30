#include <osgGaming/ByteStream.h>

#include <cstring>

namespace osgGaming
{

ByteStream::ByteStream(char* data)
	: _data(data),
	  _pos(0)
{

}

char* ByteStream::readString(int size)
{
	char* s = new char[size + 1];
	std::memcpy(&s[0], &_data[_pos], size);
	s[size] = '\0';

	_pos += size;

	return s;
}

int ByteStream::getPos()
{
	return _pos;
}

}
