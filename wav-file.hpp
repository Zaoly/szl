#pragma once
#pragma warning(push)
#pragma warning(disable: 26495) // C26495: Variable '...' is uninitialized. Always initialize a member variable (type. 6)

#include <cstdint>
#include <cstring>
#include <fstream>

namespace zaoly
{
#define ASSERT_TYPE_SIZE(type_name, byte_count) \
static_assert(                                  \
	sizeof(type_name) == (byte_count),          \
	"Size of " #type_name " is not " #byte_count " byte(s), which may cause problems")

	ASSERT_TYPE_SIZE(uint8_t, 1);
	ASSERT_TYPE_SIZE(int16_t, 2);
	ASSERT_TYPE_SIZE(int32_t, 4);
	ASSERT_TYPE_SIZE(uint16_t, 2);
	ASSERT_TYPE_SIZE(uint32_t, 4);
	ASSERT_TYPE_SIZE(float, 4);

	struct int24_t // 3-byte number, plain old data
	{
		int24_t() {}

		int24_t(int32_t num)
		{
			byte1 = num & 0xff;
			byte2 = num >> 8 & 0xff;
			byte3 = num >> 16 & 0xff;
		}

		int24_t& operator=(int32_t num)
		{
			byte1 = num & 0xff;
			byte2 = num >> 8 & 0xff;
			byte3 = num >> 16 & 0xff;
			return *this;
		}

#define ASSIGN_OPERATOR(class_name, operating_sign)   \
template <typename T>                                 \
class_name& operator##operating_sign##=(const T& num) \
{                                                     \
	return *this = *this operating_sign num;          \
}

		ASSIGN_OPERATOR(int24_t, +)  // +=
		ASSIGN_OPERATOR(int24_t, -)  // -=
		ASSIGN_OPERATOR(int24_t, *)  // *=
		ASSIGN_OPERATOR(int24_t, /)  // /=
		ASSIGN_OPERATOR(int24_t, %)  // %=
		ASSIGN_OPERATOR(int24_t, &)  // &=
		ASSIGN_OPERATOR(int24_t, |)  // |=
		ASSIGN_OPERATOR(int24_t, ^)  // ^=
		ASSIGN_OPERATOR(int24_t, <<) // <<=
		ASSIGN_OPERATOR(int24_t, >>) // >>=

		int24_t& operator++()
		{
			int32_t result = *this;
			++result;
			return *this = result;
		}

		int24_t operator++(int)
		{
			int32_t result = *this;
			int24_t ret = *this;
			result++;
			*this = result;
			return ret;
		}

		int24_t& operator--()
		{
			int32_t result = *this;
			--result;
			return *this = result;
		}

		int24_t operator--(int)
		{
			int32_t result = *this;
			int24_t ret = *this;
			result--;
			*this = result;
			return ret;
		}

		operator int32_t() const
		{
			if (byte3 >= 0)
				return
				static_cast<int32_t>(byte1) |
				static_cast<int32_t>(byte2) << 8 |
				static_cast<int32_t>(byte3) << 16;
			else
				return
				static_cast<int32_t>(byte1) |
				static_cast<int32_t>(byte2) << 8 |
				static_cast<int32_t>(byte3) << 16 |
				static_cast<int32_t>(0xff000000);
		}

		unsigned char byte1, byte2;
		signed char byte3;
	};

#define EXCEPTION_CLASS(class_name, description)  \
class class_name : public std::exception          \
{                                                 \
public:                                           \
	class_name() : std::exception(description) {} \
};

	EXCEPTION_CLASS(wav_format_error, "WAV format error")
	EXCEPTION_CLASS(data_unavailable, "Data unavailable")
	EXCEPTION_CLASS(type_mismatch, "Type mismatch")
	EXCEPTION_CLASS(fail_to_read_wav, "Fail to read WAV")
	EXCEPTION_CLASS(fail_to_write_wav, "Fail to write WAV")

	class wav_file
	{
	public:
		using WAV8BIT = uint8_t;      // Unsigned
		using WAV16BIT = int16_t;     // Signed
		using WAV24BIT = int24_t;     // Signed
		using WAV32BIT = int32_t;     // Signed
		using WAV32BIT_FLOAT = float; // Floating-point

		wav_file() {}

		wav_file(const wav_file& new_wav_file)
		{
			copy_assign(new_wav_file);
		}

		wav_file(wav_file&& new_wav_file) noexcept
		{
			move_assign(std::move(new_wav_file));
		}

		~wav_file()
		{
			delete[] _data_8bit;
			delete[] _data_16bit;
			delete[] _data_24bit;
			delete[] _data_32bit;
			delete[] _data_32bit_float;
		}

		void read(const char* filename)
		{
			_read(filename);
		}

		void read(const wchar_t* filename)
		{
			_read(filename);
		}

		void read(const std::string& filename)
		{
			_read(filename);
		}

		void read(const std::wstring& filename)
		{
			_read(filename);
		}

		void write(const char* filename) const
		{
			_write(filename);
		}

		void write(const wchar_t* filename) const
		{
			_write(filename);
		}

		void write(const std::string& filename) const
		{
			_write(filename);
		}

		void write(const std::wstring& filename) const
		{
			_write(filename);
		}

		void initialize(uint16_t channels, uint32_t sample_rate, uint16_t bits_per_sample, uint16_t format_type = 1)
		{
			if (bits_per_sample % 8 != 0)
				throw wav_format_error();
			_is_available = true;
			_size_no_header = min_size_no_header;
			_format_type = format_type;
			_channels = channels;
			_sample_rate = sample_rate;
			_bytes_per_sec = sample_rate * (bits_per_sample / 8) * channels;
			_bytes_per_moment = (bits_per_sample / 8) * channels;
			_bits_per_sample = bits_per_sample;
			_data_bytes = 0;
			_data_size = 0;
			delete_array_and_set_null(_data_8bit);
			delete_array_and_set_null(_data_16bit);
			delete_array_and_set_null(_data_24bit);
			delete_array_and_set_null(_data_32bit);
			delete_array_and_set_null(_data_32bit_float);
		}

		void data_8bit(const WAV8BIT* new_data, uint32_t data_size)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 8)
				throw type_mismatch();
			_data_bytes = data_size;
			_data_size = data_size;
			delete[] _data_8bit;
			_data_8bit = new WAV8BIT[_data_size];
			memcpy(_data_8bit, new_data, _data_bytes);
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_16bit(const WAV16BIT* new_data, uint32_t data_size)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 16)
				throw type_mismatch();
			_data_bytes = data_size * 2;
			_data_size = data_size;
			delete[] _data_16bit;
			_data_16bit = new WAV16BIT[_data_size];
			memcpy(_data_16bit, new_data, _data_bytes);
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_24bit(const WAV24BIT* new_data, uint32_t data_size)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 24)
				throw type_mismatch();
			_data_bytes = data_size * 3;
			_data_size = data_size;
			delete[] _data_24bit;
			_data_24bit = new WAV24BIT[_data_size];
			memcpy(_data_24bit, new_data, _data_bytes);
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_32bit(const WAV32BIT* new_data, uint32_t data_size)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 32)
				throw type_mismatch();
			_data_bytes = data_size * 4;
			_data_size = data_size;
			delete[] _data_32bit;
			_data_32bit = new WAV32BIT[_data_size];
			memcpy(_data_32bit, new_data, _data_bytes);
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_32bit_float(const WAV32BIT_FLOAT* new_data, uint32_t data_size)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 3 || _bits_per_sample != 32)
				throw type_mismatch();
			_data_bytes = data_size * 4;
			_data_size = data_size;
			delete[] _data_32bit_float;
			_data_32bit_float = new WAV32BIT_FLOAT[_data_size];
			memcpy(_data_32bit_float, new_data, _data_bytes);
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_8bit_by_channel(const WAV8BIT* new_data, uint32_t data_size_per_channel)
		{
			uint32_t channel{}, index{};
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 8)
				throw type_mismatch();
			_data_bytes = data_size_per_channel * _channels;
			_data_size = data_size_per_channel * _channels;
			delete[] _data_8bit;
			_data_8bit = new WAV8BIT[_data_size];
			for (; channel < _channels; ++channel)
				for (index = 0; index < data_size_per_channel; ++index)
					_data_8bit[index * _channels + channel] =
					new_data[channel * data_size_per_channel + index];
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_16bit_by_channel(const WAV16BIT* new_data, uint32_t data_size_per_channel)
		{
			uint32_t channel{}, index{};
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 16)
				throw type_mismatch();
			_data_bytes = data_size_per_channel * 2 * _channels;
			_data_size = data_size_per_channel * _channels;
			delete[] _data_16bit;
			_data_16bit = new WAV16BIT[_data_size];
			for (; channel < _channels; ++channel)
				for (index = 0; index < data_size_per_channel; ++index)
					_data_16bit[index * _channels + channel] =
					new_data[channel * data_size_per_channel + index];
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_24bit_by_channel(const WAV24BIT* new_data, uint32_t data_size_per_channel)
		{
			uint32_t channel{}, index{};
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 24)
				throw type_mismatch();
			_data_bytes = data_size_per_channel * 3 * _channels;
			_data_size = data_size_per_channel * _channels;
			delete[] _data_24bit;
			_data_24bit = new WAV24BIT[_data_size];
			for (; channel < _channels; ++channel)
				for (index = 0; index < data_size_per_channel; ++index)
					_data_24bit[index * _channels + channel] =
					new_data[channel * data_size_per_channel + index];
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_32bit_by_channel(const WAV32BIT* new_data, uint32_t data_size_per_channel)
		{
			uint32_t channel{}, index{};
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 32)
				throw type_mismatch();
			_data_bytes = data_size_per_channel * 4 * _channels;
			_data_size = data_size_per_channel * _channels;
			delete[] _data_32bit;
			_data_32bit = new WAV32BIT[_data_size];
			for (; channel < _channels; ++channel)
				for (index = 0; index < data_size_per_channel; ++index)
					_data_32bit[index * _channels + channel] =
					new_data[channel * data_size_per_channel + index];
			_size_no_header = min_size_no_header + _data_bytes;
		}

		void data_32bit_float_by_channel(const WAV32BIT_FLOAT* new_data, uint32_t data_size_per_channel)
		{
			uint32_t channel{}, index{};
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 3 || _bits_per_sample != 32)
				throw type_mismatch();
			_data_bytes = data_size_per_channel * 4 * _channels;
			_data_size = data_size_per_channel * _channels;
			delete[] _data_32bit_float;
			_data_32bit_float = new WAV32BIT_FLOAT[_data_size];
			for (; channel < _channels; ++channel)
				for (index = 0; index < data_size_per_channel; ++index)
					_data_32bit_float[index * _channels + channel] =
					new_data[channel * data_size_per_channel + index];
			_size_no_header = min_size_no_header + _data_bytes;
		}

