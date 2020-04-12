#pragma once
#include <string>
#include "Poco/Net/HTTPClientSession.h"

namespace Reach {
	namespace ActiveX {

		class Utility
		{
		public:
			static void get(const std::string & url);
			static void request(const std::string & url, const std::string & body = "");
			static std::string response();
			static std::wstring convert(const std::string& utf8);
			static std::string formatUid(const std::string& entries);
			static std::string SuperRequestGBK(const std::string& url, const std::string& body);
			static std::string SuperRequest(const std::string& url, const std::string& body);
			static std::string UTF8EncodingGBK(const std::string& inEncoding);
			static std::string UTF8JSONStreamGBK(const std::string & inString);
			static std::string GBKJSONStreamUTF8(const std::string & inString);
			static std::string GBKEncodingUTF8(const std::string& inEncoding);
			static void message(const std::string& message);
		private:
			Utility();
			Utility(const Utility&);
			Utility& operator = (const Utility&);

			static Poco::Net::HTTPClientSession _session;
			static const std::string errorMsg;
		};
//
// bstr test macros
//
#define utility_bstr_check(expr) \
	expr ? expr : ""

	}

#define utility_message_f1(fmt, arg) \
	Utility::message(Poco::format((fmt), (arg)));

#define utility_message_f2(fmt, arg1, arg2) \
	Utility::message(Poco::format((fmt), (arg1), (arg2)));

#define utility_message_f3(fmt, arg1, arg2, arg3) \
	Utility::message(Poco::format((fmt), (arg1), (arg2), (arg2)));

#define utility_message_f4(fmt, arg1, arg2, arg3, arg4) \
	Utility::message(Poco::format((fmt), (arg1), (arg2), (arg3), (arg4)));

#define utility_message_f5(fmt, arg1, arg2, arg3, arg4, arg5) \
	Utility::message(Poco::format((fmt), (arg1), (arg2), (arg3), (arg4), (arg5)));

#define utility_message(msg) \
	Utility::message(msg);
} ///Reach::ActiveX