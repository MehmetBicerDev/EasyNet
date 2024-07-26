#pragma once
#include "includes.h"




namespace easynet
{
    namespace data {
        class c_buffer
        {
        public:
            c_buffer() : position(0) {}

            c_buffer(const std::int8_t* buffer, size_t size) : position(0)
            {
                std::lock_guard<std::mutex> lock(mtx);
                data.reserve(size);
                data.insert(data.end(), buffer, buffer + size);
            }

            c_buffer(const c_buffer& buffer)
            {
                std::lock_guard<std::mutex> lock(mtx);
                data = buffer.data;
                position = buffer.position;
            }

            c_buffer& operator=(const c_buffer& buffer)
            {
                if (this != &buffer)
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    data = buffer.data;
                    position = buffer.position;
                }
                return *this;
            }

            bool write_uint(uint32_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(uint32_t));
                return true;
            }

            bool read_uint(uint32_t& value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (bytes_left() < sizeof(uint32_t))
                    return false;

                std::memcpy(&value, &data[position], sizeof(uint32_t));
                position += sizeof(uint32_t);
                return true;
            }

            bool read_int(int32_t& value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (bytes_left() < sizeof(int32_t))
                    return false;

                std::memcpy(&value, &data[position], sizeof(int32_t));
                position += sizeof(int32_t);
                return true;
            }

            bool write_int(int32_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(int32_t));
                return true;
            }

            bool read_uint16(uint16_t& value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (bytes_left() < sizeof(uint16_t))
                    return false;

                std::memcpy(&value, &data[position], sizeof(uint16_t));
                position += sizeof(uint16_t);
                return true;
            }

            bool read_uint64(uint64_t& value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (bytes_left() < sizeof(uint64_t))
                    return false;

                std::memcpy(&value, &data[position], sizeof(uint64_t));
                position += sizeof(uint64_t);
                return true;
            }

            bool write_int16(int16_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(int16_t));
                return true;
            }

            bool write_uint16(uint16_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(uint16_t));
                return true;
            }

            bool read_uint8(uint8_t& value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (bytes_left() < sizeof(uint8_t))
                    return false;

                std::memcpy(&value, &data[position], sizeof(uint8_t));
                position += sizeof(uint8_t);
                return true;
            }

            bool write_int8(int8_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(int8_t));
                return true;
            }

            bool write_uint8(uint8_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(uint8_t));
                return true;
            }

            bool write_int64(int64_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(int64_t));
                return true;
            }

            bool write_uint64(uint64_t value)
            {
                std::lock_guard<std::mutex> lock(mtx);
                uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);
                data.insert(data.end(), ptr, ptr + sizeof(uint64_t));
                return true;
            }

            bool write_bytes(const uint8_t* bytes, size_t size)
            {
                std::lock_guard<std::mutex> lock(mtx);
                data.insert(data.end(), bytes, bytes + size);
                return true;
            }

            bool write_bytes(c_buffer& buffer, bool include_size = true)
            {
                if (include_size)
                    write_uint(buffer.data.size());

                return write_bytes(buffer.data.data(), buffer.data.size());
            }

            bool read_bytes(uint8_t* bytes, size_t size)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (size > bytes_left())
                    return false;

                std::memcpy(bytes, &data[position], size);
                position += size;
                return true;
            }

            bool read_bytes(c_buffer& buffer, uint32_t dataSize = -1)
            {
                if (dataSize == -1)
                {
                    if (!read_uint(dataSize))
                        return false;
                }

                if (dataSize > bytes_left())
                    return false;

                buffer.data.resize(dataSize);
                return read_bytes(buffer.data.data(), dataSize);
            }

            bool write_string(const std::string& str)
            {
                uint16_t length = str.length();
                write_uint16(length);

                const uint8_t* strData = reinterpret_cast<const uint8_t*>(str.c_str());
                return write_bytes(strData, length);
            }

            bool read_string(std::string& str)
            {
                uint16_t length;
                if (!read_uint16(length))
                    return false;

                std::vector<uint8_t> strData(length);
                if (!read_bytes(strData.data(), length))
                    return false;

                str.assign(reinterpret_cast<char*>(strData.data()), length);
                return true;
            }

            void reset_position()
            {
                position = 0;
            }

            size_t size() const
            {
                return data.size();
            }

            size_t bytes_left() const
            {
                return size() - position;
            }

            uint8_t* buffer()
            {
                return data.data();
            }

            void remove(size_t count)
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (count < data.size())
                {
                    data.erase(data.begin(), data.begin() + count);
                    if (position >= count)
                    {
                        position -= count;
                    }
                    else
                    {
                        position = 0;
                    }
                }
                else
                {
                    data.clear();
                    position = 0;
                }
            }

        private:
            std::vector<uint8_t> data;
            size_t position;
            mutable std::mutex mtx;
        };
    }
}