#define ACCESSOR_FUNCTION(function_name) \
auto function_name() const               \
{                                        \
	return _##function_name;             \
}

		ACCESSOR_FUNCTION(is_available)
		ACCESSOR_FUNCTION(size_no_header)
		ACCESSOR_FUNCTION(format_type)
		ACCESSOR_FUNCTION(channels)
		ACCESSOR_FUNCTION(sample_rate)
		ACCESSOR_FUNCTION(bytes_per_sec)
		ACCESSOR_FUNCTION(bytes_per_moment)
		ACCESSOR_FUNCTION(bits_per_sample)
		ACCESSOR_FUNCTION(data_bytes)
		ACCESSOR_FUNCTION(data_size)

		WAV8BIT& data_8bit(size_t index)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 8)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_8bit[index];
		}

		const WAV8BIT& data_8bit(size_t index) const
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 8)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_8bit[index];
		}

		WAV16BIT& data_16bit(size_t index)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 16)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_16bit[index];
		}

		const WAV16BIT& data_16bit(size_t index) const
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 16)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_16bit[index];
		}

		WAV24BIT& data_24bit(size_t index)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 24)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_24bit[index];
		}

		const WAV24BIT& data_24bit(size_t index) const
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 24)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_24bit[index];
		}

		WAV32BIT& data_32bit(size_t index)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 32)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_32bit[index];
		}

		const WAV32BIT& data_32bit(size_t index) const
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 1 || _bits_per_sample != 32)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_32bit[index];
		}

		WAV32BIT_FLOAT& data_32bit_float(size_t index)
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 3 || _bits_per_sample != 32)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_32bit_float[index];
		}

		const WAV32BIT_FLOAT& data_32bit_float(size_t index) const
		{
			if (!_is_available)
				throw data_unavailable();
			else if (_format_type != 3 || _bits_per_sample != 32)
				throw type_mismatch();
			else if (index >= _data_size)
				throw std::out_of_range("Subscript out of range");
			else
				return _data_32bit_float[index];
		}

		WAV8BIT& data_8bit_by_channel(size_t index, size_t channel)
		{
			return data_8bit(index * _channels + channel);
		}

		const WAV8BIT& data_8bit_by_channel(size_t index, size_t channel) const
		{
			return data_8bit(index * _channels + channel);
		}

		WAV16BIT& data_16bit_by_channel(size_t index, size_t channel)
		{
			return data_16bit(index * _channels + channel);
		}

		const WAV16BIT& data_16bit_by_channel(size_t index, size_t channel) const
		{
			return data_16bit(index * _channels + channel);
		}

		WAV24BIT& data_24bit_by_channel(size_t index, size_t channel)
		{
			return data_24bit(index * _channels + channel);
		}

		const WAV24BIT& data_24bit_by_channel(size_t index, size_t channel) const
		{
			return data_24bit(index * _channels + channel);
		}

		WAV32BIT& data_32bit_by_channel(size_t index, size_t channel)
		{
			return data_32bit(index * _channels + channel);
		}

		const WAV32BIT& data_32bit_by_channel(size_t index, size_t channel) const
		{
			return data_32bit(index * _channels + channel);
		}

		WAV32BIT_FLOAT& data_32bit_float_by_channel(size_t index, size_t channel)
		{
			return data_32bit_float(index * _channels + channel);
		}

		const WAV32BIT_FLOAT& data_32bit_float_by_channel(size_t index, size_t channel) const
		{
			return data_32bit_float(index * _channels + channel);
		}

		wav_file& operator=(const wav_file& new_wav_file)
		{
			copy_assign(new_wav_file);
			return *this;
		}

		wav_file& operator=(wav_file&& new_wav_file) noexcept
		{
			move_assign(std::move(new_wav_file));
			return *this;
		}

	private:
		const uint32_t min_size_no_header = 36;

		template <typename string_type>
		void _read(const string_type filename)
		{
			std::ifstream file(filename, std::ios::in | std::ios::binary);
			uint32_t fmt_size{};
			if (file.fail())
				throw fail_to_read_wav();
			if (!read_and_check<4>(file, "RIFF"))
				throw wav_format_error();
			read_binary(file, _size_no_header);
			if (!read_and_check<8>(file, "WAVEfmt "))
				throw wav_format_error();
			read_binary(file, fmt_size);
			read_binary(file, _format_type);
			read_binary(file, _channels);
			read_binary(file, _sample_rate);
			read_binary(file, _bytes_per_sec);
			read_binary(file, _bytes_per_moment);
			read_binary(file, _bits_per_sample);
			if (fmt_size < 16)
				throw wav_format_error();
			else if (fmt_size > 16)
				file.seekg(fmt_size - 16, std::ios::cur);
			if (!read_and_check<4>(file, "data"))
				throw wav_format_error();
			read_binary(file, _data_bytes);
			_is_available = true;
			delete_array_and_set_null(_data_8bit);
			delete_array_and_set_null(_data_16bit);
			delete_array_and_set_null(_data_24bit);
			delete_array_and_set_null(_data_32bit);
			delete_array_and_set_null(_data_32bit_float);
			if (_format_type == 1)
				switch (_bits_per_sample)
				{
				case 8:
					_data_size = _data_bytes;
					_data_8bit = new WAV8BIT[_data_size];
					file.read(reinterpret_cast<char*>(_data_8bit), _data_bytes);
					break;
				case 16:
					if (_data_bytes % 2 != 0)
						throw wav_format_error();
					_data_size = _data_bytes / 2;
					_data_16bit = new WAV16BIT[_data_size];
					file.read(reinterpret_cast<char*>(_data_16bit), _data_bytes);
					break;
				case 24:
					if (_data_bytes % 3 != 0)
						throw wav_format_error();
					_data_size = _data_bytes / 3;
					_data_24bit = new WAV24BIT[_data_size];
					file.read(reinterpret_cast<char*>(_data_24bit), _data_bytes);
					break;
				case 32:
					if (_data_bytes % 4 != 0)
						throw wav_format_error();
					_data_size = _data_bytes / 4;
					_data_32bit = new WAV32BIT[_data_size];
					file.read(reinterpret_cast<char*>(_data_32bit), _data_bytes);
					break;
				default:
					_is_available = false;
					throw wav_format_error();
				}
			else if (_format_type == 3 && _bits_per_sample == 32)
			{
				if (_data_bytes % 4 != 0)
					throw wav_format_error();
				_data_size = _data_bytes / 4;
				_data_32bit_float = new WAV32BIT_FLOAT[_data_size];
				file.read(reinterpret_cast<char*>(_data_32bit_float), _data_bytes);
			}
			else
			{
				_is_available = false;
				throw wav_format_error();
			}
			_size_no_header = min_size_no_header + _data_bytes;
		}

		template <typename string_type>
		void _write(const string_type filename) const
		{
			if (!_is_available)
				throw data_unavailable();
			std::ofstream file(filename, std::ios::out | std::ios::binary);
			file.write("RIFF", 4);
			write_binary(file, _size_no_header);
			file.write("WAVEfmt ", 8);
			write_binary(file, static_cast<uint32_t>(16));
			write_binary(file, _format_type);
			write_binary(file, _channels);
			write_binary(file, _sample_rate);
			write_binary(file, _bytes_per_sec);
			write_binary(file, _bytes_per_moment);
			write_binary(file, _bits_per_sample);
			file.write("data", 4);
			write_binary(file, _data_bytes);
			if (_format_type == 1)
				switch (_bits_per_sample)
				{
				case 8:
					file.write(reinterpret_cast<const char*>(_data_8bit), _data_bytes);
					break;
				case 16:
					file.write(reinterpret_cast<const char*>(_data_16bit), _data_bytes);
					break;
				case 24:
					file.write(reinterpret_cast<const char*>(_data_24bit), _data_bytes);
					break;
				case 32:
					file.write(reinterpret_cast<const char*>(_data_32bit), _data_bytes);
					break;
				default:
					throw wav_format_error();
				}
			else if (_format_type == 3 && _bits_per_sample == 32)
				file.write(reinterpret_cast<const char*>(_data_32bit_float), _data_bytes);
			if (file.fail())
				throw fail_to_write_wav();
		}

		template <typename data_type>
		static void read_binary(std::ifstream& file, data_type& dest)
		{
			file.read(reinterpret_cast<char*>(&dest), sizeof(dest));
		}

		template <size_t count>
		static bool read_and_check(std::ifstream& file, const char* correct_string)
		{
			char read_string[count]{};
			read_binary(file, read_string);
			return memcmp(read_string, correct_string, count) == 0;
		}

		template <typename data_type>
		static void write_binary(std::ofstream& file, const data_type& dest)
		{
			file.write(reinterpret_cast<const char*>(&dest), sizeof(dest));
		}

