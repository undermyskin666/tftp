#ifndef _OMS_MSG_TFTP_OPTION_H
#define _OMS_MSG_TFTP_OPTION_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>
#include <string>
namespace oms
{
    namespace msg
    {
#define TFTP_OPT_TSIZE "tsize"
#define TFTP_OPT_BLKSIZE "blksize"
#define TFTP_OPT_TIMEOUT "timeout"

        class TFTPOpt
        {
        private:
            std::string m_key;
            std::string m_value;

            TFTPOpt &operator=(const TFTPOpt &);

        public:
            TFTPOpt() {}
            TFTPOpt(const char *key, const char *value) : m_key(key), m_value(value)
            {
            }
            TFTPOpt(const char *key, uint32_t value) : m_key(key)
            {
                SetValue(value);
            }
            TFTPOpt(const TFTPOpt &o) : m_key(o.m_key), m_value(o.m_value)
            {
            }

            void SetValue(const char *v)
            {
                if (v)
                    m_value = v;
                else
                    m_value = std::string();
            }
            void SetValue(uint32_t v)
            {
                char cache[64] = {0};
                snprintf(cache, 16, "%u", v);
                m_value = cache;
            }
            void SetValue(uint64_t v)
            {
                char cache[64] = {0};
                snprintf(cache, 16, "%llu", v);
                m_value = cache;
            }
            const char *Name() const
            {
                return m_key.c_str();
            }
            const char *Value() const
            {
                return m_value.c_str();
            }
            uint32_t UInt32Value() const
            {
                if (m_value.length() > 0)
                    return atol(m_value.c_str());
                return 0;
            }
            uint64_t UInt64Value() const
            {
                if (m_value.length() > 0)
                    return atoll(m_value.c_str());
                return 0;
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                if (!buf)
                    return -1;
                if (!len)
                    return 0;

                const uint8_t *k = NULL, *v = NULL, *kn = NULL, *vn = NULL;
                for (uint32_t i = 0; i < len; i++)
                {
                    if (!k)
                        k = &buf[i];
                    if (kn && !v)
                        v = &buf[i];

                    if (!buf[i])
                    {
                        if (!kn)
                        {
                            kn = &buf[i];
                        }
                        else
                        {
                            vn = &buf[i];
                            break;
                        }
                    }
                }
                if (k && kn && k != kn && v && vn)
                {
                    m_key = (const char *)k;
                    m_value = (const char *)v;
                    return vn - buf + 1;
                }
                return -1;
            }
            int32_t Encode(uint8_t *buf, uint32_t len) const
            {
                if (m_key.length() > 0 && buf && len >= m_key.length() + 1 + m_value.length() + 1)
                {
                    memcpy(buf, m_key.c_str(), m_key.length() + 1);
                    buf += m_key.length() + 1;

                    if (m_value.length() > 0)
                    {
                        memcpy(buf, m_value.c_str(), m_value.length());
                        buf += m_value.length();
                    }
                    *buf = 0;
                    return m_key.length() + 1 + m_value.length() + 1;
                }
                return -1;
            }
        };

        class TFTPOpts
        {
        public:
            typedef std::list<TFTPOpt>::const_iterator const_iterator;
            typedef std::list<TFTPOpt>::iterator iterator;

        private:
            std::list<TFTPOpt> m_opts;

            TFTPOpts(const TFTPOpts &);
            TFTPOpts &operator=(const TFTPOpts &);

        public:
            TFTPOpts() {}
            ~TFTPOpts() {}

            bool empty() const
            {
                return m_opts.empty();
            }
            iterator begin()
            {
                return m_opts.begin();
            }
            const_iterator begin() const
            {
                return m_opts.begin();
            }
            iterator end()
            {
                return m_opts.end();
            }
            const_iterator end() const
            {
                return m_opts.end();
            }
            iterator find(const char *name)
            {
                for (iterator it = begin(); it != end(); it++)
                {
                    if (!strcasecmp(name, it->Name()))
                        return it;
                }
                return end();
            }
            const_iterator find(const char *name) const
            {
                for (const_iterator it = begin(); it != end(); it++)
                {
                    if (!strcasecmp(name, it->Name()))
                        return it;
                }
                return end();
            }
            bool contains(const char *name) const
            {
                return find(name) != end();
            }

            void insert(const char *name, const char *value)
            {
                insert(TFTPOpt(name, value));
            }
            void insert(const char *name, uint32_t value)
            {
                insert(TFTPOpt(name, value));
            }
            void insert(const TFTPOpt &opt)
            {
                iterator it = find(opt.Name());
                if (it != end())
                    it->SetValue(opt.Value());
                else
                    m_opts.push_back(opt);
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                if (buf && len > 0)
                {
                    uint32_t off = 0;
                    int32_t ret = -1;

                    while (off < len)
                    {
                        TFTPOpt opt;
                        ret = opt.Decode(buf + off, len - off);
                        if (ret < 0)
                            return ret;
                        else if (ret == 0)
                            return off;
                        off += ret;
                        insert(opt);
                    }
                    return off;
                }
                return -1;
            }

            int32_t Encode(uint8_t *buf, uint32_t len) const
            {
                if (buf && len > 0)
                {
                    uint32_t off = 0;
                    int32_t ret = -1;

                    for (const_iterator it = begin();it != end();it++)
                    {
                        ret = it->Encode(buf + off, len - off);
                        if (ret <= 0)
                            return ret;
                        off += ret;
                    }
                    return off;
                }
                return -1;
            }
        };
    }
}

#endif