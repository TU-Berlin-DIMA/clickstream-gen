/*
 * Copyright 2010-2011 DIMA Research Group, TU Berlin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author: Alexander Alexandrov <alexander.alexandrov@tu-berlin.de>
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include "record/Record.h"
#include "record/Product.h"
#include "record/Session.h"

using namespace Myriad;

namespace ClickStreamGen
{

// forward declarations
class RequestGenerator;
class RequestHydratorChain;

enum RequestMethod
{
	GET, POST
};

enum RequestType
{
	RT_BROWSE, RT_ADD_TO_CART, RT_PURCHASE, RT_PURCHASE_SUBMIT, RT_PURCHASE_ABORT
};

// Request class
class Request: public Record
{
public:

	Request() :
			_browse_count(0)
	{
	}

	const I32u& position() const;
	void position(const I32u& v);

	void server(const String& v);
	const String& server() const;

	const RequestType& type() const;
	void type(const RequestType& v);

	const RequestMethod& method() const;
	void method(const RequestMethod& v);

	const DateTime& timestamp() const;
	void timestamp(const DateTime& v);

	const I16& browseCount() const;
	void browseCount(const I16& v);

	const ProductCategory& productCategory() const;
	void productCategory(const ProductCategory& v);

	const Interval<I64u>& productInterval() const;
	void productInterval(const Interval<I64u>& v);

	const AutoPtr<Product>& product() const;
	void product(const AutoPtr<Product>& v);

	const AutoPtr<Request>& prev() const;
	void prev(const AutoPtr<Request>& v);

	const AutoPtr<Session>& session() const;
	void session(const AutoPtr<Session>& v);

private:

	I32u _postition;

	String _server;
	RequestType _type;
	RequestMethod _method;
	DateTime _timestamp;
	I16 _browse_count;
	ProductCategory _product_category;
	Interval<I64u> _product_interval;

	AutoPtr<Product> _product;
	AutoPtr<Request> _prev;
	AutoPtr<Session> _session;
};

inline const I32u& Request::position() const
{
	return _postition;
}

inline void Request::position(const I32u& v)
{
	_postition = v;
}

inline const String& Request::server() const
{
	return _server;
}

inline void Request::server(const String& v)
{
	_server = v;
}

inline const RequestType& Request::type() const
{
	return _type;
}

inline void Request::type(const RequestType& v)
{
	_type = v;
}

inline const RequestMethod& Request::method() const
{
	return _method;
}

inline void Request::method(const RequestMethod& v)
{
	_method = v;
}

inline const DateTime& Request::timestamp() const
{
	return _timestamp;
}

inline void Request::timestamp(const DateTime& v)
{
	_timestamp = v;
}

inline const I16& Request::browseCount() const
{
	return _browse_count;
}

inline void Request::browseCount(const I16& v)
{
	_browse_count = v;
}

inline const ProductCategory& Request::productCategory() const
{
	return _product_category;
}

inline void Request::productCategory(const ProductCategory& v)
{
	_product_category = v;
}

inline const Interval<I64u>& Request::productInterval() const
{
	return _product_interval;
}

inline void Request::productInterval(const Interval<I64u>& v)
{
	_product_interval = v;
}

inline const AutoPtr<Product>& Request::product() const
{
	return _product;
}

inline void Request::product(const AutoPtr<Product>& v)
{
	_product = v;
}

inline const AutoPtr<Request>& Request::prev() const
{
	return _prev;
}

inline void Request::prev(const AutoPtr<Request>& v)
{
	_prev = v;
}

inline const AutoPtr<Session>& Request::session() const
{
	return _session;
}

inline void Request::session(const AutoPtr<Session>& v)
{
	_session = v;
}

} // namespace ClickStreamGen

namespace Myriad
{

// record traits specialization
template<> struct RecordTraits<ClickStreamGen::Request>
{
	typedef ClickStreamGen::RequestGenerator GeneratorType;
	typedef ClickStreamGen::RequestHydratorChain HydratorChainType;
};

// forward declaration of operator<<
OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::Request& record);

// enum type conversion methods
inline std::string toString(const ClickStreamGen::RequestMethod& t)
{
	if (t == ClickStreamGen::GET)
	{
		return "GET";
	}
	else if (t == ClickStreamGen::POST)
	{
		return "POST";
	}

	return "<unknown RequestMethod>";
}

template<> inline ClickStreamGen::RequestMethod toEnum<ClickStreamGen::RequestMethod>(int v)
{
	if (v == 0)
	{
		return ClickStreamGen::GET;
	}
	else
	{
		return ClickStreamGen::POST;
	}
}

} // namespace Myriad

#endif /* REQUEST_H_ */