#define COPY_PROPERTY(property_name) property_name = new_wav_file.property_name

		void copy_assign(const wav_file& new_wav_file)
		{
			WAV8BIT* new_data_8bit = nullptr;
			WAV16BIT* new_data_16bit = nullptr;
			WAV24BIT* new_data_24bit = nullptr;
			WAV32BIT* new_data_32bit = nullptr;
			WAV32BIT_FLOAT* new_data_32bit_float = nullptr;
			COPY_PROPERTY(_is_available);
			COPY_PROPERTY(_size_no_header);
			COPY_PROPERTY(_format_type);
			COPY_PROPERTY(_channels);
			COPY_PROPERTY(_sample_rate);
			COPY_PROPERTY(_bytes_per_sec);
			COPY_PROPERTY(_bytes_per_moment);
			COPY_PROPERTY(_bits_per_sample);
			COPY_PROPERTY(_data_bytes);
			COPY_PROPERTY(_data_size);
			delete_array_and_set_null(_data_8bit);
			delete_array_and_set_null(_data_16bit);
			delete_array_and_set_null(_data_24bit);
			delete_array_and_set_null(_data_32bit);
			delete_array_and_set_null(_data_32bit_float);
			if (_format_type == 1)
				switch (_bits_per_sample)
				{
				case 8:
					new_data_8bit = new WAV8BIT[_data_bytes];
					memcpy(new_data_8bit, new_wav_file._data_8bit, _data_bytes);
					delete[] _data_8bit;
					_data_8bit = new_data_8bit;
					break;
				case 16:
					new_data_16bit = new WAV16BIT[_data_bytes];
					memcpy(new_data_16bit, new_wav_file._data_16bit, _data_bytes);
					delete[] _data_16bit;
					_data_16bit = new_data_16bit;
					break;
				case 24:
					new_data_24bit = new WAV24BIT[_data_bytes];
					memcpy(new_data_24bit, new_wav_file._data_24bit, _data_bytes);
					delete[] _data_24bit;
					_data_24bit = new_data_24bit;
					break;
				case 32:
					new_data_32bit = new WAV32BIT[_data_bytes];
					memcpy(new_data_32bit, new_wav_file._data_32bit, _data_bytes);
					delete[] _data_32bit;
					_data_32bit = new_data_32bit;
					break;
				}
			else if (_format_type == 3 && _bits_per_sample == 32)
			{
				new_data_32bit_float = new WAV32BIT_FLOAT[_data_bytes];
				memcpy(new_data_32bit_float, new_wav_file._data_32bit_float, _data_bytes);
				delete[] _data_32bit_float;
				_data_32bit_float = new_data_32bit_float;
			}
		}

		void move_assign(wav_file&& new_wav_file) noexcept
		{
			COPY_PROPERTY(_is_available);
			COPY_PROPERTY(_size_no_header);
			COPY_PROPERTY(_format_type);
			COPY_PROPERTY(_channels);
			COPY_PROPERTY(_sample_rate);
			COPY_PROPERTY(_bytes_per_sec);
			COPY_PROPERTY(_bytes_per_moment);
			COPY_PROPERTY(_bits_per_sample);
			COPY_PROPERTY(_data_bytes);
			COPY_PROPERTY(_data_size);
			std::swap(_data_8bit, new_wav_file._data_8bit);
			std::swap(_data_16bit, new_wav_file._data_16bit);
			std::swap(_data_24bit, new_wav_file._data_24bit);
			std::swap(_data_32bit, new_wav_file._data_32bit);
			std::swap(_data_32bit_float, new_wav_file._data_32bit_float);
		}

		template <typename T>
		static void delete_array_and_set_null(T& pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}

		bool _is_available = false;

		uint32_t _size_no_header{};
		uint16_t _format_type{};
		uint16_t _channels{};
		uint32_t _sample_rate{};      // moment per second
		uint32_t _bytes_per_sec{};    // sample_rate * (bits_per_sample / 8) * channels, or sample_rate * bits_per_moment
		uint16_t _bytes_per_moment{}; // (bits_per_sample / 8) * channels
		uint16_t _bits_per_sample{};
		uint32_t _data_bytes{};
		uint32_t _data_size{};

		WAV8BIT* _data_8bit = nullptr;
		WAV16BIT* _data_16bit = nullptr;
		WAV24BIT* _data_24bit = nullptr;
		WAV32BIT* _data_32bit = nullptr;
		WAV32BIT_FLOAT* _data_32bit_float = nullptr;
	};
}

#pragma warning(pop)
