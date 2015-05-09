#pragma once

#include <iostream>

namespace osgGaming
{
	class ByteStream
	{
	public:
		ByteStream(char* data);

		template <class T>
		T read()
		{
			T v;
			memcpy(&v, &_data[_pos], sizeof(T));
			_pos += sizeof(T);

			return v;
		}

		char* readString(int size);

		int getPos();

	private:
		char* _data;
		int _pos;
	};
}