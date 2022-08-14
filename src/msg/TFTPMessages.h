#ifndef _OMS_MSG_TFTP_MESSAGES_H
#define _OMS_MSG_TFTP_MESSAGES_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>
#include <string>
#include "msg/TFTPOption.h"

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

        typedef enum
        {
            TFTP_OPCODE_RRQ = 1,
            TFTP_OPCODE_WRQ,
            TFTP_OPCODE_DATA,
            FTFP_OPCODE_ACK,
            TFTP_OPCODE_ERR,
            TFTP_OPCODE_OACK,
        } tftp_opcode_e;

        typedef enum
        {
            TFTP_ERR_NOT_DEFINED,
            TFTP_ERR_FILE_NOT_FOUND,
            TFTP_ERR_ACCESS_VIOLATION,
            TFTP_ERR_DISK_FULL,
            TFTP_ERR_ILLEGAL_OPERATION,
            TFTP_ERR_UNKOWN_TRANSFER_ID,
            TFTP_ERR_FILE_ALREADY_EXIST,
            TFTP_ERR_NO_SUCK_USER,
            TFTP_ERR_OPTION_NEGO
        } tftp_errcode_e;

        typedef enum
        {
            TFTP_MODE_NETASCII, //"netascii"
            TFTP_MODE_OCTET,    //"octet"
            TFTP_MODE_MAIL,     //"mail"
            TFTP_MODE_INVALID
        } tftp_transfer_mode_e;

        //  2 bytes   n bytes
        // +--------+-----------+
        // | opcode | tftp body |
        // +--------+-----------+
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
            uint16_t Opcode() const
            {
                return m_opcode;
            }
        protected:
            int32_t EncodeOpCode(uint8_t *buf, uint32_t len) const
            {
                return EncodeUInt16(m_opcode, buf, len);
            }
            int32_t DecodeOpCode(const uint8_t *buf, uint32_t len)
            {
                return DecodeUInt16(m_opcode, buf, len);
            }
            static int32_t DecodeStr(std::string &v, const uint8_t *buf, uint32_t len)
            {
                if (buf && len)
                {
                    for (uint32_t i = 0; i < len; i++)
                    {
                        if (buf[i] == 0)
                        {
                            v = (const char *)buf;
                            return i + 1;
                        }
                    }
                }
                return -1;
            }
            static int32_t DecodeUInt32(uint32_t &v, const uint8_t *buf, uint32_t len)
            {
                if (buf && len >= sizeof(v))
                {
                    uint16_t v1, v2;
                    DecodeUInt16(v1, buf, len);
                    DecodeUInt16(v2, buf + sizeof(v1), len - sizeof(v1));

                    v = (((uint32_t)v1) << 16) | v2;
                    return sizeof(v);
                }
                return -1;
            }
            static int32_t DecodeUInt16(uint16_t &v, const uint8_t *buf, uint32_t len)
            {
                if (buf && len >= sizeof(v))
                {
                    v = (((uint16_t)buf[0]) << 8) | buf[1];
                    return sizeof(v);
                }
                return -1;
            }

            static int32_t EncodeStr(const std::string &v, uint8_t *buf, uint32_t len)
            {
                if (v.length() + 1 <= len && buf && len)
                {
                    if (v.length())
                        memcpy(buf, v.c_str(), v.length() + 1);
                    else
                        buf[0] = 0;

                    return v.length() + 1;
                }
                return -1;
            }
            static int32_t EncodeUInt32(uint32_t v, uint8_t *buf, uint32_t len)
            {
                if (buf && len >= sizeof(v))
                {
                    uint16_t v1 = (v >> 16) & 0xFFFF;
                    uint16_t v2 = v & 0xFFFF;

                    EncodeUInt16(v1, buf, len);
                    EncodeUInt16(v2, buf + sizeof(v1), len - sizeof(v1));
                    return sizeof(v);
                }
                return -1;
            }
            static int32_t EncodeUInt16(uint16_t v, uint8_t *buf, uint32_t len)
            {
                if (buf && len >= sizeof(v))
                {
                    buf[0] = (v >> 8) & 0xFF;
                    buf[1] = v & 0xFF;
                    return sizeof(v);
                }
                return -1;
            }

            static const char *TransferModeToStr(tftp_transfer_mode_e mode)
            {
                switch (mode)
                {
                case TFTP_MODE_NETASCII:
                    return "netascii";
                case TFTP_MODE_OCTET:
                    return "octet";
                case TFTP_MODE_MAIL:
                    return "mail";
                default:
                    return "";
                }
                return "";
            }
            static tftp_transfer_mode_e StrToTransferMode(const char *str)
            {
                if (strcasecmp("mail", str))
                    return TFTP_MODE_MAIL;
                if (strcasecmp("octet", str))
                    return TFTP_MODE_OCTET;
                if (strcasecmp("netascii", str))
                    return TFTP_MODE_NETASCII;
                return TFTP_MODE_INVALID;
            }
        };

        class TFTPReqMessage : public TFTPMessage
        {
            std::string m_filename;
            tftp_transfer_mode_e m_transfermode;
            TFTPOpts m_opts;

        public:
            TFTPReqMessage(uint16_t opcode) : TFTPMessage(opcode), m_transfermode(TFTP_MODE_OCTET)
            {
            }
            ~TFTPReqMessage()
            {
            }

            void SetFileName(const char *filename)
            {
                m_filename = filename;
            }
            void SetTransferMode(tftp_transfer_mode_e mode)
            {
                m_transfermode = mode;
            }
            const char *FileName() const
            {
                return m_filename.c_str();
            }
            tftp_transfer_mode_e TransferMode() const
            {
                return m_transfermode;
            }

            TFTPOpts &Opts()
            {
                return m_opts;
            }
            const TFTPOpts &Opts() const
            {
                return m_opts;
            }

            int32_t Encode(uint8_t *buf, uint32_t len) const
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = EncodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = EncodeStr(m_filename, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = EncodeStr(std::string(TransferModeToStr(m_transfermode)),
                                buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = m_opts.Encode(buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = DecodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = DecodeStr(m_filename, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                std::string mode;
                ret = DecodeStr(mode, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                m_transfermode = StrToTransferMode(mode.c_str());

                ret = m_opts.Decode(buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }
        };

        class TFTPRReqMessage : public TFTPReqMessage
        {
        public:
            TFTPRReqMessage() : TFTPReqMessage(TFTP_OPCODE_RRQ)
            {
            }
            TFTPRReqMessage(const char *filename, tftp_transfer_mode_e mode) : TFTPReqMessage(TFTP_OPCODE_RRQ)
            {
                SetFileName(filename);
                SetTransferMode(mode);
            }
            ~TFTPRReqMessage()
            {
            }
        };

        class TFTPWReqMessage : public TFTPReqMessage
        {
        public:
            TFTPWReqMessage() : TFTPReqMessage(TFTP_OPCODE_WRQ)
            {
            }
            TFTPWReqMessage(const char *filename, tftp_transfer_mode_e mode) : TFTPReqMessage(TFTP_OPCODE_WRQ)
            {
                SetFileName(filename);
                SetTransferMode(mode);
            }
            ~TFTPWReqMessage()
            {
            }
        };

        class TFTPDataMessage : public TFTPMessage
        {
            uint16_t m_blockNumber;
            uint8_t *m_blockData;
            uint16_t m_blockDataLength;

        public:
            TFTPDataMessage() : TFTPMessage(TFTP_OPCODE_DATA),
                                m_blockNumber(0), m_blockData(NULL), m_blockDataLength(0)
            {
            }
            TFTPDataMessage(uint16_t blkNum, uint8_t *blkData, uint16_t blkDataLen) : TFTPMessage(TFTP_OPCODE_DATA),
                                                                                      m_blockNumber(blkNum),
                                                                                      m_blockData(NULL),
                                                                                      m_blockDataLength(0)
            {
                if (blkData && blkDataLen)
                {
                    m_blockDataLength = blkDataLen;
                    m_blockData = (uint8_t *)calloc(1, blkDataLen);
                    memcpy(m_blockData, blkData, blkDataLen);
                }
            }

            void SetBlockNumber(uint16_t blkNum)
            {
                m_blockNumber = blkNum;
            }
            void SetBlockData(const uint8_t *blkData, uint16_t blkDataLen)
            {
                if (!blkData || blkDataLen)
                {
                    free(m_blockData);
                    m_blockData = NULL;
                    m_blockDataLength = 0;
                    return;
                }
                m_blockDataLength = blkDataLen;
                m_blockData = (uint8_t *)realloc(m_blockData, blkDataLen);
                memcpy(m_blockData, blkData, blkDataLen);
            }

            uint16_t BlockNumber() const
            {
                return m_blockNumber;
            }
            const uint8_t *BlockData() const
            {
                return m_blockData;
            }
            uint16_t BlockDataLength() const
            {
                return m_blockDataLength;
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = DecodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = DecodeUInt16(m_blockNumber, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                SetBlockData(buf + off, len - off);
                return len;
            }

            int32_t Encode(uint8_t *buf, uint32_t len) const
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = EncodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = EncodeUInt16(m_blockNumber, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                if (len - off >= m_blockDataLength)
                {
                    memcpy(buf + off, m_blockData, m_blockDataLength);
                    off += m_blockDataLength;
                    return off;
                }
                return -1;
            }
        };

        class TFTPAckMessage : public TFTPMessage
        {
            uint16_t m_blockNumber;

        public:
            TFTPAckMessage(uint16_t blockNum = 0) : TFTPMessage(FTFP_OPCODE_ACK),
                                                    m_blockNumber(blockNum)
            {
            }

            uint16_t BlockNumber() const
            {
                return m_blockNumber;
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = DecodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = DecodeUInt16(m_blockNumber, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }

            int32_t Encode(uint8_t *buf, uint32_t len) const
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = EncodeOpCode(buf, len);
                if (ret <= 0)
                    return 0;
                off += ret;

                ret = EncodeUInt16(m_blockNumber, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }
        };

        class TFTPErrMessage : public TFTPMessage
        {
            uint16_t m_errorCode;
            std::string m_errorMsg;

        public:
            TFTPErrMessage(uint16_t errorCode = 0, const char *errorMsg = NULL) : TFTPMessage(TFTP_OPCODE_ERR),
                                                                                  m_errorCode(errorCode),
                                                                                  m_errorMsg(errorMsg)
            {
            }

            ~TFTPErrMessage()
            {
            }

            void SetErrorCode(uint16_t errorCode)
            {
                m_errorCode = errorCode;
            }

            void SetErrorMsg(const char *errorMsg)
            {
                m_errorMsg = errorMsg;
            }

            uint16_t ErrorCode() const
            {
                return m_errorCode;
            }

            const char *ErrorMsg() const
            {
                return m_errorMsg.c_str();
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = DecodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = DecodeUInt16(m_errorCode, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = DecodeStr(m_errorMsg, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }

            int32_t Encode(uint8_t *buf, uint32_t len) const
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = EncodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = EncodeUInt16(m_errorCode, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = EncodeStr(m_errorMsg, buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }
        };

        class TFTPOAckMessage : public TFTPMessage
        {
            TFTPOpts m_opts;

        public:
            TFTPOAckMessage() : TFTPMessage(TFTP_OPCODE_OACK)
            {
            }
            ~TFTPOAckMessage() {}

            TFTPOpts &Opts()
            {
                return m_opts;
            }

            const TFTPOpts &Opts() const
            {
                return m_opts;
            }

            int32_t Decode(const uint8_t *buf, uint32_t len)
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = DecodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = m_opts.Decode(buf + off, len - off);
                if ( ret <= 0)
                    return ret;
                off += ret;

                return off;
            }

            int32_t Encode(uint8_t* buf, uint32_t len) const
            {
                uint32_t off = 0;
                int32_t ret = -1;

                ret = EncodeOpCode(buf, len);
                if (ret <= 0)
                    return ret;
                off += ret;

                ret = m_opts.Encode(buf + off, len - off);
                if (ret <= 0)
                    return ret;
                off += ret;

                return off;
            }
        };
    }
}
#endif