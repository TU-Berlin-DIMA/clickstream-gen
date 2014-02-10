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

#ifndef SESSION_H_
#define SESSION_H_

#include "record/Record.h"
#include "record/User.h"
#include "record/Product.h"

using namespace Myriad;

namespace ClickStreamGen
{

// forward declarations
class SessionGenerator;
class SessionHydratorChain;

enum SessionType
{
	ST_BROWSE, ST_PURCHASE_ABORT, ST_PURCHASE_SUBMIT
};

// Session class
class Session: public Record
{
public:

	Session()
	{
	}

	const SessionType& type() const;
	void type(const SessionType& v);

	const I32u& numberOfRequests() const;
	void numberOfRequests(const I32u& v);

	const DateTime& sessionStart() const;
	void sessionStart(const DateTime& v);

	const ProductCategory& primaryProductCategory() const;
	void primaryProductCategory(const ProductCategory& v);

	const AutoPtr<User>& user() const;
	void user(const AutoPtr<User>& v);

private:

	SessionType _type;
	I32u _number_of_requests;
	DateTime _session_start;
	ProductCategory _primary_product_category;

	AutoPtr<User> _user;
};

inline const SessionType& Session::type() const
{
	return _type;
}

inline void Session::type(const SessionType& v)
{
	_type = v;
}

inline const I32u& Session::numberOfRequests() const
{
	return _number_of_requests;
}

inline void Session::numberOfRequests(const I32u& v)
{
	_number_of_requests = v;
}

inline const DateTime& Session::sessionStart() const
{
	return _session_start;
}

inline void Session::sessionStart(const DateTime& v)
{
	_session_start = v;
}

inline const ProductCategory& Session::primaryProductCategory() const
{
	return _primary_product_category;
}

inline void Session::primaryProductCategory(const ProductCategory& v)
{
	_primary_product_category = v;
}

inline const AutoPtr<User>& Session::user() const
{
	return _user;
}

inline void Session::user(const AutoPtr<User>& v)
{
	_user = v;
}

} // namespace ClickStreamGen

namespace Myriad
{

// record traits specialization
template<> struct RecordTraits<ClickStreamGen::Session>
{
	typedef ClickStreamGen::SessionGenerator GeneratorType;
	typedef ClickStreamGen::SessionHydratorChain HydratorChainType;
};

// forward declaration of operator<<
OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::Session& record);

} // namespace Myriad

#endif /* SESSION_H_ */
