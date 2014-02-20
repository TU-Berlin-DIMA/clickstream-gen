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

#include "io/OutputCollector.h"
#include "record/Record.h"
#include "record/Product.h"
#include "record/Request.h"
#include "record/Session.h"
#include "record/User.h"

namespace Myriad {

OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::Product& record)
{
    return out;
}

OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::Session& record)
{
    I32u ipAddress = record.user()->ipAddress();

    out << toString((ipAddress >> 24) & 0xff) << "."
        << toString((ipAddress >> 16) & 0xff) << "."
        << toString((ipAddress >> 8 ) & 0xff) << "."
        << toString((ipAddress >> 0 ) & 0xff) << " "
        << record.user()->key()               << "\n";;

	return out;
}

OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::Request& record)
{
	AutoPtr<ClickStreamGen::Session> sessionPtr = record.session();
	AutoPtr<ClickStreamGen::Product> productPtr = record.product();

	I32u ipAddress = sessionPtr->user()->ipAddress();
	int tzd = 0;

	out << DateTimeFormatter::format(record.timestamp(), "%Y-%m-%d %H:%M:%S", tzd) << " "
		<< toString((ipAddress >> 24) & 0xff) << "."
		<< toString((ipAddress >> 16) & 0xff) << "."
		<< toString((ipAddress >> 8 ) & 0xff) << "."
		<< toString((ipAddress >> 0 ) & 0xff) << " "
		<< record.server() << " "
//		<< sessionPtr->genID() << " ";
		<< toString(record.method()) << " ";

	if (record.type() == ClickStreamGen::RT_BROWSE)
	{
		out << "/catalog/" << toString(productPtr->category()) << "/" << productPtr->genID() << "/show";
	}
	else if (record.type() == ClickStreamGen::RT_ADD_TO_CART)
	{
		out << "/catalog/" << toString(productPtr->category()) << "/" << productPtr->genID() << "/addtocart";
	}
	else if (record.type() == ClickStreamGen::RT_PURCHASE)
	{
		out << "/cart/purchase";
	}
	else if (record.type() == ClickStreamGen::RT_PURCHASE_ABORT)
	{
		out << "/cart/purchase/submit";
	}
	else if (record.type() == ClickStreamGen::RT_PURCHASE_SUBMIT)
	{
		out << "/cart/purchase/abort";
	}

	out << "\n";

	return out;
}

OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::User& record)
{
    out << record.key() << " ";
    I32u ipAddress = record.ipAddress();

    out << toString((ipAddress >> 24) & 0xff) << "."
        << toString((ipAddress >> 16) & 0xff) << "."
        << toString((ipAddress >> 8 ) & 0xff) << "."
        << toString((ipAddress >> 0 ) & 0xff) << "\n";

    return out;
}

}  // namespace Myriad
