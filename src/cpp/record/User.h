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

#ifndef user_H_
#define user_H_

#include "record/Record.h"

using namespace Myriad;

namespace ClickStreamGen
{

// forward declarations
class UserGenerator;
class UserHydratorChain;

// User class
class User: public Record
{
public:

	User()
	{
	}

	const I64u& key() const;
	void key(const I64u& v);

	const I32u& ipAddress() const;
	void ipAddress(const I32u& v);

	const String& firstName() const;
	void firstName(const String& v);

	const String& lastName() const;
	void lastName(const String& v);

private:

	I64u _key;
	I32u _ip_address;
};

inline const I64u& User::key() const
{
	return _key;
}

inline void User::key(const I64u& v)
{
	_key = v;
}

inline const I32u& User::ipAddress() const
{
	return _ip_address;
}

inline void User::ipAddress(const I32u& v)
{
	_ip_address = v;
}

} // namespace ClickStreamGen

namespace Myriad
{

// record traits specialization
template<> struct RecordTraits<ClickStreamGen::User>
{
	typedef ClickStreamGen::UserGenerator GeneratorType;
	typedef ClickStreamGen::UserHydratorChain HydratorChainType;
};

// forward declaration of operator<<
OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::User& record);

} // namespace Myriad

#endif /* user_H_ */
