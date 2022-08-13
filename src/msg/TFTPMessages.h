#ifndef _OMS_MSG_TFTP_MESSAGES_H
#define _OMS_MSG_TFTP_MESSAGES_H
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <string>

namespace oms
{
    namespace msg
    {
        class ICodec
        {
        public:
            virtual int32_t Encode(uint8_t *, uint32_t) const = 0;
            virtual int32_t Decode(const uint8_t *, uint32_t) = 0;
        };
        class TFTPMessage : public ICodec
        {
        protected:
            uint16_t m_opcode;

        public:
            TFTPMessage(uint16_t opcode) : m_opcode(opcode)
            {
            }
            virtual ~TFTPMessage()
            {
            }
            uint16_t GetOpcode() const
            {
                return m_opcode;
            }
            int32_t Encode(uint8_t *data, uint32_t len) const
            {
                if (data && len >= sizeof(m_opcode))
                {
                    data[0] = (m_opcode >> 8) & 0xFF;
                    data[1] = m_opcode & 0xFF;
                    return sizeof(m_opcode);
                }
                return -1;
            }
            int32_t Decode(const uint8_t *data, uint32_t len)
            {
                if (data && len >= sizeof(m_opcode))
                {
                    m_opcode = ((uint16_t)data[0] << 8) | data[1];
                    return sizeof(m_opcode);
                }
                return -1;
            }
        };

        class TFTPOpt
        {
            std::string m_key;
            std::string m_value;

        public:
            TFTPOpt(const char *key, const char *value) : m_key(key), m_value(value)
            {
            }
            TFTPOpt(const char *key, uint32_t value) : m_key(key)
            {
                char cache[64] = {0};
                snprintf(cache, 16, "%u", value);
                m_value = cache;
            }
            const char *OpName() const
            {
                return m_key.c_str();
            }
        };

        class TFTPOpts : public ICodec
        {
        };
    }
}

#endif