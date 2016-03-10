// Copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>
#include "base/json_writer.h"

//#include "base/logging.h"
//#include "base/string_util.h"
#include "base/values.h"
//#include "base/string_escape.h"
#include <assert.h>
#define DCHECK assert

#if defined(OS_WIN)
static const char kPrettyPrintLineEnding[] = "\r\n";
#else
static const char kPrettyPrintLineEnding[] = "\n";
#endif

namespace base
{
/* static */
void JSONWriter::Write(const Value* const node,
                       bool pretty_print,
                       std::string* json)
{
    WriteWithOptionalEscape(node, pretty_print, true, json);
}

/* static */
void JSONWriter::WriteWithOptionalEscape(const Value* const node,
        bool pretty_print,
        bool escape,
        std::string* json)
{
    json->clear();
    // Is there a better way to estimate the size of the output?
    json->reserve(1024);
    JSONWriter writer(pretty_print, json);
    writer.BuildJSONString(node, 0, escape);
    if (pretty_print)
        json->append(kPrettyPrintLineEnding);
}

JSONWriter::JSONWriter(bool pretty_print, std::string* json)
    : json_string_(json),
      pretty_print_(pretty_print)
{
    DCHECK(json);
}

void JSONWriter::BuildJSONString(const Value* const node,
                                 int depth,
                                 bool escape)
{
    switch (node->GetType()) {
    case Value::TYPE_NULL:
        json_string_->append("null");
        break;

    case Value::TYPE_BOOLEAN: {
        bool value;
        bool result = node->GetAsBoolean(&value);
        DCHECK(result);
        json_string_->append(value ? "true" : "false");
        break;
    }

    case Value::TYPE_INTEGER: {
        int value;
        bool result = node->GetAsInteger(&value);
        DCHECK(result);
        std::stringstream ss;
        ss << value;
        //StringAppendF(json_string_, "%d", value);
        json_string_->append(ss.str());
        break;
    }

    case Value::TYPE_DOUBLE: {
        double value;
        bool result = node->GetAsDouble(&value);
        DCHECK(result);
        std::stringstream ss;
        ss << value;
        //std::string real = DoubleToString(value);
        std::string real = ss.str();
        // Ensure that the number has a .0 if there's no decimal or 'e'.  This
        // makes sure that when we read the JSON back, it's interpreted as a
        // real rather than an int.
        if (real.find('.') == std::string::npos &&
            real.find('e') == std::string::npos &&
            real.find('E') == std::string::npos) {
            real.append(".0");
        }
        // The JSON spec requires that non-integer values in the range (-1,1)
        // have a zero before the decimal point - ".52" is not valid, "0.52" is.
        if (real[0] == '.') {
            real.insert(0, "0");
        } else if (real.length() > 1 && real[0] == '-' && real[1] == '.') {
            // "-.1" bad "-0.1" good
            real.insert(1, "0");
        }
        json_string_->append(real);
        break;
    }

    case Value::TYPE_STRING: {
        std::string value;
        bool result = node->GetAsString(&value);
        DCHECK(result);
        //if (escape) {
        //    JsonDoubleQuote(UTF8ToUTF16(value),
        //                                   true,
        //                                   json_string_);
        //} else {
        //    JsonDoubleQuote(value, true, json_string_);
        //}
        JsonDoubleQuote(value, true, json_string_);
        break;
    }

    case Value::TYPE_LIST: {
        json_string_->append("[");
        if (pretty_print_)
            json_string_->append(" ");

        const ListValue* list = static_cast<const ListValue*>(node);
        for (size_t i = 0; i < list->GetSize(); ++i) {
            if (i != 0) {
                json_string_->append(",");
                if (pretty_print_)
                    json_string_->append(" ");
            }

            const Value* value = NULL;
            bool result = list->Get(i, &value);
            DCHECK(result);
            BuildJSONString(value, depth, escape);
        }

        if (pretty_print_)
            json_string_->append(" ");
        json_string_->append("]");
        break;
    }

    case Value::TYPE_DICTIONARY: {
        json_string_->append("{");
        if (pretty_print_)
            json_string_->append(kPrettyPrintLineEnding);

        const DictionaryValue* dict =
            static_cast<const DictionaryValue*>(node);
        bool first_value_has_been_output = false;
        for (DictionaryValue::Iterator itr(*dict); !itr.IsAtEnd(); itr.Advance()) {
            if (first_value_has_been_output) {
                json_string_->push_back(',');
                if (pretty_print_)
                    json_string_->append(kPrettyPrintLineEnding);
            }
            const Value* value = NULL;
            bool result = dict->Get(itr.key(), &value);
            DCHECK(result);
            if (pretty_print_)
                IndentLine(depth + 1);
            //AppendQuotedString(itr.key());
            JsonDoubleQuote(itr.key(), true, json_string_);
            if (pretty_print_) {
                json_string_->append(": ");
            } else {
                json_string_->append(":");
            }
            BuildJSONString(value, depth + 1, escape);
            first_value_has_been_output = true;
        }

        if (pretty_print_) {
            json_string_->append(kPrettyPrintLineEnding);
            IndentLine(depth);
            json_string_->append("}");
        } else {
            json_string_->append("}");
        }
    }
    break;
    default:
        // TODO(jhughes): handle TYPE_BINARY
        //NOTREACHED() << "unknown json type";
        break;
    }
}

void JSONWriter::AppendQuotedString(const std::string& str)
{
    JsonDoubleQuote(str, true, json_string_);
}

void JSONWriter::IndentLine(int depth)
{
    // It may be faster to keep an indent string so we don't have to keep
    // reallocating.
    json_string_->append(std::string(depth * 3, ' '));
}


// Try to escape |c| as a "SingleEscapeCharacter" (\n, etc).  If successful,
// returns true and appends the escape sequence to |dst|.  This isn't required
// by the spec, but it's more readable by humans than the \uXXXX alternatives.
template<typename CHAR>
static bool JsonSingleEscapeChar(const CHAR c, std::string* dst)
{
    // WARNING: if you add a new case here, you need to update the reader as well.
    // Note: \v is in the reader, but not here since the JSON spec doesn't
    // allow it.
    switch (c) {
    case '\b':
        dst->append("\\b");
        break;
    case '\f':
        dst->append("\\f");
        break;
    case '\n':
        dst->append("\\n");
        break;
    case '\r':
        dst->append("\\r");
        break;
    case '\t':
        dst->append("\\t");
        break;
    case '\\':
        dst->append("\\\\");
        break;
    case '"':
        dst->append("\\\"");
        break;
    default:
        return false;
    }
    return true;
}

template <class STR>
void JsonDoubleQuoteT(const STR& str,
                      bool put_in_quotes,
                      std::string* dst)
{
    if (put_in_quotes)
        dst->push_back('"');

    for (typename STR::const_iterator it = str.begin(); it != str.end(); ++it) {
        unsigned char c = *it;
        if (!JsonSingleEscapeChar(c, dst)) {
            if (c < 32 || c > 126) {
                // Technically, we could also pass through c > 126 as UTF8, but this is
                // also optional.  It would also be a pain to implement here.
                unsigned int as_uint = static_cast<unsigned int>(c);
                std::stringstream ss;
                ss << std::hex << as_uint;
                dst->append(ss.str());
                //StringAppendF(dst, "\\u%04X", as_uint);
            } else {
                unsigned char ascii = static_cast<unsigned char>(*it);
                dst->push_back(ascii);
            }
        }
    }

    if (put_in_quotes)
        dst->push_back('"');
}

void JsonDoubleQuote(const std::string& str,
                     bool put_in_quotes,
                     std::string* dst)
{
    JsonDoubleQuoteT(str, put_in_quotes, dst);
}

//void JsonDoubleQuote(const string16& str,
//                     bool put_in_quotes,
//                     std::string* dst) {
//  JsonDoubleQuoteT(str, put_in_quotes, dst);
//}
//

}



